#ifndef DEBUG_H_
#define DEBUG_H_

#include <util.h>

#ifdef SERIAL_DEBUG
#define DEBUG(...) debug_printf(__VA_ARGS__)
#define DASSERT(exp) \
	do { \
		if (!(exp)) { \
			debug_printf("ASSERT FAIL in %s at %d: %s\n", __FILE__, __LINE__, #exp); \
			throb(); \
		} \
	} while(0)
#else
#define DEBUG(...)
#define DASSERT(exp)
#endif

extern int debug_printf(const char *fmt, ...);

extern void debug_wait_connect(void);


#endif
