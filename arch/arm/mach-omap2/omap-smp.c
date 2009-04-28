/*
 * OMAP4 SMP source file. It contains platform specific fucntions
 * needed for the linux smp kernel.
 *
 * Copyright (C) 2009 Texas Instruments, Inc.
 *
 * Author:
 *      Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * Platform file needed for the OMAP4 SMP. This file is based on arm
 * realview smp platform.
 * * Copyright (c) 2002 ARM Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/smp.h>
#include <linux/io.h>

#include <asm/cacheflush.h>
#include <mach/scu.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>

/* Registers used for communicating startup information */
#define OMAP4_AUXCOREBOOT_REG0		(OMAP44XX_VA_WKUPGEN_BASE + 0x800)
#define OMAP4_AUXCOREBOOT_REG1		(OMAP44XX_VA_WKUPGEN_BASE + 0x804)

/*
 * Control for which core is the next to come out of the secondary
 * boot "Auxcontrol_register"
 */
int __cpuinitdata cpu_release = -1;

/*
 * Setup the SCU
 */
static void scu_enable(void)
{
	u32 scu_ctrl;
	void __iomem *scu_base = OMAP44XX_VA_SCU_BASE;

	scu_ctrl = __raw_readl(scu_base + SCU_CTRL);
	scu_ctrl |= 1;
	__raw_writel(scu_ctrl, scu_base + SCU_CTRL);
}

/*
 * Use SCU config register to count number of cores
 */
static unsigned int __init get_core_count(void)
{
	unsigned int ncores;
	void __iomem *scu_base = OMAP44XX_VA_SCU_BASE;

	if (scu_base) {
		ncores = __raw_readl(scu_base + SCU_CONFIG);
		ncores = (ncores & 0x03) + 1;
	} else {
			ncores = 1;
	}

	return ncores;
}

static DEFINE_SPINLOCK(boot_lock);

void __cpuinit platform_secondary_init(unsigned int cpu)
{
	trace_hardirqs_off();

	/*
	 * If any interrupts are already enabled for the primary
	 * core (e.g. timer irq), then they will not have been enabled
	 * for us: do so
	 */

	gic_cpu_init(0, IO_ADDRESS(OMAP44XX_GIC_CPU_BASE));

	/*
	 * Let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	cpu_release = -1;
	smp_wmb();

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

int __cpuinit boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	/*
	 * Set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	/*
	 * The secondary processor is waiting for an event to come out of
	 * wfe. Release it, then wait for it to flag that it has been
	 * released by resetting cpu_release.
	 *
	 * Signal the ROM code that the secondary core can be released
	 */
	cpu_release = cpu;
	__raw_writel(cpu, OMAP4_AUXCOREBOOT_REG1);
	flush_cache_all();
	/*
	 * Send a 'sev' to wake the secondary core again because
	 * ROM code will put core in WFE till the cpu_release
	 * flag is set.
	 */
	set_event();
	mb();

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (cpu_release == -1)
			break;

		udelay(10);
	}

	/*
	 * Now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	return cpu_release != -1 ? -ENOSYS : 0;
}

static void __init wakeup_secondary(void)
{

	/* cpu is not to be released from the hold yet */
	cpu_release = -1;

	/*
	 * write the address of secondary startup into the system-wide
	 * AuxCoreBoot0 where ROM code will jump and start executing
	 * on secondary core
	 */
	__raw_writel(virt_to_phys(omap_secondary_startup),	   \
					OMAP4_AUXCOREBOOT_REG0);
	/*
	 * Send a 'sev' to wake the secondary core from WFE.
	 */
	set_event();
	mb();
}

/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
void __init smp_init_cpus(void)
{
	unsigned int i, ncores = get_core_count();

	for (i = 0; i < ncores; i++)
		cpu_set(i, cpu_possible_map);
}

void __init smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int ncores = get_core_count();
	unsigned int cpu = smp_processor_id();
	int i;

	/* sanity check */
	if (ncores == 0) {
		printk(KERN_ERR
		       "OMAP4: strange core count of 0? Default to 1\n");
		ncores = 1;
	}

	if (ncores > num_possible_cpus()) {
		printk(KERN_WARNING
		       "OMAP4: no. of cores (%d) greater than configured "
		       "maximum of %d - clipping\n",
		       ncores, num_possible_cpus());
		ncores = num_possible_cpus();
	}
	smp_store_cpu_info(cpu);

	/*
	 * are we trying to boot more cores than exist?
	 */
	if (max_cpus > ncores)
		max_cpus = ncores;

#ifdef CONFIG_LOCAL_TIMERS
	/*
	 * Enable the local timer for primary CPU. If the device is
	 * dummy (!CONFIG_LOCAL_TIMERS), it was already registers in
	 * omap_timer_init
	 */
	local_timer_setup();
#endif

	/*
	 * Initialise the present map, which describes the set of CPUs
	 * actually populated at the present time.
	 */
	for (i = 0; i < max_cpus; i++)
		cpu_set(i, cpu_present_map);

	/*
	 * Initialise the SCU and wake up the secondary core using
	 *  wakeup_secondary().
	 */
	if (max_cpus > 1) {
		scu_enable();
		/*
		 * Ensure that the data accessed by CPU0 before the SCU was
		 * initialised is visible to CPU1.
		 */
		flush_cache_all();
		wakeup_secondary();
	}
}
