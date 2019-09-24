#include "intr.h"

#include "debug.h"
#include <util.h>

#define NVIC_ISER 0xE000E100
#define NVIC_ICER 0xE000E180
#define NVIC_IPR 0xE000E400

#define STACK_TOP 0x00400000

#define SYSTEM_INTR_END 16

#define SHPR3 0xE000ED23 /* system handler priority register 3 */

__attribute__ ((section(".intr_table")))
handler_t intr_table[] = {
	(handler_t)STACK_TOP,
	reset_handler,
	nmi_handler,
	hard_fault_handler,
	mpu_handler,
	bus_fault_handler,
	usage_fault_handler,
	0,
	0,
	0,
	0,
	svcall_handler,
	debug_handler,
	0,
	pendsv_handler,
	systick_handler,

	/* external interrupts */
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler,
	default_handler
};

/* XXX
 * These functions do not properly change the bounds of their inputs
 * (because I haven't bothered to write assert yet)
 */

void intr_enable(int num)
{
	uint32_t reg = NVIC_ISER + (num / 32) * 4;
	uint32_t off = num % 32;

	DEBUG("enabled reg: %x off: %d\n", reg, off);

	REG32(reg) = 1 << off;
}

void intr_disable(int num)
{
	uint32_t reg = NVIC_ICER + (num / 32) * 4;
	uint32_t off = num % 32;

	REG32(reg) = 1 << off;

}

void intr_set_handler(int num, handler_t handler)
{
	intr_table[SYSTEM_INTR_END + num] = handler;
}

void intr_set_priority(int num, uint8_t priority)
{
	uint32_t reg = NVIC_IPR + num / 4;
	uint32_t off = (num % 4) * 8;

	REG32(reg) = (REG32(reg) & ~(0xFF << off)) | (priority << off);
}

void intr_set_systick_priority(uint8_t priority)
{
	REG8(SHPR3) = priority << 5;
}

/* XXX WARNING
 * these are disabled since this will disable the systick interrupt rather
 * than just masking it as expected which could result in lost interrupts
 */
#if 0
void intr_disable_systick(void)
{
	REG32(0xE000ED24) &= ~(1 << 11);
}

void intr_enable_systick(void)
{
	REG32(0xE000ED24) |= (1 << 11);
}
#endif
