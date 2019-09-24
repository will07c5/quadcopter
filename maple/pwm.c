#include "pwm.h"

#include <gpio.h>

#define PWM_TIMER TIMER8

void pwm_init(void)
{
	timer_adv_reg_map *regs = (PWM_TIMER->regs).adv;

	timer_init(PWM_TIMER);
	timer_pause(PWM_TIMER);

	timer_set_mode(PWM_TIMER, TIMER_CH1, TIMER_PWM);
	timer_set_mode(PWM_TIMER, TIMER_CH2, TIMER_PWM);
	timer_set_mode(PWM_TIMER, TIMER_CH3, TIMER_PWM);
	timer_set_mode(PWM_TIMER, TIMER_CH4, TIMER_PWM);
	//timer_oc_set_mode(PWM_TIMER, TIMER_CH1, TIMER_OC_MODE_PWM_1, TIMER_OC_PE);
	//timer_oc_set_mode(PWM_TIMER, TIMER_CH2, TIMER_OC_MODE_PWM_1, TIMER_OC_PE);
	//timer_oc_set_mode(PWM_TIMER, TIMER_CH3, TIMER_OC_MODE_PWM_1, TIMER_OC_PE);
	//timer_oc_set_mode(PWM_TIMER, TIMER_CH4, TIMER_OC_MODE_PWM_1, TIMER_OC_PE);

	/* Set prescaler and reload value to get 50Hz */
	timer_set_prescaler(PWM_TIMER, 22);
	timer_set_reload(PWM_TIMER, 65454);

	pwm_set(TIMER_CH1, 0);
	pwm_set(TIMER_CH2, 0);
	pwm_set(TIMER_CH3, 0);
	pwm_set(TIMER_CH4, 0);

	gpio_set_mode(GPIOC, 6, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(GPIOC, 7, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(GPIOC, 8, GPIO_AF_OUTPUT_PP);
	gpio_set_mode(GPIOC, 9, GPIO_AF_OUTPUT_PP);

	/* need to do this because it is an advanced timer */
	regs->BDTR = TIMER_BDTR_MOE | TIMER_BDTR_LOCK_OFF;

	timer_generate_update(PWM_TIMER);
	timer_resume(PWM_TIMER);


}

void pwm_set(int channel, int16_t value)
{
	int scaled = 3272 + (value * 3272) / 32767;

	timer_set_compare(PWM_TIMER, channel, scaled);
}
