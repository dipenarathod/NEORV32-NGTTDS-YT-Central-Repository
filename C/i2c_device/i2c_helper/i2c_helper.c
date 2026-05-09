#include "i2c_helper.h"
#include <neorv32.h>

int i2c_available(void)
{ // Check if TWI is available
    return neorv32_twi_available();
}

int i2c_bus_init(int prsc, int cdiv, int clk_stretch)
{ // Same signature as neorv32_twi_setup()
    neorv32_twi_setup(prsc, cdiv, clk_stretch ? 1 : 0);
    return 0;
}

int i2c_get_fifo_depth(void)
{ // Get I2C FIFO depth
    return neorv32_twi_get_fifo_depth();
}

int i2c_disable(void)
{ // Disable i2c
    neorv32_twi_disable();
    return 0;
}

int i2c_enable(void)
{ // Enable i2c
    neorv32_twi_enable();
    return 0;
}

int i2c_get_scl_status(void)
{ // Get i2c SCL status
    return neorv32_twi_sense_scl();
}

int i2c_get_sda_status(void)
{ // Get i2c SDA status
    return neorv32_twi_sense_sda();
}

int i2c_busy_status(void)
{ // Check if i2c is busy
    return neorv32_twi_busy();
}

int i2c_get_data(uint8_t *data)
{ // Get data + ACK/NACH from RX FIFO
    return neorv32_twi_get(data);
}

int i2c_discard_oldest_received_data()
{ // Discard oldest data from RX FIFO if avaialble
    neorv32_twi_get_discard();
    return 0;
}

int i2c_generate_start(void)
{ // Generate start for I2C transactions
    neorv32_twi_generate_start();
    return 0;
}

int i2c_generate_stop(void)
{ // Generate stop condition for I2C transaction
    neorv32_twi_generate_stop();
    return 0;
}

int i2c_generate_start_non_blocking(void)
{ // Repeated start condition
    neorv32_twi_generate_start_nonblocking();
    return 0;
}

int i2c_generate_stop_non_blocking(void)
{ // Generate non-blocking stop
    neorv32_twi_generate_stop_nonblocking();
    return 0;
}

int i2c_write_byte(uint8_t *data, int mack)
{ // Basic primitive to send one byte
    return neorv32_twi_transfer(data, mack);
}

int i2c_write_non_blocking_byte(uint8_t data, int mack)
{ // Send non-blocking data byte (RX can be read via neorv32_twi_get()).
    neorv32_twi_send_nonblocking(data, mack);
    return 0;
}

int i2c_write(uint8_t addr7, const uint8_t *tx, size_t tx_len)
{ // Write multiple bytes to i2c peripheral
    size_t i;
    uint8_t data_byte;
    if ((tx == NULL) && (tx_len != 0))
    {
        return -1;
    }

    // 1. Generate start condition
    i2c_generate_start();

    // 2. Send the 7-bit address + the Write bit (0)
    uint8_t address_byte = (addr7 << 1) | 0;

    // 3. Transmit the address and wait for the Acknowledge bit
    if (i2c_write_byte(&address_byte, 0))
    {
        i2c_generate_stop();
        return -2;
    }

    // 4. Transmit all bytes
    for (i = 0; i < tx_len; i++)
    {
        data_byte = tx[i];
        if (i2c_write_byte(&data_byte, 0))
        {
            i2c_generate_stop();
            return -3;
        }
    }

    i2c_generate_stop();
    return 0;
}

int i2c_read(uint8_t addr7, uint8_t *rx, size_t rx_len)
{ // Read mutliple bytes from i2c peripheral
    size_t i;
    uint8_t data_byte;
    uint8_t address_byte;
    if ((rx == NULL) && (rx_len != 0))
    {
        return -1;
    }

    // 1. Generate start condition
    i2c_generate_start();

    // 2. Send the 7-bit address + the Write bit (0)
    address_byte = (uint8_t)((addr7 << 1) | 1u);

    // 3. Transmit the byte and wait for the Acknowledge bit
    if (i2c_write_byte(&address_byte, 0) != 0)
    {
        i2c_generate_stop();
        return -2;
    }

    // 4. Read bytes
    for (i = 0; i < rx_len; i++)
    {
        data_byte = 0xFF;
        (void)i2c_write_byte(&data_byte, (i + 1 < rx_len) ? 1 : 0);
        rx[i] = data_byte;
    }

    // 5. Generate stop condition
    i2c_generate_stop();

    return 0;
}

int i2c_write_read(uint8_t addr7,
                   const uint8_t *tx, size_t tx_len,
                   uint8_t *rx, size_t rx_len)
{ // Write bytes, then repeated-start and read bytes from same device

    size_t i;
    uint8_t data_byte;
    uint8_t address_byte;
    if (((tx == NULL) && (tx_len != 0)) ||
        ((rx == NULL) && (rx_len != 0)))
    {
        return -1;
    }

    // 1. Generate start conditions
    i2c_generate_start();

    // 2. Send the 7-bit address + the write bit (0) for acknowledgement
    address_byte = (addr7 << 1) | 0u;

    // 3. Transmit the byte and wait for the Acknowledge bit
    if (i2c_write_byte(&address_byte, 0) != 0)
    {
        i2c_generate_stop();
        return -2;
    }

    // 4. Transmit all bytes
    for (i = 0; i < tx_len; i++)
    {
        data_byte = tx[i];
        if (i2c_write_byte(&data_byte, 0) !=0)
        {
            i2c_generate_stop();
            return -3;
        }
    }

    // 5. Generate start conditions
    i2c_generate_start();

    // 6. Send the 7-bit address + the read bit (1)
    address_byte = (addr7 << 1) | 1u;

    // 7. Transmit the byte and wait for the Acknowledge bit
    if (i2c_write_byte(&address_byte, 0) != 0)
    {
        i2c_generate_stop();
        return -4;
    }

    // 8. Read bytes
    for (i = 0; i < rx_len; i++)
    {
        data_byte = 0xFF;
        (void)i2c_write_byte(&data_byte, (i + 1 < rx_len) ? 1 : 0);
        rx[i] = data_byte;
    }

    // 9. Generate stop condition
    i2c_generate_stop();

    return 0;
}

int i2c_ping(uint8_t addr7)
{ // Check if an address exists
    // 1. Generate start condition
    i2c_generate_start();

    // 2. Send the 7-bit address + the write bit (0)
    uint8_t address_byte = (addr7 << 1) | 0;

    // 3. Transmit the byte and wait for the Acknowledge bit
    if (i2c_write_byte(&address_byte, 0))
    {
        i2c_generate_stop();
        return -1;
    }
    i2c_generate_stop();
    return 0;
}
