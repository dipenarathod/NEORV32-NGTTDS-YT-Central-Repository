#include <neorv32.h>
#include <string.h>
#include "esp32_at.h"
#include "test_common.h"
#include "test_config.h"

int test_setup(void)
{
    neorv32_rte_setup();
    neorv32_uart0_setup(19200, 0);
    neorv32_uart0_puts("\n--- NEORV32 ESP32 AT test ---\n");

    if (!neorv32_uart1_available()) {
        neorv32_uart0_puts("[ERROR] UART1 is not enabled in RTL synthesis!\n");
        return -1;
    }

    esp32_at_status_t st = esp32_at_init(115200);
    neorv32_uart0_printf("esp32_at_init: %s\n", esp32_at_status_str(st));
    return (st == ESP32_AT_OK) ? 0 : -1;
}

int test_wifi_join(void)
{
    esp32_at_status_t st = esp32_at_send_cmd("AT+CWMODE=1", NULL, 0, 2000);
    if (st != ESP32_AT_OK) {
        neorv32_uart0_printf("CWMODE: %s\n", esp32_at_status_str(st));
        return -1;
    }

    char *c = esp32_at_cmd_begin();
    strcpy(c, "AT+CWJAP=");
    append_quoted_str(c, WIFI_SSID);
    append_comma(c);
    append_quoted_str(c, WIFI_PASS);
    // Command becomesn: AT+CWJAP="SSID","PASSWORD".
    // You can add parameters using the command builder functions and by referring to the ESP32 AT firmware
    st = esp32_at_send_cmd(c, NULL, 0, 20000);
    neorv32_uart0_printf("CWJAP: %s\n", esp32_at_status_str(st));
    return (st == ESP32_AT_OK) ? 0 : -1;
}