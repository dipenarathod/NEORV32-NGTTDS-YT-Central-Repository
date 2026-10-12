
// esp32_at_port.h - hardware layer (NEORV32 UART1, RX interrupt, delay).
// This is the only layer that touches NEORV32 peripherals.

#ifndef ESP32_AT_PORT_H
#define ESP32_AT_PORT_H

#include <stdint.h>
#include <stddef.h>

void esp32_at_port_init(uint32_t baudrate);            // Ring buffer + UART1 + RX interrupt

void esp32_at_tx(const char *s); // Transmit string over UART1, blocks while TX FIFO is full
void esp32_at_tx_bytes(const char *data, size_t len);  // Send raw bytes

int esp32_at_read_char(char *c, uint32_t timeout_ms); // Take one byte from the RX ring buffer, waiting up to timeout_ms (1 ms resolution)
// 0 = got a byte, -1 = timeout (0 ms = do not wait)

void esp32_at_flush_rx(void); // Discard all bytes in the ring buffer

uint16_t esp32_at_rx_overflow(void); // Helper to get the number of overflow bytes

void esp32_at_delay_ms(uint32_t ms); // Blocking delay

#endif