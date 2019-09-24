#ifndef UART_H_
#define UART_H_

void uart_init(void);
void uart_putc(char c);
char uart_getc(void);
void uart_puts(const char *s);
void uart_puthex(unsigned int num);

#endif
