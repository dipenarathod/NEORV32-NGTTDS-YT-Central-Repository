#include "aht21b.h"
#include "../i2c_helper/i2c_helper.h"
#include <neorv32.h>


int aht21b_probe(void)
{
  return i2c_ping(AHT21B_I2C_ADDR);
}

int aht21b_soft_reset(void)
{
  uint8_t tx[1];

  // Build the 1-byte soft-reset command.
  tx[0] = AHT21B_CMD_SOFTRESET;

  // Send the command to the sensor.
  if (i2c_write(AHT21B_I2C_ADDR, tx, sizeof(tx)) != 0)
  {
    return -1;
  }

  // Give the sensor time to complete the internal reset.
  neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 25);

  return 0;
}

int aht21b_read_status(uint8_t *status) {
    uint8_t rx[1];

    if (status == 0) {
        return -1;
    }

    if (i2c_read(AHT21B_I2C_ADDR, rx, sizeof(rx)) != 0) {
        return -2;
    }

    *status = rx[0];
    return 0;
}

int aht21b_init(void)
{
  uint8_t tx[3];
  uint8_t status = 0;

  // Reset the sensor first.
  if (aht21b_soft_reset() != 0)
  {
    return -1;
  }

  // Build and send the initialization command sequence.
  tx[0] = AHT21B_CMD_INIT;
  tx[1] = AHT21B_INIT_ARG1;
  tx[2] = AHT21B_INIT_ARG2;

  if (i2c_write(AHT21B_I2C_ADDR, tx, sizeof(tx)) != 0)
  {
    return -2;
  }

  // Wait for the initialization sequence to settle.
  neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 10);

  // Read back the status register.
  if (aht21b_read_status(&status) != 0)
  {
    return -3;
  }

  // Verify that the calibration bit is set.
  if ((status & AHT21B_STATUS_CALIBRATED) == 0)
  {
    return -4;
  }

  return 0;
}

int aht21b_measure(aht21b_data_t *data)
{
  uint8_t tx[3];
  uint8_t rx[6];
  uint8_t status;
  int i;

  // Validate destination structure pointer.
  if (data == 0)
  {
    return -1;
  }

  // Build and send the measurement-trigger command.
  tx[0] = AHT21B_CMD_TRIGGER;
  tx[1] = AHT21B_TRIGGER_ARG1;
  tx[2] = AHT21B_TRIGGER_ARG2;

  if (i2c_write(AHT21B_I2C_ADDR, tx, sizeof(tx)) != 0)
  {
    return -2;
  }

  // Wait long enough for the conversion to begin.
  neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 80);

  // Poll the status register until the busy bit clears or timeout occurs.
  for (i = 0; i < 40; i++)
  {
    if (aht21b_read_status(&status) != 0)
    {
      return -3;
    }

    if ((status & AHT21B_STATUS_BUSY) == 0)
    {
      break;
    }

    neorv32_aux_delay_ms(neorv32_sysinfo_get_clk(), 5);
  }

  // If the sensor is still busy after polling, report timeout.
  if (status & AHT21B_STATUS_BUSY)
  {
    return -4;
  }

  // Read the 6-byte measurement frame:
  // status, humidity[19:12], humidity[11:4], humidity[3:0]+temp[19:16], temp[15:8], temp[7:0]
  if (i2c_read(AHT21B_I2C_ADDR, rx, 6) != 0)
  {
    return -5;
  }

  // Store raw status byte.
  data->status = rx[0];

  // Reconstruct the 20-bit raw humidity value.
  data->raw_humidity =
      ((uint32_t)rx[1] << 12) |
      ((uint32_t)rx[2] << 4) |
      ((uint32_t)(rx[3] >> 4) & 0x0F);

  // Reconstruct the 20-bit raw temperature value.
  data->raw_temperature =
      (((uint32_t)rx[3] & 0x0F) << 16) |
      ((uint32_t)rx[4] << 8) |
      ((uint32_t)rx[5]);

  // Convert raw humidity to %RH.
  data->humidity_rh =
      ((float)data->raw_humidity * 100.0f) / 1048576.0f;

  // Convert raw temperature to degrees Celsius.
  data->temperature_c =
      ((float)data->raw_temperature * 200.0f) / 1048576.0f - 50.0f;

  return 0;
}