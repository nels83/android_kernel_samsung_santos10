/*
 * platform_gpio_keys.c: gpio_keys platform data initilization file
 *
 * (C) Copyright 2008 Intel Corporation
 * Author:
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <linux/input.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/platform_device.h>
#include <asm/intel-mid.h>
#include "platform_gpio_keys.h"

/*
 * we will search these buttons in SFI GPIO table (by name)
 * and register them dynamically. Please add all possible
 * buttons here, we will shrink them if no GPIO found.
 */
static struct gpio_keys_button __gpio_button[] = {
	{KEY_POWER,		-1, 1, "power_btn",	EV_KEY, 0, 3000},
	{KEY_PROG1,		-1, 1, "prog_btn1",	EV_KEY, 0, 20},
	{KEY_PROG2,		-1, 1, "prog_btn2",	EV_KEY, 0, 20},
	{SW_LID,		-1, 1, "lid_switch",	EV_SW,  0, 20},
	{KEY_VOLUMEUP,		-1, 1, "vol_up",	EV_KEY, 0, 20},
	{KEY_VOLUMEDOWN,	-1, 1, "vol_down",	EV_KEY, 0, 20},
	{KEY_CAMERA,		-1, 1, "camera_full",	EV_KEY, 0, 20},
	{KEY_CAMERA_FOCUS,	-1, 1, "camera_half",	EV_KEY, 0, 20},
	{SW_KEYPAD_SLIDE,	-1, 1, "MagSw1",	EV_SW,  0, 20},
	{SW_KEYPAD_SLIDE,	-1, 1, "MagSw2",	EV_SW,  0, 20},
	{KEY_CAMERA,		-1, 1, "cam_capture",	EV_KEY, 0, 20},
	{KEY_CAMERA_FOCUS,	-1, 1, "cam_focus",	EV_KEY, 0, 20},
	{KEY_MENU,              -1, 1, "fp_menu_key",   EV_KEY, 0, 20},
	{KEY_HOME,              -1, 1, "fp_home_key",   EV_KEY, 0, 20},
	{KEY_SEARCH,            -1, 1, "fp_search_key", EV_KEY, 0, 20},
	{KEY_BACK,              -1, 1, "fp_back_key",   EV_KEY, 0, 20},
	{KEY_VOLUMEUP,          -1, 1, "volume_up",     EV_KEY, 0, 20},
	{KEY_VOLUMEDOWN,        -1, 1, "volume_down",   EV_KEY, 0, 20},
	{KEY_MUTE,              -1, 1, "mute_enable",   EV_KEY, 0, 20},
	{KEY_CAMERA,            -1, 1, "camera0_sb1",   EV_KEY, 0, 20},
	{KEY_CAMERA_FOCUS,      -1, 1, "camera0_sb2",   EV_KEY, 0, 20},
};

static struct gpio_keys_button *gpio_button = __gpio_button;
static size_t nr_button = ARRAY_SIZE(__gpio_button);

static struct gpio_keys_platform_data gpio_keys = {
	.rep		= 1,
	.nbuttons	= -1, /* will fill it after search */
};

static struct platform_device pb_device = {
	.name		= DEVICE_NAME,
	.id		= -1,
	.dev		= {
		.platform_data	= &gpio_keys,
	},
};

/*
 * Shrink the non-existent buttons, register the gpio button
 * device if there is some
 */
static int __init pb_keys_init(void)
{
	struct gpio_keys_button *gb = gpio_button;
	int i, good = 0;

	for (i = 0; i < nr_button; i++) {
		gb[i].gpio = get_gpio_by_name(gb[i].desc);
		pr_info("info[%2d]: name = %s, gpio = %d\n",
			 i, gb[i].desc, gb[i].gpio);
		if (gb[i].gpio == -1)
			continue;

		if (i != good)
			gb[good] = gb[i];
		good++;
	}

	if (good) {
		gpio_keys.buttons = gpio_button;
		gpio_keys.nbuttons = good;
		return platform_device_register(&pb_device);
	}
	return 0;
}
late_initcall(pb_keys_init);

void __init intel_mid_customize_gpio_keys(struct gpio_keys_button *button,
		size_t num)
{
	gpio_button = button;
	nr_button = num;
}
