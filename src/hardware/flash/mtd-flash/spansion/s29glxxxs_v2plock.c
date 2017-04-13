/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
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




#include <pthread.h>
#include <sys/f3s_mtd.h>

/*
 * Summary
 *
 * MTD Version:    2 only
 * Bus Width:      8-bit, 16-bit and 8/16-bit hybrid
 * Locking Method: Persistent
 *
 * Description
 *
 * Use this for spansion flash capable of block locking.
 */

int f3s_s29glxxxs_v2plock(f3s_dbase_t *dbase,
                    f3s_access_t *access,
                    uint32_t flags,
                    uint32_t offset)
{
	volatile void	*memory;
	uint16_t	amd_cmd1, amd_cmd2;
	uint32_t	loops = 10000000;
	int     	error = EOK;

	F3S_BASETYPE	status1, status2;
	F3S_BASETYPE	mask6 = (1 << 6) * flashcfg.device_mult;
	F3S_BASETYPE	mask, toggle;

	memory = access->service->page(&access->socket, F3S_POWER_ALL, offset & amd_command_mask, NULL);
	if (!memory)
	{
		fprintf(stderr, "(devf  t%d::%s:%d) page() returned NULL for offset 0x%x\n",
					pthread_self(), __func__, __LINE__, offset);
		return (errno);
	}

	if (flashcfg.device_width == 1) {
		amd_cmd1 = AMD_CMD_ADDR1_W8;
		amd_cmd2 = AMD_CMD_ADDR2_W8;
	} else {
		amd_cmd1 = AMD_CMD_ADDR1_W16;
		amd_cmd2 = AMD_CMD_ADDR2_W16;
	}

	amd_cmd1 *= flashcfg.bus_width;
	amd_cmd2 *= flashcfg.bus_width;

	/* Enter the Persistent Sector Protection Mode */
	send_command(memory + amd_cmd1, AMD_UNLOCK_CMD1);
	send_command(memory + amd_cmd2, AMD_UNLOCK_CMD2);
	send_command(memory + amd_cmd1, AMD_PPB_ENTER);


	/* Issue the lock command */
	send_command(memory, AMD_PROGRAM);
	send_command(memory, AMD_PPB_SET);

	/* Wait for lock to complete */
	while (--loops) {
		status1 = readmem(memory);
		status2 = readmem(memory);

		/* Stop if DQ6 has stopped toggling */
		toggle = (status1 ^ status2) & mask6;
		if (!toggle) break;

		/* If DQ5 is *only* set on the chips that are still toggling */
		mask = (status1 & (toggle >> 1)) << 1;
		if (mask == toggle) {
			/* Poll again */
			status1 = readmem(memory);
			status2 = readmem(memory);

			/* If *any* of the same DQ6 bits are still toggling */
			if ((status1 ^ status2) & mask) {
				fprintf(stderr, "%s: %d DQ5 program error \n", __func__, __LINE__);
				send_command(memory, AMD_READ_MODE);
				error = EIO;
			}
			break;
		}
	}

	if (loops == 0) {
		fprintf(stderr, "%s: %d DQ6 toggling timeout \n", __func__, __LINE__);
		send_command(memory, AMD_READ_MODE);
		error = EIO;
	}

	if (error == EOK) { // check DQ0
		status1 = readmem(memory);
		if((status1 & 0x1)) {
			fprintf(stderr, "%s: %d DQ0 error \n", __func__, __LINE__);
			send_command(memory, AMD_READ_MODE);
			error = EIO;
		}
	}

	/* Leave Volatile Sector Protection Mode */
	send_command(memory, AMD_PROTECT_EXIT1);
	send_command(memory, AMD_PROTECT_EXIT2);
	f3s_s29glxxxs_flush(access);
	return (error);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/mtd-flash/spansion/s29glxxxs_v2plock.c $ $Rev: 816666 $")
#endif
