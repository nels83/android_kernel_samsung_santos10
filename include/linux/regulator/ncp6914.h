/*
 * ncp6914.h  --  Voltage regulation for the Onsemi 6914
 *
 * based on ncp6914.h
 *
 */

#ifndef __LINUX_REGULATOR_NCP6914_H
#define __LINUX_REGULATOR_NCP6914_H

#include <linux/regulator/machine.h>

enum {
	NCP6914_BUCK,
	NCP6914_LDO1,
	NCP6914_LDO2,
	NCP6914_LDO3,
	NCP6914_LDO4,
	NCP6914_END,
};

/**
 * ncp6914_subdev_data - regulator subdev data
 * @id: regulator id
 * @initdata: regulator init data
 */
struct ncp6914_subdev_data {
	int				id;
	struct regulator_init_data	*initdata;
};

/**
 * ncp6914_platform_data - platform data for ncp6914
 * @num_subdevs: number of regulators used
 * @subdevs: pointer to regulators used
 */
struct ncp6914_platform_data {
	int num_subdevs;
	struct ncp6914_subdev_data *subdevs;
};

/* NCP6914 Camera Sub-PMIC */
int  NCP6914_subPMIC_PowerOn(int opt);
int  NCP6914_subPMIC_PowerOff(int opt);
int  NCP6914_subPMIC_PinOnOff(int pin, int on_off);

#endif
