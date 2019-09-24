#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "../common/quadcopter_ioctl.h"
#include "../common/memory.h"
#include "../common/ringbuf.h"
#include "mmu.h"
#include "firmware.h"

MODULE_LICENSE("Dual BSD/GPL");

static struct ringbuf rb_tx;
static struct ringbuf rb_rx;
static void *tx_buf;
static void *rx_buf;


#define SUCCESS 0

static int quadcopter_open(struct inode *inode, struct file *filp)
{
	return SUCCESS;
}

static int quadcopter_release(struct inode *inode, struct file *filp)
{
	return SUCCESS;
}

static ssize_t quadcopter_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	char kbuf[512];
	size_t read_count = 0;
	int rb_count;

	while (read_count < count) {
		rb_count = ringbuf_read(&rb_rx, kbuf, 512);

		if (rb_count <= 0)
			break;

		if (copy_to_user(buf + read_count, kbuf, rb_count) < 0)
			return -EINVAL;

		read_count += rb_count;
	}

	return read_count;
}

static ssize_t quadcopter_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	char kbuf[512];
	size_t write_count = 0, count_left, to_write;
	int rb_count;

	count_left = count;

	while (write_count < count) {
		if (count_left > 512)
			to_write = 512;
		else
			to_write = count_left;

		count_left -= to_write;
		
		if (copy_from_user(kbuf, buf + write_count, to_write) < 0)
			return -EINVAL;

		rb_count = ringbuf_write(&rb_tx, kbuf, to_write);

		write_count += rb_count;

		if (rb_count < to_write)
			break;
	}

	return write_count;
}

static long quadcopter_ioctl(struct file *filp, unsigned int ioctl_num, unsigned long ioctl_param)
{
	printk(KERN_INFO "quadcopter: doing an ioctl %x\n", ioctl_num);
	switch (ioctl_num) {
	case QUADCOPTER_STOP:
		printk(KERN_INFO "quadcopter: stopping m3\n");
		mmu_disable_m3();
		firmware_enable(); /* alright to load firmware again */
		break;
	case QUADCOPTER_START:
		printk(KERN_INFO "quadcopter: programming mmu\n");
		/* TODO change these to macros */

		mmu_init();

		/* map 2MB program region */
		QUADCOPTER_MAP_RANGE(PROGRAM);

		/* map 1MB stack region */
		QUADCOPTER_MAP_RANGE(STACK);

		/* map peripheral regions */
		QUADCOPTER_MAP_RANGE(PER);
		QUADCOPTER_MAP_RANGE(CFG);

		/* map internal m3 config registers */
		QUADCOPTER_MAP_RANGE(M3_PRIVATE);

		/* map quadcopter -> linux */
		QUADCOPTER_MAP_RANGE(TX);

		/* map linux -> quadcopter */
		QUADCOPTER_MAP_RANGE(RX);

		/* map trace buffer */
		QUADCOPTER_MAP_RANGE(TRACE);

		mmu_enable();

		firmware_disable(); /* prevent reloading firmware while running */

		printk(KERN_INFO "quadcopter: starting m3\n");

		mmu_enable_m3();
		break;
	}

	return SUCCESS;
}


struct file_operations fops_ctrl = {
	.read = quadcopter_read,
	.write = quadcopter_write,
	.compat_ioctl = quadcopter_ioctl,
	.unlocked_ioctl = quadcopter_ioctl,
	.open = quadcopter_open,
	.release = quadcopter_release
};

static struct miscdevice misc_ctrl = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "quadcopter-ctrl",
	.fops = &fops_ctrl
};


static int __init quadcopter_init(void)
{
	int status;
	struct resource *quad_res = NULL;

	printk(KERN_INFO "quadcopter: loaded module\n");
	
	status = misc_register(&misc_ctrl);
	if (status < 0) {
		printk(KERN_ALERT "quadcopter: failed to register device\n");
		return status;
	}

	printk(KERN_INFO "quadcopter: registered control device with minor = %d\n", misc_ctrl.minor);

	status = mmu_init();
	if (status < 0) {
		printk(KERN_ALERT "quadcopter: failed to init mmu\n");
		goto error;
	}

	quad_res = request_mem_region(QUADCOPTER_REGION, QUADCOPTER_LENGTH, "quadcopter");

	if (!quad_res) {
		printk(KERN_ALERT "quadcopter: failed to get config region\n");
		status = -EPERM;
		goto error;
	}

	status = firmware_init();
	if (status < 0) {
		printk(KERN_ALERT "quadcopter: failed to init firmware loader\n");
		goto error;
	}

	tx_buf = ioremap(QUADCOPTER_TX_PHYS, QUADCOPTER_LEN_BYTES(TX));
	rx_buf = ioremap(QUADCOPTER_RX_PHYS, QUADCOPTER_LEN_BYTES(RX));

	ringbuf_init(&rb_tx, tx_buf, QUADCOPTER_LEN_BYTES(TX));
	ringbuf_init(&rb_rx, rx_buf, QUADCOPTER_LEN_BYTES(RX));

	return 0;

error:
	if (quad_res)
		release_mem_region(QUADCOPTER_REGION, QUADCOPTER_LENGTH);

	misc_deregister(&misc_ctrl);

	return status;
}

static void __exit quadcopter_exit(void)
{
	int status;


	firmware_release();

	mmu_release();

	release_mem_region(QUADCOPTER_REGION, QUADCOPTER_LENGTH);
	
	printk(KERN_INFO "quadcopter: unloaded module\n");
	
	status = misc_deregister(&misc_ctrl);

	if (status < 0) {
		printk(KERN_ALERT "quadcopter: failed to deregister device\n");
	}
}

module_init(quadcopter_init);
module_exit(quadcopter_exit);
