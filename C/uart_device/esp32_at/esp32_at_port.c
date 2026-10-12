// esp32_at_port.c - hardware layer (NEORV32 UART1, RX interrupt, delay)
#include "esp32_at_port.h"
#include "uart_ring_buffer.h"
#include <neorv32.h>

// Ring buffer for RX data
#define RX_BUFFER_CAPACITY 512  // Must be power of two

static char rx_char_buffer[RX_BUFFER_CAPACITY];
static uart_ring_buffer rx_ring_buffer;


// UART1 RX interrupt service routine: read all available characters from the 
// FIFO and store them in the ring buffer
static void esp32_at_rx_isr(void)
{ 
    char c;
    while (neorv32_uart1_char_received()) {
        c = (char)neorv32_uart1_char_received_get();
        (void)rb_enqueue(&rx_ring_buffer, c);
    }
}

void esp32_at_port_init(uint32_t baudrate)
{
    // Initialize the ring buffer for UART1 RX data
    // Set up before because once UART1 is enabled, the RX ISR will start filling the ring buffer
    // which is problematic at boot if the ring buffer is not initialized yet
    rb_init(&rx_ring_buffer, rx_char_buffer, RX_BUFFER_CAPACITY);
 
    // Enable UART1 hardware module and set the RX FIFO Not empty IRQ mask
    neorv32_uart1_setup(baudrate, 1 << UART_CTRL_IRQ_RX_NEMPTY);
    neorv32_uart1_enable();
 
    // Register your custom ISR in the RTE vector table
    neorv32_rte_handler_install(UART1_TRAP_CODE, esp32_at_rx_isr);

    // Enable hardware-level interrupt routing
    neorv32_cpu_csr_set(CSR_MIE, 1 << UART1_FIRQ_ENABLE);
    neorv32_cpu_csr_set(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE);
 
    // Flush RX buffer
    esp32_at_flush_rx();
}

// Blocking delay
void esp32_at_delay_ms(uint32_t ms)
{
    static uint32_t sys_clk = 0;
    if (sys_clk == 0) {
        sys_clk = neorv32_sysinfo_get_clk(); // Read once, the clock does not change at runtime
    }
    neorv32_aux_delay_ms(sys_clk, ms);
}

// Put character on UART TX
// Only for null-terminated strings.
void esp32_at_tx(const char *s)
{
    while (*s) {
        neorv32_uart1_putc(*s++);   // blocks while the TX FIFO is full
    }
}

// Put bytes on UART TX
// Does not require null-terminated strings. The length is specified by len.
void esp32_at_tx_bytes(const char *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        neorv32_uart1_putc(data[i]);
    }
}

// Take one byte from the RX ring buffer, waiting up to timeout_ms (1 ms resolution)
int esp32_at_read_char(char *c, uint32_t timeout_ms)
{ 
    uint32_t waited = 0;
    while (1) {
        if (!rb_is_empty(&rx_ring_buffer)) {
            rb_dequeue(&rx_ring_buffer, c); // Ring buffer has an element. Get it.
            return 0;
        }
        if (waited >= timeout_ms) {
            return -1; // Nothing arrived within the timeout
        }
        esp32_at_delay_ms(1); // Sleeping for 1 second. It is safe. The ISR keeps filling the ring buffer
        waited++;
    }
}

// Empty the NEORV32 UART1 receive FIFO buffer
void esp32_at_flush_rx(void)
{ 
    static char dummy;
    while (!rb_is_empty(&rx_ring_buffer)) {
        (void)rb_dequeue(&rx_ring_buffer, &dummy); // Discard all bytes in the ring buffer
    }
}

// Helper to get the number of overflow bytes
uint16_t esp32_at_rx_overflow(void)
{
    return rx_ring_buffer.overflow;
}