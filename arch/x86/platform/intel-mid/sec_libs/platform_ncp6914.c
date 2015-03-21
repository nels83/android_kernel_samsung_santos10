
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio_event.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/ncp6914.h>

#include <linux/lnw_gpio.h>
#include <asm/intel-mid.h>

#define NCP6914_LDO1_NAME "vt_core_1.5v"
#define NCP6914_LDO2_NAME "cam_io_1.8v"
#define NCP6914_LDO3_NAME "cam_a2.8v"
#define NCP6914_LDO4_NAME "ncp6914_ldo4"
#define NCP6914_BUCK_NAME "3mp_core_1.2v"


#define S5K5CCGX_DEV_ID "4-002d" /* bus : 4, slave addr : 0x2d */
#define DB8131M_DEV_ID "4-0045" /* bus : 4, slave addr : 0x45 */

static struct regulator_consumer_supply buck_consumer[] = {
	REGULATOR_SUPPLY(NCP6914_BUCK_NAME, S5K5CCGX_DEV_ID),
	REGULATOR_SUPPLY(NCP6914_BUCK_NAME, DB8131M_DEV_ID)
};
static struct regulator_consumer_supply ldo1_consumer[] = {
	REGULATOR_SUPPLY(NCP6914_LDO1_NAME, S5K5CCGX_DEV_ID),
	REGULATOR_SUPPLY(NCP6914_LDO1_NAME, DB8131M_DEV_ID)
};
static struct regulator_consumer_supply ldo2_consumer[] = {
	REGULATOR_SUPPLY(NCP6914_LDO2_NAME, S5K5CCGX_DEV_ID),
	REGULATOR_SUPPLY(NCP6914_LDO2_NAME, DB8131M_DEV_ID)
};
static struct regulator_consumer_supply ldo3_consumer[] = {
	REGULATOR_SUPPLY(NCP6914_LDO3_NAME, S5K5CCGX_DEV_ID),
	REGULATOR_SUPPLY(NCP6914_LDO3_NAME, DB8131M_DEV_ID)
};
static struct regulator_consumer_supply ldo4_consumer[] = {
	REGULATOR_SUPPLY(NCP6914_LDO4_NAME, S5K5CCGX_DEV_ID),
	REGULATOR_SUPPLY(NCP6914_LDO4_NAME, DB8131M_DEV_ID)
};
 

static struct regulator_init_data platform_ncp6914_buck_data = {
	.constraints	= {
		.name		= NCP6914_BUCK_NAME,
		.min_uV		= 1200000,  // 1.2V : 0x10001
		.max_uV		= 1200000,
		.apply_uV	= 1,
		.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	=	ARRAY_SIZE(buck_consumer),
	.consumer_supplies	=	buck_consumer,
};

static struct regulator_init_data platform_ncp6914_ldo1_data = {
	.constraints	= {
		.name		= NCP6914_LDO1_NAME,
		.min_uV		= 1500000,  // 1.5V : 0x01110
		.max_uV		= 1500000,
		.apply_uV	= 1,
		.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	=	ARRAY_SIZE(ldo1_consumer),
	.consumer_supplies	=	ldo1_consumer,
};

static struct regulator_init_data platform_ncp6914_ldo2_data = {
	.constraints	= {
		.name		= NCP6914_LDO2_NAME,
		.min_uV		= 1800000,  // 1.8V : 0x10100
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	=	ARRAY_SIZE(ldo2_consumer),
	.consumer_supplies	=	ldo2_consumer,
};

static struct regulator_init_data platform_ncp6914_ldo3_data = {
	.constraints	= {
		.name		= NCP6914_LDO3_NAME,
		.min_uV 	= 1800000,
		.max_uV 	= 1800000,
//		.min_uV		= 2800000, // 2.8V : 0x11001
//		.max_uV		= 2800000,
		.apply_uV	= 1,
		.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	=	ARRAY_SIZE(ldo3_consumer),
	.consumer_supplies	=	ldo3_consumer,
};

static struct regulator_init_data platform_ncp6914_ldo4_data = {
	.constraints	= {
		.name		= NCP6914_LDO4_NAME,
		.min_uV		= 2800000, // 2.8V : 0x11110
		.max_uV		= 2800000,
		.apply_uV	= 1,
		.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	=	ARRAY_SIZE(ldo4_consumer),
	.consumer_supplies	=	ldo4_consumer,
};
 

static struct ncp6914_subdev_data platform_ncp6914_subdev_data[] = {
	{
		.id =NCP6914_BUCK,
		.initdata = &platform_ncp6914_buck_data,
	},
	{
		.id = NCP6914_LDO1,
		.initdata = &platform_ncp6914_ldo1_data,
	},
	{
		.id = NCP6914_LDO2,
		.initdata = &platform_ncp6914_ldo2_data,
	},
	{
		.id = NCP6914_LDO3,
		.initdata = &platform_ncp6914_ldo3_data,
	},
	{
		.id = NCP6914_LDO4,
		.initdata = &platform_ncp6914_ldo4_data,
	},
};

static struct ncp6914_platform_data platform_ncp6914_pdata = {
	.num_subdevs = ARRAY_SIZE(platform_ncp6914_subdev_data),
	.subdevs = platform_ncp6914_subdev_data,
};

void *ncp6914_platform_data(void *info)
{
	return &platform_ncp6914_pdata;
}
