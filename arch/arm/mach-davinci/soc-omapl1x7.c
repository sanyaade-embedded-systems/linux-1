/*
 * TI OMAPPL1x7 SOC specific code
 *
 * 2008, MontaVista Software, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 */

#include <linux/init.h>
#include <linux/io.h>
#include <linux/types.h>
#include <mach/irqs.h>
#include <mach/irq_cp_intc.h>

static struct cp_intc_irq_data soc_omapl1x7_cp_intc = {
	.base = IO_Dx_INTC_VIRT,
	.num_host_irqs = 2,
	.host_irq = 1,
	.max_irq = IRQ_OMAPL1X7_ARMCLKSTOPREQ + 1,
};

void __init soc_omapl1x7_irq_init(void)
{
	cp_intc_init(&soc_omapl1x7_cp_intc);
}
