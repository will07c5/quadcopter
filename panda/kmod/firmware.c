#include "firmware.h"

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

#include "../common/ringbuf.h"
#include "../common/memory.h"

static int firmware_enabled = 1;
static void *firmware_ptr = NULL;
static void *trace_ptr = NULL;
static struct ringbuf rb;

static int quadcopter_open(struct inode *inode, struct file *filp)
{
	filp->f_pos = 0;

	printk(KERN_INFO "quadcopter: opened firmware device\n");

	return 0;
}

static int quadcopter_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "quadcopter: closed firmware device with f_pos = %lld\n", filp->f_pos);

	return 0;
}

static ssize_t quadcopter_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	if (firmware_enabled) {
		if (*f_pos + count > QUADCOPTER_FIRMWARE_LENGTH)
			count = QUADCOPTER_FIRMWARE_LENGTH - *f_pos;

		/* XXX not portable and bad practice but I don't really care */
		if (copy_from_user(firmware_ptr + *f_pos, buf, count))
			return -EINVAL;

		*f_pos += count;

		return count;
	}

	return -EPERM;
}

static ssize_t quadcopter_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	return ringbuf_read(&rb, buf, count);
}

static struct file_operations fops_firmware = {
	.read = quadcopter_read,
	.write = quadcopter_write,
	.open = quadcopter_open,
	.release = quadcopter_release
};

static struct miscdevice misc_firmware = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "quadcopter-firmware",
	.fops = &fops_firmware
};

int firmware_init(void) 
{
	int status;

	status = misc_register(&misc_firmware);
	if (status < 0) {
		printk(KERN_ALERT "quadcopter: failed to register firmware device\n");
		return status;
	}

	printk(KERN_INFO "quadcopter: registered firmware device with minor = %d\n", misc_firmware.minor);

	firmware_ptr = ioremap(QUADCOPTER_FIRMWARE_REGION, QUADCOPTER_FIRMWARE_LENGTH);

	trace_ptr = ioremap(QUADCOPTER_TRACE_PHYS, QUADCOPTER_LEN_BYTES(TRACE));

	ringbuf_init(&rb, trace_ptr, QUADCOPTER_LEN_BYTES(TRACE)-2*sizeof(uint32_t));

	return 0;
}

void firmware_release(void)
{
	iounmap(firmware_ptr);

//	release_mem_region(QUADCOPTER_FIRMWARE_REGION, QUADCOPTER_FIRMWARE_LENGTH);

	misc_deregister(&misc_firmware);
}

void firmware_enable(void)
{
	firmware_enabled = 1;
}

void firmware_disable(void)
{
	firmware_enabled = 0;
}
