/*
 * platform_ctp_audio_wm1811.h: CLVS audio platform data header file
 *
 * Copyright (C) 2012-2013 Intel Corporation
 * Author: Ted Choi <ted.choi@intel.com>
 * Author: Jeeja KP <jeeja.kp@intel.com>
 * Author: Dharageswari.R<dharageswari.r@intel.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */
#ifndef _PLATFORM_CTP_AUDIO_H_
#define _PLATFORM_CTP_AUDIO_H_

#include <linux/sfi.h>
struct ctp_audio_platform_data {
	/* Intel software platform id*/
	const struct soft_platform_id *spid;
};

extern void __init *ctp_audio_wm1811_platform_data(void *info) __attribute__((weak));
#endif
