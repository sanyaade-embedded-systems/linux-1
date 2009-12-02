/*
 *  arch/arm/include/asm/mach/mmc.h
 */
#ifndef ASMARM_MACH_MMC_H
#define ASMARM_MACH_MMC_H

#include <linux/mmc/host.h>

#ifdef CONFIG_MMC_EMBEDDED_SDIO
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>

struct embedded_sdio_data {
    struct sdio_cis cis;
    struct sdio_cccr cccr;
    struct sdio_embedded_func *funcs;
    int num_funcs;
    unsigned int quirks;
};
#endif

struct mmc_platform_data {
	unsigned int ocr_mask;			/* available voltages */
	u32 (*translate_vdd)(struct device *, unsigned int);
	unsigned int (*status)(struct device *);
};

#endif
