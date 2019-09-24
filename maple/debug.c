#include "debug.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#ifndef TESTING
#include <usb/usb_cdcacm.h>
#else
#include <stdio.h>
#endif

#define CONV_BUF_SIZE 64

typedef int (*out_func_t)(const char *, int);

enum length_mod {
	length_mod_none = 0,
	length_mod_hh,
	length_mod_h,
	length_mod_l,
	length_mod_ll,
	length_mod_j,
	length_mod_z,
	length_mod_t
};

/* buf should be at least CONV_BUF_SIZE */
static int convert_dec(uintmax_t value, char* buf)
{
	int pos = CONV_BUF_SIZE - 1;

	while (pos >= 0) {
		buf[pos] = (char)(value % 10) + '0';
		value /= 10;
		pos--;

		if (value == 0)
			break;
	}

	return pos;
}

/* buf should be at least CONV_BUF_SIZE */
static int convert_hex(uintmax_t value, int upper_case, char* buf)
{
	int pos = CONV_BUF_SIZE - 1;
	char cur = 0;
	char a = upper_case ? 'A' : 'a';

	while (pos >= 0) {
		cur = value & 0xF;
		buf[pos] = cur >= 10 ? cur - 10 + a : cur + '0';
		value >>= 4;
		pos--;

		if (value == 0)
			break;
	}

	return pos;
}

/* buf should be at least CONV_BUF_SIZE */
static int convert_oct(uintmax_t value, char* buf)
{
	int pos = CONV_BUF_SIZE - 1;

	while (pos >= 0) {
		buf[pos] = (char)(value & 0x7) + '0';
		value >>= 3;
		pos--;

		if (value == 0)
			break;
	}

	return pos;
}


static int output_int(int length, out_func_t out_func, va_list *args)
{
	intmax_t value;
	uintmax_t uvalue;
	int pos, len;
	char buf[CONV_BUF_SIZE];

	switch (length) {
	case length_mod_none:
		value = va_arg(*args, int);
		break;
	case length_mod_hh:
		value = (char)va_arg(*args, int);
		break;
	case length_mod_h:
		value = (short)va_arg(*args, int);
		break;
	case length_mod_l:
		value = va_arg(*args, long);
		break;
	case length_mod_ll:
		value = va_arg(*args, long long);
		break;
	case length_mod_j:
		value = va_arg(*args, intmax_t);
		break;
	case length_mod_t:
		value = va_arg(*args, ptrdiff_t);
		break;
	default:
		return 0;
	}

	if (value < 0)
		uvalue = -value;
	else
		uvalue = value;

	pos = convert_dec(uvalue, buf);

	if (value < 0) {
		buf[pos] = '-';
		pos--;
	}

	len = CONV_BUF_SIZE - pos - 1;

	out_func(&buf[pos + 1], len);

	return len;
}

static int output_uint(int length, int radix, int upper_case, out_func_t out_func, va_list *args)
{
	uintmax_t value;
	int pos, len;
	char buf[CONV_BUF_SIZE];

	switch (length) {
	case length_mod_none:
		value = va_arg(*args, unsigned int);
		break;
	case length_mod_hh:
		value = (unsigned char)va_arg(*args, unsigned int);
		break;
	case length_mod_h:
		value = (unsigned short)va_arg(*args, unsigned int);
		break;
	case length_mod_l:
		value = va_arg(*args, unsigned long);
		break;
	case length_mod_ll:
		value = va_arg(*args, unsigned long long);
		break;
	case length_mod_j:
		value = va_arg(*args, uintmax_t);
		break;
	case length_mod_z:
		value = va_arg(*args, size_t);
		break;
	default:
		return 0;
	}

	if (radix == 16)
		pos = convert_hex(value, upper_case, buf);
	else if (radix == 8)
		pos = convert_oct(value, buf);
	else
		pos = convert_dec(value, buf);

	len = CONV_BUF_SIZE - pos - 1;

	out_func(&buf[pos + 1], len);

	return len;
}

static void output_char(out_func_t out_func, va_list *args)
{
	char value = (char)va_arg(*args, int);

	out_func(&value, 1);

}

#if 0
static int output_float(int length, out_func_t out_func, va_list args)
{

}

static int output_str(out_func_t out_func, va_list args)
{

}

static int output_ptr(out_func_t out_func, va_list args)
{

}
#endif

static int parse_specifier(const char **fmtp, out_func_t out_func, va_list *args)
{
	enum length_mod length = length_mod_none;
	int count = -1;

	while (1) {
		switch (**fmtp) {
		/*
		 * Length modifiers
		 */

		case 'h':
			if (length == length_mod_h)
				length = length_mod_hh;
			else
				length = length_mod_h;

			break;
		case 'l':
			if (length == length_mod_l)
				length = length_mod_ll;
			else
				length = length_mod_l;

			break;
		case 'j':
			length = length_mod_j;
			break;
		case 'z':
			length = length_mod_z;
			break;
		case 't':
			length = length_mod_t;
			break;

		/*
		 * Conversion specifiers
		 */
		case 'd':
		case 'i':
			count = output_int(length, out_func, args);
			goto done;
		case 'o':
			count = output_uint(length, 8, 0, out_func, args);
			goto done;
		case 'u':
			count = output_uint(length, 10, 0, out_func, args);
			goto done;
		case 'X':
			count = output_uint(length, 16, 1, out_func, args);
			goto done;
		case 'x':
			count = output_uint(length, 16, 0, out_func, args);
			goto done;
		case 'f':
			//output_float(&mods, args);
			break;
		case 'c':
			output_char(out_func, args);
			count = 1;
			goto done;
		case 's':
			//output_str(out_func, args);
			break;
		case 'p':
			//output_ptr(out_func, args);
			break;
		case '%':
			out_func("%", 1);
			count = 1;
			goto done;

		default:
			goto done;
		}

		(*fmtp)++;
	}

done:
	(*fmtp)++;

	return count;
}

static int do_printf (const char *fmt, out_func_t out_func, va_list *args)
{
	int length = 0;
	int count = 0;
	int spec_len = 0;

	while (1) {
		if (fmt[length] == '\0') {
			out_func(fmt, length);
			count += length;
			return count;
		} else if (fmt[length] == '%') {
			out_func(fmt, length);
			fmt = &fmt[length + 1];
			count += length;
			length = 0;

			spec_len = parse_specifier(&fmt, out_func, args);

			if (spec_len == -1)
				return -1;
			else
				count += spec_len;
		} else {
			length++;
		}
	}
}

#ifndef TESTING
static int usb_serial_out(const char *buf, int len)
{
	int i;

	if (len == 0)
		return 0;

	for (i = 0; i < len; i++) {
		if (buf[i] == '\n')
			usb_cdcacm_putc('\r');

		usb_cdcacm_putc(buf[i]);
	}
	//usb_cdcacm_tx((const unsigned char*)buf, len);

	return 0;
}
#else
static int test_out(const char *buf, int len)
{
	for (int i = 0; i < len; i++) {
		putchar(buf[i]);
	}
}

#endif

int debug_printf(const char *fmt, ...)
{
	int ret = 0;
	out_func_t out_func;
	va_list args;
	
#ifndef TESTING
	out_func = usb_serial_out;
#else
	out_func = test_out;
#endif

	va_start(args, fmt);
	ret = do_printf(fmt, out_func, &args);
	va_end(args);

	return ret;
}

void debug_wait_connect(void)
{
	while (usb_cdcacm_data_available() == 0);
}
