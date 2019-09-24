// Sample main.cpp file. Blinks the built-in LED, sends a message out
// USART2, and turns on PWM on pin 2.

#include <gpio.h>
#include <nvic.h>
#include <rcc.h>
#include <flash.h>
#include <fsmc.h>
#include <usb/usb_cdcacm.h>
#include <stdint.h>

#include "quadcopter.h"

extern uint32_t __cs3_stm32_vector_table;

static void init(void)
{
	fsmc_nor_psram_reg_map *regs = FSMC_NOR_PSRAM1_BASE;

	/* init flash */
	flash_enable_prefetch();
	flash_set_latency(FLASH_WAIT_STATE_2);

	/* init clock to 72 MHz */
	rcc_clk_init(RCC_CLKSRC_PLL, RCC_PLLSRC_HSE, RCC_PLLMUL_9);

	/* set the prescalers for the busses to their maximum allowed frequency */
	rcc_set_prescaler(RCC_PRESCALER_AHB, RCC_AHB_SYSCLK_DIV_1);
	rcc_set_prescaler(RCC_PRESCALER_APB1, RCC_APB1_HCLK_DIV_2);
	rcc_set_prescaler(RCC_PRESCALER_APB2, RCC_APB2_HCLK_DIV_1);

	/* point the NVIC at the interrupt vector table */
	nvic_init((uint32_t)&__cs3_stm32_vector_table, 0);

	/* init gpio/afio */
	gpio_init_all();
	afio_init();

	/* enable usb serial terminal and raise irq priorities */
	nvic_irq_set_priority(NVIC_USB_HP_CAN_TX, 0xE);
	nvic_irq_set_priority(NVIC_USB_LP_CAN_RX0, 0xE);
	usb_cdcacm_enable(GPIOB, 8);

	/* init the SRAM so our program doesn't crash horribly */
	fsmc_sram_init_gpios();
	rcc_clk_enable(RCC_FSMC);

	regs->BCR = (FSMC_BCR_WREN | FSMC_BCR_MWID_16BITS | FSMC_BCR_MTYP_SRAM |
	            FSMC_BCR_MBKEN);
	fsmc_nor_psram_set_addset(regs, 0);
	fsmc_nor_psram_set_datast(regs, 3);
}

int main(void) {
	init();

	quadcopter_run();

	return 0;
}
