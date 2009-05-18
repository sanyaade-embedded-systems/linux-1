/*
 * TI DaVinci DM644x chip specific setup
 *
 * Author: Kevin Hilman, Deep Root Systems, LLC
 *
 * 2007 (c) Deep Root Systems, LLC. This file is licensed under
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
#define DA830_AUX_FREQ	24000000

static struct pll_data pll0_data = {
	.num		= 1,
	.phys_base	= DA8XX_PLL_CNTRL0_BASE,
	.flags		= PLL_HAS_PREDIV | PLL_HAS_POSTDIV,
};

static struct clk ref_clk = {
	.name		= "ref_clk",
	.rate		= DA830_AUX_FREQ,
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

static struct clk tpcc_clk = {
	.name		= "tpcc",
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
};

static struct clk uart2_clk = {
	.name		= "uart2",
	.parent		= &pll0_sysclk2,
	.lpsc		= DA8XX_LPSC_UART2,
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

static struct clk mcasp1_clk = {
	.name		= "mcasp1",
	.lpsc		= DA830_LPSC_McASP1,
	.parent		= &pll0_sysclk2,
};

static struct davinci_clk da830_clks[] = {
	CLK(NULL,		"ref",		&ref_clk),
	CLK(NULL,		"pll0",		&pll0_clk),
	CLK(NULL,		"pll0_aux",	&pll0_aux_clk),
	CLK(NULL,		"pll0_sysclk2",	&pll0_sysclk2),
	CLK(NULL,		"pll0_sysclk3",	&pll0_sysclk3),
	CLK(NULL,		"pll0_sysclk4",	&pll0_sysclk4),
	CLK(NULL,		"pll0_sysclk5",	&pll0_sysclk5),
	CLK(NULL,		"pll0_sysclk6",	&pll0_sysclk6),
	CLK(NULL,		"pll0_sysclk7",	&pll0_sysclk7),
	CLK("i2c_davinci.1",	NULL,		&i2c0_clk),
	CLK(NULL,		"timer0",	&timerp64_0_clk),
	CLK("watchdog",		NULL,		&timerp64_1_clk),
	CLK(NULL,		"tpcc",		&tpcc_clk),
	CLK(NULL,		"tptc0",	&tptc0_clk),
	CLK(NULL,		"tptc1",	&tptc1_clk),
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
	CLK("soc-audio",	NULL,		&mcasp1_clk),
	CLK(NULL,		NULL,		NULL),
};

static unsigned long pinmux_in_use[20];

/*
 *		  soc		description	mux  mode   mode  mux	 dbg
 *						reg  offset mask  mode
 */
static const struct mux_config davinci_da830_pins[] = {
DA8XX_MUX_CFG(DA830,	NUART0_CTS,	7,   24,    15,	  2,	 false)
DA8XX_MUX_CFG(DA830,	NUART0_RTS,	7,   28,    15,	  2,	 false)
DA8XX_MUX_CFG(DA830,	UART0_RXD,	8,   12,    15,	  1,	 false)
DA8XX_MUX_CFG(DA830,	UART0_TXD,	8,   16,    15,	  1,	 false)
DA8XX_MUX_CFG(DA830,	UART1_RXD,	11,  8,     15,	  1,	 false)
DA8XX_MUX_CFG(DA830,	UART1_TXD,	11,  12,    15,	  1,	 false)
DA8XX_MUX_CFG(DA830,	UART2_RXD,	8,   28,    15,	  2,	 false)
DA8XX_MUX_CFG(DA830,	UART2_TXD,	9,   0,     15,	  2,	 false)
DA8XX_MUX_CFG(DA830,	I2C1_SCL,	8,   0,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	I2C1_SDA,	8,   4,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	I2C0_SDA,	8,   12,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	I2C0_SCL,	8,   16,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	RMII_TXD_0,	10,  4,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	RMII_TXD_1,	10,  8,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	RMII_TXEN,	10,  12,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	RMII_CRS_DV,	10,  16,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	RMII_RXD_0,	10,  20,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	RMII_RXD_1,	10,  24,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	RMII_RXER,	10,  28,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	MDIO_CLK,	11,  0,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	MDIO_D,		11,  4,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_DAT_0,	13,  24,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_DAT_1,	13,  28,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_DAT_2,	14,  0,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_DAT_3,	14,  4,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_DAT_4,	14,  8,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_DAT_5,	14,  12,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_DAT_6,	14,  16,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_DAT_7,	14,  20,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_CLK,	15,  28,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	MMCSD_CMD,	16,  0,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_0,	13,  24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_1,	13,  28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_2,	14,  0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_3,	14,  4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_4,	14,  8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_5,	14,  12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_6,	14,  16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_7,	14,  20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_8,	14,  24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_9,	14,  28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_10,	15,  0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_11,	15,  4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_12,	15,  8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_13,	15,  12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_14,	15,  16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_D_15,	15,  20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_0,	15,  24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_1,	15,  28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_2,	16,  0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_3,	16,  4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_4,	16,  8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_5,	16,  12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_6,	16,  16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_7,	16,  20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_8,	16,  24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_9,	16,  28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_10,	17,  0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_11,	17,  4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_A_12,	17,  8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_BA_1,	17,  12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_BA_0,	17,  16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_CLK,	17,  20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMA_SDCKE,	17,  24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_CAS,	17,  28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_CS_4,	17,  28,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	NEMA_RAS,	18,  0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_WE,	18,  4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_CS_0,	18,  8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_CS_2,	18,  12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_CS_3,	18,  16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_OE,	18,  20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_WE_DQM_1,	18,  24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_WE_DQM_0,	18,  28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMA_CS_5,	18,  0,     15,   2,     false)
DA8XX_MUX_CFG(DA830,	EMA_WAIT_0,	19,  0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_SDCKE,	0,   8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_CLK_GLUE,	0,   12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_CLK,	0,   12,    15,   2,     false)
DA8XX_MUX_CFG(DA830,	NEMB_CS_0,	0,   16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMB_CAS,	0,   20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMB_RAS,	0,   24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMB_WE,	0,   28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_BA_1,	1,   0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_BA_0,	1,   4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_0,	1,   8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_1,	1,   12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_2,	1,   16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_3,	1,   20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_4,	1,   24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_5,	1,   28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_6,	2,   0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_7,	2,   4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_8,	2,   8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_9,	2,   12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_10,	2,   16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_11,	2,   20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_A_12,	2,   24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_31,	2,   28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_30,	3,   0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_29,	3,   4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_28,	3,   8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_27,	3,   12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_26,	3,   16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_25,	3,   20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_24,	3,   24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_23,	3,   28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_22,	4,   0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_21,	4,   4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_20,	4,   8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_19,	4,   12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_18,	4,   16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_17,	4,   20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_16,	4,   24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMB_WE_DQM_3,	4,   28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMB_WE_DQM_2,	5,   0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_0,	5,   4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_1,	5,   8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_2,	5,   12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_3,	5,   16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_4,	5,   20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_5,	5,   24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_7,	6,   0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_8,	6,   4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_9,	6,   8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_10,	6,   12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_11,	6,   16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_12,	6,   20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_13,	6,   24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_14,	6,   28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	EMB_D_15,	7,   0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMB_WE_DQM_1,	7,   4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	NEMB_WE_DQM_0,	7,   8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	SPI0_SOMI_0,	7,   12,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	SPI0_SIMO_0,	7,   16,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	SPI0_CLK,	7,   20,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NSPI0_ENA,	7,   24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NSPI0_SCS_0,	7,   28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	SPI1_SOMI_0,	8,   0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	SPI1_SIMO_0,	8,   4,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	SPI1_CLK,	8,   8,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	NSPI1_ENA,	8,   28,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	NSPI1_SCS_0,	9,   0,     15,   1,     false)
DA8XX_MUX_CFG(DA830,	GPIO3_10,	11,  12,    15,   8,     false)
DA8XX_MUX_CFG(DA830,	LCD_D_8,	14,  24,    15,   4,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_9,	14,  28,    15,   4,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_7,	15,  24,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_10,	15,   0,    15,   4,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_11,	15,   4,    15,   4,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_12,	15,   8,    15,   4,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_13,	15,  12,    15,   4,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_14,	15,  16,    15,   4,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_15,	15,  20,    15,   4,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_6,	16,   4,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_3,	16,   8,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_2,	16,  12,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_1,	16,  16,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_0,	16,  20,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_PCLK,	16,  24,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_HSYNC,	16,  28,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_VSYNC,	17,   0,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	NLCD_AC_ENB_CS,	17,   4,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_MCLK,	17,   8,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_5,	17,  12,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	LCD_D_4,	17,  16,    15,   2,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_10,	8,   20,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_11,	8,   24,    15,   1,     false)
DA8XX_MUX_CFG(DA830,	AHCLKX1,	11,  20,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	ACLKX1,		11,  24,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AFSX1,		11,  28,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AHCLKR1,	12,   0,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	ACLKR1,		12,   4,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AFSR1,		12,   8,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AMUTE1,		12,  12,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_0,		12,  16,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_1,		12,  20,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_2,		12,  24,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_3,		12,  28,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_4,		13,   0,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_5,		13,   4,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_6,		13,   8,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_7,		13,  12,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_8,		13,  16,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	AXR1_9,		13,  20,    15,   1,	 false)
DA8XX_MUX_CFG(DA830,	USB0_DRVVBUS,	9,   24,    15,   1,     false)
};

static const s8 dma_chan_da830_no_event[] = {
	20, 21,
	-1
};

static struct edma_soc_info da830_edma_info = {
	.n_channel	= 32,
	.n_region	= 4,
	.n_slot		= 128,
	.n_tc		= 2,
	.n_cc		= 1,
	.noevent	= dma_chan_da830_no_event,
};

#define SZ_32K  (32 * 1024)

static struct resource edma_resources[] = {
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

static struct platform_device da830_edma_device = {
	.name			= "edma",
	.id			= 0,
	.dev.platform_data	= &da830_edma_info,
	.num_resources		= ARRAY_SIZE(edma_resources),
	.resource		= edma_resources,
};

static struct davinci_spi_platform_data da8xx_spi_pdata0 = {
	.version = DAVINCI_SPI_VERSION_2,
	.clk_name = "SPI0CLK",
	.cc_inst = 0
};

static struct resource da8xx_spi_resources0[] = {
	[0] = {
		.start = 0x01C41000,
		.end = 0x01C41000 + 0xfff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_DA8XX_SPINT0,
		.end = IRQ_DA8XX_SPINT0,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
		.start = EDMA_CTLR_CHAN(0, 14),
		.end = EDMA_CTLR_CHAN(0, 14),
		.flags = IORESOURCE_DMA | IORESOURCE_DMA_RX_CHAN,
	},
	[3] = {
		.start = EDMA_CTLR_CHAN(0, 15),
		.end = EDMA_CTLR_CHAN(0, 15),
		.flags = IORESOURCE_DMA | IORESOURCE_DMA_TX_CHAN,
	},
	[4] = {
		.start = 1,
		.end = 1,
		.flags = IORESOURCE_DMA | IORESOURCE_DMA_EVENT_Q,
	},
};

static struct platform_device da8xx_spi_pdev0 = {
	.name = "dm_spi",
	.id = 0,
	.resource = da8xx_spi_resources0,
	.num_resources = ARRAY_SIZE(da8xx_spi_resources0),
	.dev = {
		.platform_data = &da8xx_spi_pdata0,
	},
};

static struct davinci_spi_platform_data da8xx_spi_pdata1 = {
	.version = DAVINCI_SPI_VERSION_2,
	.clk_name = "SPI1CLK",
	.cc_inst = 0
};

static struct resource da8xx_spi_resources1[] = {
	[0] = {
		.start = 0x01E12000,
		.end = 0x01E12000 + 0xfff,
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

static struct platform_device da8xx_spi_pdev1 = {
	.name = "dm_spi",
	.id = 1,
	.resource = da8xx_spi_resources1,
	.num_resources = ARRAY_SIZE(da8xx_spi_resources1),
	.dev = {
		.platform_data = &da8xx_spi_pdata1,
	},
};

void __init da8xx_init_spi0(unsigned char* chip_sel, unsigned int num_sel,
	struct spi_board_info *info, unsigned num_dev)
{
	struct davinci_spi_platform_data *pdata =
			da8xx_spi_pdev0.dev.platform_data;
	/* TODO:
	* 1.  remove ENA pin mux. It is likely not used anywhere.
	* 2.  do pinmux based on chipsel mask ala DM355
	*/
	davinci_cfg_reg(DA830_SPI0_SOMI_0);
	davinci_cfg_reg(DA830_SPI0_SIMO_0);
	davinci_cfg_reg(DA830_SPI0_CLK);

	if (num_sel && (!chip_sel || chip_sel[0] == DAVINCI_SPI_INTERN_CS))
		davinci_cfg_reg(DA830_NSPI0_SCS_0);

	spi_register_board_info(info, num_dev);

	pdata->chip_sel = chip_sel;
	pdata->num_chipselect = num_sel;
	platform_device_register(&da8xx_spi_pdev0);
}

void __init da8xx_init_spi1(unsigned char* chip_sel, unsigned int num_sel,
	struct spi_board_info *info, unsigned num_dev)
{
	struct davinci_spi_platform_data *pdata =
			da8xx_spi_pdev1.dev.platform_data;
	/* TODO
	* 1.  remove ENA pin mux. It is likely not used anywhere.
	* 2.  do pinmux based on chipsel mask ala DM355
	*/

	davinci_cfg_reg(DA830_SPI1_SOMI_0);
	davinci_cfg_reg(DA830_SPI1_SIMO_0);
	davinci_cfg_reg(DA830_SPI1_CLK);

	if (num_sel && (!chip_sel || chip_sel[0] == DAVINCI_SPI_INTERN_CS))
		davinci_cfg_reg(DA830_NSPI1_SCS_0);

	spi_register_board_info(info, num_dev);

	pdata->chip_sel = chip_sel;
	pdata->num_chipselect = num_sel;
	platform_device_register(&da8xx_spi_pdev1);
}

void __init da830_init_mcasp1()
{
	davinci_cfg_reg(DA830_AHCLKX1);
	davinci_cfg_reg(DA830_ACLKX1);
	davinci_cfg_reg(DA830_AFSX1);
	davinci_cfg_reg(DA830_AHCLKR1);
	davinci_cfg_reg(DA830_AFSR1);
	davinci_cfg_reg(DA830_AMUTE1);
	davinci_cfg_reg(DA830_ACLKR1);

	/* serializers interfaced to aic3x */
	davinci_cfg_reg(DA830_AXR1_0);
	davinci_cfg_reg(DA830_AXR1_5);
}

#if defined(CONFIG_TI_DAVINCI_EMAC) || defined(CONFIG_TI_DAVINCI_EMAC_MODULE)

static struct resource da830_emac_resources[] = {
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

static struct platform_device da830_emac_device = {
	.name		= "davinci_emac",
	.id		= 1,
	.num_resources	= ARRAY_SIZE(da830_emac_resources),
	.resource	= da830_emac_resources,
};

void da830_init_emac(struct emac_platform_data *pdata)
{
	pdata->ctrl_reg_offset		= DA8XX_EMAC_CTRL_REG_OFFSET;
	pdata->ctrl_mod_reg_offset	= DA8XX_EMAC_MOD_REG_OFFSET;
	pdata->ctrl_ram_offset		= DA8XX_EMAC_RAM_OFFSET;
	pdata->mdio_reg_offset		= DA8XX_MDIO_REG_OFFSET;
	pdata->ctrl_ram_size		= DA8XX_EMAC_CTRL_RAM_SIZE;
	pdata->version			= EMAC_VERSION_2;
	pdata->rmii_en			= 1;
	da830_emac_device.dev.platform_data = pdata;
	platform_device_register(&da830_emac_device);
}
#else

void da830_init_emac(struct emac_platform_data *unused) {}

#endif

void __init da830_init(void)
{
	davinci_clk_init(da830_clks);
	davinci_mux_register(davinci_da830_pins, ARRAY_SIZE(davinci_da830_pins),
				pinmux_in_use);
}

static int __init da830_init_devices(void)
{
	if (!cpu_is_da830())
		return 0;

	platform_device_register(&da830_edma_device);
	return 0;
}
postcore_initcall(da830_init_devices);
