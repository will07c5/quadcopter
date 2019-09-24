#include "pwm.h"

#include "debug.h"
#include "intr.h"
#include <util.h>
struct pwm_device {
	uint32_t TIOCP_CFG;
	uint32_t IRQSTATUS;
	uint32_t IRQENABLE_SET;
	uint32_t IRQENABLE_CLR;
	uint32_t TCLR;
	uint32_t TCRR;
	uint32_t TLDR;
	uint32_t TMAR;
	uint32_t TSICR;
};
/* Timer 3, 4, 9 and 11 are the only timers that the m3 can receive interrupts for */
#define PWM_TIMER3_BASEADDR 0x48034000
#define PWM_TIMER4_BASEADDR 0x48036000
#define PWM_TIMER9_BASEADDR 0x4803E000
#define PWM_TIMER11_BASEADDR 0x48088000

#define PWM_TIOCP_CFG 0x10
#define PWM_IRQSTATUS 0x28
#define PWM_IRQENABLE_SET 0x2C
#define PWM_IRQENABLE_CLR 0x30
#define PWM_TCLR 0x38
#define PWM_TCRR 0x3C
#define PWM_TLDR 0x40
#define PWM_TMAR 0x4C
#define PWM_TSICR 0x54

#define OVF_IRQ_FLAG (1 << 1)
#define MAT_IT_FLAG (1 << 0)

#define OVF_IT_ENA (1 << 1)
#define MAT_IT_ENA (1 << 0)

struct pwm_device PWM1 = {
	PWM_TIMER3_BASEADDR + PWM_TIOCP_CFG,
	PWM_TIMER3_BASEADDR + PWM_IRQSTATUS,
	PWM_TIMER3_BASEADDR + PWM_IRQENABLE_SET,
	PWM_TIMER3_BASEADDR + PWM_IRQENABLE_CLR,
	PWM_TIMER3_BASEADDR + PWM_TCLR,
	PWM_TIMER3_BASEADDR + PWM_TCRR,
	PWM_TIMER3_BASEADDR + PWM_TLDR,
	PWM_TIMER3_BASEADDR + PWM_TMAR,
	PWM_TIMER3_BASEADDR + PWM_TSICR,
};

struct pwm_device PWM2 = {
	PWM_TIMER4_BASEADDR + PWM_TIOCP_CFG,
	PWM_TIMER4_BASEADDR + PWM_IRQSTATUS,
	PWM_TIMER4_BASEADDR + PWM_IRQENABLE_SET,
	PWM_TIMER4_BASEADDR + PWM_IRQENABLE_CLR,
	PWM_TIMER4_BASEADDR + PWM_TCLR,
	PWM_TIMER4_BASEADDR + PWM_TCRR,
	PWM_TIMER4_BASEADDR + PWM_TLDR,
	PWM_TIMER4_BASEADDR + PWM_TMAR,
	PWM_TIMER4_BASEADDR + PWM_TSICR,
};

struct pwm_device PWM3 = {
	PWM_TIMER9_BASEADDR + PWM_TIOCP_CFG,
	PWM_TIMER9_BASEADDR + PWM_IRQSTATUS,
	PWM_TIMER9_BASEADDR + PWM_IRQENABLE_SET,
	PWM_TIMER9_BASEADDR + PWM_IRQENABLE_CLR,
	PWM_TIMER9_BASEADDR + PWM_TCLR,
	PWM_TIMER9_BASEADDR + PWM_TCRR,
	PWM_TIMER9_BASEADDR + PWM_TLDR,
	PWM_TIMER9_BASEADDR + PWM_TMAR,
	PWM_TIMER9_BASEADDR + PWM_TSICR,
};

struct pwm_device PWM4 = {
	PWM_TIMER11_BASEADDR + PWM_TIOCP_CFG,
	PWM_TIMER11_BASEADDR + PWM_IRQSTATUS,
	PWM_TIMER11_BASEADDR + PWM_IRQENABLE_SET,
	PWM_TIMER11_BASEADDR + PWM_IRQENABLE_CLR,
	PWM_TIMER11_BASEADDR + PWM_TCLR,
	PWM_TIMER11_BASEADDR + PWM_TCRR,
	PWM_TIMER11_BASEADDR + PWM_TLDR,
	PWM_TIMER11_BASEADDR + PWM_TMAR,
	PWM_TIMER11_BASEADDR + PWM_TSICR,
};

static void init_timer(struct pwm_device *pwm)
{
	REG32(pwm->TIOCP_CFG) = 1;
	while (REG32(pwm->TIOCP_CFG) & 1);

	REG32(pwm->IRQENABLE_SET) = OVF_IT_ENA | MAT_IT_ENA;
	REG32(pwm->TLDR) = 0xFFFE88FF; /* set reload value to get 400 Hz */
	REG32(pwm->TCRR) = 0xFFFFFFFF;
	pwm_set(pwm, UINT16_MAX);
	REG32(pwm->TCLR) = 0x00000043;
}

static void pwm1_handler(void)
{
	uint32_t stat = REG32(PWM1.IRQSTATUS);

	if (stat & OVF_IRQ_FLAG)
		REG32(0x48055194) = (1 << 0); /* GPIO_32 set */
	else
		REG32(0x48055190) = (1 << 0); /* GPIO_32 clear */

	REG32(PWM1.IRQSTATUS) = stat;
}

static void pwm2_handler(void)
{
	uint32_t stat = REG32(PWM2.IRQSTATUS);

	if (stat & OVF_IRQ_FLAG)
		REG32(0x48055194) = (1 << 1); /* GPIO_33 set */
	else
		REG32(0x48055190) = (1 << 1); /* GPIO_33 clear */

	REG32(PWM2.IRQSTATUS) = stat;
}

static void pwm3_handler(void)
{
	uint32_t stat = REG32(PWM3.IRQSTATUS);

	if (stat & OVF_IRQ_FLAG)
		REG32(0x48055194) = (1 << 2); /* GPIO_34 set */
	else
		REG32(0x48055190) = (1 << 2); /* GPIO_34 clear */

	REG32(PWM3.IRQSTATUS) = stat;

}

static void pwm4_handler(void)
{
	uint32_t stat = REG32(PWM4.IRQSTATUS);

	if (stat & OVF_IRQ_FLAG)
		REG32(0x48055194) = (1 << 3); /* GPIO_35 set */
	else
		REG32(0x48055190) = (1 << 3); /* GPIO_35 clear */

	REG32(PWM4.IRQSTATUS) = stat;

}

void pwm_init(void)
{
	/* enable gpio_32-35 output */
	REG32(0x48055134) &= ~(0xF);
	REG32(0x4A100050) = (REG32(0x4A100050) & 0xFE00FE00) | 0x00030003;
	REG32(0x4A100054) = (REG32(0x4A100054) & 0xFE00FE00) | 0x00030003;

	intr_set_handler(INTR_GPT3_IRQ, pwm1_handler);
	intr_set_handler(INTR_GPT4_IRQ, pwm2_handler);
	intr_set_handler(INTR_GPT9_IRQ, pwm3_handler);
	intr_set_handler(INTR_GPT11_IRQ, pwm4_handler);
	intr_enable(INTR_GPT3_IRQ);
	intr_enable(INTR_GPT4_IRQ);
	intr_enable(INTR_GPT9_IRQ);
	intr_enable(INTR_GPT11_IRQ);
	
	init_timer(&PWM1);
	init_timer(&PWM2);
	init_timer(&PWM3);
	init_timer(&PWM4);
}

void pwm_set(struct pwm_device *pwm, uint16_t val)
{
	uint32_t scaled = val;

	scaled *= 0x9600;
	scaled /= UINT16_MAX;

	REG32(pwm->TMAR) =  0xFFFE88FF + 0x9600 + scaled;
}

#if 0
uint32_t bench_read(void)
{
	return REG32(PWM1.TCRR);
}

void bench_reset(void)
{
	REG32(PWM1.TCRR) = 0;
}
#endif
