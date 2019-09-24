#ifndef MMU_H_
#define MMU_H_

#include <linux/types.h>

int mmu_init(void);
void mmu_release(void);
void mmu_map(uint32_t virt, uint32_t phys);
void mmu_map_range(uint32_t vstart, uint32_t pstart, unsigned int count);
void mmu_enable(void);

/* XXX these probably shouldn't be here but it is convenient for the moment */
void mmu_enable_m3(void);
void mmu_disable_m3(void);

#endif
