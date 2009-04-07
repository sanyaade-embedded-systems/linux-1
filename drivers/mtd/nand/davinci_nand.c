/*
 * davinci_nand.c - NAND Flash Driver for DaVinci family chips
 *
 * Copyright (C) 2006 Texas Instruments.
 *
 * Port to 2.6.23 Copyright (C) 2008 by:
 *   Sander Huijsen <Shuijsen@optelecom-nkf.com>
 *   Troy Kisky <troy.kisky@boundarydevices.com>
 *   Dirk Behme <Dirk.Behme@gmail.com>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <mach/nand.h>

#include <asm/mach-types.h>


#ifdef CONFIG_MTD_PARTITIONS
static inline int mtd_has_partitions(void) { return 1; }
#else
static inline int mtd_has_partitions(void) { return 0; }
#endif

#ifdef CONFIG_MTD_CMDLINE_PARTS
static inline int mtd_has_cmdlinepart(void) { return 1; }
#else
static inline int mtd_has_cmdlinepart(void) { return 0; }
#endif


/*
 * This is a device driver for the NAND flash controller found on the
 * various DaVinci family chips.  It handles up to four SoC chipselects,
 * and some flavors of secondary chipselect (e.g. based on A12) as used
 * with multichip packages.
 *
 * The 1-bit ECC hardware is supported, but not yet the newer 4-bit ECC
 * available on chips like the DM355 and OMAP-L137 and needed with the
 * more error-prone MLC NAND chips.
 *
 * This driver assumes EM_WAIT connects all the NAND devices' RDY/nBUSY
 * outputs in a "wire-AND" configuration, with no per-chip signals.
 */
struct davinci_nand_info {
	struct mtd_info		mtd;
	struct nand_chip	chip;

	struct device		*dev;
	struct clk		*clk;
	bool			partitioned;

	void __iomem		*base;
	void __iomem		*vaddr;

	u32			ioaddr;
	u32			current_cs;

	u32			mask_chipsel;
	u32			mask_ale;
	u32			mask_cle;

	u32			core_chipsel;
};

static DEFINE_SPINLOCK(davinci_nand_lock);

static u_char davinci_ecc_buf[NAND_MAX_OOBSIZE];

#define to_davinci_nand(m) container_of(m, struct davinci_nand_info, mtd)


static inline unsigned int davinci_nand_readl(struct davinci_nand_info *info,
		int offset)
{
	return __raw_readl(info->base + offset);
}

static inline void davinci_nand_writel(struct davinci_nand_info *info,
		int offset, unsigned long value)
{
	__raw_writel(value, info->base + offset);
}

/*----------------------------------------------------------------------*/

/*
 * Access to hardware control lines:  ALE, CLE, secondary chipselect.
 */

static void nand_davinci_hwcontrol(struct mtd_info *mtd, int cmd,
				   unsigned int ctrl)
{
	struct davinci_nand_info	*info = to_davinci_nand(mtd);
	u32				addr = info->current_cs;
	struct nand_chip		*nand = mtd->priv;

	/* Did the control lines change? */
	if (ctrl & NAND_CTRL_CHANGE) {
		if ((ctrl & NAND_CTRL_CLE) == NAND_CTRL_CLE)
			addr |= info->mask_cle;
		else if ((ctrl & NAND_CTRL_ALE) == NAND_CTRL_ALE)
			addr |= info->mask_ale;

		nand->IO_ADDR_W = (void __iomem __force *)addr;
	}

	if (cmd != NAND_CMD_NONE)
		iowrite8(cmd, nand->IO_ADDR_W);
}

static void nand_davinci_select_chip(struct mtd_info *mtd, int chip)
{
	struct davinci_nand_info	*info = to_davinci_nand(mtd);
	u32				addr = info->ioaddr;

	/* maybe kick in a second chipselect */
	if (chip > 0)
		addr |= info->mask_chipsel;
	info->current_cs = addr;

	info->chip.IO_ADDR_W = (void __iomem __force *)addr;
	info->chip.IO_ADDR_R = info->chip.IO_ADDR_W;
}

/*----------------------------------------------------------------------*/

/*
 * 1-bit hardware ECC ... context maintained for each core chipselect
 */

static inline u32 nand_davinci_readecc_1bit(struct mtd_info *mtd)
{
	struct davinci_nand_info *info = to_davinci_nand(mtd);

	return davinci_nand_readl(info, NANDF1ECC_OFFSET
			+ 4 * info->core_chipsel);
}

static void nand_davinci_hwctl_1bit(struct mtd_info *mtd, int mode)
{
	struct davinci_nand_info *info;
	u32 nandcfr;
	unsigned long flags;

	info = to_davinci_nand(mtd);

	/* Reset ECC hardware */
	nand_davinci_readecc_1bit(mtd);

	spin_lock_irqsave(&davinci_nand_lock, flags);

	/* Restart ECC hardware */
	nandcfr = davinci_nand_readl(info, NANDFCR_OFFSET);
	nandcfr |= BIT(8 + info->core_chipsel);
	davinci_nand_writel(info, NANDFCR_OFFSET, nandcfr);

	spin_unlock_irqrestore(&davinci_nand_lock, flags);
}

/*
 * Read hardware ECC value and pack into three bytes
 */
static int nand_davinci_calculate_1bit(struct mtd_info *mtd,
				      const u_char *dat, u_char *ecc_code)
{
	unsigned int ecc_val = nand_davinci_readecc_1bit(mtd);
	unsigned int ecc24 = (ecc_val & 0x0fff) | ((ecc_val & 0x0fff0000) >> 4);

	/* invert so that erased block ecc is correct */
	ecc24 = ~ecc24;
	ecc_code[0] = (u_char)(ecc24);
	ecc_code[1] = (u_char)(ecc24 >> 8);
	ecc_code[2] = (u_char)(ecc24 >> 16);

	return 0;
}

static int nand_davinci_correct_1bit(struct mtd_info *mtd, u_char *dat,
				     u_char *read_ecc, u_char *calc_ecc)
{
	struct nand_chip *chip = mtd->priv;
	u_int32_t eccNand = read_ecc[0] | (read_ecc[1] << 8) |
					  (read_ecc[2] << 16);
	u_int32_t eccCalc = calc_ecc[0] | (calc_ecc[1] << 8) |
					  (calc_ecc[2] << 16);
	u_int32_t diff = eccCalc ^ eccNand;

	if (diff) {
		if ((((diff >> 12) ^ diff) & 0xfff) == 0xfff) {
			/* Correctable error */
			if ((diff >> (12 + 3)) < chip->ecc.size) {
				dat[diff >> (12 + 3)] ^= BIT((diff >> 12) & 7);
				return 1;
			} else {
				return -1;
			}
		} else if (!(diff & (diff - 1))) {
			/* Single bit ECC error in the ECC itself,
			 * nothing to fix */
			return 1;
		} else {
			/* Uncorrectable error */
			return -1;
		}

	}
	return 0;
}

/*----------------------------------------------------------------------*/

/*
 * NOTE:  NAND boot requires ALE == EM_A[1], CLE == EM_A[2], so that's
 * how these chips are normally wired.  This translates to both 8 and 16
 * bit busses using ALE == BIT(3) in byte addresses, and CLE == BIT(4).
 *
 * For now we assume that configuration, or any other one which ignores
 * the two LSBs for NAND access ... so we can issue 32-bit reads/writes
 * and have that transparently morphed into multiple NAND operations.
 */
static void nand_davinci_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;

	if ((0x03 & ((unsigned)buf)) == 0 && (0x03 & len) == 0)
		ioread32_rep(chip->IO_ADDR_R, buf, len >> 2);
	else if ((0x01 & ((unsigned)buf)) == 0 && (0x01 & len) == 0)
		ioread16_rep(chip->IO_ADDR_R, buf, len >> 1);
	else
		ioread8_rep(chip->IO_ADDR_R, buf, len);
}

static void nand_davinci_write_buf(struct mtd_info *mtd,
		const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;

	if ((0x03 & ((unsigned)buf)) == 0 && (0x03 & len) == 0)
		iowrite32_rep(chip->IO_ADDR_R, buf, len >> 2);
	else if ((0x01 & ((unsigned)buf)) == 0 && (0x01 & len) == 0)
		iowrite16_rep(chip->IO_ADDR_R, buf, len >> 1);
	else
		iowrite8_rep(chip->IO_ADDR_R, buf, len);
}

/*
 * Check hardware register for wait status. Returns 1 if device is ready,
 * 0 if it is still busy.
 */
static int nand_davinci_dev_ready(struct mtd_info *mtd)
{
	struct davinci_nand_info *info = to_davinci_nand(mtd);

	return davinci_nand_readl(info, NANDFSR_OFFSET) & BIT(0);
}

/* DaVinci specific flash bbt decriptors */
static uint8_t nand_davinci_bbt_pattern[] = {'B', 'b', 't', '0' };
static uint8_t nand_davinci_mirror_pattern[] = {'1', 't', 'b', 'B' };

static struct nand_bbt_descr nand_davinci_bbt_main_descr = {
	.options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
		| NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs =	2,
	.len = 4,
	.veroffs = 16,
	.maxblocks = 4,
	.pattern = nand_davinci_bbt_pattern
};

static struct nand_bbt_descr nand_davinci_bbt_mirror_descr = {
	.options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
		| NAND_BBT_2BIT | NAND_BBT_VERSION | NAND_BBT_PERCHIP,
	.offs =	2,
	.len = 4,
	.veroffs = 16,
	.maxblocks = 4,
	.pattern = nand_davinci_mirror_pattern
};

static struct nand_ecclayout nand_davinci_layout_4bit = {
	.eccbytes = 10,
	.eccpos = {
		   6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		   22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		   38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		   54, 55, 56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = {
		{0, 6}, {16, 6}, {32, 6}, {48, 6},
		{64, 6}, {80, 6}, {96, 6}, {112, 6} }
};

static void nand_davinci_hwctl_4bit(struct mtd_info *mtd, int mode)
{
	struct davinci_nand_info *info = to_davinci_nand(mtd);
	u32 nandfcr;

	switch (mode) {
	case NAND_ECC_WRITE:
	case NAND_ECC_READ:
		/* Start a new ECC calculation for reading or writing 512 bytes
		 * of data.*/
		nandfcr = (davinci_nand_readl(info, NANDFCR_OFFSET) &
				~(3 << 4)) | (info->core_chipsel << 4) |
				BIT(12);
		davinci_nand_writel(info, NANDFCR_OFFSET, nandfcr);
		break;
	case NAND_ECC_READSYN:
		davinci_nand_readl(info, NAND_4BIT_ECC1_OFFSET);
		break;
	default:
		break;
	}
}

static u32 nand_davinci_readecc_4bit(struct mtd_info *mtd, unsigned int ecc[4])
{
	struct davinci_nand_info *info = to_davinci_nand(mtd);

	ecc[0] = davinci_nand_readl(info, NAND_4BIT_ECC1_OFFSET) &
			NAND_4BIT_ECC_MASK;
	ecc[1] = davinci_nand_readl(info, NAND_4BIT_ECC2_OFFSET) &
			NAND_4BIT_ECC_MASK;
	ecc[2] = davinci_nand_readl(info, NAND_4BIT_ECC3_OFFSET) &
			NAND_4BIT_ECC_MASK;
	ecc[3] = davinci_nand_readl(info, NAND_4BIT_ECC4_OFFSET) &
			NAND_4BIT_ECC_MASK;

	return 0;
}

static int nand_davinci_calculate_4bit(struct mtd_info *mtd,
					   const uint8_t *dat,
					   uint8_t *ecc_code)
{
	unsigned int hw_4ecc[4] = { 0, 0, 0, 0 };
	unsigned int const1 = 0, const2 = 0;
	unsigned int i;

	nand_davinci_readecc_4bit(mtd, hw_4ecc);

	/* Convert 10 bit ecc value to 8 bit */
	for (i = 0; i < 2; i++) {
		const2 = i * 5;
		const1 = i * 2;

		/* Take first 8 bits from val1 (i=0) or val5 (i=1) */
		ecc_code[const2] = hw_4ecc[const1] & 0xFF;

		/*
		 * Take 2 bits as LSB bits from val1 (i=0) or val5 (i=1) and
		 * 6 bits from val2 (i=0) or val5 (i=1)
		 */
		ecc_code[const2 + 1] =
		    ((hw_4ecc[const1] >> 8) & 0x3) | ((hw_4ecc[const1] >> 14) &
						      0xFC);

		/*
		 * Take 4 bits from val2 (i=0) or val5 (i=1) and 4 bits from
		 * val3 (i=0) or val6 (i=1)
		 */
		ecc_code[const2 + 2] =
		    ((hw_4ecc[const1] >> 22) & 0xF) |
		    ((hw_4ecc[const1 + 1] << 4) & 0xF0);

		/*
		 * Take 6 bits from val3(i=0) or val6 (i=1) and 2 bits from
		 * val4 (i=0) or  val7 (i=1)
		 */
		ecc_code[const2 + 3] =
		    ((hw_4ecc[const1 + 1] >> 4) & 0x3F) |
		    ((hw_4ecc[const1 + 1] >> 10) & 0xC0);

		/* Take 8 bits from val4 (i=0) or val7 (i=1) */
		ecc_code[const2 + 4] = (hw_4ecc[const1 + 1] >> 18) & 0xFF;
	}
	return 0;
}

static int nand_davinci_correct_4bit(struct mtd_info *mtd, uint8_t *page_data,
					  uint8_t *read_ecc, uint8_t *calc_ecc)
{
	struct davinci_nand_info *info = to_davinci_nand(mtd);
	unsigned short ecc_10bit[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int i;
	unsigned int hw_4ecc[4] = { 0, 0, 0, 0 }, iserror = 0;
	unsigned short *pspare = NULL, *pspare1 = NULL;
	unsigned int numErrors, errorAddress, errorValue;

	/*
	 * Check for an ECC where all bytes are 0xFF.  If this is the case, we
	 * will assume we are looking at an erased page and we should ignore the
	 * ECC.
	 */
	for (i = 0; i < 10; i++) {
		if (read_ecc[i] != 0xFF)
			break;
	}
	if (i == 10)
		return 0;

	/* Convert 8 bit in to 10 bit */
	pspare = (unsigned short *)&read_ecc[2];
	pspare1 = (unsigned short *)&read_ecc[0];
	/* Take 10 bits from 0th and 1st bytes */
	ecc_10bit[0] = (*pspare1) & 0x3FF;	/* 10 */
	/* Take 6 bits from 1st byte and 4 bits from 2nd byte */
	ecc_10bit[1] = (((*pspare1) >> 10) & 0x3F)
	    | (((pspare[0]) << 6) & 0x3C0);	/* 6 + 4 */
	/* Take 4 bits form 2nd bytes and 6 bits from 3rd bytes */
	ecc_10bit[2] = ((pspare[0]) >> 4) & 0x3FF;	/* 10 */
	/*Take 2 bits from 3rd byte and 8 bits from 4th byte */
	ecc_10bit[3] = (((pspare[0]) >> 14) & 0x3)
	    | ((((pspare[1])) << 2) & 0x3FC);	/* 2 + 8 */
	/* Take 8 bits from 5th byte and 2 bits from 6th byte */
	ecc_10bit[4] = ((pspare[1]) >> 8)
	    | ((((pspare[2])) << 8) & 0x300);	/* 8 + 2 */
	/* Take 6 bits from 6th byte and 4 bits from 7th byte */
	ecc_10bit[5] = (pspare[2] >> 2) & 0x3FF;	/* 10 */
	/* Take 4 bits from 7th byte and 6 bits from 8th byte */
	ecc_10bit[6] = (((pspare[2]) >> 12) & 0xF)
	    | ((((pspare[3])) << 4) & 0x3F0);	/* 4 + 6 */
	/*Take 2 bits from 8th byte and 8 bits from 9th byte */
	ecc_10bit[7] = ((pspare[3]) >> 6) & 0x3FF;	/* 10 */

	/*
	 * Write the parity values in the NAND Flash 4-bit ECC Load register.
	 * Write each parity value one at a time starting from 4bit_ecc_val8
	 * to 4bit_ecc_val1.
	 */
	for (i = 7; i >= 0; i--)
		davinci_nand_writel(info, NAND_4BIT_ECC_LOAD_OFFSET,
				    ecc_10bit[i]);

	/*
	 * Perform a dummy read to the EMIF Revision Code and Status register.
	 * This is required to ensure time for syndrome calculation after
	 * writing the ECC values in previous step.
	 */
	davinci_nand_readl(info, NANDFSR_OFFSET);

	/*
	 * Read the syndrome from the NAND Flash 4-Bit ECC 1-4 registers.
	 * A syndrome value of 0 means no bit errors. If the syndrome is
	 * non-zero then go further otherwise return.
	 */
	nand_davinci_readecc_4bit(mtd, hw_4ecc);

	if (hw_4ecc[0] == ECC_STATE_NO_ERR && hw_4ecc[1] == ECC_STATE_NO_ERR &&
	    hw_4ecc[2] == ECC_STATE_NO_ERR && hw_4ecc[3] == ECC_STATE_NO_ERR)
		return 0;

	/*
	 * Clear any previous address calculation by doing a dummy read of an
	 * error address register.
	 */
	davinci_nand_readl(info, NAND_ERR_ADD1_OFFSET);

	/*
	 * Set the addr_calc_st bit(bit no 13) in the NAND Flash Control
	 * register to 1.
	 */
	davinci_nand_writel(info, NANDFCR_OFFSET,
		davinci_nand_readl(info, NANDFCR_OFFSET) | BIT(13));

	/*
	 * Wait for the corr_state field (bits 8 to 11)in the
	 * NAND Flash Status register to be equal to 0x0, 0x1, 0x2, or 0x3.
	 */
	do {
		iserror = davinci_nand_readl(info, NANDFSR_OFFSET);
		iserror &= NANDFSR_ECC_STATE_MASK;
		iserror = iserror >> 8;
	} while ((ECC_STATE_NO_ERR != iserror) &&
		 (ECC_STATE_TOO_MANY_ERRS != iserror) &&
		 (ECC_STATE_ERR_CORR_COMP_P != iserror) &&
		 (ECC_STATE_ERR_CORR_COMP_N != iserror));

	/*
	 * ECC_STATE_TOO_MANY_ERRS (0x1) means errors cannot be
	 * corrected (five or more errors).  The number of errors
	 * calculated (err_num field) differs from the number of errors
	 * searched.  ECC_STATE_ERR_CORR_COMP_P (0x2) means error
	 * correction complete (errors on bit 8 or 9).
	 * ECC_STATE_ERR_CORR_COMP_N (0x3) means error correction
	 * complete (error exists).
	 */
	udelay(info->chip.chip_delay);
	if (iserror == ECC_STATE_NO_ERR)
		return 0;
	else if (iserror == ECC_STATE_TOO_MANY_ERRS) {
		printk(KERN_DEBUG "Too many errors to be corrected!\n");
		return -1;
	}

	numErrors = 1 + ((davinci_nand_readl(info, NANDFSR_OFFSET) >> 16) &
			  0x3);

	/* Read the error address, error value and correct */
	for (i = 0; i < numErrors; i++) {
		int addr_offset, val_offset;

		if (i > 1) {
			addr_offset = NAND_ERR_ADD2_OFFSET;
			val_offset = NAND_ERR_ERRVAL2_OFFSET;
		} else {
			addr_offset = NAND_ERR_ADD1_OFFSET;
			val_offset = NAND_ERR_ERRVAL1_OFFSET;
		}
		errorAddress = ((davinci_nand_readl(info, addr_offset) >>
				(16 * (i & 1))) & 0x3FF);
		errorAddress = ((512 + 7) - errorAddress);
		errorValue = ((davinci_nand_readl(info, val_offset) >>
				(16 * (i & 1))) & 0xFF);
		/* xor the corrupt data with error value */
		if (errorAddress < 512)
			page_data[errorAddress] ^= errorValue;
	}

	return numErrors;
}

static int davinci_nand_read_page_syndrome(struct mtd_info *mtd,
				struct nand_chip *chip, uint8_t *buf, int page)
{
	int i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *p = buf;
	uint8_t *oob = chip->oob_poi;

	/* Read the OOB area first */
	chip->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
	chip->read_buf(mtd, oob, mtd->oobsize);
	chip->cmdfunc(mtd, NAND_CMD_READ0, 0, page);

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		int stat;

		chip->ecc.hwctl(mtd, NAND_ECC_READ);
		chip->read_buf(mtd, p, eccsize);
		chip->ecc.hwctl(mtd, NAND_ECC_READSYN);

		if (chip->ecc.prepad)
			oob += chip->ecc.prepad;

		stat = chip->ecc.correct(mtd, p, oob, NULL);

		if (stat < 0)
			mtd->ecc_stats.failed++;
		else
			mtd->ecc_stats.corrected += stat;

		oob += eccbytes;

		if (chip->ecc.postpad)
			oob += chip->ecc.postpad;
	}
	return 0;
}

static void davinci_nand_write_page_syndrome(struct mtd_info *mtd,
				    struct nand_chip *chip, const uint8_t *buf)
{
	int i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	int offset = 0;
	const uint8_t *p = buf;
	uint8_t *oob = chip->oob_poi;

	/* Write in chunks of eccsize=512, calculate ECC for eccsize=512, save
	 * OOB in the temporary buffer and once all eccsteps are covered, write
	 * OOB from temporary buffer to the NAND page sparebytes region.
	 */
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		chip->ecc.hwctl(mtd, NAND_ECC_WRITE);
		chip->write_buf(mtd, p, eccsize);
		chip->ecc.calculate(mtd, p, oob + chip->ecc.prepad);

		if (chip->ecc.prepad) {
			memcpy(&davinci_ecc_buf[offset], oob, chip->ecc.prepad);
			oob += chip->ecc.prepad;
			offset += chip->ecc.prepad;
		}
		memcpy(&davinci_ecc_buf[offset], oob, eccbytes);
		oob += eccbytes;
		offset += eccbytes;

		if (chip->ecc.postpad) {
			memcpy(&davinci_ecc_buf[offset], oob,
				chip->ecc.postpad);
			oob += chip->ecc.postpad;
			offset += chip->ecc.postpad;
		}
	}

	chip->write_buf(mtd, davinci_ecc_buf, mtd->oobsize);
}

static int davinci_nand_read_oob_std(struct mtd_info *mtd,
				struct nand_chip *chip, int page, int sndcmd)
{
	if (sndcmd) {
		chip->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
		sndcmd = 0;
	}
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);
	return sndcmd;
}

static int davinci_nand_write_oob_std(struct mtd_info *mtd,
				      struct nand_chip *chip, int page)
{
	int status = 0;
	const uint8_t *buf = chip->oob_poi;
	int length = mtd->oobsize;

	chip->cmdfunc(mtd, NAND_CMD_SEQIN, mtd->writesize, page);
	chip->write_buf(mtd, buf, length);
	/* Send command to program the OOB data */
	chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);

	status = chip->waitfunc(mtd, chip);

	return status & NAND_STATUS_FAIL ? -EIO : 0;
}

static void __init nand_dm6446evm_flash_init(struct davinci_nand_info *info)
{
	u32 regval, a1cr;

	/*
	 * NAND FLASH timings @ PLL1 == 459 MHz
	 *  - AEMIF.CLK freq   = PLL1/6 = 459/6 = 76.5 MHz
	 *  - AEMIF.CLK period = 1/76.5 MHz = 13.1 ns
	 */
	regval = 0
		| (0 << 31)           /* selectStrobe */
		| (0 << 30)           /* extWait (never with NAND) */
		| (1 << 26)           /* writeSetup      10 ns */
		| (3 << 20)           /* writeStrobe     40 ns */
		| (1 << 17)           /* writeHold       10 ns */
		| (0 << 13)           /* readSetup       10 ns */
		| (3 << 7)            /* readStrobe      60 ns */
		| (0 << 4)            /* readHold        10 ns */
		| (3 << 2)            /* turnAround      ?? ns */
		| (0 << 0)            /* asyncSize       8-bit bus */
		;
	a1cr = davinci_nand_readl(info, A1CR_OFFSET);
	if (a1cr != regval) {
		dev_dbg(info->dev, "Warning: NAND config: Set A1CR " \
		       "reg to 0x%08x, was 0x%08x, should be done by " \
		       "bootloader.\n", regval, a1cr);
		davinci_nand_writel(info, A1CR_OFFSET, regval);
	}
}

/*----------------------------------------------------------------------*/

static int __init nand_davinci_probe(struct platform_device *pdev)
{
	struct davinci_nand_pdata	*pdata = pdev->dev.platform_data;
	struct davinci_nand_info	*info;
	struct resource			*res1;
	struct resource			*res2;
	void __iomem			*vaddr;
	void __iomem			*base;
	int				ret;
	u32				val;
	nand_ecc_modes_t		ecc_mode;

	/* which external chipselect will we be managing? */
	if (pdev->id < 0 || pdev->id > 3)
		return -ENODEV;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "unable to allocate memory\n");
		ret = -ENOMEM;
		goto err_nomem;
	}

	platform_set_drvdata(pdev, info);

	res1 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	res2 = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!res1 || !res2) {
		dev_err(&pdev->dev, "resource missing\n");
		ret = -EINVAL;
		goto err_nomem;
	}

	vaddr = ioremap(res1->start, res1->end - res1->start);
	base = ioremap(res2->start, res2->end - res2->start);
	if (!vaddr || !base) {
		dev_err(&pdev->dev, "ioremap failed\n");
		ret = -EINVAL;
		goto err_ioremap;
	}

	info->dev		= &pdev->dev;
	info->base		= base;
	info->vaddr		= vaddr;

	info->mtd.priv		= &info->chip;
	info->mtd.name		= dev_name(&pdev->dev);
	info->mtd.owner		= THIS_MODULE;

	info->chip.IO_ADDR_R	= vaddr;
	info->chip.IO_ADDR_W	= vaddr;
	info->chip.chip_delay	= 0;
	info->chip.select_chip	= nand_davinci_select_chip;

	/* options such as NAND_USE_FLASH_BBT or 16-bit widths */
	info->chip.options	= pdata ? pdata->options : 0;

	info->ioaddr		= (u32 __force) vaddr;

	info->current_cs	= info->ioaddr;
	info->core_chipsel	= pdev->id;
	info->mask_chipsel	= pdata->mask_chipsel;

	/* use nandboot-capable ALE/CLE masks by default */
	if (pdata && pdata->mask_ale)
		info->mask_ale	= pdata->mask_cle;
	else
		info->mask_ale	= MASK_ALE;
	if (pdata && pdata->mask_cle)
		info->mask_cle	= pdata->mask_cle;
	else
		info->mask_cle	= MASK_CLE;

	/* Set address of hardware control function */
	info->chip.cmd_ctrl	= nand_davinci_hwcontrol;
	info->chip.dev_ready	= nand_davinci_dev_ready;

	/* Speed up buffer I/O */
	info->chip.read_buf     = nand_davinci_read_buf;
	info->chip.write_buf    = nand_davinci_write_buf;

	/* use board-specific ECC config; else, the best available */
	if (pdata)
		ecc_mode = pdata->ecc_mode;
	else
		ecc_mode = NAND_ECC_HW;

	switch (ecc_mode) {
	case NAND_ECC_NONE:
	case NAND_ECC_SOFT:
		break;
	case NAND_ECC_HW:
		info->chip.ecc.calculate = nand_davinci_calculate_1bit;
		info->chip.ecc.correct = nand_davinci_correct_1bit;
		info->chip.ecc.hwctl = nand_davinci_hwctl_1bit;
		info->chip.ecc.size = 512;
		info->chip.ecc.bytes = 3;
		break;
	case NAND_ECC_HW_SYNDROME:
		dev_info(&pdev->dev, "Using 4-bit hardware ECC - Syndrome\n");
		info->chip.ecc.layout = &nand_davinci_layout_4bit;
		info->chip.ecc.calculate = nand_davinci_calculate_4bit;
		info->chip.ecc.correct = nand_davinci_correct_4bit;
		info->chip.ecc.hwctl = nand_davinci_hwctl_4bit;
		/* Override read_page/write_page and read_oob/write_oob APIs */
		info->chip.ecc.read_page = davinci_nand_read_page_syndrome;
		info->chip.ecc.write_page = davinci_nand_write_page_syndrome;
		info->chip.ecc.read_oob = davinci_nand_read_oob_std;
		info->chip.ecc.write_oob = davinci_nand_write_oob_std;
		info->chip.ecc.size = 512;
		info->chip.ecc.bytes = 10;
		info->chip.ecc.prepad = 6;
		info->chip.bbt_td = &nand_davinci_bbt_main_descr;
		info->chip.bbt_md = &nand_davinci_bbt_mirror_descr;
		break;
	default:
		ret = -EINVAL;
		goto err_ecc;
	}
	info->chip.ecc.mode = ecc_mode;

	info->clk = clk_get(&pdev->dev, "aemif");
	if (IS_ERR(info->clk)) {
		ret = PTR_ERR(info->clk);
		dev_dbg(&pdev->dev, "unable to get AEMIF clock, err %d\n", ret);
		goto err_clk;
	}

	ret = clk_enable(info->clk);
	if (ret < 0) {
		dev_dbg(&pdev->dev, "unable to enable AEMIF clock, err %d\n",
			ret);
		goto err_clk_enable;
	}

	/* EMIF timings should normally be set by the boot loader,
	 * especially after boot-from-NAND.  The *only* reason to
	 * have this special casing for the DM6446 EVM is to work
	 * with boot-from-NOR ... with CS0 manually re-jumpered
	 * (after startup) so it addresses the NAND flash, not NOR.
	 * Even for dev boards, that's unusually rude...
	 */
	if (machine_is_davinci_evm())
		nand_dm6446evm_flash_init(info);

	spin_lock_irq(&davinci_nand_lock);

	/* put CSxNAND into NAND mode */
	val = davinci_nand_readl(info, NANDFCR_OFFSET);
	val |= BIT(info->core_chipsel);
	davinci_nand_writel(info, NANDFCR_OFFSET, val);

	spin_unlock_irq(&davinci_nand_lock);

	/* Scan to find existence of the device(s) */
	ret = nand_scan(&info->mtd, pdata->mask_chipsel ? 2 : 1);
	if (ret < 0) {
		dev_dbg(&pdev->dev, "no NAND chip(s) found\n");
		goto err_scan;
	}

	if (mtd_has_partitions()) {
		struct mtd_partition	*mtd_parts = NULL;
		int			mtd_parts_nb = 0;

		if (mtd_has_cmdlinepart()) {
			static const char *probes[] __initconst =
				{ "cmdlinepart", NULL };

			const char		*master_name;

			/* Set info->mtd.name = 0 temporarily */
			master_name		= info->mtd.name;
			info->mtd.name		= (char *)0;

			/* info->mtd.name == 0, means: don't bother checking
			   <mtd-id> */
			mtd_parts_nb = parse_mtd_partitions(&info->mtd, probes,
							    &mtd_parts, 0);

			/* Restore info->mtd.name */
			info->mtd.name = master_name;
		}

		if (mtd_parts_nb <= 0 && pdata) {
			mtd_parts = pdata->parts;
			mtd_parts_nb = pdata->nr_parts;
		}

		/* Register any partitions */
		if (mtd_parts_nb > 0) {
			ret = add_mtd_partitions(&info->mtd,
					mtd_parts, mtd_parts_nb);
			if (ret == 0)
				info->partitioned = true;
		}

	} else if (pdata && pdata->nr_parts) {
		dev_warn(&pdev->dev, "ignoring %d default partitions on %s\n",
				pdata->nr_parts, info->mtd.name);
	}

	/* If there's no partition info, just package the whole chip
	 * as a single MTD device.
	 */
	if (!info->partitioned)
		ret = add_mtd_device(&info->mtd) ? -ENODEV : 0;

	if (ret < 0)
		goto err_scan;

	val = davinci_nand_readl(info, NRCSR_OFFSET);
	dev_info(&pdev->dev, "controller rev. %d.%d\n",
	       (val >> 8) & 0xff, val & 0xff);

	return 0;

err_scan:
	clk_disable(info->clk);

err_clk_enable:
	clk_put(info->clk);

err_ecc:
err_clk:
err_ioremap:
	if (base)
		iounmap(base);
	if (vaddr)
		iounmap(vaddr);

err_nomem:
	kfree(info);
	return ret;
}

static int __exit nand_davinci_remove(struct platform_device *pdev)
{
	struct davinci_nand_info *info = platform_get_drvdata(pdev);
	int status;

	if (mtd_has_partitions() && info->partitioned)
		status = del_mtd_partitions(&info->mtd);
	else
		status = del_mtd_device(&info->mtd);

	iounmap(info->base);
	iounmap(info->vaddr);

	nand_release(&info->mtd);

	clk_disable(info->clk);
	clk_put(info->clk);

	kfree(info);

	return 0;
}

static struct platform_driver nand_davinci_driver = {
	.remove		= __exit_p(nand_davinci_remove),
	.driver		= {
		.name	= "davinci_nand",
	},
};
MODULE_ALIAS("platform:davinci_nand");

static int __init nand_davinci_init(void)
{
	return platform_driver_probe(&nand_davinci_driver, nand_davinci_probe);
}
module_init(nand_davinci_init);

static void __exit nand_davinci_exit(void)
{
	platform_driver_unregister(&nand_davinci_driver);
}
module_exit(nand_davinci_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("Davinci NAND flash driver");

