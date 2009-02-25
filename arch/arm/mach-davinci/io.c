/*
 * DaVinci I/O mapping code
 *
 * Copyright (C) 2005-2006 Texas Instruments
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#include <asm/tlb.h>
#include <asm/memory.h>

#include <asm/mach/map.h>
#include <mach/clock.h>
#include <mach/mux.h>
#include <mach/cpu.h>
#include <mach/omapl1x7.h>

#define OMAPL1X7_ARM_INTC_BASE	0xFFFEE000

extern void davinci_check_revision(void);

/*
 * The machine specific code may provide the extra mapping besides the
 * default mapping provided here.
 */
static struct map_desc davinci_io_desc[] __initdata = {
	{
		.virtual	= IO_VIRT,
		.pfn		= __phys_to_pfn(IO_PHYS),
		.length		= IO_SIZE,
		.type		= MT_DEVICE
	},
};

static struct map_desc omapl1x7_intc_desc[] __initdata = {
	{
		.virtual	= IO_Dx_INTC_VIRT,
		.pfn		= __phys_to_pfn(OMAPL1X7_ARM_INTC_BASE),
		.length		= IO_Dx_INTC_SIZE,
		.type		= MT_DEVICE
	},
};

void __init davinci_map_common_io(void)
{
	iotable_init(davinci_io_desc, ARRAY_SIZE(davinci_io_desc));

	/* Normally devicemaps_init() would flush caches and tlb after
	 * mdesc->map_io(), but we must also do it here because of the CPU
	 * revision check below.
	 */
	local_flush_tlb_all();
	flush_cache_all();

	/* We want to check CPU revision early for cpu_is_xxxx() macros.
	 * IO space mapping must be initialized before we can do that.
	 */
	davinci_check_revision();

	/*
	 * Map the interrupt controller to a common address across all
	 * SoCs.  Other SoC specific mapping can go here as well.
	 */
	if (cpu_is_omapl1x7())
		iotable_init(omapl1x7_intc_desc,
				ARRAY_SIZE(omapl1x7_intc_desc));
}

#define BETWEEN(p, st, sz)	((p) >= (st) && (p) < ((st) + (sz)))
#define XLATE(p, pst, vst)	((void __iomem *)((p) - (pst) + (vst)))

/*
 * Intercept ioremap() requests for addresses in our fixed mapping regions.
 */
void __iomem *davinci_ioremap(unsigned long p, size_t size, unsigned int type)
{
	if (BETWEEN(p, IO_PHYS, IO_SIZE))
		return XLATE(p, IO_PHYS, IO_VIRT);

	return __arm_ioremap(p, size, type);
}
EXPORT_SYMBOL(davinci_ioremap);

void davinci_iounmap(volatile void __iomem *addr)
{
	unsigned long virt = (unsigned long)addr;

	if (virt >= VMALLOC_START && virt < VMALLOC_END)
		__iounmap(addr);
}
EXPORT_SYMBOL(davinci_iounmap);
