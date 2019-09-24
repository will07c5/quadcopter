#include "../kmod/ringbuf.h"

/* pure evil */
#include "../kmod/ringbuf.c"

#include <stdio.h>

static void print_ringbuf(struct ringbuf *rb)
{
	int i;

	for (i = 0; i < rb->size; i++) {
		printf("%02hhx ", rb->data[i]);
	}

	printf("\n");

	for (i = 0; i < rb->size; i++) {
		if (i == *rb->read_pos && i == *rb->write_pos) {
			printf("RW ");
		} else if (i == *rb->read_pos) {
			printf("R^ ");
		} else if (i == *rb->write_pos) {
			printf("^W ");
		} else {
			printf("   ");
		}
	}

	printf("\n");
	
	printf("R: %d W: %d\n", ringbuf_available_read(rb), ringbuf_available_write(rb));
}

#define SIZE 8

int main(int argc, char **argv)
{
	uint32_t wr = 0, rd = 0;
	char buf[SIZE];
	struct ringbuf rb;

	char data1[] = { 0x11, 0x22, 0x33, 0x44 };
	char data2[5];

	memset(buf, 0, SIZE);

	rb.write_pos = &wr;
	rb.read_pos = &rd;
	rb.data = buf;
	rb.size = SIZE;

	print_ringbuf(&rb);

	ringbuf_write(&rb, data1, 4);
	print_ringbuf(&rb);

	ringbuf_write(&rb, data1, 4);
	print_ringbuf(&rb);

	ringbuf_write(&rb, data1, 3);
	print_ringbuf(&rb);

	ringbuf_read(&rb, data2, 5);
	print_ringbuf(&rb);

	ringbuf_write(&rb, data1, 4);
	print_ringbuf(&rb);

	ringbuf_read(&rb, data2, 3);
	print_ringbuf(&rb);

	ringbuf_read(&rb, data2, 3);
	print_ringbuf(&rb);

	return 0;
}
