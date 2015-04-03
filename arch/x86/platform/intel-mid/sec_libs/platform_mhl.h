/*
 * platform_mhl.h: MHL platform data header file
 *
 * (C) Copyright 2013 Samsung Corporation
 * Author:
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */
#ifndef _PLATFORM_MHL_H_
#define _PLATFORM_MHL_H_

#if defined(CONFIG_SAMSUNG_MHL_92X4)
extern void __init *sii9234_platform_data(void *info) __attribute__((weak));
#endif
#if defined(CONFIG_SAMSUNG_MHL_8246)
extern void __init *sii8246_platform_data(void *info) __attribute__((weak));
#endif

#endif
