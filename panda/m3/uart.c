#include "uart.h"

#include "util.h"

#define SYSC_SOFTRESET (1 << 1)
#define SYSS_RESETDONE (1 << 0)

#define LCR_DIV_EN (1 << 7)
#define LCR_CHAR_LENGTH_MASK 0x03

#define UART_BASE_ADDR 0x48020000


#define UART_DLL (UART_BASE_ADDR) // Baud rate divider low byte
#define UART_DLH (UART_BASE_ADDR + 0x04) // Baud rate divider high byte
#define UART_MDR1 (UART_BASE_ADDR + 0x20) // Mode definition register 1
#define UART_LSR (UART_BASE_ADDR + 0x14) // Line status register
#define UART_SYSC (UART_BASE_ADDR + 0x54) // System configuration register
#define UART_SYSS (UART_BASE_ADDR + 0x58) // System status register
#define UART_RHR_THR (UART_BASE_ADDR) // receive/transmit
#define UART_LCR (UART_BASE_ADDR + 0x0C) 

#define LSR_TX_SR_E (1 << 6)
#define LSR_RX_FIFO_E (1 << 0)

void uart_init(void)
{
	// Perform a reset of the uart module
	REG8(UART_SYSC) = SYSC_SOFTRESET;
	while (!(REG8(UART_SYSS) & SYSS_RESETDONE));

	// Allow access to the DLL and DLH registers (config mode A)
	REG8(UART_LCR) |= LCR_DIV_EN;

	// Set baud rate to 115200
	REG8(UART_DLH) = 0x00;
	REG8(UART_DLL) = 0x1A;

	// Leave config mode A and set frame format (8n1)
	REG8(UART_LCR) = 0x03;

	// Enable uart
	REG8(UART_MDR1) = 0x00;
}

void uart_putc(char c)
{
	REG8(UART_RHR_THR) = c;
	while (!(REG8(UART_LSR) & LSR_TX_SR_E));
}

char uart_getc(void)
{
	while (!(REG8(UART_LSR) & LSR_RX_FIFO_E));
	return REG8(UART_RHR_THR);
}

void uart_puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			uart_putc('\r');

		uart_putc(*s);

		s++;
	}
}

void uart_puthex(unsigned int num)
{
	unsigned int mask = 0xF0000000;
	unsigned int shift = 28;

	while (mask) {
		unsigned int tmp = (num & mask) >> shift;

		if (tmp < 10)
			uart_putc(tmp + '0');
		else
			uart_putc(tmp - 10 + 'a');

		mask >>= 4;
		shift -= 4;
	}
}
