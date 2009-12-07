/*
 * linux/drivers/video/omap2/dss/dss.c
 *
 * Copyright (C) 2009 Nokia Corporation
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 *
 * Some code and ideas taken from drivers/video/omap/ driver
 * by Imre Deak.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define DSS_SUBSYS_NAME "DSS"

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/seq_file.h>
void __iomem  *gpio_base_dsi2;

#include <linux/i2c/twl.h>

#include <mach/display.h>
#include "dss.h"

#ifndef CONFIG_ARCH_OMAP4
/* DSS */
#define DSS_BASE                        0x48050000
/* DISPLAY CONTROLLER */
#define DISPC_BASE                      0x48050400
#else
/* DSS */
#define DSS_BASE                        0x58000000
/* DISPLAY CONTROLLER */
#define DISPC_BASE                      0x58001000
#endif

#define DSS_SZ_REGS			SZ_512

struct dss_reg {
	u16 idx;
};

#define DSS_REG(idx)			((const struct dss_reg) { idx })

#define DSS_REVISION			DSS_REG(0x0000)
#define DSS_SYSCONFIG			DSS_REG(0x0010)
#define DSS_SYSSTATUS			DSS_REG(0x0014)
#define DSS_CONTROL			DSS_REG(0x0040)
#define DSS_SDI_CONTROL			DSS_REG(0x0044)
#define DSS_PLL_CONTROL			DSS_REG(0x0048)
#define DSS_SDI_STATUS			DSS_REG(0x005C)

#ifdef CONFIG_ARCH_OMAP4
#define DSS_STATUS				DSS_REG(0x005C)
#endif
void test(void);


#define REG_GET(idx, start, end) \
	FLD_GET(dss_read_reg(idx), start, end)

#define REG_FLD_MOD(idx, val, start, end) \
	dss_write_reg(idx, FLD_MOD(dss_read_reg(idx), val, start, end))

static struct {
	void __iomem    *base;

	u32		ctx[DSS_SZ_REGS / sizeof(u32)];
} dss;

void __iomem  *dss_base;
void __iomem  *dispc_base;
#define GPIO_OE		0x134
#define GPIO_DATAOUT	0x13C
#define OMAP24XX_GPIO_CLEARDATAOUT	0x190
#define OMAP24XX_GPIO_SETDATAOUT	0x194

#define PWM2ON		3
#define PWM2OFF		4
#define TOGGLE3		2

static int _omap_dss_wait_reset(void);

static inline void dss_write_reg(const struct dss_reg idx, u32 val)
{
	__raw_writel(val, dss.base + idx.idx);
	__raw_readl(dss.base + 0x00); //sv5
}

static inline u32 dss_read_reg(const struct dss_reg idx)
{
	return __raw_readl(dss.base + idx.idx);
}

#define SR(reg) \
	dss.ctx[(DSS_##reg).idx / sizeof(u32)] = dss_read_reg(DSS_##reg)
#define RR(reg) \
	dss_write_reg(DSS_##reg, dss.ctx[(DSS_##reg).idx / sizeof(u32)])

void dss_save_context(void)
{
	if (cpu_is_omap24xx())
		return;

	SR(SYSCONFIG);
	SR(CONTROL);

#ifdef CONFIG_OMAP2_DSS_SDI
	SR(SDI_CONTROL);
	SR(PLL_CONTROL);
#endif
}

void dss_restore_context(void)
{
	if (_omap_dss_wait_reset())
		DSSERR("DSS not coming out of reset after sleep\n");

	RR(SYSCONFIG);
	RR(CONTROL);

#ifdef CONFIG_OMAP2_DSS_SDI
	RR(SDI_CONTROL);
	RR(PLL_CONTROL);
#endif
}

#undef SR
#undef RR

void dss_sdi_init(u8 datapairs)
{
	u32 l;

	BUG_ON(datapairs > 3 || datapairs < 1);

	l = dss_read_reg(DSS_SDI_CONTROL);
	l = FLD_MOD(l, 0xf, 19, 15);		/* SDI_PDIV */
	l = FLD_MOD(l, datapairs-1, 3, 2);	/* SDI_PRSEL */
	l = FLD_MOD(l, 2, 1, 0);		/* SDI_BWSEL */
	dss_write_reg(DSS_SDI_CONTROL, l);

	l = dss_read_reg(DSS_PLL_CONTROL);
	l = FLD_MOD(l, 0x7, 25, 22);	/* SDI_PLL_FREQSEL */
	l = FLD_MOD(l, 0xb, 16, 11);	/* SDI_PLL_REGN */
	l = FLD_MOD(l, 0xb4, 10, 1);	/* SDI_PLL_REGM */
	dss_write_reg(DSS_PLL_CONTROL, l);
}

void dss_sdi_enable(void)
{
	dispc_pck_free_enable(1);

	/* Reset SDI PLL */
	REG_FLD_MOD(DSS_PLL_CONTROL, 1, 18, 18); /* SDI_PLL_SYSRESET */
	udelay(1);	/* wait 2x PCLK */

	/* Lock SDI PLL */
	REG_FLD_MOD(DSS_PLL_CONTROL, 1, 28, 28); /* SDI_PLL_GOBIT */

	/* Waiting for PLL lock request to complete */
	while (dss_read_reg(DSS_SDI_STATUS) & (1 << 6))
		;

	/* Clearing PLL_GO bit */
	REG_FLD_MOD(DSS_PLL_CONTROL, 0, 28, 28);

	/* Waiting for PLL to lock */
	while (!(dss_read_reg(DSS_SDI_STATUS) & (1 << 5)))
		;

	dispc_lcd_enable_signal(1);

	/* Waiting for SDI reset to complete */
	while (!(dss_read_reg(DSS_SDI_STATUS) & (1 << 2)))
		;
}

void dss_sdi_disable(void)
{
	dispc_lcd_enable_signal(0);

	dispc_pck_free_enable(0);

	/* Reset SDI PLL */
	REG_FLD_MOD(DSS_PLL_CONTROL, 0, 18, 18); /* SDI_PLL_SYSRESET */
}

void dss_dump_regs(struct seq_file *s)
{
#define DUMPREG(r) seq_printf(s, "%-35s %08x\n", #r, dss_read_reg(r))

	dss_clk_enable(DSS_CLK_ICK | DSS_CLK_FCK1);

	DUMPREG(DSS_REVISION);
	DUMPREG(DSS_SYSCONFIG);
	DUMPREG(DSS_SYSSTATUS);
	DUMPREG(DSS_CONTROL);
	DUMPREG(DSS_SDI_CONTROL);
	DUMPREG(DSS_PLL_CONTROL);
	DUMPREG(DSS_SDI_STATUS);

	dss_clk_disable(DSS_CLK_ICK | DSS_CLK_FCK1);
#undef DUMPREG
}

void dss_select_clk_source(bool dsi, bool dispc)
{
	u32 r;
	r = dss_read_reg(DSS_CONTROL);
	r = FLD_MOD(r, dsi, 1, 1);	/* DSI_CLK_SWITCH */
	r = FLD_MOD(r, dispc, 0, 0);	/* DISPC_CLK_SWITCH */
	/* TODO: extend for LCD2 and HDMI */
	dss_write_reg(DSS_CONTROL, r);
}

int dss_get_dsi_clk_source(void)
{
	return FLD_GET(dss_read_reg(DSS_CONTROL), 1, 1);
}

int dss_get_dispc_clk_source(void)
{
	return FLD_GET(dss_read_reg(DSS_CONTROL), 0, 0);
}

#ifndef CONFIG_ARCH_OMAP4
static irqreturn_t dss_irq_handler_omap2(int irq, void *arg)
{
	dispc_irq_handler();

	return IRQ_HANDLED;
}
#endif

static irqreturn_t dss_irq_handler_omap3(int irq, void *arg)
{
	/* INT_24XX_DSS_IRQ is dedicated for DISPC interrupt request only */
	/* DSI1, DSI2 and HDMI to be handled in seperate handlers */
	dispc_irq_handler();

	return IRQ_HANDLED;
}

static int _omap_dss_wait_reset(void)
{
	unsigned timeout = 1000;

	while (REG_GET(DSS_SYSSTATUS, 0, 0) == 0) {
		udelay(1);
		if (!--timeout) {
			DSSERR("soft reset failed\n");
			return -ENODEV;
		}
	}

	return 0;
}

static int _omap_dss_reset(void)
{
#if 0 //sv3
	/* Soft reset */
	REG_FLD_MOD(DSS_SYSCONFIG, 1, 1, 1);
	return _omap_dss_wait_reset();
#endif
	return 0;
}

void dss_set_venc_output(enum omap_dss_venc_type type)
{
	int l = 0;

	if (type == OMAP_DSS_VENC_TYPE_COMPOSITE)
		l = 0;
	else if (type == OMAP_DSS_VENC_TYPE_SVIDEO)
		l = 1;
	else
		BUG();

	/* venc out selection. 0 = comp, 1 = svideo */
	REG_FLD_MOD(DSS_CONTROL, l, 6, 6);
}

void dss_set_dac_pwrdn_bgz(bool enable)
{
	REG_FLD_MOD(DSS_CONTROL, enable, 5, 5);	/* DAC Power-Down Control */
}

void dss_switch_tv_hdmi(int hdmi)
{
	REG_FLD_MOD(DSS_CONTROL, hdmi, 15, 15);	/* 0x1 for HDMI, 0x0 TV */

	if (hdmi)
		REG_FLD_MOD(DSS_CONTROL, 0, 9, 8);
}

int dss_init(bool skip_init)
{
	int r;
	u32 rev,ret;
	u32 val;
	void __iomem  *gpio1_base, *gpio2_base;
	void __iomem *mux_sec;
	
	dss_base = dss.base = ioremap(DSS_BASE, DSS_SZ_REGS);
	if (!dss.base) {
		DSSERR("can't ioremap DSS\n");
		r = -ENOMEM;
		goto fail0;
	}

	if (!skip_init) {
		/* disable LCD and DIGIT output. This seems to fix the synclost
		 * problem that we get, if the bootloader starts the DSS and
		 * the kernel resets it */
		//omap_writel(omap_readl(0x48050440) & ~0x3, 0x48050440);
		omap_writel(omap_readl(0x48041040) & ~0x3, 0x48041040);


		/* We need to wait here a bit, otherwise we sometimes start to
		 * get synclost errors, and after that only power cycle will
		 * restore DSS functionality. I have no idea why this happens.
		 * And we have to wait _before_ resetting the DSS, but after
		 * enabling clocks.
		 */
		msleep(50);

		_omap_dss_reset();
	}

	/* autoidle */
	REG_FLD_MOD(DSS_SYSCONFIG, 1, 0, 0);

	/* Select DPLL */
	REG_FLD_MOD(DSS_CONTROL, 0, 0, 0);


#ifndef CONFIG_ARCH_OMAP4
	r = request_irq(INT_24XX_DSS_IRQ,
			cpu_is_omap24xx()
			? dss_irq_handler_omap2
			: dss_irq_handler_omap3,
			0, "OMAP DSS", NULL);
#else
	r = request_irq(INT_44XX_DSS_IRQ,
			dss_irq_handler_omap3,
			0, "OMAP DSS", (void *)1);
#endif
	if (r < 0) {
		DSSERR("omap2 dss: request_irq failed\n");
		goto fail1;
	}

	dss_save_context();

	rev = dss_read_reg(DSS_REVISION);
	printk(KERN_INFO "OMAP DSS rev %d.%d\n",
			FLD_GET(rev, 7, 4), FLD_GET(rev, 3, 0));

#if 1
	gpio_base_dsi2=ioremap(0x48059000,0x1000);

	mux_sec = ioremap(0x4A100000,0x1000);
	val = __raw_readl(mux_sec + 0x1CC); //mux for gpio 27 or 52 dont know
	val &= ~(0xFfff);
	val |=  0x03;
	__raw_writel(val,mux_sec + 0x1CC);

	val = __raw_readl(mux_sec + 0x086); //mux for gpio 59
	val &= ~(0xFfff);
	val |=  0x03;
	__raw_writel(val,mux_sec + 0x086);

	val = __raw_readl(mux_sec + 0x0EA); //mux for GPio 104
	val &= ~(0xFfff);
	val |=  0x03;
	__raw_writel(val,mux_sec + 0x0EA);


	val = __raw_readl(gpio_base_dsi2+GPIO_OE);
	val &= ~0x100;
	__raw_writel(val, gpio_base_dsi2+GPIO_OE);

	mdelay(120);

	/* To output signal high */
	val = __raw_readl(gpio_base_dsi2+OMAP24XX_GPIO_SETDATAOUT);
	val |= 0x100;
	__raw_writel(val, gpio_base_dsi2+OMAP24XX_GPIO_SETDATAOUT);
	mdelay(100);

	val = __raw_readl(gpio_base_dsi2+OMAP24XX_GPIO_CLEARDATAOUT);
	val |= 0x100;
	__raw_writel(val, gpio_base_dsi2+OMAP24XX_GPIO_CLEARDATAOUT);
	mdelay(120);

	val = __raw_readl(gpio_base_dsi2+OMAP24XX_GPIO_SETDATAOUT);
	val |= 0x100;
	__raw_writel(val, gpio_base_dsi2+OMAP24XX_GPIO_SETDATAOUT);

	mdelay(120);
	printk("GPIO 104 reset done ");
#endif
#if 1

	ret = twl_i2c_write_u8(TWL6030_MODULE_PWM, 0xFF, PWM2ON); //0xBD = 0xFF
	ret = twl_i2c_write_u8(TWL6030_MODULE_PWM, 0x7F, PWM2OFF); //0xBE = 0x7F
	ret = twl_i2c_write_u8(TWL6030_MODULE_AUX, 0x30, TOGGLE3);
	gpio2_base=ioremap(0x4a310000,0x1000);
	gpio1_base=ioremap(0x48055000,0x1000);

	/* To output signal low */	
	rev = __raw_readl(gpio2_base+OMAP24XX_GPIO_CLEARDATAOUT);
	rev |= (1<<27);
	__raw_writel(rev, gpio2_base+OMAP24XX_GPIO_CLEARDATAOUT);	
	mdelay(120);	


	rev = __raw_readl(gpio2_base+GPIO_OE);	
	rev &= ~(1<<27);	
	__raw_writel(rev, gpio2_base+GPIO_OE);	
	#if 0
	/* To output signal low */	
	rev = __raw_readl(gpio2_base+OMAP24XX_GPIO_CLEARDATAOUT);
	rev |= (1<<27);
	__raw_writel(rev, gpio2_base+OMAP24XX_GPIO_CLEARDATAOUT);
	mdelay(120);	
	/* To output signal high */	
	rev = __raw_readl(gpio2_base+OMAP24XX_GPIO_SETDATAOUT);	
	rev |= (1<<27);	
	__raw_writel(rev, gpio2_base+OMAP24XX_GPIO_SETDATAOUT);	
	mdelay(120);	
	#endif
	/* To output signal low */	
	rev = __raw_readl(gpio2_base+OMAP24XX_GPIO_CLEARDATAOUT);
	rev |= (1<<27);
	__raw_writel(rev, gpio2_base+OMAP24XX_GPIO_CLEARDATAOUT);	
	mdelay(120);	

	
	/* To output signal high */
	rev = __raw_readl(gpio1_base+OMAP24XX_GPIO_SETDATAOUT);
	rev |= (1<<27);	
	__raw_writel(rev, gpio1_base+OMAP24XX_GPIO_SETDATAOUT);
	mdelay(120);	

	rev = __raw_readl(gpio1_base+GPIO_OE);	
	rev &= ~(1<<27);
	__raw_writel(rev, gpio1_base+GPIO_OE);	
	mdelay(120);	
	#if 0
	rev = __raw_readl(gpio1_base+OMAP24XX_GPIO_CLEARDATAOUT);
	rev |= (1<<27);
	__raw_writel(rev, gpio1_base+OMAP24XX_GPIO_CLEARDATAOUT);
	mdelay(120);
	#endif
	/* To output signal high */	
	rev = __raw_readl(gpio1_base+OMAP24XX_GPIO_SETDATAOUT);
	rev |= (1<<27);	
	__raw_writel(rev, gpio1_base+OMAP24XX_GPIO_SETDATAOUT);
	mdelay(120);
#endif


	return 0;

fail1:
	iounmap(dss.base);
fail0:
	return r;
}

void dss_exit(void)
{
#ifndef CONFIG_ARCH_OMAP4
	free_irq(INT_24XX_DSS_IRQ, NULL);
#else
	free_irq(INT_44XX_DSS_IRQ, NULL);
#endif

	iounmap(dss.base);
}

