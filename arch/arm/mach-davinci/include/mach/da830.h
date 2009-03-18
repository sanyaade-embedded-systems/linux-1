/*
 * This file contains the processor specific definitions
 * of the TI DA830.
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
#ifndef __ASM_ARCH_DA830_H
#define __ASM_ARCH_DA830_H

#include <mach/hardware.h>

void __init da830_init(void);
void da830_init_rtc(void);

#define DA830_BOOT_CFG_BASE		0x01C14000

#define DA830_KICK0             (DA830_BOOT_CFG_BASE + 0x38)
#define DA830_KICK0_MAGIC       0x83e70b13
#define DA830_KICK1             (DA830_BOOT_CFG_BASE + 0x3c)
#define DA830_KICK1_MAGIC       0x95a4f1e0

/*#define PINMUX2         (DAVINCI_SYSTEM_MODULE_BASE + 0x08)
#define PINMUX3         (DAVINCI_SYSTEM_MODULE_BASE + 0x0c)
#define PINMUX4         (DAVINCI_SYSTEM_MODULE_BASE + 0x10)*/

#if 0
#define DA830_PINMUX0   (DA830_BOOT_CFG_BASE + 0x120)
#define DA830_PINMUX1   (DA830_BOOT_CFG_BASE + 0x124)
#define DA830_PINMUX2   (DA830_BOOT_CFG_BASE + 0x128)
#define DA830_PINMUX3   (DA830_BOOT_CFG_BASE + 0x12C)
#define DA830_PINMUX4   (DA830_BOOT_CFG_BASE + 0x130)
#define DA830_PINMUX5   (DA830_BOOT_CFG_BASE + 0x134)
#define DA830_PINMUX6   (DA830_BOOT_CFG_BASE + 0x138)
#define DA830_PINMUX7   (DA830_BOOT_CFG_BASE + 0x13C)
#define DA830_PINMUX8   (DA830_BOOT_CFG_BASE + 0x140)
#define DA830_PINMUX9   (DA830_BOOT_CFG_BASE + 0x144)
#define DA830_PINMUX10  (DA830_BOOT_CFG_BASE + 0x148)
#define DA830_PINMUX11  (DA830_BOOT_CFG_BASE + 0x14C)
#define DA830_PINMUX12  (DA830_BOOT_CFG_BASE + 0x150)
#define DA830_PINMUX13  (DA830_BOOT_CFG_BASE + 0x154)
#define DA830_PINMUX14  (DA830_BOOT_CFG_BASE + 0x158)
#define DA830_PINMUX15  (DA830_BOOT_CFG_BASE + 0x15C)
#define DA830_PINMUX16  (DA830_BOOT_CFG_BASE + 0x160)
#define DA830_PINMUX17  (DA830_BOOT_CFG_BASE + 0x164)
#define DA830_PINMUX18  (DA830_BOOT_CFG_BASE + 0x168)
#define DA830_PINMUX19  (DA830_BOOT_CFG_BASE + 0x16C)
#else
#define DA830_PINMUX0   (0x120)
#define DA830_PINMUX1   (0x124)
#define DA830_PINMUX2   (0x128)
#define DA830_PINMUX3   (0x12C)
#define DA830_PINMUX4   (0x130)
#define DA830_PINMUX5   (0x134)
#define DA830_PINMUX6   (0x138)
#define DA830_PINMUX7   (0x13C)
#define DA830_PINMUX8   (0x140)
#define DA830_PINMUX9   (0x144)
#define DA830_PINMUX10  (0x148)
#define DA830_PINMUX11  (0x14C)
#define DA830_PINMUX12  (0x150)
#define DA830_PINMUX13  (0x154)
#define DA830_PINMUX14  (0x158)
#define DA830_PINMUX15  (0x15C)
#define DA830_PINMUX16  (0x160)
#define DA830_PINMUX17  (0x164)
#define DA830_PINMUX18  (0x168)
#define DA830_PINMUX19  (0x16C)
#endif

#define DA830_CFGCHIP0  (DA830_BOOT_CFG_BASE + 0x17c)
#define DA830_CFGCHIP1  (DA830_BOOT_CFG_BASE + 0x180)
#define DA830_CFGCHIP2  (DA830_BOOT_CFG_BASE + 0x184)
#define DA830_CFGCHIP3  (DA830_BOOT_CFG_BASE + 0x188)
#define DA830_CFGCHIP4  (DA830_BOOT_CFG_BASE + 0x18c)

#define DA830_EMAC_CPPI_PORT_BASE	0x01E20000
#define DA830_EMAC_CPGMACSS_BASE	0x01E22000
#define DA830_EMAC_CPGMAC_BASE	0x01E23000
#define DA830_EMAC_MDIO_BASE		0x01E24000
#define DA830_I2C0_BASE		0x01C22000
#define DA830_I2C1_BASE		0x01E28000

#define DA830_LCD_CNTRL_BASE			0x01E13000
#define DA830_EMIF30_ASYNC_DATA_CE5_BASE	0xC0000000

/* da830 cfg regs have to be unlocked by writing magic values to kick regs */
#ifdef CONFIG_MACH_DA830_EVM
#define davinci_cfg_readl(a) ({                                         \
	davinci_writel(DA830_KICK0_MAGIC, DA830_KICK0); /* unlock */    \
	davinci_writel(DA830_KICK1_MAGIC, DA830_KICK1);                 \
	davinci_readl(a);                                               \
	/* davinci_writel(0, DA830_KICK0);*/ /* lock */                 \
})
#define davinci_cfg_writel(v,a) ({                                      \
	davinci_writel(DA830_KICK0_MAGIC, DA830_KICK0); /* unlock */    \
	davinci_writel(DA830_KICK1_MAGIC, DA830_KICK1);                 \
	davinci_writel(v, a);                                            \
	/*davinci_writel(0, DA830_KICK0);*/ /* lock */                  \
})
#else
#define davinci_cfg_readl(a)    (davinci_readl(a))
#define davinci_cfg_writel(v, a) (davinci_writel(v, a))
#endif

#endif /* __ASM_ARCH_DA830_H */
