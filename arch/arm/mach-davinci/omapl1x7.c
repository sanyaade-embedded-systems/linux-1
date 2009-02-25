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
};

void __init omapl1x7_init(void)
{
	davinci_clk_init(omapl1x7_clks);
	davinci_mux_register(davinci_omapl1x7_pins,
				ARRAY_SIZE(davinci_omapl1x7_pins));
}
