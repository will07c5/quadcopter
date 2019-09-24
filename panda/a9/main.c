#include "mmu.h"

#include <util.h>

#define CORE_CM2_BASE_ADDR 0x4A008700
#define CORE_PRM_BASE_ADDR 0x4A306700
#define CM_MPU_M3_MPU_M3_CLKCTRL (CORE_CM2_BASE_ADDR + 0x220)
#define CM_MPU_M3_CLKSTCTRL (CORE_CM2_BASE_ADDR + 0x200)
#define RM_MPU_M3_RSTCTRL (CORE_PRM_BASE_ADDR + 0x210)

#define M3_MMU_RESET (1 << 2)
#define M3_CPU1_RESET (1 << 0)

#define CLKTRCTRL_SW_WKUP 0x2
#define MODULEMODE (1 << 0)

#define LINUX_BASE ????

#include "uart.h"

void blink(void);

/* memory map:
 * 0x00000000 - program
 * 0x00100000
 * 0x00200000 - guard page
 * 0x00300000 - stack region (stack starts at 0x003FFFFF and grows down)
 * 0x48000000 - peripheral region
 */
int main(void)
{
	uart_init();


	uart_puts("hello\n");
	uart_puthex(*(unsigned int*)0x9E100000);

	/* wakeup and take m3 mmu out of reset */
	REG32(CM_MPU_M3_MPU_M3_CLKCTRL) = MODULEMODE;
	REG32(CM_MPU_M3_CLKSTCTRL) = CLKTRCTRL_SW_WKUP;
	REG32(RM_MPU_M3_RSTCTRL) &= ~M3_MMU_RESET;

	mmu_init();

	/* map 2MB program region */
	mmu_map_range(0x00000000, 0x9E100000, 2);

	/* map 1MB stack region */
	mmu_map(0x00300000, 0x9E300000);

	/* map 16MB peripheral region */
	mmu_map_range(0x48000000, 0x48000000, 16);
	
	mmu_map_range(0x4A000000, 0x4A000000, 16);

	mmu_map(0xE0000000, 0xE0000000);

	mmu_enable();

	/* take first m3 core out of reset */
	REG32(RM_MPU_M3_RSTCTRL) &= ~M3_CPU1_RESET;

//	((void (*)(void))LINUX_BASE)();

//	blink();

	/* should never get here */
	while (1) { }
	return 0;
}

void blink(void)
{
	int i;

	REG32(0x4a310134) = ~(1 << 8);

	while (1) {
		REG32(0x4a310194) = (1 << 8);

		for (i = 0; i < 1000000; i++);

		REG32(0x4a310190) = (1 << 8);

		for (i = 0; i < 1000000; i++);
	}
}

