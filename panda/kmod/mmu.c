#include "mmu.h"

#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/module.h>

#include "../common/memory.h"

/* register offsets */
#define CM_MPU_M3_MPU_M3_CLKCTRL 0x220
#define CM_MPU_M3_CLKSTCTRL 0x200
#define RM_MPU_M3_RSTCTRL 0x210

#define MMU_TTB 0x4C
#define MMU_CNTL 0x44

#define MMU_VIRT_MASK 0xFFF00000
#define MMU_VIRT_SHIFT 20

/* register bit defines */
#define CNTL_TWLENABLE (1 << 2)
#define CNTL_MMUENABLE (1 << 1) 
#define M3_MMU_RESET (1 << 2)
#define M3_CPU1_RESET (1 << 0)
#define CLKTRCTRL_SW_WKUP 0x2
#define MODULEMODE (1 << 0)


/* These bits specify that the table entry is a section and that
 * there is no endianness conversion */
#define MMU_TABLE_SECTION 0xC02

static void __iomem *mmu_ptr = NULL;
static void __iomem *table_ptr = NULL;

static void *core_cm2_ptr = NULL;
static void *core_prm_ptr = NULL;

int mmu_init(void)
{
//	static struct resource *mmu_res = NULL;
//	static struct resource *table_res = NULL;

	/* XXX - it would probably be best to use the already existing iommu driver, 
	 * but for now just manipulate the mmu registers directly despite the iommu driver owning them */
#if 0
	mmu_res = request_mem_region(MMU_CONFIG_REGION, MMU_CONFIG_LENGTH, "cortex-m3-mpu");

	if (!mmu_res) {
		printk(KERN_ALERT "quadcopter: failed to get mmu mem region\n");
		return -1;
	}

	table_res = request_mem_region(MMU_TABLE_REGION, MMU_TABLE_LENGTH, "cortex-m3-mpu-table");

	if (!table_res) {
		printk(KERN_ALERT "quadcopter: failed to get mmu table mem region\n");
		return -EPERM; 
	}
#endif

	mmu_ptr = ioremap_nocache(MMU_CONFIG_REGION, MMU_CONFIG_LENGTH);
	core_cm2_ptr = ioremap_nocache(CORE_CM2_REGION, CORE_CM2_LENGTH);
	core_prm_ptr = ioremap_nocache(CORE_PRM_REGION, CORE_PRM_LENGTH);
	table_ptr = ioremap(MMU_TABLE_REGION, MMU_TABLE_LENGTH);

	memset_io(table_ptr, 0, MMU_TABLE_LENGTH);

	return 0;
}

void mmu_release(void)
{
	iounmap(mmu_ptr);
	iounmap(core_cm2_ptr);
	iounmap(core_prm_ptr);
	iounmap(table_ptr);

//	release_mem_region(MMU_CONFIG_REGION, MMU_CONFIG_LENGTH);
//	release_mem_region(MMU_TABLE_REGION, MMU_TABLE_LENGTH);
}

void mmu_map(uint32_t virt, uint32_t phys)
{
	printk(KERN_INFO "quadcopter: mapped %x -> %x\n", virt, phys);
	
	virt = ((virt & MMU_VIRT_MASK) >> MMU_VIRT_SHIFT) * 4;

	/* place entry in table (don't bother with second level entries) */
	iowrite32((phys & MMU_VIRT_MASK) | MMU_TABLE_SECTION, table_ptr + virt);
}

void mmu_map_range(uint32_t vstart, uint32_t pstart, unsigned int count)
{
	int i;
	uint32_t pcur;
	uint32_t vcur;

	for (i = 0; i < count; i++) {
		vcur = vstart + (i << MMU_VIRT_SHIFT);
		pcur = pstart + (i << MMU_VIRT_SHIFT);
		mmu_map(vcur, pcur);
	}
}

void mmu_enable(void)
{
	uint32_t reg;
	printk(KERN_INFO "quadcopter: enabling m3 mmu\n");

	/* bring the mmu out of reset */
	iowrite32(MODULEMODE, core_cm2_ptr + CM_MPU_M3_MPU_M3_CLKCTRL);
	iowrite32(CLKTRCTRL_SW_WKUP, core_cm2_ptr + CM_MPU_M3_CLKSTCTRL);

	reg = ioread32(core_prm_ptr + RM_MPU_M3_RSTCTRL);
	iowrite32(reg & ~M3_MMU_RESET, core_prm_ptr + RM_MPU_M3_RSTCTRL);

	/* set table address and enable mmu */
	iowrite32(MMU_TABLE_REGION, mmu_ptr + MMU_TTB);
	iowrite32(CNTL_TWLENABLE | CNTL_MMUENABLE, mmu_ptr + MMU_CNTL);
}

void mmu_enable_m3(void)
{
	int32_t reg = ioread32(core_prm_ptr + RM_MPU_M3_RSTCTRL);
	iowrite32(reg & ~M3_CPU1_RESET, core_prm_ptr + RM_MPU_M3_RSTCTRL);
}

void mmu_disable_m3(void)
{
	int32_t reg = ioread32(core_prm_ptr + RM_MPU_M3_RSTCTRL);
	iowrite32(reg | M3_CPU1_RESET, core_prm_ptr + RM_MPU_M3_RSTCTRL);
}
