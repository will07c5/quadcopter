#include "util.h"

#include "debug.h"

#include "i2c.h"
#include "sensors.h"
#include "pwm.h"
#include "intr.h"
#include "command.h"
#include "control.h"

#include <stdint.h>

/* XXX this shouldn't be here */
void blink(void);
void blink(void)
{
	int i;

	REG32(0x4a310134) = ~(1 << 8);

	
	while (1) {
		REG32(0x4a310194) = (1 << 8);

		for (i = 0; i < 10000; i++);

		REG32(0x4a310190) = (1 << 8);

		for (i = 0; i < 10000; i++);
	}
}

/* XXX this also shouldn't be here */
static void set_motor(const uint8_t *buf, uint8_t length)
{
	uint8_t motor;
	uint16_t val;

	if (length != 3) {
		DEBUG("invalid length for set_motor command %hhuc\n", length);
		return;
	}

	motor = buf[0];
	val = (buf[2] << 8) | buf[1];

	DEBUG("set_motor %hhu to %hu\n", motor, val);

	switch (motor) {
	case 1:
		pwm_set(&PWM1, val);
		break;
	case 2:
		pwm_set(&PWM2, val);
		break;
	case 3:
		pwm_set(&PWM3, val);
		break;
	case 4:
		pwm_set(&PWM4, val);
		break;
	default:
		DEBUG("invalid motor\n");
		break;
	}
}

int main(void)
{
	debug_init();

	DEBUG("Quadcopter is starting...\n");

	DEBUG("Enabling clocks for GPT3, GPT4, GPT9, GPT11 and I2C4.\n");
	/* turn on clocks for timers and i2c */
	REG32(0x4A009430) = 2;
	REG32(0x4A009440) = 2;
	REG32(0x4A009448) = 2;
	REG32(0x4A009450) = 2;
	REG32(0x4A0094B8) = 2;

	/* enable exception handlers */
	REG32(0xE000ED24) = 0x70000;

	DEBUG("Initializing I2C4 and sensors.\n");
	i2c_init(I2C_SPEED_STANDARD);
	sensors_init();

	DEBUG("Initializing PWM.\n");
	pwm_init();

	DEBUG("Initializing command and control.\n");
	command_init();
	control_init();

	command_register('m', set_motor);

	DEBUG("Doing final setup.\n");
	/* lower priority of systick so it doesn't interfere with pwm */
	intr_set_systick_priority(1);

	/* change muxing so that the light can blink */
	REG32(0x4A100130) = (REG32(0x4A100130) & 0xFFF8FFFF) | 0x00030000;
	REG32(0x4A310134) = ~(1 << 8);

	/* setup systick to fire at 400 Hz */
	REG32(0xE000E014) = 2000000;//500000;
	REG32(0xE000E010) = 0x7;

	DEBUG("Quadcopter is ready.\n");

	while (1) { }
	return 0;
}
