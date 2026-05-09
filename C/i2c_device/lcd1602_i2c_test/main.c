#include "neorv32.h"
#include <stdint.h>
#include "lcd1602_i2c.h"
#include "../i2c_helper/i2c_helper.h"

#define BAUD_RATE 19200

int main(void)
{
  int rc;
  uint32_t counter = 0;

  neorv32_rte_setup();

  if (neorv32_uart0_available() == 0)
  {
    return 1;
  }

  neorv32_uart0_setup(BAUD_RATE, 0);

  neorv32_uart0_printf("\n--- LCD1602 I2C Test ---\n");

  if (i2c_available() == 0)
  {
    neorv32_uart0_printf("ERROR: TWI controller not available.\n");
    return 1;
  }

  i2c_bus_init(CLK_PRSC_2048, 15, 1);

  neorv32_uart0_printf("SCL idle: %u\n", (i2c_get_scl_status() != 0) ? 1 : 0);
  neorv32_uart0_printf("SDA idle: %u\n", (i2c_get_sda_status() != 0) ? 1 : 0);

  rc = lcd_init();
  neorv32_uart0_printf("lcd_init rc: %i\n", rc);

  if (rc != 0)
  {
    neorv32_uart0_printf("LCD init failed.\n");
    neorv32_uart0_printf("Check LCD address, contrast pot, and wiring.\n");
    return 1;
  }

  /* Backlight test */
  neorv32_uart0_printf("Testing backlight OFF for 2 seconds...\n");
  rc = lcd_backlight_off();
  neorv32_uart0_printf("lcd_backlight_off rc: %i\n", rc);
  neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 2000);

  neorv32_uart0_printf("Testing backlight ON for 2 seconds...\n");
  rc = lcd_backlight_on();
  neorv32_uart0_printf("lcd_backlight_on rc: %i\n", rc);
  neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 2000);

  lcd_clear();

  lcd_set_cursor(0, 0);
  lcd_write_string("NEORV32 LCD");

  lcd_set_cursor(0, 1);
  lcd_write_string("Hello!");

  while (1)
  {
    char line[17];

    counter++;

    /* Optional periodic backlight test */
    if ((counter % 10) == 0)
    {
      lcd_backlight_off();
      neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 300);
      lcd_backlight_on();
    }

    /* Show a simple counter on line 2 */
    line[0]  = 'C';
    line[1]  = 'n';
    line[2]  = 't';
    line[3]  = ':';
    line[4]  = ' ';
    line[5]  = (char)('0' + ((counter / 10000) % 10));
    line[6]  = (char)('0' + ((counter / 1000) % 10));
    line[7]  = (char)('0' + ((counter / 100) % 10));
    line[8]  = (char)('0' + ((counter / 10) % 10));
    line[9]  = (char)('0' + (counter % 10));
    line[10] = ' ';
    line[11] = ' ';
    line[12] = ' ';
    line[13] = ' ';
    line[14] = ' ';
    line[15] = ' ';
    line[16] = '\0';

    lcd_set_cursor(0, 1);
    lcd_write_string(line);

    neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 1000);
  }

  return 0;
}