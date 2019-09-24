#include "handlers.h"

#include "debug.h"
#include "control.h"
#include <util.h>

extern int main(void);
extern void blink(void);

void reset_handler(void)
{
	main();
}

void nmi_handler(void)
{
	DEBUG("NMI\n");
	blink();
	while (1) { }
}

void hard_fault_handler(void)
{
	DEBUG("HARD FAULT\n");
	DEBUG("HFSR = %x\n", REG32(0xE000ED2C));
	blink();
	while (1) { }
}

void mpu_handler(void)
{
	DEBUG("MPU FAULT\n");
	blink();
	while (1) { }
}

void bus_fault_handler(void)
{
	DEBUG("BUS FAULT\n");
	DEBUG("BFAR = %x\n", REG32(0xE000ED38));
	DEBUG("BFSR = %x\n", REG8(0xE000ED29));

	DEBUG("ERROR_LOG_L %x\n", REG32(0x48001058));
	DEBUG("ERROR_LOG_H %x\n", REG32(0x4800105C));
	DEBUG("ERROR_LOG_ADDR_L %x\n", REG32(0x48001060));
	DEBUG("ERROR_LOG_ADDR_H %x\n", REG32(0x48001064));

	blink();
	while (1) { }
}

void usage_fault_handler(void)
{
	DEBUG("USAGE FAULT\n");
	blink();
	while (1) { }
}

void svcall_handler(void)
{
	DEBUG("SVCALL???\n");
	blink();
	while (1) { }
}

void debug_handler(void)
{
	DEBUG("DEBUG\n");
	blink();
	while (1) { }
}

void pendsv_handler(void)
{
	DEBUG("PENDSV???\n");
	blink();
	while (1) { }
}

void systick_handler(void)
{
	control_update();
}

void default_handler(void)
{
	DEBUG("Arrived at default handler.\n");
	blink();
	while (1) { }
}


