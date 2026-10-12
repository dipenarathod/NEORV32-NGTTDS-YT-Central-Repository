// uart_ring_buffer.h
#ifndef UART_RING_BUFFER_H
#define UART_RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char *buffer;               // Pointer to the data array
    uint16_t mask;              // Capacity minus one (capacity must be a power of two, max 32768)
    volatile uint16_t head;     // Write index, only modified by the producer (ISR)
    volatile uint16_t tail;     // Read index, only modified by the consumer (main code)
    volatile uint16_t overflow; // Bytes dropped because the buffer was full, only modified by the ISR
} uart_ring_buffer;

void rb_init(uart_ring_buffer *rb, char *buffer_array, uint16_t capacity);
bool rb_is_full(const uart_ring_buffer *rb);
bool rb_is_empty(const uart_ring_buffer *rb);
bool rb_enqueue(uart_ring_buffer *rb, char data);
bool rb_dequeue(uart_ring_buffer *rb, char *out_data);
#endif