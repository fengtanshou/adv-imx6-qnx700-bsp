/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef IMX6X_DEVNP_IOCTL_H
#define IMX6X_DEVNP_IOCTL_H

#include <stdint.h>


#define IMX6X_IOCTL SIOCGDRVSPEC

#define GET_BRCM_SQI	0x1000
#define READ_BRCM_MII	0x1001
#define WRITE_BRCM_MII	0x1002
#define ENABLE_BRCM_PHY_LOWPOWER		0x1003
#define DISABLE_BRCM_PHY_LOWPOWER		0x1004

typedef struct {
    uint8_t	sqi;		/* sqi  */
} mx6q_get_sqi_t;

typedef struct {
	uint8_t		address;		/* mii register address  */
	uint16_t	data;			/* for read, data is the value read from register, for write, data is the value write to the register*/
} mx6q_mii_request_t;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/devnp/mx6x/public/hw/imx6x_devnp_ioctl.h $ $Rev: 778284 $")
#endif
