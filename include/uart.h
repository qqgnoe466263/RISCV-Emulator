#ifndef _UART_H_
#define UART0__UART_H_

#include "mem.h"

#define UART0_RHR     (VIRT_UART0 + 0) // Receive Holding Register (for input bytes)
#define UART0_THR     (VIRT_UART0 + 0) // Transmit Holding Register (for output bytes)
#define UART0_IER     (VIRT_UART0 + 1) // Interrupt Enable Register
#define UART0_FCR     (VIRT_UART0 + 2) // FIFO Control Register
#define UART0_ISR     (VIRT_UART0 + 2) // Interrupt Status Register
#define UART0_LCR     (VIRT_UART0 + 3) // Line Control Register

/* LSR (Line Status Register) 
 * (Ref : https://www.lookrs232.com/rs232/lsr.html)
 * Bit 0 (when set) shows data ready, which means that a byte has been received by 
 *       the UART and is at the receiver holding register(RHR) ready to be read.
 * Bit 5 (when set) only shows that transmitter holding register(THR) is empty.
 * Bit 6 (when set) signals that transmitter holding register and the shift register are empty.
 */
#define UART0_LSR               (VIRT_UART0 + 5) // Line Status Register
#define UART0_LSR_RHR_EMPTY     (1 << 0)
#define UART0_LSR_THR_EMPTY     (1 << 5)
#define UART0_LSR_THR_SR_EMPTY  (1 << 6) 

#endif
