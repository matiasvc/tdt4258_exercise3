/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <errno.h>
#include "efm32gg.h"

#define NAME = "GPIOGamepad"

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init gamepad_init(void)
{
	printk(KERN_ALERT "Init starting\n");

	// Request reservation of GPIO pins used by gamepad, preventing overlapping calls from other drivers
	if (request_mem_region(GPIO_PA_BASE, 0x24, NAME) == NULL)
	{
		printk(KERN_ALERT "Failed to reserve GPIO_PA_BASE\n");
		return EIO; // I/O error, error code 5
	}

	// TODO????? Enable GPIO clock here?

	// Setup GPIO as written on page 28 in the Lab Exercise Document
	iowrite32(0x33333333, GPIO_PC_MODEL);
	iowrite32(0xff, GPIO_PC_DOUT);

	// TODO: Create interrupt handlers here

	// Setup GPIO interrupts as written on page 28 in the Lab Exercise Document
	iowrite32(0x22222222, GPIO_EXTIPSELL);
	iowrite32(0xff, GPIO_EXTIFALL);
	iowrite32(0xff, GPIO_EXTIRISE);
	iowrite32(0xff, GPIO_IEN);
	iowrite32(0x802, ISER0); // enables interrupt handling

	// TODO: do more stuff here


	printk(KERN_ALERT "Init done\n");
	return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit gamepad_exit(void)
{
	printk(KERN_ALERT "Exit starting\n");

	// Release reservation of GPIO pins used by gamepad
	release_mem_region(GPIO_PA_BASE, 0x24);

	printk(KERN_ALERT "Exit done\n");
}

module_init(gamepad_init);
module_exit(gamepad_exit);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");
