// Test 4: HTTP GET with the ESP-AT HTTP commands (AT+HTTPCGET).
// The module opens the connection, sends the request and returns only the body.
// On the PC run: python3 -m http.server 8000
// (The same test done by hand over TCP is in test_http_get_tcp.c)
#include <neorv32.h>
#include <string.h>
#include "esp32_at.h"
#include "test_common.h"
#include "test_config.h"

// Reply format: "+HTTPCGET:<size>,<body>" followed by OK. It is copied to resp (cut off if resp is too small).
static esp32_at_status_t http_get(const char *host, uint16_t port, const char *path,
                                  char *resp, size_t resp_size)
{
    char url[128];
    strcpy(url, "http://");
    append_str(url, host);
    append_str(url, ":");
    append_int32(url, port);
    append_str(url, path);

    // AT+HTTPCGET="url"  (keep the whole command under 256 bytes, longer URLs need AT+HTTPURLCFG)
    char *c = esp32_at_cmd_begin();
    strcpy(c, "AT+HTTPCGET=");
    append_quoted_str(c, url);
    return esp32_at_send_cmd(c, resp, resp_size, 10000);
}

int main(void)
{
    static char resp[1024];

    if (test_setup() != 0) return 1;
    if (test_wifi_join() != 0) return 1;

    uint16_t dropped_before = esp32_at_rx_overflow();

    esp32_at_status_t st = http_get(PC_IP, PC_HTTP_PORT, "/", resp, sizeof(resp));
    neorv32_uart0_printf("AT+HTTPCGET /: %s\n", esp32_at_status_str(st));
    neorv32_uart0_puts(resp);

    // A page that does not exist: the module reports the failed request as an error
    st = http_get(PC_IP, PC_HTTP_PORT, "/does-not-exist", resp, sizeof(resp));
    neorv32_uart0_printf("\nAT+HTTPCGET /does-not-exist: %s\n", esp32_at_status_str(st));

    neorv32_uart0_printf("Bytes dropped by the ring buffer: %u\n",
                         (unsigned)(uint16_t)(esp32_at_rx_overflow() - dropped_before));
    return 0;
}