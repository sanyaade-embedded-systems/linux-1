/*
 * Secondary CPU startup routine source file.
 *
 * Copyright (C) 2009 Texas Instruments, Inc.
 *
 * Author:
 *      Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * Interface functions needed for the SMP. This file is based on arm
 * realview smp platform.
 * Copyright (c) 2003 ARM Limited.
 *
 * This program is free software,you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/linkage.h>
#include <linux/init.h>

	__INIT

/*
 * OMAP4 specific entry point for secondary CPU to jump from ROM
 * code.  This routine also provides a holding flag into which
 * secondary core is held until we're ready for it to initialise.
 * The primary core will update the this flag using a hardware
 * register AuxCoreBoot1.
 */
ENTRY(omap_secondary_startup)
	mrc	p15, 0, r0, c0, c0, 5
	and	r0, r0, #15
	adr	r4, 1f
	ldmia	r4, {r5, r6}
	sub	r4, r4, r5
	add	r6, r6, r4
hold:	ldr	r7, [r6]	 @ read from AuxCoreBoot1
	cmp	r7, r0
	bne	hold

	/*
	 * we've been released from the holding pen: secondary_stack
	 * should now contain the SVC stack for this core
	 */
	b	secondary_startup

1:	.long	.
	.long	cpu_release

