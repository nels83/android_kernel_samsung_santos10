/* arch/x86/platform/intel-mid/board-santos10-muxset-r08.c
 *
 * Copyright (C) 2013 Samsung Electronics Co, Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/crc32.h>
#include <linux/lnw_gpio.h>
#include <linux/string.h>

#include <asm/intel_muxtbl.h>
#include <asm/sec_muxtbl.h>

static struct intel_muxtbl muxtbl[] = {
	/* [-----] GP_CORE_037 (gp_core_037) - SubPMIC enable */
	INTEL_MUXTBL("hdmi_cec", "clv_gpio_1", hdmi_cec,
		     "NCP_EN", 133, LNW_GPIO,
		     BYPASS, OD_ENABLE, NONE),
};

add_sec_muxtbl_to_list(santos103g, 11, muxtbl);
add_sec_muxtbl_to_list(santos10wifi, 11, muxtbl);
