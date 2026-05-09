#ifndef AHT21B_H
#define AHT21B_H

#include <stdint.h>

#define AHT21B_I2C_ADDR            0x38

#define AHT21B_CMD_INIT            0xBE
#define AHT21B_CMD_TRIGGER         0xAC
#define AHT21B_CMD_SOFTRESET       0xBA

#define AHT21B_INIT_ARG1           0x08
#define AHT21B_INIT_ARG2           0x00
#define AHT21B_TRIGGER_ARG1        0x33
#define AHT21B_TRIGGER_ARG2        0x00

#define AHT21B_STATUS_BUSY         0x80
#define AHT21B_STATUS_CALIBRATED   0x08

typedef struct {
  uint8_t status;
  uint32_t raw_humidity;
  uint32_t raw_temperature;
  float humidity_rh;
  float temperature_c;
} aht21b_data_t;

int aht21b_probe(void);
int aht21b_soft_reset(void);
int aht21b_init(void);
int aht21b_read_status(uint8_t *status);
int aht21b_measure(aht21b_data_t *data);

#endif