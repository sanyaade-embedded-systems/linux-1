/*
 * mem.c
 *
 * DSP-BIOS Bridge driver support functions for TI OMAP processors.
 *
 * Implementation of platform specific memory services.
 *
 * Copyright (C) 2005-2006 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*  ----------------------------------- Host OS */
#include <dspbridge/host_os.h>

/*  ----------------------------------- DSP/BIOS Bridge */
#include <dspbridge/std.h>
#include <dspbridge/dbdefs.h>
#include <dspbridge/errbase.h>

/*  ----------------------------------- Trace & Debug */
#include <dspbridge/dbc.h>

/*  ----------------------------------- This */
#include <dspbridge/mem.h>
#include <dspbridge/list.h>

/*  ----------------------------------- Defines */
#define MEM512MB   0x1fffffff

/*  ----------------------------------- Globals */


/*
 *  ======== mem_alloc ========
 *  Purpose:
 *      Allocate memory from the paged or non-paged pools.
 */
void *mem_alloc(u32 byte_size, enum mem_poolattrs type)
{
	void *mem = NULL;

	if (byte_size > 0) {
		switch (type) {
		case MEM_NONPAGED:
			/* Fall through */
		case MEM_PAGED:
			mem = kmalloc(byte_size,
				      (in_atomic())? GFP_ATOMIC : GFP_KERNEL);
			break;
		case MEM_LARGEVIRTMEM:
			mem = vmalloc(byte_size);
			break;

		default:
			break;
		}
	}

	return mem;
}

/*
 *  ======== mem_calloc ========
 *  Purpose:
 *      Allocate zero-initialized memory from the paged or non-paged pools.
 */
void *mem_calloc(u32 byte_size, enum mem_poolattrs type)
{
	void *mem = NULL;

	if (byte_size > 0) {
		switch (type) {
		case MEM_NONPAGED:
			/*  Fall through */
		case MEM_PAGED:
			mem = kzalloc(byte_size,
				      (in_atomic())? GFP_ATOMIC : GFP_KERNEL);
			break;
		case MEM_LARGEVIRTMEM:
			mem = __vmalloc(byte_size,
					GFP_KERNEL | __GFP_HIGHMEM | __GFP_ZERO,
					PAGE_KERNEL);
			break;
		default:
			break;
		}
	}

	return mem;
}

/*
 *  ======== mem_exit ========
 *  Purpose:
 *      Discontinue usage of the MEM module.
 */
void mem_exit(void)
{
}

/*
 *  ======== services_mem_init ========
 *  Purpose:
 *      Initialize MEM module private state.
 */
bool services_mem_init(void)
{
	return true;
}
