#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

typedef enum {
	I2C1,
	I2C2,
	I2C3,
	I2C4
} i2c_bus;

typedef enum {
	I2C_SPEED_STANDARD,
	I2C_SPEED_FAST,
	I2C_SPEED_FAST_PLUS
} i2c_speed;

extern void i2c_init(i2c_speed speed);
int i2c_read_regs(uint8_t addr, uint8_t reg, uint8_t *buf, int count);
int i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t val);

#endif
