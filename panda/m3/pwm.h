#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

struct pwm_device;

extern struct pwm_device PWM1;
extern struct pwm_device PWM2;
extern struct pwm_device PWM3;
extern struct pwm_device PWM4;

void pwm_init(void);
void pwm_set(struct pwm_device *pwm, uint16_t value);
/*
void pwm1_handler(void);
void pwm2_handler(void);
void pwm3_handler(void);
void pwm4_handler(void);
*/
#endif
