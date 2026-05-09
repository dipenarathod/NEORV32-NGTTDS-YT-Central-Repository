#ifndef I2C_HELPER_H
#define I2C_HELPER_H

#include <stddef.h>
#include <stdint.h>


int i2c_available(void); //Check if TWI is available
int i2c_bus_init(int prsc, int cdiv, int clk_stretch);  //Same signature as neorv32_twi_setup()

int i2c_get_fifo_depth(void);//Get I2C FIFO depth

int i2c_disable(void); //Disable i2c
int i2c_enable(void);   //Enable i2c

int i2c_get_scl_status(void);//Get i2c SCL status
int i2c_get_sda_status(void);//Get i2c SDA status
int i2c_busy_status(void);//Check if i2c is busy or not

int i2c_get_data(uint8_t *data);//Get data + ACK/NACH from RX FIFO

int i2c_discard_oldest_received_data();//Discard oldest data from RX FIFO if avaialble

int i2c_generate_start(void); //Generate start condition for I2C transaction
int i2c_generate_stop(void);//Generate stop condition for I2C transaction

int i2c_generate_start_non_blocking(void);//Repeated start condition
int i2c_generate_stop_non_blocking(void);//Generate non-blocking stop

 /**********************************************************************//**
 * Send data byte (RX can be read via neorv32_twi_get()).
 *
 * @note Non-blocking function; does not check the TX FIFO.
 *
 * @param[in] data Data byte to be send.
 * @param[in] mack Generate ACK by host controller when set.
 **************************************************************************/
int i2c_write_non_blocking_byte(uint8_t data, int mack); 

int i2c_write_byte(uint8_t *data, int mack); //Basic primitive to send one byte

int i2c_write(uint8_t addr7, const uint8_t *tx, size_t tx_len); //Write multiple bytes to i2c peripheral

int i2c_read(uint8_t addr7, uint8_t *rx, size_t rx_len); //Read multiple bytes from i2c peripheral

int i2c_ping(uint8_t addr7); //Check if an address exists

int i2c_write_read(uint8_t addr7,
                   const uint8_t *tx, size_t tx_len,
                   uint8_t *rx, size_t rx_len); //Write bytes, then repeated-start and read bytes from same device

#endif