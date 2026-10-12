// esp32_at_core.h - AT protocol layer: send commands, match status lines, deliver unsolicited lines
#ifndef ESP32_AT_CORE_H
#define ESP32_AT_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    ESP32_AT_OK           =  0,
    ESP32_AT_ERROR        = -1,
    ESP32_AT_TIMEOUT      = -2,
    ESP32_AT_INVALID_RESP = -3,
    ESP32_AT_BUSY         = -4,
    ESP32_AT_READY        = -5,
    ESP32_AT_FAIL         = -6,
    ESP32_AT_INVALID_USER_ARG = -7
} esp32_at_status_t;

#define ESP32_AT_DEFAULT_TIMEOUT_MS  2000
#define ESP32_AT_RESET_TIMEOUT_MS    5000


typedef void (*esp32_at_urc_cb_t)(const char *line); // Function signature for user defined callback

uint16_t esp32_at_rx_overflow(void); // Helper to get the number of overflow bytes


esp32_at_status_t esp32_at_init(uint32_t baudrate); // Initialize UART1 and verify connection to ESP32
esp32_at_status_t esp32_at_test(void); // Test basic AT connection

const char *esp32_at_status_str(esp32_at_status_t st); // Convert status code to string for debug printing

esp32_at_status_t esp32_at_send_cmd(const char *cmd, char *resp_buf, size_t buf_size, uint32_t timeout_ms); // Send an AT command (without CR-LF) and wait for OK / ERROR / FAIL / SEND OK / SEND FAIL
esp32_at_status_t esp32_at_wait_for(const char *expected, char *resp_buf, size_t buf_size, uint32_t timeout_ms); // Wait until specific substring is received
esp32_at_status_t esp32_at_send_data(const char *data, size_t len, uint32_t timeout_ms); // Wait for '>' prompt, send raw bytes, wait for "SEND OK" / "SEND FAIL"

void esp32_at_poll(void);   // Call regularly from the main loop
                            // Reads complete lines from the ring buffer and passes each one to a user callback.
// send_cmd calls poll before sending, instead of flushing. 
// Stale lines now reach the callback instead of being lost.

void esp32_at_set_urc_handler(esp32_at_urc_cb_t cb); // Set URC callback. Called for each unsolicited line
                                                     // Callback must not call send_cmd, only set flags

void esp32_at_debug_stress_test(uint32_t stall_ms);


#endif  