// esp32_at_core.c - AT protocol layer (no NEORV32 specifics, see esp32_at_port.c)
#include "esp32_at_core.h"
#include "esp32_at_port.h"
#include <string.h>
#include <stdio.h>

#define ESP32_AT_INIT_RETRIES   15   // Number of "AT" probes before init gives up
#define ESP32_AT_INIT_PROBE_MS  300  // How long each probe waits for a reply



#define AT_LINE_MAX    256  // Longest line that is compared against status strings
#define AT_WINDOW_MAX  16  // Longest expected token esp32_at_wait_for can look for

#define URC_LINE_MAX 256

static esp32_at_urc_cb_t urc_cb = NULL; // User-defined callback for unsolicited lines
// static char urc_line[AT_LINE_MAX]; // Holds the most recent line received from the ESP32, including unsolicited lines
static char urc_line[URC_LINE_MAX]; // Holds the most recent line received from the ESP32, including unsolicited lines
static size_t urc_len = 0; // Length of the most recent line received from the ESP32, including unsolicited lines


// Convert status code to string for debug printing
const char *esp32_at_status_str(esp32_at_status_t st)
{
    switch (st) {
    case ESP32_AT_OK:               return "OK";
    case ESP32_AT_ERROR:            return "ERROR";
    case ESP32_AT_TIMEOUT:          return "TIMEOUT";
    case ESP32_AT_INVALID_RESP:     return "INVALID_RESP";
    case ESP32_AT_BUSY:             return "BUSY";
    case ESP32_AT_READY:            return "READY";
    case ESP32_AT_FAIL:             return "FAIL";
    case ESP32_AT_INVALID_USER_ARG: return "INVALID_USER_ARG";
    default:                        return "UNKNOWN";
    }
}


// URC methods ---------------------------------------

// Set URC callback. Called for each unsolicited line
void esp32_at_set_urc_handler(esp32_at_urc_cb_t cb) 
{
    urc_cb = cb;
}

// Lines that are unsolicited even when they show up in the middle of a command
// TODO: Can't there be more types of URC?
static bool esp32_at_is_urc(const char *line)
{
    return strncmp(line, "+IPD,", 5) == 0 ||
           strncmp(line, "WIFI ", 5) == 0 ||
           strncmp(line, "+MQTT", 5) == 0 ||
           strcmp(line, "CLOSED") == 0;
}

// Read every byte currently in the ring buffer, hand each complete line to the callback
void esp32_at_poll(void)
{

    // Line Ending Pair (\r\n): ESP firmware typically pairs \r with \n (Line Feed, ASCII 10) to form a complete newline sequence (CRLF). 
    // The \r moves the cursor/pointer back to the start of the line, while \n moves it down to the next line.
    char c;
    while (esp32_at_read_char(&c, 0) == 0) {
        if (c == '\r') { // Line terminator in an AT response. Skip it.
            continue;
        }
        if (c != '\n') { // The character is neither a line terminator, nor a newline character, so it is a standard character.
                         // Add it to the read line
            if (urc_len < sizeof(urc_line) - 1) {
                urc_line[urc_len++] = c;
            }
            continue;
        }
        // Encountered a new line character
        urc_line[urc_len] = '\0'; // We have a complete response line
        if (urc_len > 0 && urc_cb) {
            urc_cb(urc_line);   // Check for URC in the callback function
        }
        urc_len = 0; // Reset the line
    }
}

// Receiving methods -------------------------------------
// Collect incoming characters until expected target string is found
esp32_at_status_t esp32_at_wait_for(const char *expected, char *resp_buf, size_t buf_size, uint32_t timeout_ms)
{ 
    char window[AT_WINDOW_MAX]; // Holds only the most recent strlen(expected) characters received
    size_t exp_len = strlen(expected);
    size_t filled = 0;
    size_t idx = 0;
    char c;
    uint32_t elapsed = 0;

    if (exp_len == 0 || exp_len > sizeof(window)) {
        return ESP32_AT_INVALID_RESP; // Expected string does not fit in the window
    }

    if (resp_buf && buf_size > 0) {
        resp_buf[0] = '\0'; // Caller buffer is optional, it only receives a copy of the response
    }

    while (elapsed < timeout_ms) {
        // Get number of clock ticks
        if (esp32_at_read_char(&c, 1) == 0) {
            if (resp_buf && (idx + 1) < buf_size) {
                resp_buf[idx++] = c;
                resp_buf[idx] = '\0';
            }
            if (filled < exp_len) {
                window[filled++] = c; // Window is not full yet
            } else {
                memmove(window, window + 1, exp_len - 1); // Slide the window to right by one character
                window[exp_len - 1] = c;
            }
            if (filled == exp_len && memcmp(window, expected, exp_len) == 0) {
                return ESP32_AT_OK;
            }
        } else {
            // Get number of clock ticks
            elapsed++; // Not ideal. Should compute the difference between clock ticks, and use the system clock to compute time elapsed. Add time elapsed to variable.
        }
    }
    return ESP32_AT_TIMEOUT;
}


// Read lines until a status line (OK / ERROR / FAIL / SEND OK / SEND FAIL / busy) or timeout
static esp32_at_status_t esp32_at_wait_status(char *resp_buf, size_t buf_size, uint32_t timeout_ms)
{
    char line[AT_LINE_MAX]; // Buffer to store ESP32 response line
    size_t line_len = 0;    // Current length of ESP32 response line stored
    bool line_overflow = false; // Is the ESP32 current response line longer than our buffer?
    size_t idx = 0; // Track index in programmer response buffer that is displayed to the user
    char c; // Character read from ESP32
    uint32_t elapsed = 0; 

    if (resp_buf && buf_size > 0) {
        resp_buf[0] = '\0';
    }

    while (elapsed < timeout_ms) {
        if (esp32_at_read_char(&c, 1) == 0) {
            if (resp_buf && (idx + 1) < buf_size) { // Store line in programmer response buffer
                resp_buf[idx++] = c;
                resp_buf[idx] = '\0';
            }
            if (c == '\r') {  // Line terminator in an AT response. Skip it.
                continue; 
            }
            if (c != '\n') {    // The character is neither a line terminator, nor a newline character, so it is a standard character.
                                // Add it to the ESP32 response line if adding it does not cause an overflow
                if (line_len < sizeof(line) - 1) {
                    line[line_len++] = c;
                } else { // If it overflows, then it is too long to be a status line, skip it
                    line_overflow = true; 
                }
                continue;
            }
            // End of line: compare the whole line against the known status lines
            line[line_len] = '\0';
            if (!line_overflow) {
                if (strcmp(line, "OK") == 0 || strcmp(line, "SEND OK") == 0) return ESP32_AT_OK;
                if (strcmp(line, "ERROR") == 0 || strncmp(line, "ERR CODE:", 9) == 0) return ESP32_AT_ERROR;
                if (strcmp(line, "FAIL") == 0 || strcmp(line, "SEND FAIL") == 0) return ESP32_AT_FAIL;
                if (strcmp(line, "busy p...") == 0 || strcmp(line, "busy s...") == 0) return ESP32_AT_BUSY;
            }
            if (!line_overflow && esp32_at_is_urc(line) && urc_cb) { // TODO: comment
                urc_cb(line);
            }
            // Reset ESP32 line buffer for next line
            line_len = 0;
            line_overflow = false;
        } else {
            elapsed++; // Not ideal. Should compute the difference between clock ticks, and use the system clock to compute time elapsed. Add time elapsed to variable.
        }
    }

    return ESP32_AT_TIMEOUT;
}

// Sending methods --------------------------------------------------

// Send an AT command (without CR-LF) and wait for OK / ERROR / FAIL / SEND OK / SEND FAIL
esp32_at_status_t esp32_at_send_cmd(const char *cmd, char *resp_buf, size_t buf_size, uint32_t timeout_ms)
{
    esp32_at_poll();   // Deliver pending unsolicited lines
    urc_len = 0;       // Drop any half-received line
    esp32_at_tx(cmd);
    esp32_at_tx("\r\n"); // Send command to ESP32
    return esp32_at_wait_status(resp_buf, buf_size, timeout_ms);
}

// Wait for '>' prompt, send raw bytes, wait for "SEND OK"
esp32_at_status_t esp32_at_send_data(const char *data, size_t len, uint32_t timeout_ms)
{
    // ESP-AT prints/responds with ">" when it is ready to accept data
    esp32_at_status_t st = esp32_at_wait_for(">", NULL, 0, ESP32_AT_DEFAULT_TIMEOUT_MS);
    if (st != ESP32_AT_OK) { // Did not encounter ">"
        return st;
    }
    esp32_at_tx_bytes(data, len);
    return esp32_at_wait_status(NULL, 0, timeout_ms); // Check if ESP32 got the data correctly
}


// Initialize AT communication by setting up everything --------------
esp32_at_status_t esp32_at_init(uint32_t baudrate)
{   
    esp32_at_port_init(baudrate);
    // The ESP32 may still be booting and printing its log, so probe with short "AT" commands until it answers
    esp32_at_status_t status = ESP32_AT_TIMEOUT;
    for (int attempt = 0; attempt < ESP32_AT_INIT_RETRIES; attempt++) {
        status = esp32_at_send_cmd("AT", NULL, 0, ESP32_AT_INIT_PROBE_MS);
        if (status == ESP32_AT_OK) {
            break;
        }
        esp32_at_delay_ms(100); // Pause so quick ERROR replies do not burn all retries at once
    }
    if (status != ESP32_AT_OK) {
        return status; // ESP32 never answered, report the last failure
    }

    // Let late replies to earlier probes arrive, so they cannot be mistaken for the answer to ATE0
    esp32_at_delay_ms(50);
    esp32_at_flush_rx();

    (void)esp32_at_send_cmd("ATE0", NULL, 0, ESP32_AT_DEFAULT_TIMEOUT_MS);

    return esp32_at_test();
}


// Send basic "AT" test command to verify line presence
esp32_at_status_t esp32_at_test(void)
{ 
    return esp32_at_send_cmd("AT", NULL, 0, ESP32_AT_DEFAULT_TIMEOUT_MS);
}




