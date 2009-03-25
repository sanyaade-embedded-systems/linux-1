/*
 *  Board-specific MMC configuration
 */

#ifndef _DAVINCI_MMC_H
#define _DAVINCI_MMC_H

#include <linux/types.h>
#include <linux/mmc/host.h>

struct davinci_mmc_config {
	/* get_cd()/get_wp() may sleep */
	int	(*get_cd)(int module);
	int	(*get_ro)(int module);
	/* wires == 0 is equivalent to wires == 4 (4-bit parallel) */
	u8	wires;

	u32     max_freq;

	/* any additional host capabilities: OR'd in to mmc->f_caps */
	u32     caps;

	/* Instance of EDMA Channel Controller this MMC is on */
	u8	cc_inst;
};
void davinci_setup_mmc(int module, struct davinci_mmc_config *config);

enum {
	EDMA_CC_INST_0 = 0,
	EDMA_CC_INST_1,
};

#endif
