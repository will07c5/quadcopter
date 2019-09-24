/* ringbuf.h
 *
 * A simple ring buffer implementation.
 */

#ifndef RINGBUF_H_
#define RINGBUF_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

struct ringbuf {
	uint32_t size;
	volatile uint32_t *write_pos;
	volatile uint32_t *read_pos;
	char *data;
};

/* base should point to an area of memory big enough for size bytes 
 * + 8 bytes for read and write pointers */
void ringbuf_init(struct ringbuf *rb, void *base, uint32_t size);

int ringbuf_write(struct ringbuf *rb, const char *buf, int count);
int ringbuf_read(struct ringbuf *rb, char *buf, int count);
int ringbuf_available_read(struct ringbuf *rb);
int ringbuf_available_write(struct ringbuf *rb);

char ringbuf_getc(struct ringbuf *rb);
void ringbuf_putc(struct ringbuf *rb, char c);

#endif
