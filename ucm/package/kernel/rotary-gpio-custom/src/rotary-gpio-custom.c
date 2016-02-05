/*
 *  Custom GPIO-based rotary driver
 *
 *  Copyright (C) 2010 Claudio Mignanti <c.mignanti@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  Strongly based on Custom GPIO-based I2C driver by:
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 * ---------------------------------------------------------------------------
 *
 *  The behaviour of this driver can be altered by setting some parameters
 *  from the insmod command line.
 *
 *  The following parameters are adjustable:
 *
 *	bus0	These four arguments can be arrays of
 *	bus1	1-8 unsigned integers as follows:
 *	bus2
 *	bus3	<id>,<steps>,<axis>,<gpioa>,<gpiob>,<inverted>
 *
 *
 *  If this driver is built into the kernel, you can use the following kernel
 *  command line parameters, with the same values as the corresponding module
 *  parameters listed above:
 *
 *	rotary-gpio-custom.bus0
 *	rotary-gpio-custom.bus1
 *	rotary-gpio-custom.bus2
 *	rotary-gpio-custom.bus3
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/rotary_encoder.h>

#define DRV_NAME	"rotary-gpio-custom"
#define DRV_DESC	"Custom GPIO-based rotary driver"
#define DRV_VERSION	"0.1.0"

#define PFX		DRV_NAME ": "

#define BUS_PARAM_REQUIRED	5
#define BUS_PARAM_COUNT		6
#define BUS_COUNT_MAX		4

static unsigned int bus0[BUS_PARAM_COUNT] __initdata;
static unsigned int bus1[BUS_PARAM_COUNT] __initdata;
static unsigned int bus2[BUS_PARAM_COUNT] __initdata;
static unsigned int bus3[BUS_PARAM_COUNT] __initdata;

static unsigned int bus_nump[BUS_COUNT_MAX] __initdata;

#define BUS_PARM_DESC \
	" config -> id,steps,axis,gpioa,gpiob[,inverted]"

module_param_array(bus0, uint, &bus_nump[0], 0);
MODULE_PARM_DESC(bus0, "bus0" BUS_PARM_DESC);
module_param_array(bus1, uint, &bus_nump[1], 0);
MODULE_PARM_DESC(bus1, "bus1" BUS_PARM_DESC);
module_param_array(bus2, uint, &bus_nump[2], 0);
MODULE_PARM_DESC(bus2, "bus2" BUS_PARM_DESC);
module_param_array(bus3, uint, &bus_nump[3], 0);
MODULE_PARM_DESC(bus3, "bus3" BUS_PARM_DESC);

static struct platform_device *devices[BUS_COUNT_MAX];
static unsigned int nr_devices;

static void rotary_gpio_custom_cleanup(void)
{
	int i;

	for (i = 0; i < nr_devices; i++)
		if (devices[i])
			platform_device_put(devices[i]);
}

static int __init rotary_gpio_custom_add_one(unsigned int id,
					     unsigned int *params)
{
	struct platform_device *pdev;
	struct rotary_encoder_platform_data pdata;
	int err;

	if (!bus_nump[id])
		return 0;

	if (bus_nump[id] < BUS_PARAM_REQUIRED) {
		printk(KERN_ERR PFX "not enough parameters for bus%d\n", id);
		err = -EINVAL;
		goto err;
	}

	pdev = platform_device_alloc("rotary-gpio", params[0]);
	if (!pdev) {
		err = -ENOMEM;
		goto err;
	}

	pdata.steps = params[1];
	pdata.axis = params[2];
	pdata.relative_axis = false;
	pdata.rollover = false;
	pdata.gpio_a = params[3];
	pdata.gpio_b = params[4];

	if (params[5] == 1) {
		pdata.inverted_a = 1;
		pdata.inverted_b = 1;
	} else {
		pdata.inverted_a = 0;
		pdata.inverted_b = 0;
	}

	err = platform_device_add_data(pdev, &pdata, sizeof(pdata));
	if (err)
		goto err_put;

	err = platform_device_add(pdev);
	if (err)
		goto err_put;

	devices[nr_devices++] = pdev;
	return 0;

err_put:
	platform_device_put(pdev);
err:
	return err;
}

static int __init rotary_gpio_custom_probe(void)
{
	int err;

	printk(KERN_INFO DRV_DESC " version " DRV_VERSION "\n");

	err = rotary_gpio_custom_add_one(0, bus0);
	if (err)
		goto err;

	err = rotary_gpio_custom_add_one(1, bus1);
	if (err)
		goto err;

	err = rotary_gpio_custom_add_one(2, bus2);
	if (err)
		goto err;

	err = rotary_gpio_custom_add_one(3, bus3);
	if (err)
		goto err;

	if (!nr_devices) {
		printk(KERN_ERR PFX "no bus parameter(s) specified\n");
		err = -ENODEV;
		goto err;
	}

	return 0;

err:
	rotary_gpio_custom_cleanup();
	return err;
}

#ifdef MODULE
static int __init rotary_gpio_custom_init(void)
{
	return rotary_gpio_custom_probe();
}
module_init(rotary_gpio_custom_init);

static void __exit rotary_gpio_custom_exit(void)
{
	rotary_gpio_custom_cleanup();
}
module_exit(rotary_gpio_custom_exit);
#else
subsys_initcall(rotary_gpio_custom_probe);
#endif /* MODULE*/

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org >");
MODULE_AUTHOR("Claudio Mignanti <c.mignanti@gmail.com>");
MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
