/*
 * platform_mhl.c: MHL platform data header file
 *
 * (C) Copyright 2013 Samsung Corporation
 * Author:
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/irq.h>
#include <linux/delay.h>
#ifdef CONFIG_SAMSUNG_MHL_92X4
#include <linux/sii9234.h>
#endif
#ifdef CONFIG_SAMSUNG_MHL_8246
#include <linux/sii8246.h>
#endif

#include <linux/input.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/lnw_gpio.h>
#include <linux/power_supply.h>
#include <asm/intel-mid.h>
#include <asm/intel_scu_ipcutil.h>
#include <asm/intel_scu_flis.h>
#include <asm/intel_muxtbl.h>
#include <asm/sec_muxtbl.h>
#include "platform_mhl.h"

#ifndef GPIO_LEVEL_LOW
#define GPIO_LEVEL_LOW 0
#endif

#ifndef GPIO_LEVEL_HIGH
#define GPIO_LEVEL_HIGH 1
#endif

#ifdef CONFIG_SAMSUNG_MHL
#define GPIO_HDMI_EN get_gpio_by_name("HDMI_EN")
#define GPIO_MHL_RST get_gpio_by_name("MHL_RST")
#define GPIO_MHL_INT get_gpio_by_name("MHL_INT")

static BLOCKING_NOTIFIER_HEAD(acc_notifier);

int acc_register_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&acc_notifier, nb);
}

int acc_unregister_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&acc_notifier, nb);
}

int acc_notify(int event)
{
	return blocking_notifier_call_chain(&acc_notifier, event, NULL);
}


#if defined(CONFIG_SAMSUNG_MHL_8246)
static void sii8246_cfg_gpio(void)
{
	int ret;

	ret = gpio_request(GPIO_HDMI_EN, "HDMI_EN");
	if (ret) {
		pr_err("%s: failed to request gpio(pin %d)\n", __func__,
				GPIO_HDMI_EN);
		return;
	}
	ret = gpio_direction_output(GPIO_HDMI_EN, GPIO_LEVEL_LOW);
	if (ret < 0) {
		pr_err("%s: fail to set direction gpio %d\n", __func__,
				GPIO_HDMI_EN);
		gpio_free(GPIO_HDMI_EN);
		return;
	}

	ret = gpio_request(GPIO_MHL_RST, "MHL_RST");
	if (ret) {
		pr_err("%s: failed to request gpio(pin %d)\n", __func__,
				GPIO_MHL_RST);
		return;
	}
	ret = gpio_direction_output(GPIO_MHL_RST, GPIO_LEVEL_LOW);
	if (ret < 0) {
		pr_err("%s: fail to set direction gpio %d\n", __func__,
				GPIO_MHL_RST);
		gpio_free(GPIO_MHL_RST);
		return;
	}

	ret = gpio_request(GPIO_MHL_INT, "MHL_INT");
	if (ret) {
		pr_err("%s: failed to request gpio(pin %d)\n", __func__,
				GPIO_MHL_INT);
		return;
	}
}

static void sii8246_power_onoff(bool on)
{

	pr_info("%s(%d)\n", __func__, on);
	if (on) {
		gpio_set_value(GPIO_HDMI_EN, GPIO_LEVEL_HIGH);

	} else {
		gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_LOW);
		usleep_range(10000, 20000);
		gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_HIGH);

		gpio_set_value(GPIO_HDMI_EN, GPIO_LEVEL_LOW);

		gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_LOW);

	}
}

static void sii8246_reset(void)
{
	pr_info("%s()\n", __func__);

	gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_LOW);
	usleep_range(10000, 20000);
	gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_HIGH);
}

int get_mhl_8246_int_irq(void)
{

	pr_info("%s, gpio: %d, irq:%d\n", __func__,
				 GPIO_MHL_INT, gpio_to_irq(GPIO_MHL_INT));
	return gpio_to_irq(GPIO_MHL_INT);
}

static void mhl_8246_usb_switch_control(bool on)
{

	pr_info("%s() [MHL] USB path change : %s\n",
	       __func__, on ? "MHL" : "USB");
}

static void muic77693_mhl_8246_cb(bool on, int charger)
{
	union power_supply_propval value;
	int i, ret = 0;
	struct power_supply *psy;
	int current_cable_type = POWER_SUPPLY_TYPE_MISC;
	int sub_type = ONLINE_SUB_TYPE_MHL;
	int power_type = ONLINE_POWER_TYPE_UNKNOWN;

	pr_info("%s, on: %d charger: %d\n", __func__, on, charger);
	if (on) {
		switch (charger) {
		case 0:
			power_type = ONLINE_POWER_TYPE_MHL_500;
			break;
		case 1:
			power_type = ONLINE_POWER_TYPE_MHL_900;
			break;
		case 2:
			power_type = ONLINE_POWER_TYPE_MHL_1500;
			break;
		case 3:
		default:
			power_type = ONLINE_POWER_TYPE_USB;
		}
	} else {
		current_cable_type = POWER_SUPPLY_TYPE_BATTERY;
	}

	for (i = 0; i < 10; i++) {
		psy = power_supply_get_by_name("battery");
		if (psy)
			break;
	}
	if (i == 10) {
		pr_err("%s: fail to get battery ps\n", __func__);
		return;
	}

	value.intval = current_cable_type<<ONLINE_TYPE_MAIN_SHIFT
		| sub_type<<ONLINE_TYPE_SUB_SHIFT
		| power_type<<ONLINE_TYPE_PWR_SHIFT;
	ret = psy->set_property(psy, POWER_SUPPLY_PROP_ONLINE, &value);
	if (ret) {
		pr_err("%s: fail to set power_suppy ONLINE property(%d)\n",
		__func__, ret);
	}
}

static struct sii8246_platform_data sii8246_pdata = {
	.init = sii8246_cfg_gpio,
	.hw_onoff = sii8246_power_onoff,
	.hw_reset = sii8246_reset,
	.get_irq = get_mhl_8246_int_irq,
	.reg_notifier   = acc_register_notifier,
	.unreg_notifier = acc_unregister_notifier,
	.vbus_present = muic77693_mhl_8246_cb,
#ifdef CONFIG_EXTCON
	.extcon_name = "max77693-muic"
#endif
};

void *sii8246_platform_data(void *info)
{

	pr_info("(%s:%d)\n", __func__, __LINE__);
	return &sii8246_pdata;
}
#endif

#if defined(CONFIG_SAMSUNG_MHL_92X4)
static void sii9234_cfg_gpio(void)
{
	int ret;

	ret = gpio_request(GPIO_HDMI_EN, "HDMI_EN");
	if (ret) {
		pr_err("%s: failed to request gpio(pin %d)\n", __func__,
				GPIO_HDMI_EN);
		return;
	}
	ret = gpio_direction_output(GPIO_HDMI_EN, GPIO_LEVEL_LOW);
	if (ret < 0) {
		pr_err("%s: fail to set direction gpio %d\n", __func__,
				GPIO_HDMI_EN);
		gpio_free(GPIO_HDMI_EN);
		return;
	}

	ret = gpio_request(GPIO_MHL_RST, "MHL_RST");
	if (ret) {
		pr_err("%s: failed to request gpio(pin %d)\n", __func__,
				GPIO_MHL_RST);
		return;
	}
	ret = gpio_direction_output(GPIO_MHL_RST, GPIO_LEVEL_LOW);
	if (ret < 0) {
		pr_err("%s: fail to set direction gpio %d\n", __func__,
				GPIO_MHL_RST);
		gpio_free(GPIO_MHL_RST);
		return;
	}

	ret = gpio_request(GPIO_MHL_INT, "MHL_INT");
	if (ret) {
		pr_err("%s: failed to request gpio(pin %d)\n", __func__,
				GPIO_MHL_INT);
		return;
	}
}

static void sii9234_power_onoff(bool on)
{
	struct intel_muxtbl *hdmi_hpd_muxtbl =
		intel_muxtbl_find_by_net_name("HDMI_HPD");
	if (!hdmi_hpd_muxtbl) {
		pr_err("%s: failed to get HDMI_HPD muxbl!\n", __func__);
		return;
	}

	pr_info("%s(%d)\n", __func__, on);
	if (on) {
		/* To avoid floating state of the HPD pin *
		 * in the absence of external pull-up     */
		if (config_pin_flis(hdmi_hpd_muxtbl->mux.pinname, PULL, NONE))
			pr_err("%s: failed to configure HDMI_HPD pull-none!\n",
				       __func__);

		gpio_set_value(GPIO_HDMI_EN, GPIO_LEVEL_HIGH);

	} else {
		gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_LOW);
		usleep_range(10000, 20000);
		gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_HIGH);

		/* To avoid floating state of the HPD pin *
		 * in the absence of external pull-up     */
		if (config_pin_flis(hdmi_hpd_muxtbl->mux.pinname, PULL,
					DOWN_20K))
			pr_err("%s: failed to configure pull value!\n",
				       __func__);

		gpio_set_value(GPIO_HDMI_EN, GPIO_LEVEL_LOW);

		gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_LOW);

	}
}

static void sii9234_reset(void)
{
	pr_info("%s()\n", __func__);

	gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_LOW);
	usleep_range(10000, 20000);
	gpio_set_value(GPIO_MHL_RST, GPIO_LEVEL_HIGH);
}

int get_mhl_int_irq(void)
{
	pr_info("%s, gpio: %d, irq:%d\n", __func__,
				 GPIO_MHL_INT, gpio_to_irq(GPIO_MHL_INT));
	return gpio_to_irq(GPIO_MHL_INT);
}

static void mhl_usb_switch_control(bool on)
{
	pr_info("%s() [MHL] USB path change : %s\n",
	       __func__, on ? "MHL" : "USB");
}

static void muic77693_mhl_cb(bool on, int charger)
{
	union power_supply_propval value;
	int i, ret = 0;
	struct power_supply *psy;
	int current_cable_type = POWER_SUPPLY_TYPE_MISC;
	int sub_type = ONLINE_SUB_TYPE_MHL;
	int power_type = ONLINE_POWER_TYPE_UNKNOWN;

	pr_info("%s, on: %d charger: %d\n", __func__, on, charger);
	if (on) {
		switch (charger) {
		case 0:
			power_type = ONLINE_POWER_TYPE_MHL_500;
			break;
		case 1:
			power_type = ONLINE_POWER_TYPE_MHL_900;
			break;
		case 2:
			power_type = ONLINE_POWER_TYPE_MHL_1500;
			break;
		case 3:
		default:
			power_type = ONLINE_POWER_TYPE_USB;
		}
	} else {
		current_cable_type = POWER_SUPPLY_TYPE_BATTERY;
	}

	for (i = 0; i < 10; i++) {
		psy = power_supply_get_by_name("battery");
		if (psy)
			break;
	}
	if (i == 10) {
		pr_err("%s: fail to get battery ps\n", __func__);
		return;
	}

	value.intval = current_cable_type<<ONLINE_TYPE_MAIN_SHIFT
		| sub_type<<ONLINE_TYPE_SUB_SHIFT
		| power_type<<ONLINE_TYPE_PWR_SHIFT;
	ret = psy->set_property(psy, POWER_SUPPLY_PROP_ONLINE, &value);
	if (ret) {
		pr_err("%s: fail to set power_suppy ONLINE property(%d)\n",
		__func__, ret);
	}
}

static struct sii9234_platform_data sii9234_pdata = {
	.init = sii9234_cfg_gpio,
	.hw_onoff = sii9234_power_onoff,
	.hw_reset = sii9234_reset,
	.get_irq = get_mhl_int_irq,
	.reg_notifier   = acc_register_notifier,
	.unreg_notifier = acc_unregister_notifier,
	.vbus_present = muic77693_mhl_cb,
#ifdef CONFIG_EXTCON
	.extcon_name = "max77693-muic"
#endif
};

void *sii9234_platform_data(void *info)
{
	pr_info("(%s:%d)\n", __func__, __LINE__);
	return &sii9234_pdata;
}
#endif

#endif
