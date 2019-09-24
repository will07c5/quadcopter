#ifndef PWM_H_
#define PWM_H_

#include <timer.h>
#include <stdint.h>

#define PWM_NORTH TIMER_CH1
#define PWM_SOUTH TIMER_CH2
#define PWM_EAST TIMER_CH3
#define PWM_WEST TIMER_CH4

void pwm_init(void);

/* channel between 0-3 and value must be greater than zero */
void pwm_set(int channel, int16_t value);


#endif

