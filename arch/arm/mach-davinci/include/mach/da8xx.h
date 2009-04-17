/*
 * This file contains the processor specific definitions
 * of the TI DA830/DA850
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
#ifndef __ASM_ARCH_DA8XX_H
#define __ASM_ARCH_DA8XX_H

#include <mach/hardware.h>
#include <mach/emac.h>

void __init da830_init(void);
void __init da850_init(void);
void da8xx_init_rtc(void);

struct spi_board_info;
void __init da8xx_init_spi0(unsigned char* chip_sel, unsigned int num_sel,
	struct spi_board_info *info, unsigned num_dev);
void __init da850_init_spi1(unsigned char* chip_sel, unsigned int num_sel,
	struct spi_board_info *info, unsigned num_dev);

void __init da830_init_mcasp1(void);
void da830_init_emac(struct emac_platform_data *);
void da850_init_emac(struct emac_platform_data *);
int get_async3_src(void);

#define DA8XX_BOOT_CFG_BASE		0x01C14000

#define DA8XX_KICK0             (DA8XX_BOOT_CFG_BASE + 0x38)
#define DA8XX_KICK0_MAGIC       0x83e70b13
#define DA8XX_KICK1             (DA8XX_BOOT_CFG_BASE + 0x3c)
#define DA8XX_KICK1_MAGIC       0x95a4f1e0

#define DA8XX_PINMUX0   (0x120)
#define DA8XX_PINMUX1   (0x124)
#define DA8XX_PINMUX2   (0x128)
#define DA8XX_PINMUX3   (0x12C)
#define DA8XX_PINMUX4   (0x130)
#define DA8XX_PINMUX5   (0x134)
#define DA8XX_PINMUX6   (0x138)
#define DA8XX_PINMUX7   (0x13C)
#define DA8XX_PINMUX8   (0x140)
#define DA8XX_PINMUX9   (0x144)
#define DA8XX_PINMUX10  (0x148)
#define DA8XX_PINMUX11  (0x14C)
#define DA8XX_PINMUX12  (0x150)
#define DA8XX_PINMUX13  (0x154)
#define DA8XX_PINMUX14  (0x158)
#define DA8XX_PINMUX15  (0x15C)
#define DA8XX_PINMUX16  (0x160)
#define DA8XX_PINMUX17  (0x164)
#define DA8XX_PINMUX18  (0x168)
#define DA8XX_PINMUX19  (0x16C)

#define DA8XX_CFGCHIP0  (DA8XX_BOOT_CFG_BASE + 0x17c)
#define DA8XX_CFGCHIP1  (DA8XX_BOOT_CFG_BASE + 0x180)
#define DA8XX_CFGCHIP2  (DA8XX_BOOT_CFG_BASE + 0x184)
#define DA8XX_CFGCHIP3  (DA8XX_BOOT_CFG_BASE + 0x188)
#define DA8XX_CFGCHIP4  (DA8XX_BOOT_CFG_BASE + 0x18c)

#define DA8XX_I2C0_BASE		0x01C22000
#define DA8XX_I2C1_BASE		0x01E28000

#define DA8XX_EMAC_CPPI_PORT_BASE	0x01E20000
#define DA8XX_EMAC_CTRL_REG_OFFSET     0x3000
#define DA8XX_EMAC_MOD_REG_OFFSET      0x2000
#define DA8XX_EMAC_RAM_OFFSET          0x0000
#define DA8XX_MDIO_REG_OFFSET          0x4000
#define DA8XX_EMAC_CTRL_RAM_SIZE       SZ_8K

#define DA8XX_LCD_CNTRL_BASE			0x01E13000
#define DA8XX_EMIF30_ASYNC_DATA_CE5_BASE	0xC0000000

#endif /* __ASM_ARCH_DA8XX_H */
