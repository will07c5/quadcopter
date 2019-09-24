#ifndef MMU_H_
#define MMU_H_

#include <util.h>

void mmu_init(void);
void mmu_map(virt_t virt, phys_t phys);
void mmu_map_range(virt_t vstart, phys_t pstart, unsigned int count);
void mmu_enable(void);

#endif
