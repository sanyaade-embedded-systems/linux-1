/*
 * TI DaVinci DM850 chip specific setup
 *
 * Author: Sudhakar Rajashekhara
 *
 * 2009 (c) Texas Instruments. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/spi/davinci_spi_master.h>

#include <mach/da8xx.h>
#include <mach/clock.h>
#include <mach/cpu.h>
#include <mach/edma.h>
#include <mach/irqs.h>
#include <mach/psc.h>
#include <mach/mux.h>

#include "clock.h"
#include "mux.h"

/*
 * Device specific clocks
 */
#define DA850_AUX_FREQ	24000000

static struct pll_data pll0_data = {
	.num		= 1,
	.phys_base	= DA8XX_PLL_CNTRL0_BASE,
	.flags		= PLL_HAS_PREDIV | PLL_HAS_POSTDIV,
};

static struct clk ref_clk = {
	.name		= "ref_clk",
	.rate		= DA850_AUX_FREQ,
};

static struct clk pll0_clk = {
	.name		= "pll0",
	.parent		= &ref_clk,
	.pll_data	= &pll0_data,
	.flags		= CLK_PLL,
};

static struct clk pll0_aux_clk = {
	.name		= "pll0_aux_clk",
	.parent		= &pll0_clk,
	.flags		= CLK_PLL | PRE_PLL,
};

static struct clk pll0_sysclk2 = {
	.name		= "pll0_sysclk2",
	.parent		= &pll0_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV2,
};

static struct clk pll0_sysclk3 = {
	.name		= "pll0_sysclk3",
	.parent		= &pll0_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV3,
};

static struct clk pll0_sysclk4 = {
	.name		= "pll0_sysclk4",
	.parent		= &pll0_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV4,
};

static struct clk pll0_sysclk5 = {
	.name		= "pll0_sysclk5",
	.parent		= &pll0_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV5,
};

static struct clk pll0_sysclk6 = {
	.name		= "pll0_sysclk6",
	.parent		= &pll0_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV6,
};

static struct clk pll0_sysclk7 = {
	.name		= "pll0_sysclk7",
	.parent		= &pll0_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV7,
};

static struct pll_data pll1_data = {
	.num		= 2,
	.phys_base	= DA8XX_PLL_CNTRL1_BASE,
	.flags		= PLL_HAS_PREDIV | PLL_HAS_POSTDIV,
};

static struct clk pll1_clk = {
	.name		= "pll1",
	.parent		= &ref_clk,
	.pll_data	= &pll1_data,
	.flags		= CLK_PLL,
};

static struct clk pll1_aux_clk = {
	.name		= "pll1_aux_clk",
	.parent		= &pll1_clk,
	.flags		= CLK_PLL | PRE_PLL,
};

static struct clk pll1_sysclk2 = {
	.name		= "pll1_sysclk2",
	.parent		= &pll1_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV2,
};

static struct clk pll1_sysclk3 = {
	.name		= "pll1_sysclk3",
	.parent		= &pll1_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV3,
};

static struct clk pll1_sysclk4 = {
	.name		= "pll1_sysclk4",
	.parent		= &pll1_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV4,
};

static struct clk pll1_sysclk5 = {
	.name		= "pll1_sysclk5",
	.parent		= &pll1_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV5,
};

static struct clk pll1_sysclk6 = {
	.name		= "pll1_sysclk6",
	.parent		= &pll1_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV6,
};

static struct clk pll1_sysclk7 = {
	.name		= "pll1_sysclk7",
	.parent		= &pll1_clk,
	.flags		= CLK_PLL,
	.div_reg	= PLLDIV7,
};

static struct clk i2c0_clk = {
	.name		= "i2c0",
	.parent		= &pll0_aux_clk,
};

static struct clk timerp64_0_clk = {
	.name		= "timer0",
	.parent		= &pll0_aux_clk,
};

static struct clk timerp64_1_clk = {
	.name		= "timer1",
	.parent		= &pll0_aux_clk,
};

static struct clk tpcc0_clk = {
	.name		= "tpcc0",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_TPCC,
	.flags		= CLK_PSC | ALWAYS_ENABLED,
};

static struct clk tptc0_clk = {
	.name		= "tptc0",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_TPTC0,
	.flags		= ALWAYS_ENABLED,
};

static struct clk tptc1_clk = {
	.name		= "tptc1",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_TPTC1,
	.flags		= ALWAYS_ENABLED,
};

static struct clk tpcc1_clk = {
	.name		= "tpcc1",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA850_LPSC_TPCC1,
	.flags		= CLK_PSC | ALWAYS_ENABLED,
};

static struct clk tptc2_clk = {
	.name		= "tptc2",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA850_LPSC_TPTC2,
	.flags		= ALWAYS_ENABLED,
};

static struct clk mmcsd_clk = {
	.name		= "mmcsd",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_MMC_SD,
};

static struct clk uart0_clk = {
	.name		= "uart0",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_UART0,
};

static struct clk uart1_clk = {
	.name		= "uart1",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_UART1,
	.dup_parent	= &pll1_sysclk2,
};

static struct clk uart2_clk = {
	.name		= "uart2",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_UART2,
	.dup_parent	= &pll1_sysclk2,
};

static struct clk spi0_clk = {
	.name		= "spi0",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_SPI0,
};

static struct clk spi1_clk = {
	.name		= "spi1",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_SPI1,
	.dup_parent	= &pll1_sysclk2,
};

static struct clk lcdc_clk = {
	.name		= "lcdc",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_LCDCNTL,
};

static struct clk aemif_clk = {
	.name		= "aemif",
	.parent		= &pll0_sysclk3,
	.lpsc		= DA8XX_LPSC_EMIF25,
	.flags		= ALWAYS_ENABLED,
};

static struct clk emac_clk = {
	.name		= "emac",
	.parent		= &pll0_sysclk4,
	.lpsc		= DA8XX_LPSC_CPGMAC,
};

static struct clk gpio_clk = {
	.name		= "gpio",
	.parent		= &pll0_sysclk4,
	.lpsc		= DA8XX_LPSC_GPIO,
};

static struct clk emif3_clk = {
	.name		= "emif3",
	.parent		= &pll0_sysclk5,
	.lpsc		= DA8XX_LPSC_EMIF3C,
	.flags		= ALWAYS_ENABLED,
};

static struct clk arm_clk = {
	.name		= "arm",
	.parent		= &pll0_sysclk6,
	.lpsc		= DA8XX_LPSC_ARM,
	.flags		= ALWAYS_ENABLED,
};

static struct clk mcasp_clk = {
	.name		= "mcasp",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_McASP0,
	.dup_parent	= &pll1_sysclk2,
};

static struct davinci_clk da850_clks[] = {
	CLK(NULL,		"ref",		&ref_clk),
	CLK(NULL,		"pll0",		&pll0_clk),
	CLK(NULL,		"pll0_aux",	&pll0_aux_clk),
	CLK(NULL,		"pll0_sysclk2",	&pll0_sysclk2),
	CLK(NULL,		"pll0_sysclk3",	&pll0_sysclk3),
	CLK(NULL,		"pll0_sysclk4",	&pll0_sysclk4),
	CLK(NULL,		"pll0_sysclk5",	&pll0_sysclk5),
	CLK(NULL,		"pll0_sysclk6",	&pll0_sysclk6),
	CLK(NULL,		"pll0_sysclk7",	&pll0_sysclk7),
	CLK(NULL,		"pll1",		&pll1_clk),
	CLK(NULL,		"pll1_aux",	&pll1_aux_clk),
	CLK(NULL,		"pll1_sysclk2",	&pll1_sysclk2),
	CLK(NULL,		"pll1_sysclk3",	&pll1_sysclk3),
	CLK(NULL,		"pll1_sysclk4",	&pll1_sysclk4),
	CLK(NULL,		"pll1_sysclk5",	&pll1_sysclk5),
	CLK(NULL,		"pll1_sysclk6",	&pll1_sysclk6),
	CLK(NULL,		"pll1_sysclk7",	&pll1_sysclk7),
	CLK("i2c_davinci.1",	NULL,		&i2c0_clk),
	CLK(NULL,		"timer0",	&timerp64_0_clk),
	CLK("watchdog",		NULL,		&timerp64_1_clk),
	CLK(NULL,		"tpcc0",	&tpcc0_clk),
	CLK(NULL,		"tptc0",	&tptc0_clk),
	CLK(NULL,		"tptc1",	&tptc1_clk),
	CLK(NULL,		"tpcc1",	&tpcc1_clk),
	CLK(NULL,		"tptc2",	&tptc2_clk),
	CLK("davinci_mmc.0",	NULL,		&mmcsd_clk),
	CLK(NULL,		"uart0",	&uart0_clk),
	CLK(NULL,		"uart1",	&uart1_clk),
	CLK(NULL,		"uart2",	&uart2_clk),
	CLK("dm_spi.0",		NULL,		&spi0_clk),
	CLK("dm_spi.1",		NULL,		&spi1_clk),
	CLK("da8xx_lcdc.0",	NULL,		&lcdc_clk),
	CLK(NULL,		"aemif",	&aemif_clk),
	CLK("davinci_emac.1",	NULL,		&emac_clk),
	CLK(NULL,		"gpio",		&gpio_clk),
	CLK(NULL,		"emif3",	&emif3_clk),
	CLK(NULL,		"arm",		&arm_clk),
	CLK("soc-audio",	NULL,		&mcasp_clk),
	CLK(NULL,		NULL,		NULL),
};

static unsigned long pinmux_in_use[20];

/*
 *		soc	description	mux  mode   mode  mux	 dbg
 *					reg  offset mask  mode
 */
static const struct mux_config davinci_da850_pins[] = {
	/* UART0 function */
	DA8XX_MUX_CFG(DA850,	NUART0_CTS,	3,   24,    15,	  2,	 false)
	DA8XX_MUX_CFG(DA850,	NUART0_RTS,	3,   28,    15,	  2,	 false)
	DA8XX_MUX_CFG(DA850,	UART0_RXD,	3,   16,    15,	  2,	 false)
	DA8XX_MUX_CFG(DA850,	UART0_TXD,	3,   20,    15,	  2,	 false)
	/* UART1 function */
	DA8XX_MUX_CFG(DA850,	NUART1_CTS,	0,   20,    15,	  4,	 false)
	DA8XX_MUX_CFG(DA850,	NUART1_RTS,	0,   16,    15,	  4,	 false)
	DA8XX_MUX_CFG(DA850,	UART1_RXD,	4,   24,    15,	  2,	 false)
	DA8XX_MUX_CFG(DA850,	UART1_TXD,	4,   28,    15,	  2,	 false)
	/* UART2 function */
	DA8XX_MUX_CFG(DA850,	NUART2_CTS,	0,   28,    15,	  4,	 false)
	DA8XX_MUX_CFG(DA850,	NUART2_RTS,	0,   24,    15,	  4,	 false)
	DA8XX_MUX_CFG(DA850,	UART2_RXD,	4,   16,    15,	  2,	 false)
	DA8XX_MUX_CFG(DA850,	UART2_TXD,	4,   20,    15,	  2,	 false)
	/* EMAC function */
	DA8XX_MUX_CFG(DA850,	RMII_TXD_0,	14,  12,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	RMII_TXD_1,	14,  8,     15,   8,     false)
	DA8XX_MUX_CFG(DA850,	RMII_TXEN,	14,  16,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	RMII_CRS_DV,	15,  4,     15,   8,     false)
	DA8XX_MUX_CFG(DA850,	RMII_RXD_0,	14,  24,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	RMII_RXD_1,	14,  20,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	RMII_RXER,	14,  28,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	RMII_MHZ_50_CLK,15,  0,     15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MDIO_CLK,	4,   0,     15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MDIO_D,		4,   4,     15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_TXEN,	2,   4,     15,	  8,     false)
	DA8XX_MUX_CFG(DA850,	MII_TXCLK,	2,   8,     15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_COL,	2,   12,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_TXD_3,	2,   16,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_TXD_2,	2,   20,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_TXD_1,	2,   24,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_TXD_0,	2,   28,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_RXCLK,	3,   0,     15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_RXDV,	3,   4,     15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_RXER,	3,   8,     15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_CRS,	3,   12,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_RXD_3,	3,   16,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_RXD_2,	3,   20,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_RXD_1,	3,   24,    15,   8,     false)
	DA8XX_MUX_CFG(DA850,	MII_RXD_0,	3,   28,    15,   8,     false)
	/* I2C1 function */
	DA8XX_MUX_CFG(DA850,	I2C1_SCL,	4,   16,    15,   4,     false)
	DA8XX_MUX_CFG(DA850,	I2C1_SDA,	4,   20,    15,   4,     false)
	/* I2C0 function */
	DA8XX_MUX_CFG(DA850,	I2C0_SDA,	4,   12,    15,   2,     false)
	DA8XX_MUX_CFG(DA850,	I2C0_SCL,	4,   8,     15,   2,     false)
	/* MMC/SD0 function */
	DA8XX_MUX_CFG(DA850,	MMCSD0_DAT_0,	10,  8,     15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD0_DAT_1,	10,  12,    15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD0_DAT_2,	10,  16,    15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD0_DAT_3,	10,  20,    15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD0_DAT_4,	10,  24,    15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD0_DAT_5,	10,  28,    15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD0_DAT_6,	11,  0,     15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD0_DAT_7,	11,  4,     15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD0_CLK,	10,  0,     15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD0_CMD,	10,  4,     15,   2,     false)
	/* MMC/SD1 function */
	DA8XX_MUX_CFG(DA850,	MMCSD1_DAT_0,	18,  8,     15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD1_DAT_1,	19,  16,    15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD1_DAT_2,	19,  12,    15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD1_DAT_3,	19,  8,     15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD1_DAT_4,	19,  4,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD1_DAT_5,	19,  0,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD1_DAT_6,	18,  28,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD1_DAT_7,	18,  24,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD1_CLK,	18,  12,    15,   2,     false)
	DA8XX_MUX_CFG(DA850,	MMCSD1_CMD,	18,  16,    15,   2,     false)
	/* EMIF2.5/EMIFA function */
	DA8XX_MUX_CFG(DA850,	EMA_D_7,	9,   0,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_6,	9,   4,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_5,	9,   8,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_4,	9,   12,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_3,	9,   16,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_2,	9,   20,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_1,	9,   24,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_0,	9,   28,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_8,	8,   28,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_9,	8,   24,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_10,	8,   20,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_11,	8,   16,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_12,	8,   12,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_13,	8,   8,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_14,	8,   4,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_D_15,	8,   0,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_0,	12,  28,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_1,	12,  24,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_2,	12,  20,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_3,	12,  16,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_4,	12,  12,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_5,	12,  8,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_6,	12,  4,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_7,	12,  0,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_8,	11,  28,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_9,	11,  24,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_10,	11,  20,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_11,	11,  16,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_12,	11,  12,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_13,	11,  8,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_14,	11,  4,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_15,	11,  0,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_16,	10,  28,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_17,	10,  24,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_18,	10,  20,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_19,	10,  16,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_20,	10,  12,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_21,	10,  8,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_22,	10,  4,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_A_23,	10,  0,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_BA_1,	5,   24,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_BA_0,	5,   28,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_CLK,	6,   0,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_SDCKE,	6,   4,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_RAS,	6,   8,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_CAS,	6,   12,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_WE_DQM_0,	6,   16,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_WE_DQM_1,	6,   20,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_WAIT_1,	6,   24,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_CS_0,	6,   28,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_CS_2,	7,   0,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_CS_3,	7,   4,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_CS_4,	7,   8,     15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_CS_5,	7,   12,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_WE,	7,   16,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	NEMA_OE,	7,   20,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_RNW,	7,   24,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	EMA_WAIT_0,	7,   28,    15,   1,     false)
	/* EMIF3a/EMIFB function - pins are not muxed */
	/* SPI0 function */
	DA8XX_MUX_CFG(DA850,	SPI0_CLK,	3,   0,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI0_ENA,	3,   4,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI0_SOMI,	3,   8,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI0_SIMO,	3,   12,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI0_CS_5,	3,   16,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI0_CS_4,	3,   20,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI0_CS_3,	3,   24,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI0_CS_2,	3,   28,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI0_CS_1,	4,   0,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI0_CS_0,	4,   4,    15,   1,     false)
	/* SPI1 function */
	DA8XX_MUX_CFG(DA850,	SPI1_CS_7,	4,   8,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_CS_6,	4,   12,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_CS_5,	4,   16,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_CS_4,	4,   20,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_CS_3,	4,   24,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_CS_2,	4,   28,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_CS_1,	5,   0,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_CS_0,	5,   4,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_CLK,	5,   8,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_ENA,	5,   12,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_SOMI,	5,   16,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	SPI1_SIMO,	5,   20,   15,   1,     false)
	/* McASP function */
	DA8XX_MUX_CFG(DA850,	MCASP_ACLKR,	0,   0,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_ACLKX,	0,   4,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AFSR,	0,   8,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AFSX,	0,   12,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AHCLKR,	0,   16,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AHCLKX,	0,   20,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AMUTE,	0,   24,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_15,	1,   0,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_14,	1,   4,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_13,	1,   8,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_12,	1,   12,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_11,	1,   16,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_10,	1,   20,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_9,	1,   24,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_8,	1,   28,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_7,	2,   0,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_6,	2,   4,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_5,	2,   8,    15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_4,	2,   12,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_3,	2,   16,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_2,	2,   20,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_1,	2,   24,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	MCASP_AXR_0,	2,   28,   15,   1,     false)
	/* LCD function */
	DA8XX_MUX_CFG(DA850,	LCD_D_7,	16,   8,   15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_6,	16,   12,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_5,	16,   16,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_4,	16,   20,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_3,	16,   24,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_2,	16,   28,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_1,	17,   0,   15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_0,	17,   4,   15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_15,	17,   8,   15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_14,	17,   12,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_13,	17,   16,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_12,	17,   20,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_11,	17,   24,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_10,	17,   28,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_9,	18,   0,   15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_D_8,	18,   4,   15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_PCLK,	18,   24,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_MCLK,	18,   28,  15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_HSYNC,	19,   0,   15,   2,     false)
	DA8XX_MUX_CFG(DA850,	LCD_VSYNC,	19,   4,   15,   2,     false)
	DA8XX_MUX_CFG(DA850,	NLCD_AC_ENB_CS,	19,   24,  15,   2,     false)
	/* VPIF function */
	DA8XX_MUX_CFG(DA850,	VPIF_CLKIN0,	14,   0,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_CLKIN1,	14,   4,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_7,	14,   8,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_6,	14,   12,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_5,	14,   16,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_4,	14,   20,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_3,	14,   24,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_2,	14,   28,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_1,	15,   0,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_0,	15,   4,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_15_V,	15,   8,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_14_H,	15,   12,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_13_FLD,15,   16,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_12,	15,   20,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_11,	15,   24,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_10,	15,   28,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_9,	16,   0,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DIN_8,	16,   4,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_7,	16,   8,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_6,	16,   12,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_5,	16,   16,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_4,	16,   20,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_3,	16,   24,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_2,	16,   28,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_1,	17,   0,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_0,	17,   4,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_15,	17,   8,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_14,	17,   12,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_13,	17,   16,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_12,	17,   20,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_11,	17,   24,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_10,	17,   28,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_9,	18,   0,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_DOUT_8,	18,   4,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_CLKIN_2,	19,   8,   15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_CLKO_2,	19,   12,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_CLKIN_3,	19,   16,  15,   1,     false)
	DA8XX_MUX_CFG(DA850,	VPIF_CLKO_3,	19,   20,  15,   1,     false)
	/* Timer0 function */
	DA8XX_MUX_CFG(DA850,	TM64P0_IN12,	4,    0,   15,   0,     false)
	DA8XX_MUX_CFG(DA850,	TM64P0_OUT12,	4,    0,   15,   2,     false)
	/* Timer1 function */
	DA8XX_MUX_CFG(DA850,	TM64P1_IN12,	4,    4,   15,   0,     false)
	DA8XX_MUX_CFG(DA850,	TM64P1_OUT12,	4,    4,   15,   2,     false)
	/* Timer2 function */
	DA8XX_MUX_CFG(DA850,	TM64P2_IN12,	5,    0,   15,   0,     false)
	DA8XX_MUX_CFG(DA850,	TM64P2_OUT12,	4,    8,   15,   4,     false)
	/* Timer3 function */
	DA8XX_MUX_CFG(DA850,	TM64P3_IN12,	5,    4,   15,   0,     false)
	DA8XX_MUX_CFG(DA850,	TM64P3_OUT12,	4,    12,  15,   4,     false)
	/* SATA function */
	DA8XX_MUX_CFG(DA850,	SATA_MP_SWITCH,	3,    24,  15,   0,     false)
	DA8XX_MUX_CFG(DA850,	SATA_CP_DET,	3,    28,  15,   0,     false)
	DA8XX_MUX_CFG(DA850,	SATA_LED,	4,    24,  15,   4,     false)
	DA8XX_MUX_CFG(DA850,	SATA_CP_POD,	4,    28,  15,   4,     false)
	/* GPIO function */
	DA8XX_MUX_CFG(DA850,	GPIO4_0,	10,   28,  15,   8,	false)
	DA8XX_MUX_CFG(DA850,	GPIO4_1,	10,   24,  15,   8,	false)
};

static const s8 dma0_chan_da850_no_event[] = {
	20, 21
	-1
};

static struct edma_soc_info da850_edma0_info = {
	.n_channel	= 32,
	.n_region	= 8,
	.n_slot		= 128,
	.n_tc		= 2,
	.n_cc		= 1,
	.noevent	= dma0_chan_da850_no_event,
};

#define SZ_32K  (32 * 1024)

static struct resource edma0_resources[] = {
	{
		.name	= "edma_cc0",
		.start	= 0x01c00000,
		.end	= 0x01c00000 + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_tc0",
		.start	= 0x01c08000,
		.end	= 0x01c08000 + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_tc1",
		.start	= 0x01c08400,
		.end	= 0x01c08400 + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_DA8XX_CCINT0,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA8XX_CCERRINT,
		.flags	= IORESOURCE_IRQ,
	},
	/* not using TC*_ERR */
};

static struct platform_device da850_edma0_device = {
	.name			= "edma",
	.id			= 0,
	.dev.platform_data	= &da850_edma0_info,
	.num_resources		= ARRAY_SIZE(edma0_resources),
	.resource		= edma0_resources,
};

static const s8 dma1_chan_da850_no_event[] = {
	19, 20, 21, 22,	23,
	30, 31,
	-1	
};

static struct edma_soc_info da850_edma1_info = {
	.n_channel	= 32,
	.n_region	= 8,
	.n_slot		= 128,
	.n_tc		= 1,
	.n_cc		= 1,
	.noevent	= dma1_chan_da850_no_event,
};

static struct resource edma1_resources[] = {
	{
		.name	= "edma_cc1",
		.start	= 0x01e30000,
		.end	= 0x01e30000 + SZ_32K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.name	= "edma_tc2",
		.start	= 0x01e38000,
		.end	= 0x01e38000 + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_DA850_CCINT1,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA850_CCERRINT1,
		.flags	= IORESOURCE_IRQ,
	},
	/* not using TC*_ERR */
};

static struct platform_device da850_edma1_device = {
	.name			= "edma",
	.id			= 1,
	.dev.platform_data	= &da850_edma1_info,
	.num_resources		= ARRAY_SIZE(edma1_resources),
	.resource		= edma1_resources,
};

static struct davinci_spi_platform_data da850_spi_pdata1 = {
	.version = DAVINCI_SPI_VERSION_2,
	.clk_name = "SPI1CLK",
	.cc_inst = 0
};

static struct resource da850_spi_resources1[] = {
	[0] = {
		.start = 0x01F0E000,
		.end = 0x01F0E000 + 0xfff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_DA8XX_SPINT1,
		.end = IRQ_DA8XX_SPINT1,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
		.start = 18,
		.end = 18,
		.flags = IORESOURCE_DMA | IORESOURCE_DMA_RX_CHAN,
	},
	[3] = {
		.start = 19,
		.end = 19,
		.flags = IORESOURCE_DMA | IORESOURCE_DMA_TX_CHAN,
	},
	[4] = {
		.start = 1,
		.end = 1,
		.flags = IORESOURCE_DMA | IORESOURCE_DMA_EVENT_Q,
	},
};

static struct platform_device da850_spi_pdev1 = {
	.name = "dm_spi",
	.id = 1,
	.resource = da850_spi_resources1,
	.num_resources = ARRAY_SIZE(da850_spi_resources1),
	.dev = {
		.platform_data = &da850_spi_pdata1,
	},
};

void __init da850_init_spi1(unsigned char* chip_sel, unsigned int num_sel,
	struct spi_board_info *info, unsigned num_dev)
{
	struct davinci_spi_platform_data *pdata =
			da850_spi_pdev1.dev.platform_data;

	/* TODO
	* 1.  remove ENA pin mux. It is likely not used anywhere.
	* 2.  do pinmux based on chipsel mask ala DM355
	*/

	davinci_cfg_reg(DA850_SPI1_SOMI);
	davinci_cfg_reg(DA850_SPI1_SIMO);
	davinci_cfg_reg(DA850_SPI1_CLK);

	if (num_sel && (!chip_sel || chip_sel[0] == DAVINCI_SPI_INTERN_CS))
		davinci_cfg_reg(DA850_SPI1_CS_0);

	spi_register_board_info(info, num_dev);

	pdata->chip_sel = chip_sel;
	pdata->num_chipselect = num_sel;
	platform_device_register(&da850_spi_pdev1);
}

void __init da850_init_mcasp(void)
{
	davinci_cfg_reg(DA850_MCASP_ACLKR);
	davinci_cfg_reg(DA850_MCASP_ACLKX);
	davinci_cfg_reg(DA850_MCASP_AFSR);
	davinci_cfg_reg(DA850_MCASP_AFSX);
	davinci_cfg_reg(DA850_MCASP_AHCLKR);
	davinci_cfg_reg(DA850_MCASP_AHCLKX);
	davinci_cfg_reg(DA850_MCASP_AMUTE);
	/* serializers interfaced to aic3x */
	davinci_cfg_reg(DA850_MCASP_AXR_11);
	davinci_cfg_reg(DA850_MCASP_AXR_12);
}

#if defined(CONFIG_TI_DAVINCI_EMAC) || defined(CONFIG_TI_DAVINCI_EMAC_MODULE)

static struct resource da850_emac_resources[] = {
	{
		.start	= DA8XX_EMAC_CPPI_PORT_BASE,
		.end	= DA8XX_EMAC_CPPI_PORT_BASE + 0x47ff,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= IRQ_DA8XX_C0_RX_THRESH_PULSE,
		.end	= IRQ_DA8XX_C0_RX_THRESH_PULSE,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA8XX_C0_RX_PULSE,
		.end	= IRQ_DA8XX_C0_RX_PULSE,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA8XX_C0_TX_PULSE,
		.end	= IRQ_DA8XX_C0_TX_PULSE,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= IRQ_DA8XX_C0_MISC_PULSE,
		.end	= IRQ_DA8XX_C0_MISC_PULSE,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device da850_emac_device = {
	.name		= "davinci_emac",
	.id		= 1,
	.num_resources	= ARRAY_SIZE(da850_emac_resources),
	.resource	= da850_emac_resources,
};

void da850_init_emac(struct emac_platform_data *pdata)
{
	pdata->ctrl_reg_offset		= DA8XX_EMAC_CTRL_REG_OFFSET;
	pdata->ctrl_mod_reg_offset	= DA8XX_EMAC_MOD_REG_OFFSET;
	pdata->ctrl_ram_offset		= DA8XX_EMAC_RAM_OFFSET;
	pdata->mdio_reg_offset		= DA8XX_MDIO_REG_OFFSET;
	pdata->ctrl_ram_size		= DA8XX_EMAC_CTRL_RAM_SIZE;
	pdata->version			= EMAC_VERSION_2;
	da850_emac_device.dev.platform_data	= pdata;
	platform_device_register(&da850_emac_device);
}
#else

void da850_init_emac(struct emac_platform_data *unused) {}

#endif

int get_async3_src(void)
{
	unsigned int *addr = IO_ADDRESS(DA8XX_CFGCHIP3);

        return ((__raw_readl(addr) & 0x10) ? 1 : 0);
}

void __init da850_init(void)
{
	davinci_clk_init(da850_clks);
	davinci_mux_register(davinci_da850_pins, ARRAY_SIZE(davinci_da850_pins),
				pinmux_in_use);
}

static int __init da850_init_devices(void)
{
	if (!cpu_is_da850())
		return 0;

	platform_device_register(&da850_edma0_device);
	platform_device_register(&da850_edma1_device);
	return 0;
}
postcore_initcall(da850_init_devices);
