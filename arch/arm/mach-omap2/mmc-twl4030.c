/*
 * linux/arch/arm/mach-omap2/mmc-twl4030.c
 *
 * Copyright (C) 2007-2008 Texas Instruments
 * Copyright (C) 2008 Nokia Corporation
 * Author: Texas Instruments
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/mmc/host.h>
#include <linux/regulator/consumer.h>

#include <mach/hardware.h>
#include <mach/control.h>
#include <mach/mmc.h>
#include <mach/board.h>

#ifdef CONFIG_MMC_EMBEDDED_SDIO
#include <asm/mach/mmc.h>
#include <linux/mmc/sdio_ids.h>
#endif

#include "mmc-twl4030.h"
#include <linux/i2c/twl.h>

#if defined(CONFIG_REGULATOR) && \
	(defined(CONFIG_MMC_OMAP_HS) || defined(CONFIG_MMC_OMAP_HS_MODULE))

static u16 control_pbias_offset;
static u16 control_devconf1_offset;
static u16 control_mmc1;

#define HSMMC_NAME_LEN	9
#define	PHOENIX_MMC_CTRL		0x0C	/* 0xEE */
#define PHOENIX_CFG_INPUT_PUPD3		0x10    /* 0xF2 */
#define MMC_GRP				0x18
#define MMC_TRANS			0x19
#define MMC_STATE			0x1a
#define MMC_VOLTAGE			0x1b
#define MMC_VDD_33_34			0x00200000
#define MMC_VDD_32_33			0x00100000
#define MMC_VDD_165_195			0x00000080

static struct twl_mmc_controller {
	struct omap_mmc_platform_data	*mmc;
	/* Vcc == configured supply
	 * Vcc_alt == optional
	 *   -	MMC1, supply for DAT4..DAT7
	 *   -	MMC2/MMC2, external level shifter voltage supply, for
	 *	chip (SDIO, eMMC, etc) or transceiver (MMC2 only)
	 */
	struct regulator		*vcc;
	struct regulator		*vcc_aux;
	char				name[HSMMC_NAME_LEN + 1];
} hsmmc[OMAP44XX_NR_MMC];

static int twl_mmc_card_detect(int irq)
{
	if (!cpu_is_omap44xx())	{
		unsigned i;
		for (i = 0; i < ARRAY_SIZE(hsmmc); i++) {
			struct omap_mmc_platform_data *mmc;

			mmc = hsmmc[i].mmc;
			if (!mmc)
				continue;
			if (irq != mmc->slots[0].card_detect_irq)
				continue;

			/* NOTE: assumes card detect signal is active-low */
			return !gpio_get_value_cansleep
					(mmc->slots[0].switch_pin);
		}
	} else {
		/* BIT0 of REG_SIMCTRL
		 * 0 - Card not present
		 * 1 - Card present
		 */
		u8 read_reg;
		unsigned res;
		res = twl_i2c_read_u8(TWL6030_MODULE_PM_MISC,
					&read_reg, PHOENIX_MMC_CTRL);
		if (res < 0) {
			printk(KERN_ERR"%s: i2c_read fail at %x \n",
						__func__, PHOENIX_MMC_CTRL);
			return -1;
		} else {
			return read_reg & 0x1;
		}
	}
	return -ENOSYS;
}

static int twl_mmc_get_ro(struct device *dev, int slot)
{
	struct omap_mmc_platform_data *mmc = dev->platform_data;

	/* NOTE: assumes write protect signal is active-high */
	return gpio_get_value_cansleep(mmc->slots[0].gpio_wp);
}

static int twl_mmc_get_cover_state(struct device *dev, int slot)
{
	struct omap_mmc_platform_data *mmc = dev->platform_data;

	/* NOTE: assumes card detect signal is active-low */
	return !gpio_get_value_cansleep(mmc->slots[0].switch_pin);
}

/*
 * MMC Slot Initialization.
 */
static int twl_mmc_late_init(struct device *dev)
{
	struct omap_mmc_platform_data *mmc = dev->platform_data;
	int ret = 0;
	int i;
	u8 regs;

	/* MMC/SD/SDIO doesn't require a card detect switch */
	if (!cpu_is_omap44xx()) {
		if (gpio_is_valid(mmc->slots[0].switch_pin)) {
			ret = gpio_request(mmc->slots[0].switch_pin, "mmc_cd");
			if (ret)
				goto done;
			ret = gpio_direction_input(mmc->slots[0].switch_pin);
			if (ret)
				goto err;
		}
	}

	/* require at least main regulator */
	for (i = 0; i < ARRAY_SIZE(hsmmc); i++) {
		if (hsmmc[i].name == mmc->slots[0].name) {
			struct regulator *reg;

			hsmmc[i].mmc = mmc;

			reg = regulator_get(dev, "vmmc");
			if (IS_ERR(reg)) {
				dev_dbg(dev, "vmmc regulator missing\n");
				/* HACK: until fixed.c regulator is usable,
				 * we don't require a main regulator
				 * for MMC2 or MMC3
				 */
				if (i != 0)
					break;
				ret = PTR_ERR(reg);
				hsmmc[i].vcc = NULL;
				goto err;
			}
			hsmmc[i].vcc = reg;
			mmc->slots[0].ocr_mask = mmc_regulator_get_ocrmask(reg);

			/* allow an aux regulator */
			reg = regulator_get(dev, "vmmc_aux");
			hsmmc[i].vcc_aux = IS_ERR(reg) ? NULL : reg;

			/* UGLY HACK:  workaround regulator framework bugs.
			 * When the bootloader leaves a supply active, it's
			 * initialized with zero usecount ... and we can't
			 * disable it without first enabling it.  Until the
			 * framework is fixed, we need a workaround like this
			 * (which is safe for MMC, but not in general).
			 */
			if (!cpu_is_omap44xx()) {
				if (regulator_is_enabled(hsmmc[i].vcc) > 0) {
					regulator_enable(hsmmc[i].vcc);
					regulator_disable(hsmmc[i].vcc);
				}
				if (hsmmc[i].vcc_aux) {
					if (regulator_is_enabled(reg) > 0) {
						regulator_enable(reg);
						regulator_disable(reg);
					}
				}
			}
			if (i == 0) {
				/* Enabling the LDO for MMC-1 */
				regs = 0x01;
				twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
								regs, MMC_GRP);
				regs = 0x03;
				twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_TRANS);
				regs = 0x21;
				twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_STATE);
				regs = 0x15;
				twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_VOLTAGE);

				/* configure card-detect for MMC-1 */
				msleep(200);
				regs = 0x04;
				twl_i2c_write_u8(TWL6030_MODULE_PM_MISC,
							regs, PHOENIX_MMC_CTRL);
				regs = 0x11;
				twl_i2c_write_u8(TWL6030_MODULE_PM_MISC,
						regs, PHOENIX_CFG_INPUT_PUPD3);
			}

			break;
		}
	}

	return 0;

err:
	gpio_free(mmc->slots[0].switch_pin);
done:
	mmc->slots[0].card_detect_irq = 0;
	mmc->slots[0].card_detect = NULL;

	dev_err(dev, "err %d configuring card detect\n", ret);
	return ret;
}

static void twl_mmc_cleanup(struct device *dev)
{
	struct omap_mmc_platform_data *mmc = dev->platform_data;
	int i;

	gpio_free(mmc->slots[0].switch_pin);
	for(i = 0; i < ARRAY_SIZE(hsmmc); i++) {
		regulator_put(hsmmc[i].vcc);
		regulator_put(hsmmc[i].vcc_aux);
	}
}

#ifdef CONFIG_PM

static int twl_mmc_suspend(struct device *dev, int slot)
{
	struct omap_mmc_platform_data *mmc = dev->platform_data;

	disable_irq(mmc->slots[0].card_detect_irq);
	return 0;
}

static int twl_mmc_resume(struct device *dev, int slot)
{
	struct omap_mmc_platform_data *mmc = dev->platform_data;

	enable_irq(mmc->slots[0].card_detect_irq);
	return 0;
}

#else
#define twl_mmc_suspend	NULL
#define twl_mmc_resume	NULL
#endif

static int twl_mmc1_set_power(struct device *dev, int slot, int power_on,
				int vdd)
{
	u32 reg;
	int ret = 0;
	struct twl_mmc_controller *c = &hsmmc[0];
	struct omap_mmc_platform_data *mmc = dev->platform_data;
	u8 regs;

	/*
	 * Assume we power both OMAP VMMC1 (for CMD, CLK, DAT0..3) and the
	 * card with Vcc regulator (from twl4030 or whatever).  OMAP has both
	 * 1.8V and 3.0V modes, controlled by the PBIAS register.
	 *
	 * In 8-bit modes, OMAP VMMC1A (for DAT4..7) needs a supply, which
	 * is most naturally TWL VSIM; those pins also use PBIAS.
	 *
	 * FIXME handle VMMC1A as needed ...
	 */
	if (power_on) {
		if (cpu_is_omap2430()) {
			reg = omap_ctrl_readl(OMAP243X_CONTROL_DEVCONF1);
			if ((1 << vdd) >= MMC_VDD_30_31)
				reg |= OMAP243X_MMC1_ACTIVE_OVERWRITE;
			else
				reg &= ~OMAP243X_MMC1_ACTIVE_OVERWRITE;
			omap_ctrl_writel(reg, OMAP243X_CONTROL_DEVCONF1);
		}

		if (!cpu_is_omap44xx()) {
			if (mmc->slots[0].internal_clock) {
				reg = omap_ctrl_readl(OMAP2_CONTROL_DEVCONF0);
				reg |= OMAP2_MMCSDIO1ADPCLKISEL;
				omap_ctrl_writel(reg, OMAP2_CONTROL_DEVCONF0);
			}
			reg = omap_ctrl_readl(control_pbias_offset);
			reg |= OMAP2_PBIASSPEEDCTRL0;
			reg &= ~OMAP2_PBIASLITEPWRDNZ0;
			omap_ctrl_writel(reg, control_pbias_offset);
		} else {
			reg = omap_ctrl_readl(control_pbias_offset);
			reg &= ~(OMAP4_MMC1_PBIASLITE_PWRDNZ |
						OMAP4_MMC1_PWRDWNZ);
			omap_ctrl_writel(reg, control_pbias_offset);
		}
		/* Hack need to fix it */
		if ((vdd == 0x12) || (vdd == 0x7)) {
			regs = 0x01;
			twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_GRP);
			regs = 0x03;
			twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_TRANS);
			regs = 0x21;
			twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_STATE);
			regs = 0x15;
			twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
						regs, MMC_VOLTAGE);
		} else {
			regs = 0x01;
			twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_GRP);
			regs = 0x03;
			twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_TRANS);
			regs = 0x00;
			twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_STATE);
			regs = 0x09;
			twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_STATE);
			regs = 0x15;
			twl_i2c_write_u8(TWL6030_MODULE_PM_SLAVE_LDO,
							regs, MMC_VOLTAGE);
		}

		if (!cpu_is_omap44xx())
			ret = mmc_regulator_set_ocr(c->vcc, vdd);

		/* 100ms delay required for PBIAS configuration */
		msleep(100);
		if (!cpu_is_omap44xx()) {
			reg = omap_ctrl_readl(control_pbias_offset);
			reg |= (OMAP2_PBIASLITEPWRDNZ0 |
					OMAP2_PBIASSPEEDCTRL0);
			if ((1 << vdd) <= MMC_VDD_165_195)
				reg &= ~OMAP2_PBIASLITEVMODE0;
			else
				reg |= OMAP2_PBIASLITEVMODE0;
			omap_ctrl_writel(reg, control_pbias_offset);
		} else {
			reg = omap_ctrl_readl(control_pbias_offset);
			reg |= OMAP4_MMC1_PBIASLITE_PWRDNZ;
			if ((1 << vdd) <= MMC_VDD_165_195)
				reg &= ~(OMAP4_MMC1_PBIASLITE_VMODE);
			else
				reg |= (OMAP4_MMC1_PBIASLITE_VMODE);

			reg |= (OMAP4_MMC1_PBIASLITE_PWRDNZ |
						OMAP4_MMC1_PWRDWNZ);
			omap_ctrl_writel(reg, control_pbias_offset);
		}
	} else {
		if (!cpu_is_omap44xx()) {
			reg = omap_ctrl_readl(control_pbias_offset);
			reg &= ~OMAP2_PBIASLITEPWRDNZ0;
			omap_ctrl_writel(reg, control_pbias_offset);
		} else {
			reg = omap_ctrl_readl(control_pbias_offset);
			reg &= ~(OMAP4_MMC1_PBIASLITE_PWRDNZ |
						OMAP4_MMC1_PWRDWNZ);
			omap_ctrl_writel(reg, control_pbias_offset);
		}

		if (!cpu_is_omap44xx())
			ret = mmc_regulator_set_ocr(c->vcc, 0);

		/* 100ms delay required for PBIAS configuration */
		msleep(100);
		if (!cpu_is_omap44xx()) {
			reg = omap_ctrl_readl(control_pbias_offset);
			reg |= (OMAP2_PBIASSPEEDCTRL0 | OMAP2_PBIASLITEPWRDNZ0 |
				OMAP2_PBIASLITEVMODE0);
			omap_ctrl_writel(reg, control_pbias_offset);
		} else {
			reg = omap_ctrl_readl(control_pbias_offset);
			reg |= (OMAP4_MMC1_PBIASLITE_PWRDNZ |
				OMAP4_MMC1_PBIASLITE_VMODE |
				OMAP4_MMC1_PWRDWNZ);
			omap_ctrl_writel(reg, control_pbias_offset);
		}
	}

	return ret;
}

static int twl_mmc23_set_power(struct device *dev, int slot, int power_on, int vdd)
{
	int ret = 0;
	struct twl_mmc_controller *c = NULL;
	struct omap_mmc_platform_data *mmc = dev->platform_data;
	int i;

	for (i = 1; i < ARRAY_SIZE(hsmmc); i++) {
		if (mmc == hsmmc[i].mmc) {
			c = &hsmmc[i];
			break;
		}
	}

	if (c == NULL)
		return -ENODEV;

	/* If we don't see a Vcc regulator, assume it's a fixed
	 * voltage always-on regulator.
	 */
	if (!c->vcc)
		return 0;

	/*
	 * Assume Vcc regulator is used only to power the card ... OMAP
	 * VDDS is used to power the pins, optionally with a transceiver to
	 * support cards using voltages other than VDDS (1.8V nominal).  When a
	 * transceiver is used, DAT3..7 are muxed as transceiver control pins.
	 *
	 * In some cases this regulator won't support enable/disable;
	 * e.g. it's a fixed rail for a WLAN chip.
	 *
	 * In other cases vcc_aux switches interface power.  Example, for
	 * eMMC cards it represents VccQ.  Sometimes transceivers or SDIO
	 * chips/cards need an interface voltage rail too.
	 */
	if (power_on) {
#if 0
		/* only MMC2 supports a CLKIN */
		if (mmc->slots[0].internal_clock) {
			u32 reg;
			if (!cpu_is_omap44xx()) {
				reg = omap_ctrl_readl(control_devconf1_offset);
				reg |= OMAP2_MMCSDIO2ADPCLKISEL;
				omap_ctrl_writel(reg, control_devconf1_offset);
			}
		}
		ret = mmc_regulator_set_ocr(c->vcc, vdd);
		/* enable interface voltage rail, if needed */
		if (ret == 0 && c->vcc_aux) {
			ret = regulator_enable(c->vcc_aux);
			if (ret < 0)
				ret = mmc_regulator_set_ocr(c->vcc, 0);
		}
	} else {
		if (c->vcc_aux && (ret = regulator_is_enabled(c->vcc_aux)) > 0)
			ret = regulator_disable(c->vcc_aux);
		if (ret == 0)
			ret = mmc_regulator_set_ocr(c->vcc, 0);
#endif
	}

	return ret;
}

static struct omap_mmc_platform_data *hsmmc_data[OMAP44XX_NR_MMC] __initdata;

#ifdef CONFIG_MMC_EMBEDDED_SDIO
int omap_wifi_status_register(void (*callback)(int card_present,
	void *dev_id), void *dev_id);
int omap_wifi_status(int irq);

static struct sdio_embedded_func wifi_func_array[] = {
	{
		.f_class        = SDIO_CLASS_BT_A,
		.f_maxblksize   = 512,
	},
	{
		.f_class        = SDIO_CLASS_WLAN,
		.f_maxblksize   = 512,
	},
};

static struct embedded_sdio_data omap_wifi_emb_data = {
	.cis    = {
		.vendor         = SDIO_VENDOR_ID_TI,
		.device         = SDIO_DEVICE_ID_TI_WL12xx,
		.blksize        = 512,
		.max_dtr        = 24000000,
	},
	.cccr   = {
		.multi_block    = 1,
		.low_speed      = 0,
		.wide_bus       = 1,
		.high_power     = 0,
		.high_speed     = 0,
		.disable_cd = 1,
	},
	.funcs  = wifi_func_array,
	.num_funcs = 2,
	.quirks = MMC_QUIRK_VDD_165_195 | MMC_QUIRK_LENIENT_FUNC0,
};
#endif

void __init twl4030_mmc_init(struct twl4030_hsmmc_info *controllers)
{
	struct twl4030_hsmmc_info *c;
	int nr_hsmmc = ARRAY_SIZE(hsmmc_data);
	u32 reg;

	if (cpu_is_omap2430()) {
		control_pbias_offset = OMAP243X_CONTROL_PBIAS_LITE;
		control_devconf1_offset = OMAP243X_CONTROL_DEVCONF1;
		nr_hsmmc = 2;
	}

	if (cpu_is_omap44xx()) {
		control_pbias_offset = OMAP44XX_CONTROL_PBIAS_LITE;
		control_mmc1 = OMAP44XX_CONTROL_MMC1;
		reg = omap_ctrl_readl(control_mmc1);
		reg |= (OMAP4_CONTROL_SDMMC1_PUSTRENGTHGRP0 |
			OMAP4_CONTROL_SDMMC1_PUSTRENGTHGRP1);
		reg &= ~(OMAP4_CONTROL_SDMMC1_PUSTRENGTHGRP2 |
			OMAP4_CONTROL_SDMMC1_PUSTRENGTHGRP3);
		reg |= (OMAP4_CONTROL_SDMMC1_DR0_SPEEDCTRL |
			OMAP4_CONTROL_SDMMC1_DR0_SPEEDCTRL |
			OMAP4_CONTROL_SDMMC1_DR0_SPEEDCTRL);
		omap_ctrl_writel(reg, control_mmc1);
	} else {
		control_pbias_offset = OMAP343X_CONTROL_PBIAS_LITE;
		control_devconf1_offset = OMAP343X_CONTROL_DEVCONF1;
	}

	for (c = controllers; c->mmc; c++) {
		struct twl_mmc_controller *twl = hsmmc + c->mmc - 1;
		struct omap_mmc_platform_data *mmc = hsmmc_data[c->mmc - 1];

		if (!c->mmc || c->mmc > nr_hsmmc) {
			pr_debug("MMC%d: no such controller\n", c->mmc);
			continue;
		}
		if (mmc) {
			pr_debug("MMC%d: already configured\n", c->mmc);
			continue;
		}

		mmc = kzalloc(sizeof(struct omap_mmc_platform_data), GFP_KERNEL);
		if (!mmc) {
			pr_err("Cannot allocate memory for mmc device!\n");
			return;
		}

		if (c->name)
			strncpy(twl->name, c->name, HSMMC_NAME_LEN);
		else
			snprintf(twl->name, ARRAY_SIZE(twl->name),
				"mmc%islot%i", c->mmc, 1);

#ifdef CONFIG_MMC_EMBEDDED_SDIO
		if (c->mmc == CONFIG_TIWLAN_MMC_CONTROLLER) {
			mmc->slots[0].embedded_sdio = &omap_wifi_emb_data;
			mmc->slots[0].register_status_notify =
				&omap_wifi_status_register;
			mmc->slots[0].card_detect = &omap_wifi_status;
		}
#endif

		mmc->slots[0].name = twl->name;
		mmc->nr_slots = 1;
		mmc->slots[0].wires = c->wires;
		mmc->slots[0].internal_clock = !c->ext_clock;
		mmc->dma_mask = 0xffffffff;
		mmc->init = twl_mmc_late_init;

		if (!cpu_is_omap44xx()) {
			/* note: twl4030 card detect GPIOs can disable
			 * VMMCx ...
			 */
			if (gpio_is_valid(c->gpio_cd)) {
				mmc->cleanup = twl_mmc_cleanup;
				mmc->suspend = twl_mmc_suspend;
				mmc->resume = twl_mmc_resume;

				mmc->slots[0].switch_pin = c->gpio_cd;
				mmc->slots[0].card_detect_irq =
							gpio_to_irq(c->gpio_cd);
				if (c->cover_only)
					mmc->slots[0].get_cover_state =
							twl_mmc_get_cover_state;
				else
					mmc->slots[0].card_detect =
							twl_mmc_card_detect;
			} else
				mmc->slots[0].switch_pin = -EINVAL;

			/* write protect normally uses an OMAP gpio */
			if (gpio_is_valid(c->gpio_wp)) {
				gpio_request(c->gpio_wp, "mmc_wp");
				gpio_direction_input(c->gpio_wp);

				mmc->slots[0].gpio_wp = c->gpio_wp;
				mmc->slots[0].get_ro = twl_mmc_get_ro;
			} else
				mmc->slots[0].gpio_wp = -EINVAL;
		} else {
			/* Dummy Initialization for OMAP4 */
			mmc->init = twl_mmc_late_init;
			mmc->cleanup = twl_mmc_cleanup;
			mmc->suspend = twl_mmc_suspend;
			mmc->resume = twl_mmc_resume;
			mmc->slots[0].switch_pin = 0;
			mmc->slots[0].get_cover_state = NULL;
			mmc->slots[0].switch_pin = c->gpio_cd;
			/* HardCoding Phoenix number for only MMC1 */
			if (c->mmc == 1)
				mmc->slots[0].card_detect_irq = 373;
			else
				mmc->slots[0].card_detect_irq = 0;
			mmc->slots[0].card_detect = twl_mmc_card_detect;
			mmc->slots[0].gpio_wp = 0;
			mmc->slots[0].get_ro = NULL;
		}
		/* NOTE:  MMC slots should have a Vcc regulator set up.
		 * This may be from a TWL4030-family chip, another
		 * controllable regulator, or a fixed supply.
		 *
		 * temporary HACK: ocr_mask instead of fixed supply
		 */
		mmc->slots[0].ocr_mask = c->ocr_mask;

		switch (c->mmc) {
		case 1:
			/* on-chip level shifting via PBIAS0/PBIAS1 */
			mmc->slots[0].set_power = twl_mmc1_set_power;
			break;
		case 2:
			if (c->ext_clock)
				c->transceiver = 1;
			if (c->transceiver && c->wires > 4)
				c->wires = 4;
			/* FALLTHROUGH */
		case 3:
			/* off-chip level shifting, or none */
			mmc->slots[0].set_power = twl_mmc23_set_power;
			break;
		case 4:
			/* TODO */
			mmc->slots[0].set_power = twl_mmc23_set_power;
			break;
		case 5:
			/* TODO */
			mmc->slots[0].set_power = twl_mmc23_set_power;
#ifdef CONFIG_MMC_EMBEDDED_SDIO
			mmc->slots[0].ocr_mask  = MMC_VDD_165_195;
#endif
			break;
		default:
			pr_err("MMC%d configuration not supported!\n", c->mmc);
			kfree(mmc);
			continue;
		}
		hsmmc_data[c->mmc - 1] = mmc;
	}

	omap2_init_mmc(hsmmc_data, OMAP44XX_NR_MMC);

	/* pass the device nodes back to board setup code */
	for (c = controllers; c->mmc; c++) {
		struct omap_mmc_platform_data *mmc = hsmmc_data[c->mmc - 1];

		if (!c->mmc || c->mmc > nr_hsmmc)
			continue;
		c->dev = mmc->dev;
	}
}

#endif
