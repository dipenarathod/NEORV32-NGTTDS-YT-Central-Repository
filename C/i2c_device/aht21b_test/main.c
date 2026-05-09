//Used ChatGPT's help

#include "neorv32.h"
#include <stdint.h>
#include "aht21b.h"
#include "../i2c_helper/i2c_helper.h"

#define BAUD_RATE 19200

static void print_fixed_2(float x)
{
  int32_t scaled = (int32_t)(x * 100.0f);
  int32_t abs_scaled = (scaled < 0) ? -scaled : scaled;
  uint32_t ip = (uint32_t)(abs_scaled / 100);
  uint32_t fp = (uint32_t)(abs_scaled % 100);

  if (scaled < 0)
  {
    neorv32_uart0_putc('-');
  }

  neorv32_uart0_printf("%u.", ip);
  if (fp < 10)
  {
    neorv32_uart0_putc('0');
  }
  neorv32_uart0_printf("%u", fp);
}

int main(void)
{
  aht21b_data_t data;
  int rc;

  // Install the runtime exception handlers.
  neorv32_rte_setup();

  // UART is required for test output.
  if (neorv32_uart0_available() == 0)
  {
    return 1;
  }

  // Initialize UART0.
  neorv32_uart0_setup(BAUD_RATE, 0);

  neorv32_uart0_printf("\n--- AHT21B Test ---\n");

  // Check whether the TWI/I2C controller exists.
  if (i2c_available() == 0)
  {
    neorv32_uart0_printf("ERROR: TWI controller not available.\n");
    return 1;
  }

  // Configure the shared I2C bus.
  i2c_bus_init(CLK_PRSC_2048, 15, 1);

  // Probe the AHT21B address.
  rc = aht21b_probe();
  if (rc != 0)
  {
    neorv32_uart0_printf("AHT21B probe failed: %i\n", rc);
    return 1;
  }
  neorv32_uart0_printf("AHT21B probe OK\n");

  uint8_t status;
  rc = aht21b_read_status(&status);
  if (rc == 0)
  {
    neorv32_uart0_printf("Initial status: 0x%x\n", status);
  }
  else
  {
    neorv32_uart0_printf("Initial status read failed: %i\n", rc);
  }

  // Initialize the sensor.
  rc = aht21b_init();
  if (rc != 0)
  {
    neorv32_uart0_printf("AHT21B init failed: %i\n", rc);
    return 1;
  }

  neorv32_uart0_printf("AHT21B init OK\n");

  // Periodically trigger and print measurements.
  while (1)
  {
    rc = aht21b_measure(&data);
    if (rc != 0)
    {
      neorv32_uart0_printf("Measurement failed: %i\n", rc);
    }
    else
    {
      neorv32_uart0_printf("Status 0x%x | T = ", data.status);
      print_fixed_2(data.temperature_c);
      neorv32_uart0_printf(" C | RH = ");
      print_fixed_2(data.humidity_rh);
      neorv32_uart0_printf(" %%\n");
    }

    neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 1000);
  }

  return 0;
}