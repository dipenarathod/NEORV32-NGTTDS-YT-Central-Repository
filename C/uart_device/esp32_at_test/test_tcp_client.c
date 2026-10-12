// Test 3: TCP client with unsolicited lines. On the PC run: nc -lvp 5000
// Type text in the nc window -> it is printed here. Close nc -> the board reconnects.
#include <neorv32.h>
#include <stdlib.h>
#include <string.h>
#include "esp32_at.h"
#include "test_common.h"
#include "test_config.h"

static int  ipd_len    = 0;
static bool tcp_closed = false;
static bool wifi_lost  = false;

// Only set flags here, never call send_cmd from the callback
// To listen on Ubuntu, run:
// nc -lvp 5000 (replace 5000 with PC_TCP_PORT from test_config.h)

static void on_line(const char *line)
{
    neorv32_uart0_printf("[URC] %s\n", line);
    if (strncmp(line, "+IPD,", 5) == 0)            ipd_len = atoi(line + 5);
    else if (strcmp(line, "CLOSED") == 0)          tcp_closed = true;
    else if (strcmp(line, "WIFI DISCONNECT") == 0) wifi_lost = true;
}

static esp32_at_status_t tcp_connect(void)
{
    char *c = esp32_at_cmd_begin();
    strcpy(c, "AT+CIPSTART=\"TCP\",");
    append_quoted_str(c, PC_IP);
    append_comma(c);
    append_int32(c, PC_TCP_PORT);
    return esp32_at_send_cmd(c, NULL, 0, 10000);
}

int main(void)
{
    char resp[1024];

    if (test_setup() != 0) return 1;
    if (test_wifi_join() != 0) return 1;

    esp32_at_set_urc_handler(on_line);
    esp32_at_send_cmd("AT+CIPRECVMODE=1", NULL, 0, 2000); // Before CIPSTART

    esp32_at_status_t st = tcp_connect();
    neorv32_uart0_printf("CIPSTART: %s\n", esp32_at_status_str(st));

    if (st == ESP32_AT_OK) {
        if (esp32_at_send_cmd("AT+CIPSEND=5", NULL, 0, 2000) == ESP32_AT_OK) {
            st = esp32_at_send_data("hello", 5, 5000);
            neorv32_uart0_printf("send_data: %s\n", esp32_at_status_str(st));
        }
    }

    while (1) {
        esp32_at_poll();

        if (ipd_len > 0) {
            int n = ipd_len;
            ipd_len = 0;
            char *c = esp32_at_cmd_begin();
            strcpy(c, "AT+CIPRECVDATA=");
            append_int32(c, n);
            if (esp32_at_send_cmd(c, resp, sizeof(resp), 2000) == ESP32_AT_OK) {
                neorv32_uart0_puts(resp);
            }
        }
        if (wifi_lost) {
            wifi_lost = false;
            neorv32_uart0_puts("Wi-Fi lost, waiting for the module to reconnect\n");
        }
        if (tcp_closed) {
            tcp_closed = false;
            st = tcp_connect();
            neorv32_uart0_printf("reconnect: %s\n", esp32_at_status_str(st));
            if (st != ESP32_AT_OK) {
                tcp_closed = true; // Try again after a pause
                esp32_at_delay_ms(2000);
            }
        }
    }
}