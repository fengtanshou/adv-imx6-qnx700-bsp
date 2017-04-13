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
 * Description
 *
 * This routiene is implemented as a temporary work-around for a cache issue
 * present in all Cypress S29GL-S family of parts. The intent is to clear the
 * NOR cache by reading one word (2 bytes) from 64 independent pages (2x the
 * cache depth). As confirmed by Cypress, this is the only way to guarentee
 * that the cache is cleared.
 */

#define PAGE_SIZE 32
#define WORD_SIZE 2

int __attribute__((optimize("O0"))) f3s_s29glxxxs_flush(f3s_access_t *access)
{
	uint8_t 	*memory;
	uint8_t		buffer[WORD_SIZE];
	int 		i;

	memory = access->service->page(&access->socket, F3S_POWER_ALL, 0, NULL);
	if (memory == NULL) {
		if (verbose) {
			fprintf(stderr, "(devf  t%d::%s:%d) page() returned NULL for offset 0x%x\n",
					pthread_self(), __func__, __LINE__, 0);
		}
		return (-1);
	}
	for(i = 0; i < 64; i++){
		memcpy(buffer, memory, sizeof(buffer));
		memory += PAGE_SIZE;
	}

	return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/flash/mtd-flash/spansion/s29glxxxs_flush.c $ $Rev: 816666 $")
#endif
