#include "debug.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	debug_printf("Testing printf oct\n");

	for (int i = 0; i < 4; i++) {
		uintmax_t test = 0;
		memset(&test, i < 2 ? i : 0512 + i, sizeof(test));

		debug_printf("HHo %hho TEST\n", (char)test);
		printf      ("HHo %hho REAL\n", (char)test);
		debug_printf(" Ho %ho TEST\n", (short)test);
		printf      (" Ho %ho REAL\n", (short)test);
		debug_printf("  o %o TEST\n", (int)test);
		printf      ("  o %o REAL\n", (int)test);
		debug_printf(" Lo %lo TEST\n", (long)test);
		printf      (" Lo %lo REAL\n", (long)test);
		debug_printf("LLo %llo TEST\n", (long long)test);
		printf      ("LLo %llo REAL\n", (long long)test);
	}

	debug_printf("Testing printf dec\n");

	for (int i = 0; i < 4; i++) {
		uintmax_t test = 0;
		memset(&test, i < 2 ? i : 0xA9 + i, sizeof(test));

		debug_printf("HHd %hhd TEST\n", (char)test);
		printf      ("HHd %hhd REAL\n", (char)test);
		debug_printf(" Hd %hd TEST\n", (short)test);
		printf      (" Hd %hd REAL\n", (short)test);
		debug_printf("  d %d TEST\n", (int)test);
		printf      ("  d %d REAL\n", (int)test);
		debug_printf(" Ld %ld TEST\n", (long)test);
		printf      (" Ld %ld REAL\n", (long)test);
		debug_printf("LLd %lld TEST\n", (long long)test);
		printf      ("LLd %lld REAL\n", (long long)test);

		debug_printf("HHu %hhu TEST\n", (char)test);
		printf      ("HHu %hhu REAL\n", (char)test);
		debug_printf(" Hu %hu TEST\n", (short)test);
		printf      (" Hu %hu REAL\n", (short)test);
		debug_printf("  u %u TEST\n", (int)test);
		printf      ("  u %u REAL\n", (int)test);
		debug_printf(" Lu %lu TEST\n", (long)test);
		printf      (" Lu %lu REAL\n", (long)test);
		debug_printf("LLu %llu TEST\n", (long long)test);
		printf      ("LLu %llu REAL\n", (long long)test);

	}


	debug_printf("Testing printf hex\n");
	for (int i = 0; i < 4; i++) {
		uintmax_t test = 0;
		memset(&test, i < 2 ? i : 0xA9 + i, sizeof(test));

		debug_printf("HHx %hhx TEST\n", (char)test);
		printf      ("HHx %hhx REAL\n", (char)test);
		debug_printf(" Hx %hx TEST\n", (short)test);
		printf      (" Hx %hx REAL\n", (short)test);
		debug_printf("  x %x TEST\n", (int)test);
		printf      ("  x %x REAL\n", (int)test);
		debug_printf(" Lx %lx TEST\n", (long)test);
		printf      (" Lx %lx REAL\n", (long)test);
		debug_printf("LLx %llx TEST\n", (long long)test);
		printf      ("LLx %llx REAL\n", (long long)test);
		
		debug_printf("HHX %hhX TEST\n", (char)test);
		printf      ("HHX %hhX REAL\n", (char)test);
		debug_printf(" HX %hX TEST\n", (short)test);
		printf      (" HX %hX REAL\n", (short)test);
		debug_printf("  X %X TEST\n", (int)test);
		printf      ("  X %X REAL\n", (int)test);
		debug_printf(" LX %lX TEST\n", (long)test);
		printf      (" LX %lX REAL\n", (long)test);
		debug_printf("LLX %llX TEST\n", (long long)test);
		printf      ("LLX %llX REAL\n", (long long)test);
	}

	return 0;
}
