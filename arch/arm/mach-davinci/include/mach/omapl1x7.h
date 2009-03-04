/*
 * This file contains the processor specific definitions
 * of the TI OMAPL1x7.
 *
 * Copyright (C) 2008 Texas Instruments.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __ASM_ARCH_OMAPL1X7_H
#define __ASM_ARCH_OMAPL1X7_H

#include <mach/hardware.h>

void __init omapl1x7_init(void);
void omapl1x7_init_rtc(void);

#define OMAPL1X7_BOOT_CFG_BASE		0x01C14000

#define OMAPL1X7_KICK0             (OMAPL1X7_BOOT_CFG_BASE + 0x38)
#define OMAPL1X7_KICK0_MAGIC       0x83e70b13
#define OMAPL1X7_KICK1             (OMAPL1X7_BOOT_CFG_BASE + 0x3c)
#define OMAPL1X7_KICK1_MAGIC       0x95a4f1e0

/*#define PINMUX2         (DAVINCI_SYSTEM_MODULE_BASE + 0x08)
#define PINMUX3         (DAVINCI_SYSTEM_MODULE_BASE + 0x0c)
#define PINMUX4         (DAVINCI_SYSTEM_MODULE_BASE + 0x10)*/

#if 0
#define OMAPL1X7_PINMUX0   (OMAPL1X7_BOOT_CFG_BASE + 0x120)
#define OMAPL1X7_PINMUX1   (OMAPL1X7_BOOT_CFG_BASE + 0x124)
#define OMAPL1X7_PINMUX2   (OMAPL1X7_BOOT_CFG_BASE + 0x128)
#define OMAPL1X7_PINMUX3   (OMAPL1X7_BOOT_CFG_BASE + 0x12C)
#define OMAPL1X7_PINMUX4   (OMAPL1X7_BOOT_CFG_BASE + 0x130)
#define OMAPL1X7_PINMUX5   (OMAPL1X7_BOOT_CFG_BASE + 0x134)
#define OMAPL1X7_PINMUX6   (OMAPL1X7_BOOT_CFG_BASE + 0x138)
#define OMAPL1X7_PINMUX7   (OMAPL1X7_BOOT_CFG_BASE + 0x13C)
#define OMAPL1X7_PINMUX8   (OMAPL1X7_BOOT_CFG_BASE + 0x140)
#define OMAPL1X7_PINMUX9   (OMAPL1X7_BOOT_CFG_BASE + 0x144)
#define OMAPL1X7_PINMUX10  (OMAPL1X7_BOOT_CFG_BASE + 0x148)
#define OMAPL1X7_PINMUX11  (OMAPL1X7_BOOT_CFG_BASE + 0x14C)
#define OMAPL1X7_PINMUX12  (OMAPL1X7_BOOT_CFG_BASE + 0x150)
#define OMAPL1X7_PINMUX13  (OMAPL1X7_BOOT_CFG_BASE + 0x154)
#define OMAPL1X7_PINMUX14  (OMAPL1X7_BOOT_CFG_BASE + 0x158)
#define OMAPL1X7_PINMUX15  (OMAPL1X7_BOOT_CFG_BASE + 0x15C)
#define OMAPL1X7_PINMUX16  (OMAPL1X7_BOOT_CFG_BASE + 0x160)
#define OMAPL1X7_PINMUX17  (OMAPL1X7_BOOT_CFG_BASE + 0x164)
#define OMAPL1X7_PINMUX18  (OMAPL1X7_BOOT_CFG_BASE + 0x168)
#define OMAPL1X7_PINMUX19  (OMAPL1X7_BOOT_CFG_BASE + 0x16C)
#else
#define OMAPL1X7_PINMUX0   (0x120)
#define OMAPL1X7_PINMUX1   (0x124)
#define OMAPL1X7_PINMUX2   (0x128)
#define OMAPL1X7_PINMUX3   (0x12C)
#define OMAPL1X7_PINMUX4   (0x130)
#define OMAPL1X7_PINMUX5   (0x134)
#define OMAPL1X7_PINMUX6   (0x138)
#define OMAPL1X7_PINMUX7   (0x13C)
#define OMAPL1X7_PINMUX8   (0x140)
#define OMAPL1X7_PINMUX9   (0x144)
#define OMAPL1X7_PINMUX10  (0x148)
#define OMAPL1X7_PINMUX11  (0x14C)
#define OMAPL1X7_PINMUX12  (0x150)
#define OMAPL1X7_PINMUX13  (0x154)
#define OMAPL1X7_PINMUX14  (0x158)
#define OMAPL1X7_PINMUX15  (0x15C)
#define OMAPL1X7_PINMUX16  (0x160)
#define OMAPL1X7_PINMUX17  (0x164)
#define OMAPL1X7_PINMUX18  (0x168)
#define OMAPL1X7_PINMUX19  (0x16C)
#endif

#define OMAPL1X7_CFGCHIP0  (OMAPL1X7_BOOT_CFG_BASE + 0x17c)
#define OMAPL1X7_CFGCHIP1  (OMAPL1X7_BOOT_CFG_BASE + 0x180)
#define OMAPL1X7_CFGCHIP2  (OMAPL1X7_BOOT_CFG_BASE + 0x184)
#define OMAPL1X7_CFGCHIP3  (OMAPL1X7_BOOT_CFG_BASE + 0x188)
#define OMAPL1X7_CFGCHIP4  (OMAPL1X7_BOOT_CFG_BASE + 0x18c)

#define OMAPL1X7_EMAC_CPPI_PORT_BASE	0x01E20000
#define OMAPL1X7_EMAC_CPGMACSS_BASE	0x01E22000
#define OMAPL1X7_EMAC_CPGMAC_BASE	0x01E23000
#define OMAPL1X7_EMAC_MDIO_BASE		0x01E24000
#define OMAPL1X7_I2C0_BASE		0x01C22000
#define OMAPL1X7_I2C1_BASE		0x01E28000

#define OMAPL1X7_LCD_CNTRL_BASE			0x01E13000
#define OMAPL1X7_EMIF30_ASYNC_DATA_CE5_BASE	0xC0000000

/* omapl1x7 cfg regs have to be unlocked by writing magic values to kick regs */
#ifdef CONFIG_MACH_OMAPL1X7_EVM
#define davinci_cfg_readl(a) ({                                         \
	davinci_writel(OMAPL1X7_KICK0_MAGIC, OMAPL1X7_KICK0); /* unlock */    \
	davinci_writel(OMAPL1X7_KICK1_MAGIC, OMAPL1X7_KICK1);                 \
	davinci_readl(a);                                               \
	/* davinci_writel(0, OMAPL1X7_KICK0);*/ /* lock */                 \
})
#define davinci_cfg_writel(v,a) ({                                      \
	davinci_writel(OMAPL1X7_KICK0_MAGIC, OMAPL1X7_KICK0); /* unlock */    \
	davinci_writel(OMAPL1X7_KICK1_MAGIC, OMAPL1X7_KICK1);                 \
	davinci_writel(v, a);                                            \
	/*davinci_writel(0, OMAPL1X7_KICK0);*/ /* lock */                  \
})
#else
#define davinci_cfg_readl(a)    (davinci_readl(a))
#define davinci_cfg_writel(v, a) (davinci_writel(v, a))
#endif

#endif /* __ASM_ARCH_OMAPL1X7_H */
