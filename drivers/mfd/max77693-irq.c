/*
 * max77693-irq.c - Interrupt controller support for MAX77693
 *
 * Copyright (C) 2012 Samsung Electronics Co.Ltd
 * SangYoung Son <hello.son@samsung.com>
 *
 * This program is not provided / owned by Maxim Integrated Products.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This driver is based on max8997-irq.c
 */

#include <linux/err.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/irqdomain.h>
#include <linux/mfd/max77693.h>
#include <linux/mfd/max77693-private.h>

static const u8 max77693_mask_reg[] = {
	[LED_INT] = MAX77693_LED_REG_FLASH_INT_MASK,
	[TOPSYS_INT] = MAX77693_PMIC_REG_TOPSYS_INT_MASK,
	[CHG_INT] = MAX77693_CHG_REG_CHG_INT_MASK,
	[MUIC_INT1] = MAX77693_MUIC_REG_INTMASK1,
	[MUIC_INT2] = MAX77693_MUIC_REG_INTMASK2,
	[MUIC_INT3] = MAX77693_MUIC_REG_INTMASK3,
};

static struct regmap *max77693_get_regmap(struct max77693_dev *max77693,
				enum max77693_irq_source src)
{
	switch (src) {
	case LED_INT ... CHG_INT:
		return max77693->regmap;
	case MUIC_INT1 ... MUIC_INT3:
		return max77693->regmap_muic;
	default:
		return ERR_PTR(-EINVAL);
	}
}

struct max77693_irq_data {
	int mask;
	enum max77693_irq_source group;
};

#define DECLARE_IRQ(idx, _group, _mask)	\
	[(idx)] = { .group = (_group), .mask = (_mask) }
static const struct max77693_irq_data max77693_irqs[] = {
	DECLARE_IRQ(MAX77693_LED_IRQ_FLED2_OPEN,	LED_INT, 1 << 0),
	DECLARE_IRQ(MAX77693_LED_IRQ_FLED2_SHORT,	LED_INT, 1 << 1),
	DECLARE_IRQ(MAX77693_LED_IRQ_FLED1_OPEN,	LED_INT, 1 << 2),
	DECLARE_IRQ(MAX77693_LED_IRQ_FLED1_SHORT,	LED_INT, 1 << 3),
	DECLARE_IRQ(MAX77693_LED_IRQ_MAX_FLASH,		LED_INT, 1 << 4),

	DECLARE_IRQ(MAX77693_TOPSYS_IRQ_T120C_INT,	TOPSYS_INT, 1 << 0),
	DECLARE_IRQ(MAX77693_TOPSYS_IRQ_T140C_INT,	TOPSYS_INT, 1 << 1),
	DECLARE_IRQ(MAX77693_TOPSYS_IRQ_LOWSYS_INT,	TOPSYS_INT, 1 << 3),

	DECLARE_IRQ(MAX77693_CHG_IRQ_BYP_I,		CHG_INT, 1 << 0),
	DECLARE_IRQ(MAX77693_CHG_IRQ_THM_I,		CHG_INT, 1 << 2),
	DECLARE_IRQ(MAX77693_CHG_IRQ_BAT_I,		CHG_INT, 1 << 3),
	DECLARE_IRQ(MAX77693_CHG_IRQ_CHG_I,		CHG_INT, 1 << 4),
	DECLARE_IRQ(MAX77693_CHG_IRQ_CHGIN_I,		CHG_INT, 1 << 6),

	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT1_ADC,		MUIC_INT1, 1 << 0),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT1_ADC_LOW,	MUIC_INT1, 1 << 1),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT1_ADC_ERR,	MUIC_INT1, 1 << 2),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT1_ADC1K,	MUIC_INT1, 1 << 3),

	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT2_CHGTYP,	MUIC_INT2, 1 << 0),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT2_CHGDETREUN,	MUIC_INT2, 1 << 1),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT2_DCDTMR,	MUIC_INT2, 1 << 2),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT2_DXOVP,	MUIC_INT2, 1 << 3),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT2_VBVOLT,	MUIC_INT2, 1 << 4),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT2_VIDRM,	MUIC_INT2, 1 << 5),

	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT3_EOC,		MUIC_INT3, 1 << 0),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT3_CGMBC,	MUIC_INT3, 1 << 1),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT3_OVP,		MUIC_INT3, 1 << 2),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT3_MBCCHG_ERR,	MUIC_INT3, 1 << 3),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT3_CHG_ENABLED,	MUIC_INT3, 1 << 4),
	DECLARE_IRQ(MAX77693_MUIC_IRQ_INT3_BAT_DET,	MUIC_INT3, 1 << 5),
};

static void max77693_irq_lock(struct irq_data *data)
{
	struct max77693_dev *max77693 = irq_get_chip_data(data->irq);

	mutex_lock(&max77693->irqlock);
}

static void max77693_irq_sync_unlock(struct irq_data *data)
{
	struct max77693_dev *max77693 = irq_get_chip_data(data->irq);
	int i;

	dev_info(max77693->dev, "sync irq (un)mask\n");
	for (i = 0; i < MAX77693_IRQ_GROUP_NR; i++) {
		u8 mask_reg = max77693_mask_reg[i];
		struct regmap *map = max77693_get_regmap(max77693, i);

		if (mask_reg == MAX77693_REG_INVALID ||
				IS_ERR_OR_NULL(map))
			continue;
		max77693->irq_masks_cache[i] = max77693->irq_masks_cur[i];

		max77693_write_reg(map, max77693_mask_reg[i],
				max77693->irq_masks_cur[i]);
	}

	mutex_unlock(&max77693->irqlock);
}

static const inline struct max77693_irq_data *
irq_to_max77693_irq(struct max77693_dev *max77693, int irq)
{
	return &max77693_irqs[irq];
}

static void max77693_irq_mask(struct irq_data *data)
{
}

static void max77693_irq_unmask(struct irq_data *data)
{
	struct max77693_dev *max77693 = irq_get_chip_data(data->irq);
	int irq_id = data->hwirq - max77693->irq_base;
	const struct max77693_irq_data *irq_data =
				irq_to_max77693_irq(max77693, irq_id);

	if (irq_data->group >= MAX77693_IRQ_GROUP_NR)
		return;

	if (irq_data->group >= MUIC_INT1 && irq_data->group <= MUIC_INT3)
		max77693->irq_masks_cur[irq_data->group] |= irq_data->mask;
	else
		max77693->irq_masks_cur[irq_data->group] &= ~irq_data->mask;

	dev_info(max77693->dev, "unmask: irq=%d, hwirq=%ld, id=%d, mask=%x\n",
			data->irq, data->hwirq, irq_id,
			max77693->irq_masks_cur[irq_data->group]);
}

static struct irq_chip max77693_irq_chip = {
	.name			= "max77693",
	.irq_bus_lock		= max77693_irq_lock,
	.irq_bus_sync_unlock	= max77693_irq_sync_unlock,
	.irq_mask		= max77693_irq_mask,
	.irq_unmask		= max77693_irq_unmask,
};

#define MAX77693_IRQSRC_CHG		(1 << 0)
#define MAX77693_IRQSRC_TOP		(1 << 1)
#define MAX77693_IRQSRC_FLASH		(1 << 2)
#define MAX77693_IRQSRC_MUIC		(1 << 3)
static irqreturn_t max77693_irq_thread(int irq, void *data)
{
	struct max77693_dev *max77693 = data;
	u8 irq_reg[MAX77693_IRQ_GROUP_NR] = {};
	u8 irq_src;
	int ret;
	int i, cur_irq;
	unsigned long hwirq;
	int level;
	u8 chg, int1, int2, int3;

	/* SW Workaround:
	 * Sometimes, MUIC block in MAX77693 chip is reset by voltage drop.
	 * INTMASK1 and INTMASK2 register should not be 0 which is reset value.
	 * If thoese are 0, reconfigures MUIC INTMASK and CTRL3 register.
	 */
	if (max77693_check_muic_reset()) {
		dev_warn(max77693->dev, "detect MUIC reset, reconfigure it.\n");
		dev_warn(max77693->dev, "cache INTMASK1=0x%x, INTMASK2=0x%x\n",
				max77693->irq_masks_cur[MUIC_INT1],
				max77693->irq_masks_cur[MUIC_INT2]);
		max77693_write_reg(max77693->regmap_muic,
				MAX77693_MUIC_REG_INTMASK1, 0x09);
		max77693_write_reg(max77693->regmap_muic,
				MAX77693_MUIC_REG_INTMASK2, 0x11);
		max77693->irq_masks_cur[MUIC_INT1] = 0x09;
		max77693->irq_masks_cur[MUIC_INT2] = 0x11;
		max77693_muic_set_ctrl3();
	}

	level = gpio_get_value(max77693->irq_gpio);
	dev_info(max77693->dev, "irq gpio pre-state=%d, irq=%d\n", level, irq);

	/* max77693 sometimes lost INTMASK register value.
	 * check interrupt mask.
	 */
	max77693_read_reg(max77693->regmap,
			MAX77693_CHG_REG_CHG_INT_MASK, &chg);
	max77693_read_reg(max77693->regmap_muic,
			MAX77693_MUIC_REG_INTMASK1, &int1);
	max77693_read_reg(max77693->regmap_muic,
			MAX77693_MUIC_REG_INTMASK2, &int2);
	max77693_read_reg(max77693->regmap_muic,
			MAX77693_MUIC_REG_INTMASK3, &int3);
	dev_info(max77693->dev, "[INTMASK] chg: 0x%x, muic: 0x%x,0x%x,0x%x\n",
			chg, int1, int2, int3);

clear_retry:
	ret = max77693_read_reg(max77693->regmap, MAX77693_PMIC_REG_INTSRC,
				&irq_src);
	if (ret < 0) {
		dev_err(max77693->dev, "Failed to read interrupt source: %d\n",
				ret);
		return IRQ_NONE;
	}

	if (irq_src & MAX77693_IRQSRC_CHG)
		/* CHG_INT */
		ret = max77693_read_reg(max77693->regmap,
					MAX77693_CHG_REG_CHG_INT,
					&irq_reg[CHG_INT]);

	if (irq_src & MAX77693_IRQSRC_TOP)
		/* TOPSYS_INT */
		ret = max77693_read_reg(max77693->regmap,
			MAX77693_PMIC_REG_TOPSYS_INT, &irq_reg[TOPSYS_INT]);

	if (irq_src & MAX77693_IRQSRC_FLASH)
		/* LED_INT */
		ret = max77693_read_reg(max77693->regmap,
			MAX77693_LED_REG_FLASH_INT, &irq_reg[LED_INT]);

	if (irq_src & MAX77693_IRQSRC_MUIC)
		/* MUIC INT1 ~ INT3 */
		max77693_bulk_read(max77693->regmap_muic,
			MAX77693_MUIC_REG_INT1, MAX77693_NUM_IRQ_MUIC_REGS,
			&irq_reg[MUIC_INT1]);

	level = gpio_get_value(max77693->irq_gpio);
	if (!level) {
		dev_warn(max77693->dev, "irq gpio(%d) is not cleared yet.\n",
				max77693->irq_gpio);
		goto clear_retry;
	}

	dev_info(max77693->dev,
		"irq status: CHG=0x%x, TOPSYS=0x%x, LED=0x%x, MUIC=%x,%x,%x\n",
			irq_reg[CHG_INT], irq_reg[TOPSYS_INT],
			irq_reg[LED_INT], irq_reg[MUIC_INT1],
			irq_reg[MUIC_INT2], irq_reg[MUIC_INT3]);

	/* Apply masking */
	for (i = 0; i < MAX77693_IRQ_GROUP_NR; i++) {
		if (i >= MUIC_INT1 && i <= MUIC_INT3)
			irq_reg[i] &= max77693->irq_masks_cur[i];
		else
			irq_reg[i] &= ~max77693->irq_masks_cur[i];
	}

	/* Report */
	for (i = 0; i < MAX77693_IRQ_NR; i++) {
		if (irq_reg[max77693_irqs[i].group] & max77693_irqs[i].mask) {
			hwirq = max77693->irq_base + i;
			cur_irq = irq_find_mapping(max77693->irq_domain, hwirq);

			dev_info(max77693->dev,
				"%s: virq=%d, hwirq=%ld, base=%d, id=%d\n",
				__func__, cur_irq, hwirq, max77693->irq_base,
				i);

			if (cur_irq)
				handle_nested_irq(cur_irq);
			else
				dev_warn(max77693->dev,
					"failed to find virq: %ld,%d,%d\n",
					hwirq, max77693->irq_base, i);
		}
	}

	return IRQ_HANDLED;
}

int max77693_irq_resume(struct max77693_dev *max77693)
{
	if (max77693->irq)
		max77693_irq_thread(0, max77693);

	return 0;
}

static int max77693_irq_domain_map(struct irq_domain *d, unsigned int irq,
				irq_hw_number_t hw)
{
	struct max77693_dev *max77693 = d->host_data;

	irq_set_chip_data(irq, max77693);
	irq_set_chip_and_handler(irq, &max77693_irq_chip, handle_edge_irq);
	irq_set_nested_thread(irq, 1);
#ifdef CONFIG_ARM
	set_irq_flags(irq, IRQF_VALID);
#else
	irq_set_noprobe(irq);
#endif
	return 0;
}

static struct irq_domain_ops max77693_irq_domain_ops = {
	.map = max77693_irq_domain_map,
};

int max77693_irq_init(struct max77693_dev *max77693)
{
	struct irq_domain *domain;
	int i;
	int ret = 0;
	u8 intsrc_mask;

	mutex_init(&max77693->irqlock);

	/* Mask individual interrupt sources */
	for (i = 0; i < MAX77693_IRQ_GROUP_NR; i++) {
		struct regmap *map;
		/* MUIC IRQ  0:MASK 1:NOT MASK */
		/* Other IRQ 1:MASK 0:NOT MASK */
		if (i >= MUIC_INT1 && i <= MUIC_INT3) {
			max77693->irq_masks_cur[i] = 0x00;
			max77693->irq_masks_cache[i] = 0x00;
		} else {
			max77693->irq_masks_cur[i] = 0xff;
			max77693->irq_masks_cache[i] = 0xff;
		}
		map = max77693_get_regmap(max77693, i);

		if (IS_ERR_OR_NULL(map))
			continue;
		if (max77693_mask_reg[i] == MAX77693_REG_INVALID)
			continue;
		if (i >= MUIC_INT1 && i <= MUIC_INT3)
			max77693_write_reg(map, max77693_mask_reg[i], 0x00);
		else
			max77693_write_reg(map, max77693_mask_reg[i], 0xff);
	}

	domain = irq_domain_add_linear(NULL, MAX77693_IRQ_NR,
					&max77693_irq_domain_ops, max77693);
	if (!domain) {
		dev_err(max77693->dev, "could not create irq domain\n");
		ret = -ENODEV;
		goto err_irq;
	}
	max77693->irq_domain = domain;

	/* Unmask max77693 interrupt */
	ret = max77693_read_reg(max77693->regmap,
			MAX77693_PMIC_REG_INTSRC_MASK, &intsrc_mask);
	if (ret < 0) {
		dev_err(max77693->dev, "fail to read PMIC register\n");
		goto err_irq;
	}

	intsrc_mask &= ~(MAX77693_IRQSRC_CHG);
	intsrc_mask &= ~(MAX77693_IRQSRC_FLASH);
	intsrc_mask &= ~(MAX77693_IRQSRC_MUIC);
	ret = max77693_write_reg(max77693->regmap,
			MAX77693_PMIC_REG_INTSRC_MASK, intsrc_mask);
	if (ret < 0) {
		dev_err(max77693->dev, "fail to write PMIC register\n");
		goto err_irq;
	}

	ret = request_threaded_irq(max77693->irq, NULL, max77693_irq_thread,
			   IRQF_TRIGGER_FALLING|IRQF_ONESHOT|IRQF_NO_SUSPEND,
			   "max77693-irq", max77693);
	if (ret)
		dev_err(max77693->dev, "Failed to request IRQ %d: %d\n",
			max77693->irq, ret);
err_irq:
	return ret;
}

void max77693_irq_exit(struct max77693_dev *max77693)
{
	if (max77693->irq)
		free_irq(max77693->irq, max77693);
}
