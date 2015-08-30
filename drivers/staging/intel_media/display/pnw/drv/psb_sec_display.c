/* arch/x86/platform/intel-mid/board-santos10-display.c
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

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/lnw_gpio.h>
#include <linux/i2c/tc35876x-sec.h>
#include <linux/regulator/consumer.h>
#include <linux/module.h>

#include <video/cmc624.h>
#include <video/psb_sec_bl.h>

#include <asm/intel-mid.h>
#include <asm/intel_muxtbl.h>

#include "../../../../../arch/x86/platform/intel-mid/sec_libs/sec_common.h"
#include "../../../../../arch/x86/platform/intel-mid/sec_libs/platform_cmc624_tune.h"

int santos10_init_tune_list(void)
{
	u32 id;
	int bg;
	int cabc;

	/* init command */
	TUNE_DATA_ID(id, MENU_CMD_INIT, MENU_SKIP, MENU_SKIP, MENU_SKIP,
				MENU_SKIP, MENU_SKIP, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, cmc624_init, ARRAY_SIZE(cmc624_init));

	/* Auto Browser CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_BROWSER,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_browser_cabcoff,
				ARRAY_SIZE(tune_auto_browser_cabcoff));

	/* Auto Browser CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_BROWSER,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_browser_cabcon,
				ARRAY_SIZE(tune_auto_browser_cabcon));

	/* Auto Gallery CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_GALLERY,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_gallery_cabcoff,
				ARRAY_SIZE(tune_auto_gallery_cabcoff));

	/* Auto Gallery CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_GALLERY,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_gallery_cabcon,
				ARRAY_SIZE(tune_auto_gallery_cabcon));

	/* Auto UI CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_UI,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_ui_cabcoff,
				ARRAY_SIZE(tune_auto_ui_cabcoff));

	/* Auto UI CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_UI,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_ui_cabcon,
				ARRAY_SIZE(tune_auto_ui_cabcon));

	/* Auto Video CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_VIDEO,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_video_cabcoff,
				ARRAY_SIZE(tune_auto_video_cabcoff));

	/* Auto Video CABC-on  */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_VIDEO,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_video_cabcon,
				ARRAY_SIZE(tune_auto_video_cabcon));

	/* Auto VT-call CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_VT,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_vtcall_cabcoff,
				ARRAY_SIZE(tune_auto_vtcall_cabcoff));

	/* Auto VT-call CABC-on  */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_VT,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_auto_vtcall_cabcon,
				ARRAY_SIZE(tune_auto_vtcall_cabcon));

	/* Bypass ON  */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_SKIP, MENU_SKIP,
		MENU_BYPASS_ON, MENU_SKIP, MENU_SKIP, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_bypass, ARRAY_SIZE(tune_bypass));

	/* Bypass OFF  */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_SKIP, MENU_SKIP,
		MENU_BYPASS_OFF, MENU_SKIP, MENU_SKIP, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_bypass, ARRAY_SIZE(tune_bypass));

	/* Camera */
	for (bg = MENU_MODE_DYNAMIC; bg < MAX_BACKGROUND_MODE; bg++) {
		for (cabc = MENU_CABC_OFF; cabc < MAX_CABC_MODE; cabc++) {
			TUNE_DATA_ID(id, MENU_CMD_TUNE, bg, MENU_APP_CAMERA,
				MENU_SKIP, MENU_SKIP, cabc, MENU_SKIP,
				MENU_SKIP);
			cmc624_register_tune_data(id, tune_camera,
						ARRAY_SIZE(tune_camera));
		}
	}

	/* Dynamic Browser CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_BROWSER,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_browser_cabcoff,
				ARRAY_SIZE(tune_dynamic_browser_cabcoff));

	/* Dynamic Browser CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_BROWSER,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_browser_cabcon,
				ARRAY_SIZE(tune_dynamic_browser_cabcon));

	/* Dynamic Gallery CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_GALLERY,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_gallery_cabcoff,
				ARRAY_SIZE(tune_dynamic_gallery_cabcoff));

	/* Dynamic Gallery CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_GALLERY,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_gallery_cabcon,
				ARRAY_SIZE(tune_dynamic_gallery_cabcon));

	/* Dynamic UI CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_UI,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_ui_cabcoff,
				ARRAY_SIZE(tune_dynamic_ui_cabcoff));

	/* Dynamic UI CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_UI,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_ui_cabcon,
				ARRAY_SIZE(tune_dynamic_ui_cabcon));

	/* Dynamic Video cabc-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_VIDEO,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_video_cabcoff,
				ARRAY_SIZE(tune_dynamic_video_cabcoff));

	/* Dynamic Video cabc-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_VIDEO,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_video_cabcon,
				ARRAY_SIZE(tune_dynamic_video_cabcon));

	/* Dynamic VT-call CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_VT,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_vtcall_cabcoff,
				ARRAY_SIZE(tune_dynamic_vtcall_cabcoff));

	/* Dynamic VT-call CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_VT,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_dynamic_vtcall_cabcon,
				ARRAY_SIZE(tune_dynamic_vtcall_cabcon));

	/* eBook CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_EBOOK,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_ebook_cabcoff,
				ARRAY_SIZE(tune_ebook_cabcoff));

	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_EBOOK,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_ebook_cabcoff,
				ARRAY_SIZE(tune_ebook_cabcoff));

	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_EBOOK,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_ebook_cabcoff,
				ARRAY_SIZE(tune_ebook_cabcoff));

	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_EBOOK,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_ebook_cabcoff,
				ARRAY_SIZE(tune_ebook_cabcoff));

	/* eBook CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_DYNAMIC, MENU_APP_EBOOK,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_ebook_cabcon,
				ARRAY_SIZE(tune_ebook_cabcon));

	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_EBOOK,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_ebook_cabcon,
				ARRAY_SIZE(tune_ebook_cabcon));

	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_EBOOK,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_ebook_cabcon,
				ARRAY_SIZE(tune_ebook_cabcon));

	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_AUTO, MENU_APP_EBOOK,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_ebook_cabcon,
				ARRAY_SIZE(tune_ebook_cabcon));

	/* Movie Browser CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_BROWSER,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_ui_cabcoff,
				ARRAY_SIZE(tune_movie_ui_cabcoff));

	/* Movie browser CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_BROWSER,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_browser_cabcon,
				ARRAY_SIZE(tune_movie_browser_cabcon));

	/* Movie Gallery CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_GALLERY,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_gallery_cabcoff,
				ARRAY_SIZE(tune_movie_gallery_cabcoff));

	/* Movie UI CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_UI,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_ui_cabcoff,
				ARRAY_SIZE(tune_movie_ui_cabcoff));

	/* Movie UI CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_UI,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_ui_cabcon,
				ARRAY_SIZE(tune_movie_ui_cabcon));

	/* Movie Gallery CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_GALLERY,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_gallery_cabcon,
				ARRAY_SIZE(tune_movie_gallery_cabcon));

	/* Movie Video CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_VIDEO,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_video_cabcoff,
				ARRAY_SIZE(tune_movie_video_cabcoff));

	/* Movie Video CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_VIDEO,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_video_cabcon,
				ARRAY_SIZE(tune_movie_video_cabcon));

	/* Movie VT-call CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_VT,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_vtcall_cabcoff,
				ARRAY_SIZE(tune_movie_vtcall_cabcoff));

	/* Movie VT-call CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_MOVIE, MENU_APP_VT,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_movie_vtcall_cabcon,
				ARRAY_SIZE(tune_movie_vtcall_cabcon));

	/* Negative CABC-off*/
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_SKIP, MENU_SKIP,
		MENU_SKIP, MENU_NEGATIVE_ON, MENU_CABC_OFF, MENU_SKIP,
		MENU_SKIP);
	cmc624_register_tune_data(id, tune_negative_cabcoff,
				ARRAY_SIZE(tune_negative_cabcoff));

	/* Negative CABC-on*/
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_SKIP, MENU_SKIP,
		MENU_SKIP, MENU_NEGATIVE_ON, MENU_CABC_ON, MENU_SKIP,
		MENU_SKIP);
	cmc624_register_tune_data(id, tune_negative_cabcon,
				ARRAY_SIZE(tune_negative_cabcon));

	/* Color-blind CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_SKIP, MENU_SKIP,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP,
		MENU_ACC_COLOR_BLIND);
	cmc624_register_tune_data(id, tune_color_blind_cabcoff,
				ARRAY_SIZE(tune_color_blind_cabcoff));

	/* Color-blind CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_SKIP, MENU_SKIP,
		MENU_SKIP,  MENU_SKIP, MENU_CABC_ON, MENU_SKIP,
		MENU_ACC_COLOR_BLIND);
	cmc624_register_tune_data(id, tune_color_blind_cabcon,
				ARRAY_SIZE(tune_color_blind_cabcon));

	/* Standard Browser CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_BROWSER,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_ui_cabcoff,
				ARRAY_SIZE(tune_standard_browser_cabcoff));

	/* Stadard browser CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_BROWSER,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_browser_cabcon,
				ARRAY_SIZE(tune_standard_browser_cabcon));


	/* Standard UI CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_UI,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_ui_cabcoff,
				ARRAY_SIZE(tune_standard_ui_cabcoff));

	/* Standard UI CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_UI,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_ui_cabcon,
				ARRAY_SIZE(tune_standard_ui_cabcon));

	/* Standard Gallery CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_GALLERY,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_gallery_cabcoff,
				ARRAY_SIZE(tune_standard_gallery_cabcoff));

	/* Standard Gallery CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_GALLERY,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_gallery_cabcon,
				ARRAY_SIZE(tune_standard_gallery_cabcon));

	/* Standard VT-call CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_VT,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_vtcall_cabcoff,
				ARRAY_SIZE(tune_standard_vtcall_cabcoff));

	/* Standard VT-call CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_VT,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_vtcall_cabcon,
				ARRAY_SIZE(tune_standard_vtcall_cabcon));

	/* Standard Video CABC-off */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_VIDEO,
		MENU_SKIP, MENU_SKIP, MENU_CABC_OFF, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_video_cabcoff,
				ARRAY_SIZE(tune_standard_video_cabcoff));

	/* Standard Video CABC-on */
	TUNE_DATA_ID(id, MENU_CMD_TUNE, MENU_MODE_STANDARD, MENU_APP_VIDEO,
		MENU_SKIP, MENU_SKIP, MENU_CABC_ON, MENU_SKIP, MENU_SKIP);
	cmc624_register_tune_data(id, tune_standard_video_cabcon,
				ARRAY_SIZE(tune_standard_video_cabcon));

	return 0;
}
EXPORT_SYMBOL(santos10_init_tune_list);
