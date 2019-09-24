#include "command.h"

#include <memory.h>
#include <string.h>
#include <ringbuf.h>

#include "debug.h"

static struct ringbuf rb_tx;
static struct ringbuf rb_rx;

static uint8_t cmd_buf[256];
static int cmd_buf_i = 0;
static int cmd_buf_left;
static uint8_t cmd_buf_len;

static int connected = 0;

static cmd_func_t cmd_map[128];

static void connect_cmd(const uint8_t *buf, uint8_t length)
{
	(void)buf;
	(void)length;

	connected = 1;
	command_send('c', NULL, 0);
	DEBUG("connected\n");
}

static void disconnect_cmd(const uint8_t *buf, uint8_t length)
{
	(void)buf;
	(void)length;

	connected = 0;
	command_send('d', NULL, 0);
	DEBUG("disconnected\n");
}

void command_init(void)
{
	/* TX and RX swapped since the defines are specified relative to the host processor */
	ringbuf_init(&rb_tx, (void*)QUADCOPTER_RX_VIRT, QUADCOPTER_LEN_BYTES(RX));
	ringbuf_init(&rb_rx, (void*)QUADCOPTER_TX_VIRT, QUADCOPTER_LEN_BYTES(TX));

	memset(cmd_map, 0, sizeof(cmd_map));
	
	command_register('c', connect_cmd);
	command_register('d', disconnect_cmd);

	cmd_buf_i = -1;
}

void command_process(void)
{
	int avail = ringbuf_available_read(&rb_rx);
	uint8_t cmd;
	cmd_func_t func;
	int i;

	if (avail == 0)
		return;

	if (cmd_buf_i == -1) {
		cmd_buf_len = ringbuf_getc(&rb_rx);
		cmd_buf_left = cmd_buf_len + 1;
		avail--;
		cmd_buf_i = 0;
	}

	for (i = 0; i < avail && i < cmd_buf_left; i++) {
		cmd_buf[cmd_buf_i++] = ringbuf_getc(&rb_rx);
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

	DEBUG("registered command %c\n", cmd);

	cmd_map[cmd] = func;
}

void command_send(unsigned char cmd, const uint8_t *data, unsigned int length)
{
	DASSERT(length <= 255);

	ringbuf_putc(&rb_tx, (uint8_t)length);
	ringbuf_putc(&rb_tx, cmd);
	
	ringbuf_write(&rb_tx, (const char*)data, length);
}

int command_connected(void)
{
	return connected;
}
