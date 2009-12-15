/*
 * arch/arm/mm/cache-l2x0.c - L210/L220 cache controller support
 *
 * Copyright (C) 2007 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/io.h>

#include <asm/cacheflush.h>
#include <asm/hardware/cache-l2x0.h>

#define CACHE_LINE_SIZE		32

static void __iomem *l2x0_base;
static DEFINE_SPINLOCK(l2x0_lock);
static void l2x0_flush_range(unsigned long start, unsigned long end);

static inline void sync_writel(unsigned long val, unsigned long reg,
			       unsigned long complete_mask)
{
	unsigned long flags;

	spin_lock_irqsave(&l2x0_lock, flags);
#ifdef CONFIG_ARM_ERRATA_484863
	asm volatile("swp %0, %0, [%1]\n" : "+r" (val) : "r" (l2x0_base + reg));
#else
	writel(val, l2x0_base + reg);
#endif
	/* wait for the operation to complete */
	while (readl(l2x0_base + reg) & complete_mask)
		;
	spin_unlock_irqrestore(&l2x0_lock, flags);
}

static inline void cache_sync(void)
{
	sync_writel(0, L2X0_CACHE_SYNC, 1);
}

static inline void l2x0_inv_all(void)
{
	/* invalidate all ways */
	sync_writel(0xff, L2X0_INV_WAY, 0xff);
	cache_sync();
}

static void l2x0_inv_range(unsigned long start, unsigned long end)
{
	unsigned long addr;

	if (start & (CACHE_LINE_SIZE - 1)) {
		start &= ~(CACHE_LINE_SIZE - 1);
#ifdef CONFIG_PL310_ERRATA_588369
		l2x0_flush_range(start, start + CACHE_LINE_SIZE - 1);
#else
		sync_writel(start, L2X0_CLEAN_INV_LINE_PA, 1);
#endif
		start += CACHE_LINE_SIZE;
	}

	if (end & (CACHE_LINE_SIZE - 1)) {
		end &= ~(CACHE_LINE_SIZE - 1);
#ifdef CONFIG_PL310_ERRATA_588369
		l2x0_flush_range(end, end + CACHE_LINE_SIZE - 1);
#else
		sync_writel(end, L2X0_CLEAN_INV_LINE_PA, 1);
#endif
		sync_writel(end, L2X0_CLEAN_INV_LINE_PA, 1);
	}

	for (addr = start; addr < end; addr += CACHE_LINE_SIZE)
		sync_writel(addr, L2X0_INV_LINE_PA, 1);
	cache_sync();
}

static void l2x0_clean_range(unsigned long start, unsigned long end)
{
	unsigned long addr;

	start &= ~(CACHE_LINE_SIZE - 1);
	for (addr = start; addr < end; addr += CACHE_LINE_SIZE)
		sync_writel(addr, L2X0_CLEAN_LINE_PA, 1);
	cache_sync();
}

static void l2x0_flush_range(unsigned long start, unsigned long end)
{
	unsigned long addr;

	start &= ~(CACHE_LINE_SIZE - 1);
#ifdef CONFIG_PL310_ERRATA_588369
	/*
	 * Disable Write-Back and Cache Linefill (set bits [1:0] of the Debug
	 * Control Register)
  	 */
	__asm__ __volatile__(
	"stmfd r13!, {r0-r12, r14}\n"
	"mov r0, #3\n"
	"ldr r12, =0x100\n"
	"dsb\n"
	"smc\n"
	"ldmfd r13!, {r0-r12, r14}");

	/* Clean by PA */
	for (addr = start; addr < end; addr += CACHE_LINE_SIZE) 
		sync_writel(addr, L2X0_CLEAN_LINE_PA, 1);
	
	/* Invalidate by PA */
	for (addr = start; addr < end; addr += CACHE_LINE_SIZE) 
		sync_writel(addr, L2X0_INV_LINE_PA, 1);
	/*
	 * Enable Write-Back and Cache Linefill (set bits [1:0] of the Debug
  	 * Control Register)
  	 */
	__asm__ __volatile__(
	"stmfd r13!, {r0-r12, r14}\n"
	"mov r0, #0\n"
	"ldr r12, =0x100\n"
	"dsb\n"
	"smc\n"
	"ldmfd r13!, {r0-r12, r14}");
#else
	for (addr = start; addr < end; addr += CACHE_LINE_SIZE)
		sync_writel(addr, L2X0_CLEAN_INV_LINE_PA, 1);
#endif
	cache_sync();
}

#ifdef CONFIG_OMAP_L2_EVENT_DEBUG
static void pl310_configure_event_counter(void)
{
	__u32 readVal = 0;

	readVal = readl(l2x0_base + L2X0_EVENT_CNT_CTRL);
	readVal = readVal | 0x2;
	readVal = readVal | 0x4;
	writel(readVal, l2x0_base + L2X0_EVENT_CNT_CTRL);
	printk(KERN_INFO "$L2 EVENT Counter reset\n");

	/* EVENT_COUNTER0 : CO */
	writel((1 << 2), l2x0_base + L2X0_EVENT_CNT0_CFG);

	/*EVENT_COUNTER1 : DHRHIT*/
	writel((2 << 2), l2x0_base + L2X0_EVENT_CNT1_CFG);
	printk(KERN_INFO "$L2 EVENT Counter configured\n");


	readVal = readl(l2x0_base + L2X0_EVENT_CNT_CTRL);
	readVal = readVal | 0x1;
	writel(readVal, l2x0_base + L2X0_EVENT_CNT_CTRL);
	printk(KERN_INFO "Event Counter Enabled \n");
}


__u32 pl310_read_event_counter(__u32 event_counter)
{
	__u32 readVal = 0;

	switch (event_counter) {
	case 1:
		readVal = readl(l2x0_base + L2X0_EVENT_CNT0_VAL);
		break;
	case 2:
		readVal = readl(l2x0_base + L2X0_EVENT_CNT1_VAL);
		break;
	};

	return readVal;
}
#endif

void __init l2x0_init(void __iomem *base, __u32 aux_val, __u32 aux_mask)
{
	__u32 aux;

	l2x0_base = base;

	if (!(readl(l2x0_base + L2X0_CTRL) & 1)) {
		/* L2X0 cache controller disabled */
		aux = readl(l2x0_base + L2X0_AUX_CTRL);
		aux &= aux_mask;
		aux |= aux_val;
		writel(aux, l2x0_base + L2X0_AUX_CTRL);

		l2x0_inv_all();

		/* enable L2X0 */
		writel(1, l2x0_base + L2X0_CTRL);
	}

	outer_cache.inv_range = l2x0_inv_range;
	outer_cache.clean_range = l2x0_clean_range;
	outer_cache.flush_range = l2x0_flush_range;

	printk(KERN_INFO "L2X0 cache controller enabled\n");

#ifdef CONFIG_OMAP_L2_EVENT_DEBUG
	pl310_configure_event_counter();
	aux = readl(l2x0_base + L2X0_CTRL);
	printk(KERN_INFO "L2X0_CTRL = %x \n", aux);
#endif
}

