/*
 * This is a demo Linux kernel module.
 */

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/signal.h>
#include <asm/siginfo.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "efm32gg.h"


/* Constants */
#define DEVICE_NAME "GPIOGamepad"
#define DEVICE_COUNT 1

dev_t device_number;
uint32_t last_data;
struct cdev gamepad_cdev;
struct class* device_class;
struct fasync_struct* async_queue; /* (LLD p. 170) */

/* Method Prototypes */
irqreturn_t gamepad_interrupt_handler(int irq, void* dev_id, struct pt_regs* regs);
static int gamepad_open(struct inode* inode, struct file* filp);
int gamepad_release(struct inode *inode, struct file *filp);
static ssize_t gamepad_read(struct file* filp, char* __user buff, size_t count, loff_t* offp);
static ssize_t gamepad_write(struct file* filp, char* __user buff, size_t count, loff_t* offp);
static int gamepad_fasync(int fd, struct file* filp, int mode);

/* Avaiable operations on this driver */
struct file_operations gamepad_fops = {
	.owner = THIS_MODULE,
	.open = gamepad_open,
	.release = gamepad_release,
	.read = gamepad_read,
	.write = gamepad_write,
	.fasync = gamepad_fasync
};

/*
 * Function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init gamepad_init(void)
{
	int value;
	printk(KERN_ALERT "Module initialization begin.\n");


	/* Obtain device numbers (LDD p. 45) */
	value = alloc_chrdev_region(&device_number, 0, DEVICE_COUNT, DEVICE_NAME);
	if (value < 0)
	{
		printk(KERN_ERR "Failed to dynamically-allocate device numbers.");
		return -1;
	}

	/* Create device node */
	device_class = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(device_class, NULL, device_number, NULL, DEVICE_NAME);

	/* Char device registration (LDD p. 55) */
	cdev_init(&gamepad_cdev, &gamepad_fops);
	gamepad_cdev.owner = THIS_MODULE;
	cdev_add(&gamepad_cdev, device_number, DEVICE_COUNT);

	/* Request reservation of GPIO pins used by gamepad, preventing overlapping calls from other drivers */
	if (request_mem_region(GPIO_PC_BASE, 0x24, DEVICE_NAME) == NULL)
	{
		printk(KERN_ALERT "Failed to reserve GPIO_PC_BASE in memory.\n");
		return -1;
	}

	/* Setup GPIO as written on page 28 in the Lab Exercise Document */
	iowrite32(0x33333333, GPIO_PC_MODEL);
	iowrite32(0xff, GPIO_PC_DOUT);

	/* Register interrupt handlers. GPIO_EVEN = 17 and GPIO_ODD = 18 */
	value = request_irq(17, (irq_handler_t)gamepad_interrupt_handler, 0, DEVICE_NAME, &gamepad_cdev);
	if (value < 0)
	{
		printk(KERN_ERR "Failed to request irq even.");
		return -1;
	}
	value = request_irq(18, (irq_handler_t)gamepad_interrupt_handler, 0, DEVICE_NAME, &gamepad_cdev);
	if (value < 0)
	{
		printk(KERN_ERR "Failed to request irq odd.");
		return -1;
	}

	/* Setup GPIO interrupts as written on page 28 in the Lab Exercise Document */
	iowrite32(0x22222222, GPIO_EXTIPSELL);
	iowrite32(0xff, GPIO_EXTIFALL);
	iowrite32(0xff, GPIO_EXTIRISE);
	iowrite32(0xff, GPIO_IEN);
	//iowrite32(0x802, ISER0); /* Enables interrupt handling */


	printk(KERN_ALERT "Module initialization done.\n");
	return 0;
}

/*
 * Function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit gamepad_exit(void)
{
	printk(KERN_ALERT "Exit starting.\n");

	/* Disable interrupts */
	iowrite32(0x0000, GPIO_IEN);

	printk(KERN_DEBUG "Freeing interrupts.\n");
	free_irq(17, &gamepad_cdev);
	free_irq(18, &gamepad_cdev);

	printk(KERN_DEBUG "Releasing reservation of GPIO pins used by gamepad.\n");
	release_mem_region(GPIO_PC_BASE, 0x24);

	printk(KERN_DEBUG "Removing char device from system.\n");
	device_destroy(device_class, device_number);
	class_destroy(device_class);
	cdev_del(&gamepad_cdev);

	printk(KERN_DEBUG "Deallocating device numbers.\n");
	unregister_chrdev_region(device_number, DEVICE_COUNT);

	//printk(KERN_DEBUG "Remove this filp from the asynchronously notified filp's.\n");
	//gamepad_fasync(-1, flip, 0);

	printk(KERN_ALERT "Exit done.\n");
}

/* Interrupt handler between hardware and the driver */
irqreturn_t gamepad_interrupt_handler(int irq, void* dev_id, struct pt_regs* regs)
{
	iowrite32(ioread32(GPIO_IF), GPIO_IFC); /* Clear interrupt */
	last_data = ioread32(GPIO_PC_DIN);
	if (async_queue)
	{
		kill_fasync(&async_queue, SIGIO, POLL_IN);
	}
	return IRQ_HANDLED;
}

/* --- This is the functions that can be called from user space (from LDD p. 58) --- */

/* Not useful since all configurations is done in gamepad_init */
static int gamepad_open(struct inode* inode, struct file* filp)
{
	printk(KERN_INFO "Gamepad driver opened.\n");
	return 0;
}

/* Not useful since all releasing is done in gamepad_exit */
/* Remove this filp from the asynchronously notified filp's. */
int gamepad_release(struct inode *inode, struct file *filp)
{
  printk(KERN_INFO "Gamepad driver released.\n");
	return fasync_helper(-1, filp, 0, &async_queue);
}

/* Read buttons status and copy from kernel space to user space */
static ssize_t gamepad_read(struct file* filp, char* __user buff, size_t count, loff_t* offp)
{
    int value = copy_to_user(buff, &last_data, 1);
	if (value > 0)
	{
		printk(KERN_ALERT "Could not copy all bytes from GPIO_PC_DIN.\n");
		return -1;
	}
    return 1;
}

/* Do not use this as it's not necessary to write to buttons. */
static ssize_t gamepad_write(struct file* filp, char* __user buff, size_t count, loff_t* offp)
{
    printk(KERN_INFO "Writing...");
    return 0;
}

/* Setup asynchronous notification to user space */
static int gamepad_fasync(int fd, struct file* filp, int mode)
{
    return fasync_helper(fd, filp, mode, &async_queue);
}

/* Module configurations */
module_init(gamepad_init);
module_exit(gamepad_exit);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_VERSION("v1.0.0");
MODULE_LICENSE("GPL");
