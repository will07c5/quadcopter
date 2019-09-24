#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

#define REG8(reg) (*(volatile uint8_t*)(reg))
#define REG16(reg) (*(volatile uint16_t*)(reg))
#define REG32(reg) (*(volatile uint32_t*)(reg))

typedef uint32_t phys_t;
typedef uint32_t virt_t;


#define BUF2FLOAT(buf, off) (*(const float*)&(buf)[(off)*sizeof(float)])

#endif

