/*
 * TI DaVinci Power and Sleep Controller (PSC)
 *
 * Copyright (C) 2006 Texas Instruments.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>

#include <mach/cpu.h>
#include <mach/da8xx.h>
#include <mach/psc.h>
#include <mach/mux.h>

#define DAVINCI_PWR_SLEEP_CNTRL_BASE	0x01C41000
#define DA8XX_PSC0_BASE		0x01C10000
#define DA8XX_PSC1_BASE		0x01E27000

/* PSC register offsets */
#define EPCPR		0x070
#define PTCMD		0x120
#define PTSTAT		0x128
#define PDSTAT		0x200
#define PDCTL1		0x304
#define MDSTAT		0x800
#define MDCTL		0xA00
#define MDSTAT_STATE_MASK 0x1f

static u32 davinci_psc_base[] = { (u32)IO_ADDRESS(DAVINCI_PWR_SLEEP_CNTRL_BASE) };
static u32 da8xx_psc_base[] = { (u32)IO_ADDRESS(DA8XX_PSC0_BASE),
				   (u32)IO_ADDRESS(DA8XX_PSC1_BASE) };
static u32 *psc_base_array;

/* Return nonzero iff the domain's clock is active */
int __init davinci_psc_is_clk_active(unsigned int id)
{
	void __iomem *psc_base = IO_ADDRESS(DAVINCI_PWR_SLEEP_CNTRL_BASE);
	u32 mdstat = __raw_readl(psc_base + MDSTAT + 4 * id);

	/* if clocked, state can be "Enable" or "SyncReset" */
	return mdstat & BIT(12);
}

/* Enable or disable a PSC domain */
void davinci_psc_config(unsigned int domain, unsigned int id, char enable)
{
	u32 epcpr, ptcmd, ptstat, pdstat, pdctl1, mdstat, mdctl;
	u32 next_state = enable ? 0x3 : 0x2; /* 0x3 enables, 0x2 disables */
	void __iomem *psc_base;
	u32 domain_shift;

	if (id > 64)
		return;

	if (cpu_is_da8xx())
		psc_base_array = da8xx_psc_base;
	else
		psc_base_array = davinci_psc_base;

	if (cpu_is_da8xx() && id >= 32) {
		id -= 32;
		domain = 1;
	}

	psc_base = (void __iomem *)(psc_base_array[domain]);
	/* XXX domain != which psc reg to use--we always use domain 0 */
	domain_shift = 0;	

	mdctl = __raw_readl(psc_base + MDCTL + 4 * id);
	mdctl &= ~MDSTAT_STATE_MASK;
	mdctl |= next_state;
	__raw_writel(mdctl, psc_base + MDCTL + 4 * id);

	pdstat = __raw_readl(psc_base + PDSTAT);
	if ((pdstat & 0x00000001) == 0) {
		pdctl1 = __raw_readl(psc_base + PDCTL1);
		pdctl1 |= 0x1;
		__raw_writel(pdctl1, psc_base + PDCTL1);

		ptcmd = 1 << domain_shift;
		__raw_writel(ptcmd, psc_base + PTCMD);

		do {
			epcpr = __raw_readl(psc_base + EPCPR);
		} while ((((epcpr >> domain_shift) & 1) == 0));

		pdctl1 = __raw_readl(psc_base + PDCTL1);
		pdctl1 |= 0x100;
		__raw_writel(pdctl1, psc_base + PDCTL1);

		do {
			ptstat = __raw_readl(psc_base +
					       PTSTAT);
		} while (!(((ptstat >> domain_shift) & 1) == 0));
	} else {
		ptcmd = 1 << domain_shift;
		__raw_writel(ptcmd, psc_base + PTCMD);

		do {
			ptstat = __raw_readl(psc_base + PTSTAT);
		} while (!(((ptstat >> domain_shift) & 1) == 0));
	}

	do {
		mdstat = __raw_readl(psc_base + MDSTAT + 4 * id);
	} while (!((mdstat & MDSTAT_STATE_MASK) == next_state));
}
