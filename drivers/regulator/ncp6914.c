/*
 * ncp6914.c
 *
 * based on max8893.c
 *
 */

#include <linux/module.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/ncp6914.h>
#include <linux/slab.h>
#include <linux/gpio.h>

/* Register map */
#define	NCP6914_REG_GENRAL_SETTINGS	0x00
#define	NCP6914_REG_LDO1_SETTINGS	0x01
#define	NCP6914_REG_LDO2_SETTINGS	0x02
#define	NCP6914_REG_LDO3_SETTINGS	0x03
#define	NCP6914_REG_LDO4_SETTINGS	0x04
#define	NCP6914_REG_SPARE	0x05
#define	NCP6914_REG_BUCK_SETTINGS1	 0x06
#define	NCP6914_REG_BUCK_SETTINGS2	 0x07
#define	NCP6914_REG_ENABLE_BITS	 0x08
#define	NCP6914_REG_PULLDOWN_BITS 0x09
#define	NCP6914_REG_STATUS_BITS 0x0A
#define	NCP6914_REG_INTERRUPT_BITS 0x0B
#define	NCP6914_REG_INTERRUPT_MASK 0x0C

#define LDO1_LDO2_LDO3_V1_20    0x00
#define LDO1_LDO2_LDO3_V1_25    0x01
#define LDO1_LDO2_LDO3_V1_30    0x02
#define LDO1_LDO2_LDO3_V1_35    0x03
#define LDO1_LDO2_LDO3_V1_40    0x04
#define LDO1_LDO2_LDO3_V1_45    0x05
#define LDO1_LDO2_LDO3_V1_50    0x06 // 1.5
#define LDO1_LDO2_LDO3_V1_55    0x07
#define LDO1_LDO2_LDO3_V1_60    0x08
#define LDO1_LDO2_LDO3_V1_65    0x09
#define LDO1_LDO2_LDO3_V1_70    0x0A
#define LDO1_LDO2_LDO3_V1_75    0x0B
#define LDO1_LDO2_LDO3_V1_80    0x0C // 1.8
#define LDO1_LDO2_LDO3_V1_85    0x0D
#define LDO1_LDO2_LDO3_V1_90    0x0E
#define LDO1_LDO2_LDO3_V2_00    0x0F
#define LDO1_LDO2_LDO3_V2_10    0x10
#define LDO1_LDO2_LDO3_V2_20    0x11
#define LDO1_LDO2_LDO3_V2_30    0x12
#define LDO1_LDO2_LDO3_V2_40    0x13
#define LDO1_LDO2_LDO3_V2_50    0x14
#define LDO1_LDO2_LDO3_V2_60    0x15
#define LDO1_LDO2_LDO3_V2_65    0x16
#define LDO1_LDO2_LDO3_V2_70    0x17
#define LDO1_LDO2_LDO3_V2_75    0x18
#define LDO1_LDO2_LDO3_V2_80    0x19 // 2.8
#define LDO1_LDO2_LDO3_V2_85    0x1A
#define LDO1_LDO2_LDO3_V2_90    0x1B
#define LDO1_LDO2_LDO3_V2_95    0x1C
#define LDO1_LDO2_LDO3_V3_00    0x1D
#define LDO1_LDO2_LDO3_V3_10    0x1E
#define LDO1_LDO2_LDO3_V3_30    0x1F

#define LDO4_V1_00    0x00
#define LDO4_V1_10    0x04
#define LDO4_V1_20    0x05
#define LDO4_V1_25    0x06
#define LDO4_V1_30    0x07
#define LDO4_V1_35    0x08
#define LDO4_V1_40    0x09
#define LDO4_V1_45    0x0A
#define LDO4_V1_50    0x0B
#define LDO4_V1_55    0x0C
#define LDO4_V1_60    0x0D
#define LDO4_V1_65    0x0E
#define LDO4_V1_70    0x0F
#define LDO4_V1_75    0x10
#define LDO4_V1_80    0x11
#define LDO4_V1_85    0x12
#define LDO4_V1_90    0x13
#define LDO4_V2_00    0x14
#define LDO4_V2_10    0x15
#define LDO4_V2_20    0x16
#define LDO4_V2_30    0x17
#define LDO4_V2_40    0x18
#define LDO4_V2_50    0x19
#define LDO4_V2_60    0x1A
#define LDO4_V2_65    0x1B
#define LDO4_V2_70    0x0C
#define LDO4_V2_75    0x0D
#define LDO4_V2_80    0x0E
#define LDO4_V2_85    0x0F

#define LDO_BUCK_DELAY_SHIFT  5

#define LDO_BUCK_DELAY_2ms   (0 << LDO_DELAY_SHIFT)
#define LDO_BUCK_DELAY_4ms   (1 << LDO_DELAY_SHIFT)
#define LDO_BUCK_DELAY_6ms   (2 << LDO_DELAY_SHIFT)
#define LDO_BUCK_DELAY_8ms   (3 << LDO_DELAY_SHIFT)
#define LDO_BUCK_DELAY_10ms  (4 << LDO_DELAY_SHIFT)
#define LDO_BUCK_DELAY_12ms  (5 << LDO_DELAY_SHIFT)
#define LDO_BUCK_DELAY_14ms  (6 << LDO_DELAY_SHIFT)
#define LDO_BUCK_DELAY_16ms  (7 << LDO_DELAY_SHIFT)

/* BUCK_V1 and BUCK_V2 setting table */
#define BUCK_V0_75    0x00
#define BUCK_V0_80    0x01
#define BUCK_V0_85    0x02
#define BUCK_V0_90    0x03
#define BUCK_V0_95    0x04
#define BUCK_V1_00    0x05
#define BUCK_V1_05    0x06
#define BUCK_V1_10    0x00
#define BUCK_V1_15    0x08
#define BUCK_V1_20    0x09 //
#define BUCK_V1_25    0x0A
#define BUCK_V1_30    0x0B
#define BUCK_V1_35    0x0C
#define BUCK_V1_40    0x0D
#define BUCK_V1_45    0x0E
#define BUCK_V1_50    0x0F
#define BUCK_V1_55    0x10
#define BUCK_V1_60    0x11
#define BUCK_V1_65    0x12
#define BUCK_V1_70    0x13
#define BUCK_V1_75    0x14
#define BUCK_V1_80    0x15
#define BUCK_V1_85    0x16
#define BUCK_V1_90    0x17
#define BUCK_V1_95    0x18
#define BUCK_V2_00    0x19
#define BUCK_V2_05    0x1A
#define BUCK_V2_10    0x1B
#define BUCK_V2_15    0x1C
#define BUCK_V2_20    0x1D
#define BUCK_V2_25    0x1E
#define BUCK_V2_30    0x1F
/* Useful bit values and masks */

#define	NCP6914_REG_BIT0_SET		0x01
#define	NCP6914_REG_BIT1_SET		0x02
#define	NCP6914_REG_BIT2_SET		0x04
#define	NCP6914_REG_BIT3_SET		0x08
#define	NCP6914_REG_BIT4_SET		0x10
#define	NCP6914_REG_BIT5_SET		0x20
#define	NCP6914_REG_BIT6_SET		0x40
#define	NCP6914_REG_BIT7_SET		0x80

#define	NCP6914_REG_REGEN_BIT_LDO1EN		0x01
#define	NCP6914_REG_REGEN_BIT_LDO2EN		0x02
#define	NCP6914_REG_REGEN_BIT_LDO3EN		0x04
#define	NCP6914_REG_REGEN_BIT_LDO4EN		0x08
#define	NCP6914_REG_REGEN_BIT_BUCKEN		0x20
#define	NCP6914_REG_REGEN_BIT_DVS_V2V1		0x80

#define GPIO42_PWR_ON 42
#define GPIO37_PWR_ON 133

struct ncp6914 {
	struct i2c_client *client;
	struct mutex lock;
	struct regulator_dev *rdev[];
};

struct ncp6914_desc {
	struct regulator_desc desc;
	u8 reg;
	u8 val;
	u8 mask;
};

#if defined CONFIG_REGULATOR_NCP6914 // VE_GROUP
static unsigned int sec_board_rev;

static int __init sec_mhl_set_board_rev(char *str)
{
	unsigned int rev;
	if (!kstrtouint(str, 0, &rev))
		sec_board_rev = rev;

	pr_info("%s: sec_board_rev(%d)\n", __func__, sec_board_rev);

	return 0;
}
early_param("androidboot.revision", sec_mhl_set_board_rev);
#endif

static struct ncp6914_desc *rdev_get_ncp6914_desc(struct regulator_dev *rdev)
{
	return container_of(rdev->desc, struct ncp6914_desc, desc);
}

static int ncp6914_onoff(struct regulator_dev *rdev, bool enable)
{
	struct ncp6914 *ncp6914 = rdev_get_drvdata(rdev);
	struct ncp6914_desc *desc = rdev_get_ncp6914_desc(rdev);
	int ret;
	u8 val;
	
	printk("NCP6914 %s %d\n",__func__,__LINE__);

	mutex_lock(&ncp6914->lock);
	ret = i2c_smbus_read_byte_data(ncp6914->client, NCP6914_REG_ENABLE_BITS);
	if (ret < 0) {
		dev_err(&rdev->dev, "%s: i2c read failed\n", __func__);
		goto err;
	}
	val = ret;

	if (enable)
		val |= desc->mask;
	else
		val &= ~desc->mask;

	ret = i2c_smbus_write_byte_data(ncp6914->client, NCP6914_REG_ENABLE_BITS, val);
	if (ret) {
		dev_err(&rdev->dev, "%s: i2c write failed\n", __func__);
		goto err;
	}
	

	mutex_unlock(&ncp6914->lock);
	printk("NCP6914 %s %d 0x%x\n",__func__,__LINE__, val);

	if (enable && val)
	{
		printk("NCP6914 %s %d GPIO ON\n",__func__,__LINE__);

            if(sec_board_rev == 10) {
            	gpio_set_value(GPIO42_PWR_ON, 1);
            } else if(sec_board_rev > 10) {
      			gpio_set_value(GPIO37_PWR_ON, 1);
	      } else {
				printk("NCP6914 have not been used on this board");
		}
	}
	else if (!enable && !(val & 0x20))
	{
		printk("NCP6914 %s %d GPIO OFF\n",__func__,__LINE__);
            if(sec_board_rev == 10) {
            	gpio_set_value(GPIO42_PWR_ON, 0);
            } else if(sec_board_rev > 10) {
      			gpio_set_value(GPIO37_PWR_ON, 0);
      	} else {
				printk("NCP6914 have not been used on this board");
		}
	}
err:
	return ret;
}

static int ncp6914_enable(struct regulator_dev *rdev)
{
	printk("NCP6914 %s %d\n",__func__,__LINE__);

	return ncp6914_onoff(rdev, true);
}

static int ncp6914_disable(struct regulator_dev *rdev)
{
	printk("NCP6914 %s %d\n",__func__,__LINE__);

	return ncp6914_onoff(rdev, false);
}

static int ncp6914_is_enabled(struct regulator_dev *rdev)
{
	struct ncp6914 *ncp6914 = rdev_get_drvdata(rdev);
	struct ncp6914_desc *desc = rdev_get_ncp6914_desc(rdev);
	int val;
	printk("NCP6914 %s %d\n",__func__,__LINE__);

	val = i2c_smbus_read_byte_data(ncp6914->client, NCP6914_REG_ENABLE_BITS);	 
	return !!(val & desc->mask);
}

static int ncp6914_set(struct regulator_dev *rdev, int min_uV, int max_uV,
		unsigned *sel)
{
	struct ncp6914 *ncp6914 = rdev_get_drvdata(rdev);
	struct ncp6914_desc *desc = rdev_get_ncp6914_desc(rdev);
	int ret;

    ret = i2c_smbus_write_byte_data(ncp6914->client,
					desc->reg, desc->val);
	if (ret) {
		dev_err(&rdev->dev, "%s: i2c write failed\n", __func__);
		return ret;
	}
	
	printk("NCP6914 %s %s 0x%X 0x%X\n",__func__,desc->desc.name,desc->reg, desc->val);
	return 0;
}

static struct regulator_ops ncp6914_ops = {
	.enable = ncp6914_enable,
	.disable = ncp6914_disable,
	.is_enabled = ncp6914_is_enabled,
	//.list_voltage = ncp6914_list,
	.set_voltage = ncp6914_set,
	//.get_voltage = ncp6914_get,
};

static struct ncp6914_desc ncp6914_reg[] = {
	{
		.desc = {
			.name = "BUCK",
			.id = NCP6914_BUCK,
			.ops = &ncp6914_ops,
			.type = REGULATOR_VOLTAGE,
			.n_voltages = 0x1F + 1,
			.owner = THIS_MODULE,
		},
		.mask = NCP6914_REG_REGEN_BIT_BUCKEN,
		.reg = NCP6914_REG_BUCK_SETTINGS1,
		.val = BUCK_V1_20,
	},
	{
		.desc = {
			.name = "LDO1",
			.id = NCP6914_LDO1,
			.ops = &ncp6914_ops,
			.type = REGULATOR_VOLTAGE,
			.n_voltages = 0x1F + 1,
			.owner = THIS_MODULE,
		},
		.mask = NCP6914_REG_REGEN_BIT_LDO1EN,
		.reg = NCP6914_REG_LDO1_SETTINGS,
		.val = LDO1_LDO2_LDO3_V1_50,
	},
	{
		.desc = {
			.name = "LDO2",
			.id = NCP6914_LDO2,
			.ops = &ncp6914_ops,
			.type = REGULATOR_VOLTAGE,
			.n_voltages = 0x1F + 1,
			.owner = THIS_MODULE,
		},
		.mask = NCP6914_REG_REGEN_BIT_LDO2EN,
		.reg = NCP6914_REG_LDO2_SETTINGS,
		.val = LDO1_LDO2_LDO3_V1_80,
	},
	{
		.desc = {
			.name = "LDO3",
			.id = NCP6914_LDO3,
			.ops = &ncp6914_ops,
			.type = REGULATOR_VOLTAGE,
			.n_voltages = 0x1F + 1,
			.owner = THIS_MODULE,
		},
		.mask = NCP6914_REG_REGEN_BIT_LDO3EN,
		.reg = NCP6914_REG_LDO3_SETTINGS,
		.val = LDO1_LDO2_LDO3_V2_80,
	},
	{ // NC
		.desc = {
			.name = "LDO4",
			.id = NCP6914_LDO4,
			.ops = &ncp6914_ops,
			.type = REGULATOR_VOLTAGE,
			.n_voltages = 0x0F + 1,
			.owner = THIS_MODULE,
		},
		.mask = NCP6914_REG_REGEN_BIT_LDO4EN,
		.reg = NCP6914_REG_LDO4_SETTINGS,
		.val = LDO4_V1_00,
	},
};


static int __devinit ncp6914_probe(struct i2c_client *client,
				   const struct i2c_device_id *i2c_id)
{
	struct regulator_dev **rdev;
	struct ncp6914_platform_data *pdata = client->dev.platform_data;
	struct ncp6914 *ncp6914;
	int i, id, ret = -EINVAL;
#if defined (CONFIG_REGULATOR_MAX8893)
	extern unsigned int sec_board_rev;

	if (sec_board_rev < 10)
		return ret;
#endif

	if (pdata->num_subdevs > NCP6914_END) {
		dev_err(&client->dev, "Too many regulators found!\n");
		goto out;
	}
	printk("NCP6914 %s %d\n",__func__,__LINE__);
	ncp6914 = kzalloc(sizeof(struct ncp6914) +
			sizeof(struct regulator_dev *) * NCP6914_END,
			GFP_KERNEL);
	if (!ncp6914) {
		ret = -ENOMEM;
		goto out;
	}

      if(sec_board_rev == 10) {
            gpio_request(GPIO42_PWR_ON, "SUBPMU_PWRON");
      } else {
            gpio_request(GPIO37_PWR_ON, "SUBPMU_PWRON");
      }
	mutex_init(&ncp6914->lock);
	ncp6914->client = client;
	rdev = ncp6914->rdev;


	/* Finally register devices */
	for (i = 0; i < pdata->num_subdevs; i++) {

		id = pdata->subdevs[i].id;

		rdev[i] = regulator_register(&ncp6914_reg[id].desc,
					     &client->dev,
					     pdata->subdevs[i].initdata,
					     ncp6914, NULL);
		if (IS_ERR(rdev[i])) {
			ret = PTR_ERR(rdev[i]);
			dev_err(&client->dev, "failed to register %s\n",
				ncp6914_reg[id].desc.name);
			goto err_unregister;
		}
	}
	printk("NCP6914 %s %d\n",__func__,__LINE__);

	i2c_set_clientdata(client, ncp6914);
	dev_info(&client->dev, "Maxim 8893 regulator driver loaded\n");
	return 0;

err_unregister:
	while (--i >= 0)
		regulator_unregister(rdev[i]);

	kfree(ncp6914);
out:
	return ret;
}

static int __devexit ncp6914_remove(struct i2c_client *client)
{
	struct ncp6914 *ncp6914 = i2c_get_clientdata(client);
	struct regulator_dev **rdev = ncp6914->rdev;
	int i;
	printk("NCP6914 %s %d\n",__func__,__LINE__);

	for (i = 0; i < NCP6914_END; i++)
		if (rdev[i])
			regulator_unregister(rdev[i]);
	mutex_destroy(&ncp6914->lock);
	kfree(ncp6914);

	return 0;
}

static const struct i2c_device_id ncp6914_id[] = {
	{ "ncp6914", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ncp6914_id);

static struct i2c_driver ncp6914_driver = {
	.probe = ncp6914_probe,
	.remove = __devexit_p(ncp6914_remove),
	.driver		= {
		.name	= "ncp6914",
		.owner	= THIS_MODULE,
	},
	.id_table	= ncp6914_id,
};

static int __init ncp6914_init(void)
{
	return i2c_add_driver(&ncp6914_driver);
}
subsys_initcall(ncp6914_init);

static void __exit ncp6914_exit(void)
{
	printk("NCP6914 %s %d\n",__func__,__LINE__);

	i2c_del_driver(&ncp6914_driver);
}
module_exit(ncp6914_exit);
