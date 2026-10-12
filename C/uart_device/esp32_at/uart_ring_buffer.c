// uart_ring_buffer.c
#include "uart_ring_buffer.h"

void rb_init(uart_ring_buffer *rb, char *buffer_array, uint16_t capacity)
{ // Capacity must be a power of two so indices can wrap with a mask instead of a division
    rb->buffer = buffer_array;
    rb->mask = capacity - 1;
    rb->head = 0;
    rb->tail = 0;
    rb->overflow = 0;
}

bool rb_is_full(const uart_ring_buffer *rb)
{ // Number of stored bytes (head - tail, wraps correctly) has reached the capacity
    return (uint16_t)(rb->head - rb->tail) > rb->mask;
}

bool rb_is_empty(const uart_ring_buffer *rb)
{ // Nothing to read when both indices are equal
    return rb->head == rb->tail;
}

bool rb_enqueue(uart_ring_buffer *rb, char data)
{ // Producer side (ISR): drop the byte and count it if the buffer is full
    if (rb_is_full(rb)) {
        rb->overflow++;
        return false;
    }
    rb->buffer[rb->head & rb->mask] = data;
    rb->head++; // Publish the byte only after it is stored
    return true;
}

bool rb_dequeue(uart_ring_buffer *rb, char *out_data)
{ // Consumer side (main code)
    if (rb_is_empty(rb)) {
        return false;
    }
    *out_data = rb->buffer[rb->tail & rb->mask];
    __asm__ volatile ("" ::: "memory"); // Finish reading the slot before handing it back to the ISR
    /* 
        It tells the GCC/Clang compiler: "Do not reorder any memory reads or writes across this line."
        Without the memory barrier, a compiler optimizing at -O2 or -O3 might look 
        at the code and decide it can reorder, split, or cache those operations differently. 
        Specifically, it prevents the compiler from moving the update to rb->tail 
        before the actual data has been read out of the buffer array.
    */
    // Never came across this online, so this was fun.
    rb->tail++;
    return true;
}