#include "command.h"

#include <usart.h>
#include <stm32.h>
#include <usb/usb_cdcacm.h>
#include <gpio.h>
#include <timer.h>
#include <nvic.h>
#include <string.h>

#include "debug.h"

#define CMD_UART USART1

static uint8_t cmd_buf[256];
static int cmd_buf_i = 0;
static int cmd_buf_left;
static uint8_t cmd_buf_len;

static int connected = 0;

static cmd_func_t cmd_map[128];

void test_cmd(const uint8_t *buf, uint8_t length)
{
	command_send('t', buf, length);
}

void connect_cmd(const uint8_t *buf, uint8_t length)
{
	connected = 1;
	command_send('c', NULL, 0);
	debug_printf("connected\n");
}

void disconnect_cmd(const uint8_t *buf, uint8_t length)
{
	connected = 0;
	command_send('d', NULL, 0);
	debug_printf("disconnected\n");
}

void command_init(void)
{
	gpio_set_mode(GPIOA, 9, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(GPIOA, 10, GPIO_INPUT_FLOATING);
	
	usart_init(CMD_UART);
	usart_set_baud_rate(CMD_UART, STM32_PCLK2, 115200);
	nvic_irq_set_priority(NVIC_USART1, 0xD);
	usart_enable(CMD_UART);

	memset(cmd_map, 0, sizeof(cmd_map));
	
	command_register('t', test_cmd);
	command_register('c', connect_cmd);
	command_register('d', disconnect_cmd);

	cmd_buf_i = -1;
}

void command_btconfig(void)
{
	uint8_t c;

	gpio_set_mode(GPIOA, 9, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(GPIOA, 10, GPIO_INPUT_FLOATING);

	usart_init(CMD_UART);
	usart_set_baud_rate(CMD_UART, STM32_PCLK2, 115200);
	usart_enable(CMD_UART);

	while (1) {
		if (usart_data_available(CMD_UART) > 0) {
			//usb_cdcacm_putc('p');
			c = usart_getc(CMD_UART);
			usb_cdcacm_putc(c);
		}

		if (usb_cdcacm_data_available() > 0) {
			usb_cdcacm_rx(&c, 1);
			
			if (c == '\r') {
				usb_cdcacm_putc('\n');
				usart_putc(CMD_UART, '\n');
			}

			usb_cdcacm_putc(c);
			usart_putc(CMD_UART, c);
		}
	}
}

void command_process(void)
{
	int avail = usart_data_available(CMD_UART);
	uint8_t cmd;
	cmd_func_t func;
	int i;

	if (avail == 0)
		return;

	if (cmd_buf_i == -1) {
		cmd_buf_len = usart_getc(CMD_UART);
		cmd_buf_left = cmd_buf_len + 1;
		avail--;
		cmd_buf_i = 0;
	}

	for (i = 0; i < avail && i < cmd_buf_left; i++) {
		cmd_buf[cmd_buf_i++] = usart_getc(CMD_UART);
	}

	cmd_buf_left -= i;

	if (cmd_buf_left == 0) {
		cmd = cmd_buf[0];
		DASSERT(cmd < 128);
		func = cmd_map[cmd];

		cmd_buf_i = -1;

		if (func == NULL) {
			command_send('e', NULL, 0);
			return;
		}

		func(&cmd_buf[1], cmd_buf_len);

	}
}

void command_register(uint8_t cmd, cmd_func_t func)
{
	DASSERT(cmd < 128);

	cmd_map[cmd] = func;
}

void command_send(unsigned char cmd, const uint8_t *data, unsigned int length)
{
	int i;

	DASSERT(length <= 255);

	usart_putc(CMD_UART, (uint8_t)length);
	usart_putc(CMD_UART, cmd);
	
	for(i = 0; i < length; i++) {
		usart_putc(CMD_UART, data[i]);
	}
	
	//if (length > 0)
	//	usart_tx(CMD_UART, data, length);
}

int command_connected(void)
{
	return connected;
}
