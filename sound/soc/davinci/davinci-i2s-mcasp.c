/*
 * ALSA SoC I2S (McASP) Audio Layer for TI DAVINCI processor
 *
 * Author: Nirmal Pandey <n-pandey@ti.com>,
 *         Suresh Rajashekara <suresh.r@ti.com>
 *         Steve Chen, <schen@.mvista.com>
 *
 * Copyright:   (C) 2008 MontaVista Software, Inc., <source@mvista.com>
 * Copyright:   (C) 2008  Texas Instruments, India
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/clk.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>

#include "davinci-pcm.h"
#include "davinci-i2s-mcasp.h"

static inline void mcasp_set_bits(void __iomem *reg, u32 val)
{
	__raw_writel(__raw_readl(reg) | val, reg);
}

static inline void mcasp_clr_bits(void __iomem *reg, u32 val)
{
	__raw_writel((__raw_readl(reg) & ~(val)), reg);
}

static inline void mcasp_mod_bits(void __iomem *reg, u32 val, u32 mask)
{
	__raw_writel((__raw_readl(reg) & ~mask) | val, reg);
}

static inline void mcasp_set_reg(void __iomem *reg, u32 val)
{
	__raw_writel(val, reg);
}

static inline u32 mcasp_get_reg(void __iomem *reg)
{
	return (unsigned int)__raw_readl(reg);
}

static inline void mcasp_set_ctl_reg(void __iomem *regs, u32 val)
{
	mcasp_set_bits(regs, val);
	while ((mcasp_get_reg(regs) & val) != val)
		;
}
int davinci_i2s_startup(struct snd_pcm_substream *substream,
							struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	struct davinci_audio_dev *dev = rtd->dai->cpu_dai->private_data;

	cpu_dai->dma_data = dev->dma_params[substream->stream];

	return 0;
}
EXPORT_SYMBOL(davinci_i2s_startup);

int davinci_i2s_mcasp_probe(struct platform_device *pdev,
							struct snd_soc_dai *dai)
{
	int tmp, link_cnt;
	int count = 0;
	int backup_count = 0;
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_card *card = socdev->card;
	struct snd_soc_dai *cpu_dai;
	struct davinci_audio_dev *dev;
	struct resource *mem, *ioarea;
	struct evm_snd_platform_data *parray = pdev->dev.platform_data;
	struct evm_snd_platform_data *pdata;
	struct davinci_pcm_dma_params *dma_data;
	int ret = 0;

	dev = kzalloc(sizeof(struct davinci_audio_dev) * card->num_links,
		      GFP_KERNEL);
	if (!dev)
		return  -ENOMEM;

	dma_data = kzalloc(sizeof(struct davinci_pcm_dma_params) *
			(card->num_links << 1), GFP_KERNEL);
	if (!dma_data)
		goto err_release_dev;

	for (link_cnt = 0; link_cnt < card->num_links; link_cnt++) {
		mem = platform_get_resource(pdev, IORESOURCE_MEM, link_cnt);
		if (!mem) {
			dev_err(&pdev->dev, "no mem resource?\n");
			ret = -ENODEV;
			backup_count = 0;
			goto err_release_data;
		}

		ioarea = request_mem_region(mem->start,
				(mem->end - mem->start) + 1, pdev->name);
		if (!ioarea) {
			dev_err(&pdev->dev, "Audio region already claimed\n");
			ret = -EBUSY;
			backup_count = 1;
			goto err_release_data;
		}


		cpu_dai = card->dai_link[link_cnt].cpu_dai;
		cpu_dai->private_data = &dev[link_cnt];
		pdata = &parray[link_cnt];
		dev[link_cnt].clk = clk_get(&pdev->dev, NULL);

		if (IS_ERR(dev[link_cnt].clk)) {
			ret = -ENODEV;
			backup_count = 2;
			goto err_release_data;
		}
		clk_enable(dev[link_cnt].clk);

		dev[link_cnt].base = (void __iomem *)IO_ADDRESS(mem->start);
		dev[link_cnt].op_mode = pdata->op_mode;
		dev[link_cnt].tdm_slots = pdata->tdm_slots;
		dev[link_cnt].num_serializer = pdata->num_serializer;
		dev[link_cnt].serial_dir = pdata->serial_dir;
		dev[link_cnt].codec_fmt = pdata->codec_fmt;
		dev[link_cnt].version = pdata->version;
		dev[link_cnt].txnumevt = pdata->txnumevt;
		dev[link_cnt].rxnumevt = pdata->rxnumevt;

		if (dev[link_cnt].txnumevt > 64)
			dev[link_cnt].txnumevt = 64;

		if (dev[link_cnt].rxnumevt > 64)
			dev[link_cnt].rxnumevt = 64;

		dma_data[count].name = "I2S PCM Stereo out";
		dma_data[count].channel = pdata->tx_dma_ch;
		dma_data[count].eventq_no = pdata->eventq_no;
		dma_data[count].dma_addr = (dma_addr_t) (pdata->tx_dma_offset +
					   io_v2p(dev[link_cnt].base));
		dev[link_cnt].dma_params[SNDRV_PCM_STREAM_PLAYBACK] =
				&dma_data[count];


		count++;
		dma_data[count].name = "I2S PCM Stereo in";
		dma_data[count].channel = pdata->rx_dma_ch;
		dma_data[count].eventq_no = pdata->eventq_no;
		dma_data[count].dma_addr = (dma_addr_t)(pdata->rx_dma_offset +
					    io_v2p(dev[link_cnt].base));
		dev[link_cnt].dma_params[SNDRV_PCM_STREAM_CAPTURE] =
				&dma_data[count];
		count++;

	}
	return 0;

err_release_data:
	for (tmp = link_cnt; tmp >= 0; tmp--) {
		if (backup_count > 2)
			clk_disable(dev[tmp].clk);

		if (backup_count > 1) {
			mem = platform_get_resource(pdev, IORESOURCE_MEM, tmp);
			release_mem_region(mem->start,
					   (mem->end - mem->start) + 1);
		}
		backup_count = 3;
	}
	kfree(dma_data);

err_release_dev:
	kfree(dev);

	return ret;
}
EXPORT_SYMBOL(davinci_i2s_mcasp_probe);

void davinci_i2s_mcasp_remove(struct platform_device *pdev,
						struct snd_soc_dai *dai)
{
	int i;
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_card *card = socdev->card;
	struct snd_soc_dai *cpu_dai = card->dai_link->cpu_dai;
	struct davinci_audio_dev *dev_list = cpu_dai->private_data;
	struct davinci_audio_dev *dev;
	struct resource *mem;
	struct davinci_pcm_dma_params *ptr;

	for (i = 0; i < card->num_links; i++) {
		dev = &dev_list[i];
		clk_disable(dev->clk);
		clk_put(dev->clk);
		dev->clk = NULL;

		mem = platform_get_resource(pdev, IORESOURCE_MEM, i);
		release_mem_region(mem->start, (mem->end - mem->start) + 1);
	}
	ptr = dev_list->dma_params[SNDRV_PCM_STREAM_PLAYBACK];
	kfree(ptr);
	kfree(dev_list);
}
EXPORT_SYMBOL(davinci_i2s_mcasp_remove);


void mcasp_start_rx(struct davinci_audio_dev *dev)
{
	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLR_REG, RXHCLKRST);
	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLR_REG, RXCLKRST);
	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLR_REG, RXSERCLR);
	mcasp_set_reg(dev->base + DAVINCI_MCASP_RXBUF_REG, 0);

	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLR_REG, RXSMRST);
	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLR_REG, RXFSRST);
	mcasp_set_reg(dev->base + DAVINCI_MCASP_RXBUF_REG, 0);

	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLR_REG, RXSMRST);
	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLR_REG, RXFSRST);
}

void mcasp_start_tx(struct davinci_audio_dev *dev)
{
	u8 offset = 0, i;
	u32 cnt;

	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLX_REG, TXHCLKRST);
	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLX_REG, TXCLKRST);
	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLX_REG, TXSERCLR);
	mcasp_set_reg(dev->base + DAVINCI_MCASP_TXBUF_REG, 0);

	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLX_REG, TXSMRST);
	mcasp_set_ctl_reg(dev->base + DAVINCI_MCASP_GBLCTLX_REG, TXFSRST);
	mcasp_set_reg(dev->base + DAVINCI_MCASP_TXBUF_REG, 0);

	for (i = 0; i < dev->num_serializer; i++) {
		if (dev->serial_dir[i] == TX_MODE) {
			offset = i;
			break;
		}
	}

	/* wait for TX ready */
	cnt = 0;
	while (!(mcasp_get_reg(dev->base + DAVINCI_MCASP_XRSRCTL_REG(offset)) &
		 TXSTATE) && (cnt < 100000))
		cnt++;

	mcasp_set_reg(dev->base + DAVINCI_MCASP_TXBUF_REG, 0);
}

static void davinci_mcasp_start(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct davinci_audio_dev *dev = rtd->dai->cpu_dai->private_data;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		mcasp_start_tx(dev);
	else
		mcasp_start_rx(dev);
}

void mcasp_stop_rx(struct davinci_audio_dev *dev)
{
	mcasp_set_reg(dev->base + DAVINCI_MCASP_GBLCTLR_REG, 0);
	mcasp_set_reg(dev->base + DAVINCI_MCASP_RXSTAT_REG, 0xFFFFFFFF);
}

void mcasp_stop_tx(struct davinci_audio_dev *dev)
{
	mcasp_set_reg(dev->base + DAVINCI_MCASP_GBLCTLX_REG, 0);
	mcasp_set_reg(dev->base + DAVINCI_MCASP_TXSTAT_REG, 0xFFFFFFFF);
}

static void davinci_mcasp_stop(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct davinci_audio_dev *dev = rtd->dai->cpu_dai->private_data;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		mcasp_stop_tx(dev);
	else
		mcasp_stop_rx(dev);
}

static int davinci_i2s_mcasp_set_dai_fmt(struct snd_soc_dai *cpu_dai,
					 unsigned int fmt)
{
	struct davinci_audio_dev *dev = cpu_dai->private_data;
	void __iomem *base = dev->base;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		/* codec is clock and frame slave */
		mcasp_set_bits(base + DAVINCI_MCASP_ACLKXCTL_REG, ACLKXE);
		mcasp_set_bits(base + DAVINCI_MCASP_TXFMCTL_REG, AFSXE);

		mcasp_set_bits(base + DAVINCI_MCASP_ACLKRCTL_REG, ACLKRE);
		mcasp_set_bits(base + DAVINCI_MCASP_RXFMCTL_REG, AFSRE);

		mcasp_set_bits(base + DAVINCI_MCASP_PDIR_REG, (0x7 << 26));
		break;
	case SND_SOC_DAIFMT_CBM_CFS:
		/* codec is clock master and frame slave */
		mcasp_set_bits(base + DAVINCI_MCASP_ACLKXCTL_REG, ACLKXE);
		mcasp_set_bits(base + DAVINCI_MCASP_TXFMCTL_REG, AFSXE);

		mcasp_set_bits(base + DAVINCI_MCASP_ACLKRCTL_REG, ACLKRE);
		mcasp_set_bits(base + DAVINCI_MCASP_RXFMCTL_REG, AFSRE);

		mcasp_set_bits(base + DAVINCI_MCASP_PDIR_REG, (0x2d << 26));
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		/* codec is clock and frame master */
		mcasp_clr_bits(base + DAVINCI_MCASP_ACLKXCTL_REG, ACLKXE);
		mcasp_clr_bits(base + DAVINCI_MCASP_TXFMCTL_REG, AFSXE);

		mcasp_clr_bits(base + DAVINCI_MCASP_ACLKRCTL_REG, ACLKRE);
		mcasp_clr_bits(base + DAVINCI_MCASP_RXFMCTL_REG, AFSRE);

		mcasp_clr_bits(base + DAVINCI_MCASP_PDIR_REG, (0x3f << 26));
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_IB_NF:
		mcasp_clr_bits(base + DAVINCI_MCASP_ACLKXCTL_REG, ACLKXPOL);
		mcasp_clr_bits(base + DAVINCI_MCASP_TXFMCTL_REG, FSXPOL);

		mcasp_set_bits(base + DAVINCI_MCASP_ACLKRCTL_REG, ACLKRPOL);
		mcasp_clr_bits(base + DAVINCI_MCASP_RXFMCTL_REG, FSRPOL);
		break;
	case SND_SOC_DAIFMT_NB_IF:
		mcasp_set_bits(base + DAVINCI_MCASP_ACLKXCTL_REG, ACLKXPOL);
		mcasp_set_bits(base + DAVINCI_MCASP_TXFMCTL_REG, FSXPOL);
		mcasp_clr_bits(base + DAVINCI_MCASP_ACLKRCTL_REG, ACLKRPOL);
		mcasp_set_bits(base + DAVINCI_MCASP_RXFMCTL_REG, FSRPOL);
		break;
	case SND_SOC_DAIFMT_IB_IF:
		mcasp_clr_bits(base + DAVINCI_MCASP_ACLKXCTL_REG, ACLKXPOL);
		mcasp_set_bits(base + DAVINCI_MCASP_TXFMCTL_REG, FSXPOL);

		mcasp_set_bits(base + DAVINCI_MCASP_ACLKRCTL_REG, ACLKRPOL);
		mcasp_set_bits(base + DAVINCI_MCASP_RXFMCTL_REG, FSRPOL);
		break;
	case SND_SOC_DAIFMT_NB_NF:
		mcasp_set_bits(base + DAVINCI_MCASP_ACLKXCTL_REG, ACLKXPOL);
		mcasp_clr_bits(base + DAVINCI_MCASP_TXFMCTL_REG, FSXPOL);

		mcasp_clr_bits(base + DAVINCI_MCASP_ACLKRCTL_REG, ACLKRPOL);
		mcasp_clr_bits(base + DAVINCI_MCASP_RXFMCTL_REG, FSRPOL);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int davinci_config_channel_size(struct snd_pcm_substream *substream,
				       int channel_size)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct davinci_audio_dev *dev = rtd->dai->cpu_dai->private_data;
	u32 fmt = 0;
	u32 mask;

	switch (channel_size) {
	case DAVINCI_AUDIO_WORD_8:
		fmt = 0x03 << 4;
		mask = 0x000000FF;
		break;

	case DAVINCI_AUDIO_WORD_12:
		fmt = 0x05 << 4;
		mask = 0x00000FFF;
		break;

	case DAVINCI_AUDIO_WORD_16:
		fmt = 0x07 << 4;
		mask = 0x0000FFFF;
		break;

	case DAVINCI_AUDIO_WORD_20:
		fmt = 0x09 << 4;
		mask = 0x000FFFFF;
		break;

	case DAVINCI_AUDIO_WORD_24:
		fmt = 0x0B << 4;
		mask = 0x00FFFFFF;
		break;

	case DAVINCI_AUDIO_WORD_28:
		fmt = 0x0D << 4;
		mask = 0x0FFFFFFF;
		break;

	case DAVINCI_AUDIO_WORD_32:
		fmt = 0x0F << 4;
		mask = 0xFFFFFFFF;
		break;

	default:
		return -1;
	}

	mcasp_set_bits(dev->base + DAVINCI_MCASP_TXFMT_REG, TXROT(4));
	mcasp_set_bits(dev->base + DAVINCI_MCASP_RXFMT_REG, RXROT(4));
	mcasp_set_reg(dev->base + DAVINCI_MCASP_TXMASK_REG, mask);
	mcasp_set_reg(dev->base + DAVINCI_MCASP_RXMASK_REG, mask);
	mcasp_mod_bits(dev->base + DAVINCI_MCASP_RXFMT_REG, fmt, (0x0F << 4));
	mcasp_mod_bits(dev->base + DAVINCI_MCASP_TXFMT_REG, fmt, (0x0F << 4));
	return 0;
}

static void davinci_hw_common_param(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct davinci_audio_dev *dev = rtd->dai->cpu_dai->private_data;
	u8 cnt = 0;
	int i;

	/* Default configuration */
	mcasp_set_bits(dev->base + DAVINCI_MCASP_PWREMUMGT_REG, SOFT);

	/* All PINS as McASP */
	mcasp_set_reg(dev->base + DAVINCI_MCASP_PFUNC_REG, 0x00000000);

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		mcasp_set_reg(dev->base + DAVINCI_MCASP_TXSTAT_REG, 0xFFFFFFFF);
		mcasp_clr_bits(dev->base + DAVINCI_MCASP_XEVTCTL_REG,
				TXDATADMADIS);
	} else {
		mcasp_set_reg(dev->base + DAVINCI_MCASP_RXSTAT_REG, 0xFFFFFFFF);
		mcasp_clr_bits(dev->base + DAVINCI_MCASP_REVTCTL_REG,
				RXDATADMADIS);
	}

	for (i = 0; i < dev->num_serializer; i++) {
		mcasp_set_bits(dev->base + DAVINCI_MCASP_XRSRCTL_REG(i),
			       dev->serial_dir[i]);
		if (dev->serial_dir[i] == TX_MODE)
			mcasp_set_bits(dev->base + DAVINCI_MCASP_PDIR_REG,
				       AXR(i));
		else if (dev->serial_dir[i] == RX_MODE)
			mcasp_clr_bits(dev->base + DAVINCI_MCASP_PDIR_REG,
				       AXR(i));
	}

	if (dev->txnumevt && substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		for (i = 0; i < dev->num_serializer; i++) {
			if (dev->serial_dir[i] == TX_MODE)
				cnt++;;
		}

		mcasp_mod_bits(dev->base + DAVINCI_MCASP_WFIFOCTL, cnt,
								NUMDMA_MASK);
		mcasp_mod_bits(dev->base + DAVINCI_MCASP_WFIFOCTL,
					(dev->txnumevt << 8), NUMEVT_MASK);
		mcasp_set_bits(dev->base + DAVINCI_MCASP_WFIFOCTL, FIFO_ENABLE);
	}

	cnt = 0;
	if (dev->rxnumevt && substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		for (i = 0; i < dev->num_serializer; i++) {
			if (dev->serial_dir[i] == RX_MODE)
				cnt++;;
		}

		mcasp_mod_bits(dev->base + DAVINCI_MCASP_RFIFOCTL, cnt,
								NUMDMA_MASK);
		mcasp_mod_bits(dev->base + DAVINCI_MCASP_RFIFOCTL,
					(dev->rxnumevt << 8), NUMEVT_MASK);
		mcasp_set_bits(dev->base + DAVINCI_MCASP_WFIFOCTL, FIFO_ENABLE);
	}
}

static void davinci_hw_iis_param(struct snd_pcm_substream *substream)
{
	int i, active_slots;
	u32 mask = 0;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct davinci_audio_dev *dev = rtd->dai->cpu_dai->private_data;

	active_slots = (dev->tdm_slots > 31) ? 32 : dev->tdm_slots;
	for (i = 0; i < active_slots; i++)
		mask |= (1 << i);

	mcasp_clr_bits(dev->base + DAVINCI_MCASP_ACLKXCTL_REG, TX_ASYNC);

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		/* bit stream is MSB first */
		mcasp_set_bits(dev->base + DAVINCI_MCASP_AHCLKXCTL_REG,
				AHCLKXE);
		mcasp_set_reg(dev->base + DAVINCI_MCASP_TXTDM_REG, mask);
		mcasp_set_bits(dev->base + DAVINCI_MCASP_TXFMT_REG, TXORD);

		if ((dev->tdm_slots >= 2) || (dev->tdm_slots <= 32))
			mcasp_mod_bits(dev->base + DAVINCI_MCASP_TXFMCTL_REG,
				       FSXMOD(dev->tdm_slots), FSXMOD(0x1FF));
		else
			printk(KERN_ERR "playback tdm slot %d not supported\n",
				dev->tdm_slots);

		mcasp_set_reg(dev->base + DAVINCI_MCASP_TXMASK_REG, 0xFFFFFFFF);
		mcasp_clr_bits(dev->base + DAVINCI_MCASP_TXFMCTL_REG, FSXDUR);

	} else {
		/* bit stream is MSB first with no delay */
		mcasp_set_bits(dev->base + DAVINCI_MCASP_RXFMT_REG,
				FSRDLY(0) | RXORD);
		mcasp_set_bits(dev->base + DAVINCI_MCASP_AHCLKRCTL_REG,
				AHCLKRE);
		mcasp_set_reg(dev->base + DAVINCI_MCASP_RXTDM_REG, mask);

		if ((dev->tdm_slots >= 2) || (dev->tdm_slots <= 32))
			mcasp_mod_bits(dev->base + DAVINCI_MCASP_RXFMCTL_REG,
				       FSRMOD(dev->tdm_slots), FSRMOD(0x1FF));
		else
			printk(KERN_ERR "capture tdm slot %d not supported\n",
				dev->tdm_slots);

		mcasp_set_reg(dev->base + DAVINCI_MCASP_RXMASK_REG, 0xFFFFFFFF);
		mcasp_clr_bits(dev->base + DAVINCI_MCASP_RXFMCTL_REG, FSRDUR);
	}
}

/* S/PDIF */
static void davinci_hw_dit_param(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct davinci_audio_dev *dev = rtd->dai->cpu_dai->private_data;

	/* Set the PDIR for Serialiser as output */
	mcasp_set_bits(dev->base + DAVINCI_MCASP_PDIR_REG, AFSX);

	/* TXMASK for 24 bits */
	mcasp_set_reg(dev->base + DAVINCI_MCASP_TXMASK_REG, 0x00FFFFFF);

	/* Set the TX format : 24 bit right rotation, 32 bit slot, Pad 0
	   and LSB first */
	mcasp_set_bits(dev->base + DAVINCI_MCASP_TXFMT_REG,
		       TXROT(6) | TXSSZ(15));

	/* Set TX frame synch : DIT Mode, 1 bit width, internal, rising edge */
	mcasp_set_reg(dev->base + DAVINCI_MCASP_TXFMCTL_REG,
		      AFSXE | FSXMOD(0x180));

	/* Set the TX tdm : for all the slots */
	mcasp_set_reg(dev->base + DAVINCI_MCASP_TXTDM_REG, 0xFFFFFFFF);

	/* Set the TX clock controls : div = 1 and internal */
	mcasp_set_bits(dev->base + DAVINCI_MCASP_ACLKXCTL_REG,
		       ACLKXE | TX_ASYNC);

	mcasp_clr_bits(dev->base + DAVINCI_MCASP_XEVTCTL_REG, TXDATADMADIS);

	/* Only 44100 and 48000 are valid, both have the same setting */
	mcasp_set_bits(dev->base + DAVINCI_MCASP_AHCLKXCTL_REG,
		       AHCLKXDIV(3));

	/* Enable the DIT */
	mcasp_set_bits(dev->base + DAVINCI_MCASP_TXDITCTL_REG, DITEN);
}

static int davinci_i2s_mcasp_hw_params(
					struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params,
					struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct davinci_audio_dev *dev = rtd->dai->cpu_dai->private_data;
	struct davinci_pcm_dma_params *dma_params =
					dev->dma_params[substream->stream];
	int word_length;
	u8 numevt;

	davinci_hw_common_param(substream);

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		numevt = dev->txnumevt;
	else
		numevt = dev->rxnumevt;

	if (!numevt)
		numevt = 1;

	if (dev->op_mode == DAVINCI_MCASP_DIT_MODE)
		davinci_hw_dit_param(substream);
	else
		davinci_hw_iis_param(substream);

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S8:
		dma_params->data_type = 1 * numevt;
		word_length = DAVINCI_AUDIO_WORD_8;
		break;
	case SNDRV_PCM_FORMAT_S16_LE:
		dma_params->data_type = 2 * numevt;
		word_length = DAVINCI_AUDIO_WORD_16;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		dma_params->data_type = 4 * numevt;
		word_length = DAVINCI_AUDIO_WORD_32;
		break;
	default:
		printk(KERN_WARNING "davinci-i2s: unsupported PCM format");
		return -EINVAL;
	}

	if (dev->version == MCASP_VERSION_2)
		dma_params->acnt = 4 * numevt;
	else
		dma_params->acnt = dma_params->data_type;

	davinci_config_channel_size(substream, word_length);

	return 0;
}

static int davinci_i2s_mcasp_trigger(struct snd_pcm_substream *substream,
				     int cmd, struct snd_soc_dai *dai)
{
	int ret = 0;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		davinci_mcasp_start(substream);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		davinci_mcasp_stop(substream);
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

struct snd_soc_dai davinci_iis_mcasp_dai[] = {
	{
		.name = "davinci-i2s",
		.id = 0,
		.probe = davinci_i2s_mcasp_probe,
		.remove = davinci_i2s_mcasp_remove,
		.playback = {
			.channels_min = 1,
			.channels_max = 384,
			.rates = DAVINCI_I2S_RATES,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
		},
		.capture = {
			.channels_min = 1,
			.channels_max = 384,
			.rates = DAVINCI_I2S_RATES,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
		},
		.ops = {
			.startup = davinci_i2s_startup,
			.trigger = davinci_i2s_mcasp_trigger,
			.hw_params = davinci_i2s_mcasp_hw_params,
			.set_fmt = davinci_i2s_mcasp_set_dai_fmt,
		},
	},
	{
		.name = "davinci-dit",
		.id = 1,
		.playback = {
			.channels_min = 1,
			.channels_max = 384,
			.rates = DAVINCI_I2S_RATES,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
		},
		.ops = {
			.startup = davinci_i2s_startup,
			.trigger = davinci_i2s_mcasp_trigger,
			.hw_params = davinci_i2s_mcasp_hw_params,
			.set_fmt = davinci_i2s_mcasp_set_dai_fmt,
		},
	},

};
EXPORT_SYMBOL_GPL(davinci_iis_mcasp_dai);

static int __init davinci_i2s_mcasp_init(void)
{
	return snd_soc_register_dais(davinci_iis_mcasp_dai,
				ARRAY_SIZE(davinci_iis_mcasp_dai));

}
module_init(davinci_i2s_mcasp_init);

static void __exit davinci_i2s_mcasp_exit(void)
{
	return snd_soc_unregister_dais(davinci_iis_mcasp_dai,
					ARRAY_SIZE(davinci_iis_mcasp_dai));

}
module_exit(davinci_i2s_mcasp_exit);



MODULE_AUTHOR("Steve Chen");
MODULE_DESCRIPTION("TI DAVINCI I2S (McASP) SoC Interface");
MODULE_LICENSE("GPL");

