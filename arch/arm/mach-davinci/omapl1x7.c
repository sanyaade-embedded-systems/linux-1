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

#include <mach/omapl1x7.h>
#include <mach/clock.h>
#include <mach/psc.h>
#include <mach/mux.h>

#include "clock.h"
#include "mux.h"

/*
 * Device specific clocks
 */
#define OMAPL1X7_REF_FREQ	12000000
#define OMAPL1X7_AUX_FREQ	24000000

static struct pll_data pll1_data = {
	.num       = 1,
	.phys_base = OMAPL1X7_PLL_CNTRL0_BASE,
};

static struct clk ref_clk = {
	.name = "ref_clk",
	.rate = OMAPL1X7_REF_FREQ,
};

static struct clk aux_clkin = {
	.name = "aux_clkin",
	.rate = OMAPL1X7_AUX_FREQ,
};

static struct clk pll1_clk = {
	.name = "pll1",
	.parent = &ref_clk,
	.pll_data = &pll1_data,
	.flags = CLK_PLL,
};

static struct clk pll1_sysclk1 = {
	.name = "pll1_sysclk1",
	.parent = &pll1_clk,
	.flags = CLK_PLL,
	.div_reg = PLLDIV1,
};

static struct clk pll1_sysclk2 = {
	.name = "pll1_sysclk2",
	.parent = &pll1_clk,
	.flags = CLK_PLL,
	.div_reg = PLLDIV2,
};

static struct clk pll1_sysclk3 = {
	.name = "pll1_sysclk3",
	.parent = &pll1_clk,
	.flags = CLK_PLL,
	.div_reg = PLLDIV3,
};

static struct clk pll1_sysclk4 = {
	.name = "pll1_sysclk4",
	.parent = &pll1_clk,
	.flags = CLK_PLL,
	.div_reg = PLLDIV4,
};

static struct clk pll1_sysclk5 = {
	.name = "pll1_sysclk5",
	.parent = &pll1_clk,
	.flags = CLK_PLL,
	.div_reg = PLLDIV3,
};

static struct clk pll1_sysclk6 = {
	.name = "pll1_sysclk6",
	.parent = &pll1_clk,
	.flags = CLK_PLL,
	.div_reg = PLLDIV1,
};

static struct clk pll1_sysclk7 = {
	.name = "pll1_sysclk7",
	.parent = &pll1_clk,
	.flags = CLK_PLL,
	.div_reg = PLLDIV7,
};

static struct clk pll1_sysclkbp = {
	.name = "pll1_sysclkbp",
	.parent = &pll1_clk,
	.flags = CLK_PLL | PRE_PLL,
	.div_reg = BPDIV,
};

static struct clk pll1_aux_clk = {
	.name = "pll1_aux_clk",
	.parent = &pll1_clk,
	.flags = CLK_PLL | PRE_PLL,
};

static struct clk arm_clk = {
	.name = "arm",
	.parent = &pll1_sysclk6,
	.lpsc = OMAPL1X7_LPSC_ARM,
	.flags = ALWAYS_ENABLED,
};

static struct clk uart0_clk = {
	.name = "uart0",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_UART0,
};

static struct clk uart1_clk = {
	.name = "uart1",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_UART1,
};

static struct clk uart2_clk = {
	.name = "uart2",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_UART2,
};

static struct clk timer0_clk = {
	.name = "timer0",
	.parent = &aux_clkin,
};

static struct clk watchdog_clk = {
	.name = "watchdog",
	.parent = &aux_clkin,
};

static struct clk i2c_clk = {
	.name = "I2CCLK",
	.parent = &pll1_sysclk4,
	.lpsc = OMAPL1X7_LPSC_I2C,
};

static struct clk emac_clk = {
	.name = "emac",
	.parent = &pll1_sysclk4,
	.lpsc = OMAPL1X7_LPSC_CPGMAC,
};

static struct clk gpio_clk = {
	.name = "gpio",
	.parent = &pll1_sysclk4,
	.lpsc = OMAPL1X7_LPSC_GPIO,
};

static struct clk edma_cc0_clk = {
	.name = "edma_cc0",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_TPCC,
	.flags = CLK_PSC | ALWAYS_ENABLED,
};

static struct clk edma_tc0_clk = {
	.name = "edma_tc0",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_TPTC0,
	.flags = ALWAYS_ENABLED,
};

static struct clk edma_tc1_clk = {
	.name = "edma_tc1",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_TPTC1,
	.flags = ALWAYS_ENABLED,
};

static struct clk mmcsd_clk = {
	.name = "mmcsd",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_MMC_SD,
};

static struct clk emif3_clk = {
	.name = "emif3",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_EMIF3C,
};

static struct clk aemif_clk = {
	.name = "aemif",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_EMIF25,
};

static struct clk spi0_clk = {
	.name = "SPI0CLK",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_SPI0,
};

static struct clk spi1_clk = {
	.name = "SPI1CLK",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_SPI1,
};

static struct clk lcdc_clk = {
	.name = "LCDCTRLCLK",
	.parent = &pll1_sysclk2,
	.lpsc = OMAPL1X7_LPSC_LCDCNTL,
};

static struct clk *omapl1x7_clks[] __initdata = {
	&ref_clk,
	&aux_clkin,
	&pll1_clk,
	&pll1_sysclk1,
	&pll1_sysclk2,
	&pll1_sysclk3,
	&pll1_sysclk4,
	&pll1_sysclk5,
	&pll1_sysclk6,
	&pll1_sysclk7,
	&pll1_sysclkbp,
	&pll1_aux_clk,
	&arm_clk,
	&uart0_clk,
	&uart1_clk,
	&uart2_clk,
	&timer0_clk,
	&watchdog_clk,
	&i2c_clk,
	&emac_clk,
	&gpio_clk,
	&edma_cc0_clk,
	&edma_tc0_clk,
	&edma_tc1_clk,
	&mmcsd_clk,
	&emif3_clk,
	&aemif_clk,
	&spi0_clk,
	&spi1_clk,
	&lcdc_clk,
	NULL,
};

/*
 *		  soc		description	mux  mode   mode  mux	 dbg
 *						reg  offset mask  mode
 */
static const struct mux_config davinci_omapl1x7_pins[] = {
OMAPL1X7_MUX_CFG(OMAPL1X7,	NUART0_CTS,	7,   24,    15,	  2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NUART0_RTS,	7,   28,    15,	  2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	UART0_RXD,	8,   12,    15,	  1,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	UART0_TXD,	8,   16,    15,	  1,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	UART1_RXD,	11,  8,     15,	  1,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	UART1_TXD,	11,  12,    15,	  1,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	UART2_RXD,	8,   28,    15,	  2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	UART2_TXD,	9,   0,     15,	  2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	I2C1_SCL,	8,   0,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	I2C1_SDA,	8,   4,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	I2C0_SDA,	8,   12,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	I2C0_SCL,	8,   16,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	RMII_TXD_0,	10,  4,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	RMII_TXD_1,	10,  8,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	RMII_TXEN,	10,  12,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	RMII_CRS_DV,	10,  16,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	RMII_RXD_0,	10,  20,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	RMII_RXD_1,	10,  24,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	RMII_RXER,	10,  28,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MDIO_CLK,	11,  0,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MDIO_D,		11,  4,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_DAT_0,	13,  24,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_DAT_1,	13,  28,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_DAT_2,	14,  0,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_DAT_3,	14,  4,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_DAT_4,	14,  8,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_DAT_5,	14,  12,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_DAT_6,	14,  16,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_DAT_7,	14,  20,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_CLK,	15,  28,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	MMCSD_CMD,	16,  0,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_0,	13,  24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_1,	13,  28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_2,	14,  0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_3,	14,  4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_4,	14,  8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_5,	14,  12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_6,	14,  16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_7,	14,  20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_8,	14,  24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_9,	14,  28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_10,	15,  0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_11,	15,  4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_12,	15,  8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_13,	15,  12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_14,	15,  16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_D_15,	15,  20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_0,	15,  24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_1,	15,  28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_2,	16,  0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_3,	16,  4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_4,	16,  8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_5,	16,  12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_6,	16,  16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_7,	16,  20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_8,	16,  24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_9,	16,  28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_10,	17,  0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_11,	17,  4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_A_12,	17,  8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_BA_1,	17,  12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_BA_0,	17,  16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_CLK,	17,  20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_SDCKE,	17,  24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_CAS,	17,  28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_CS_4,	17,  28,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_RAS,	18,  0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_WE,	18,  4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_CS_0,	18,  8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_CS_2,	18,  12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_CS_3,	18,  16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_OE,	18,  20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_WE_DQM_1,	18,  24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_WE_DQM_0,	18,  28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMA_CS_5,	18,  0,     15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMA_WAIT_0,	19,  0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_SDCKE,	0,   8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_CLK_GLUE,	0,   12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_CLK,	0,   12,    15,   2,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMB_CS_0,	0,   16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMB_CAS,	0,   20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMB_RAS,	0,   24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMB_WE,	0,   28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_BA_1,	1,   0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_BA_0,	1,   4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_0,	1,   8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_1,	1,   12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_2,	1,   16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_3,	1,   20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_4,	1,   24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_5,	1,   28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_6,	2,   0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_7,	2,   4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_8,	2,   8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_9,	2,   12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_10,	2,   16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_11,	2,   20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_A_12,	2,   24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_31,	2,   28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_30,	3,   0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_29,	3,   4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_28,	3,   8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_27,	3,   12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_26,	3,   16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_25,	3,   20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_24,	3,   24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_23,	3,   28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_22,	4,   0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_21,	4,   4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_20,	4,   8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_19,	4,   12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_18,	4,   16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_17,	4,   20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_16,	4,   24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMB_WE_DQM_3,	4,   28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMB_WE_DQM_2,	5,   0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_0,	5,   4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_1,	5,   8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_2,	5,   12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_3,	5,   16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_4,	5,   20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_5,	5,   24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_7,	6,   0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_8,	6,   4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_9,	6,   8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_10,	6,   12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_11,	6,   16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_12,	6,   20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_13,	6,   24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_14,	6,   28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	EMB_D_15,	7,   0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMB_WE_DQM_1,	7,   4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NEMB_WE_DQM_0,	7,   8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	SPI0_SOMI_0,	7,   12,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	SPI0_SIMO_0,	7,   16,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	SPI0_CLK,	7,   20,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NSPI0_ENA,	7,   24,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NSPI0_SCS_0,	7,   28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	SPI1_SOMI_0,	8,   0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	SPI1_SIMO_0,	8,   4,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	SPI1_CLK,	8,   8,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NSPI1_ENA,	8,   28,    15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NSPI1_SCS_0,	9,   0,     15,   1,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	GPIO3_10,	11,  12,    15,   8,     false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_8,	14,  24,    15,   4,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_9,	14,  28,    15,   4,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_7,	15,  24,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_10,	15,   0,    15,   4,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_11,	15,   4,    15,   4,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_12,	15,   8,    15,   4,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_13,	15,  12,    15,   4,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_14,	15,  16,    15,   4,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_15,	15,  20,    15,   4,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_6,	16,   4,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_3,	16,   8,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_2,	16,  12,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_1,	16,  16,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_0,	16,  20,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_PCLK,	16,  24,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_HSYNC,	16,  28,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_VSYNC,	17,   0,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	NLCD_AC_ENB_CS,	17,   4,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_MCLK,	17,   8,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_5,	17,  12,    15,   2,	 false)
OMAPL1X7_MUX_CFG(OMAPL1X7,	LCD_D_4,	17,  16,    15,   2,	 false)
};

void __init omapl1x7_init(void)
{
	davinci_clk_init(omapl1x7_clks);
	davinci_mux_register(davinci_omapl1x7_pins,
				ARRAY_SIZE(davinci_omapl1x7_pins));
}
