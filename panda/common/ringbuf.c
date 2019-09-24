#include "ringbuf.h"

#ifdef __KERNEL__
#include <asm/io.h>
#else
#include <string.h>
#define memcpy_toio memcpy
#define memcpy_fromio memcpy
#endif

void ringbuf_init(struct ringbuf *rb, void *base, uint32_t size)
{
	rb->write_pos = base;
	rb->read_pos = base + 4;
	rb->data = base + 8;
	rb->size = size - 2*sizeof(uint32_t);

	*rb->write_pos = 0;
	*rb->read_pos = 0;
}

int ringbuf_write(struct ringbuf *rb, const char *buf, int count)
{
	uint32_t count_tmp;
	uint32_t write_tmp = *rb->write_pos;
	int avail;

	avail = ringbuf_available_write(rb);
	if (count > avail)
		count = avail;

	if (write_tmp + count >= rb->size) {
		count_tmp = rb->size - write_tmp;
		memcpy_toio(rb->data + write_tmp, buf, count_tmp);

		if (count - count_tmp > 0)
			memcpy_toio(rb->data, buf + count_tmp, count - count_tmp);

		write_tmp = count - count_tmp;
	} else {
		memcpy_toio(rb->data + write_tmp, buf, count);
		write_tmp += count;
	}

	*rb->write_pos = write_tmp;

	return count;
}

int ringbuf_read(struct ringbuf *rb, char *buf, int count)
{
	uint32_t count_tmp;
	uint32_t read_tmp = *rb->read_pos;
	int avail;

	avail = ringbuf_available_read(rb);
	if (count > avail)
		count = avail;

	if (read_tmp + count >= rb->size) {
		count_tmp = rb->size - read_tmp;
		memcpy_fromio(buf, rb->data + read_tmp, count_tmp);

		if (count - count_tmp > 0)
			memcpy_fromio(buf + count_tmp, rb->data, count - count_tmp);

		read_tmp = count - count_tmp;
	} else {
		memcpy_fromio(buf, rb->data + read_tmp, count);
		read_tmp += count;
	}

	*rb->read_pos = read_tmp;

	return count;
}

int ringbuf_available_read(struct ringbuf *rb)
{
	uint32_t write_tmp = *rb->write_pos;
	uint32_t read_tmp = *rb->read_pos;

	if (write_tmp > read_tmp)
		return write_tmp - read_tmp;
	else if (write_tmp < read_tmp)
		return rb->size - (read_tmp - write_tmp);
	else
		return 0;
}

int ringbuf_available_write(struct ringbuf *rb)
{
	uint32_t read_tmp = *rb->read_pos;
	uint32_t write_tmp = *rb->write_pos;

	if (read_tmp > write_tmp)
		return read_tmp - write_tmp - 1;
	else if (read_tmp < write_tmp)
		return rb->size - (write_tmp - read_tmp) - 1;
	else
		return rb->size - 1;
}

char ringbuf_getc(struct ringbuf *rb)
{
	char buf[1];
	ringbuf_read(rb, buf, 1);
	return buf[0];
}

void ringbuf_putc(struct ringbuf *rb, char c)
{
	ringbuf_write(rb, &c, 1);
}
