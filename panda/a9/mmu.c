#include "mmu.h"

#include "uart.h"

#include <string.h>

#define MMU_BASE_ADDR 0x55082000

#define MMU_TTB (MMU_BASE_ADDR + 0x4C)
#define MMU_CNTL (MMU_BASE_ADDR + 0x44)

#define MMU_VIRT_MASK 0xFFF00000
#define MMU_VIRT_SHIFT 20

/* These bits specify that the table entry is a section and that
 * there is no endianness conversion */
#define MMU_TABLE_SECTION 0xC02

#define CNTL_TWLENABLE (1 << 2)
#define CNTL_MMUENABLE (1 << 1)

uint32_t table[4096] __attribute__ ((aligned (0x4000)));

void mmu_init(void)
{
	int i;
//	memset(table, 0, sizeof(table));
	for (i = 0; i < 4096; i++) {
		table[i] = 0;
	}
}

void mmu_map(virt_t virt, phys_t phys)
{
	virt = (virt & MMU_VIRT_MASK) >> MMU_VIRT_SHIFT;

	/* place entry in table (don't bother with second level entries) */
	table[virt] = (phys & MMU_VIRT_MASK) | MMU_TABLE_SECTION;

	uart_puts("just mapped ");
	uart_puthex(virt);
	uart_puts(" ");
	uart_puthex(phys);
	uart_puts("\n");
}

void mmu_map_range(virt_t vstart, phys_t pstart, unsigned int count)
{
	uart_puts("About to map range ");
	uart_puthex(vstart);
	uart_puts(" ");
	uart_puthex(pstart);
	uart_puts(" ");
	uart_puthex(count);
	uart_puts("\n");

	int i;
	phys_t pcur;
	virt_t vcur;

	for (i = 0; i < count; i++) {
		vcur = vstart + (i << MMU_VIRT_SHIFT);
		pcur = pstart + (i << MMU_VIRT_SHIFT);
		mmu_map(vcur, pcur);
	}
}

void mmu_enable(void)
{
	int i;
	uart_puts("MMU Table at address ");
	uart_puthex((unsigned int)table);
	uart_puts("\n");
	for (i = 0; i < 4096; i++) {
		if (table[i] == 0)
			continue;

		uart_puthex(i << MMU_VIRT_SHIFT);
		uart_puts(": ");
		uart_puthex(table[i]);
		uart_puts("\n");
	}

	/* set table address and enable mmu */
	REG32(MMU_TTB) = (uint32_t)table;
	REG32(MMU_CNTL) = CNTL_TWLENABLE | CNTL_MMUENABLE;
}

