// Test 1: UART link, init and AT+GMR
#include <neorv32.h>
#include "esp32_at.h"
#include "test_common.h"

int main(void)
{
    char resp[256];

    if (test_setup() != 0) return 1;

    esp32_at_status_t st = esp32_at_send_cmd("AT+GMR", resp, sizeof(resp), 2000);
    neorv32_uart0_printf("AT+GMR: %s\n", esp32_at_status_str(st));
    neorv32_uart0_puts(resp);
    return 0;
}