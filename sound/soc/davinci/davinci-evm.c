/*
 * ASoC driver for TI DAVINCI EVM platform
 *
 * Author:      Vladimir Barinov, <vbarinov@embeddedalley.com>
 * Copyright:   (C) 2007 MontaVista Software, Inc., <source@mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <asm/dma.h>
#include <asm/mach-types.h>

#include <mach/asp.h>
#include <mach/edma.h>
#include <mach/mux.h>
#include <mach/cpu.h>


#include "../codecs/tlv320aic3x.h"
#include "../codecs/codec_stubs.h"
#include "davinci-pcm.h"
#include "davinci-i2s.h"
#include "davinci-i2s-mcasp.h"

#define DA830_MCASP1_CNTRL_BASE 	0x01D04000
#define DA830_MCASP1_FIFO_CNTRL_BASE	0x01D05000
#define DA830_MCASP1_DATAL_BASE 	0x01D06000

#define DA850_MCASP_CNTRL_BASE		0x01D00000
#define DA850_MCASP_FIFO_CNTRL_BASE	0x01D01000
#define DA850_MCASP_DATAL_BASE		0x01D02000

#define AUDIO_FORMAT (SND_SOC_DAIFMT_DSP_B | \
		SND_SOC_DAIFMT_CBM_CFM | SND_SOC_DAIFMT_IB_NF)
static int evm_hw_params(struct snd_pcm_substream *substream,
			 struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	int ret = 0;
	unsigned sysclk;

	/* ASP1 on DM355 EVM is clocked by an external oscillator */
	if (cpu_is_davinci_dm355() || cpu_is_davinci_dm646x())
		sysclk = 27000000;

	/* ASP0 in DM6446 EVM is clocked by U55, as configured by
	 * board-dm644x-evm.c using GPIOs from U18.  There are six
	 * options; here we "know" we use a 48 KHz sample rate.
	 */
	else if (cpu_is_davinci_dm644x())
		sysclk = 12288000;

	else if (cpu_is_da830() || cpu_is_da850())

		sysclk = 24576000;
	else
		return -EINVAL;

	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, AUDIO_FORMAT);
	if (ret < 0)
		return ret;

	/* set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, AUDIO_FORMAT);
	if (ret < 0)
		return ret;

	/* set the codec system clock */
	ret = snd_soc_dai_set_sysclk(codec_dai, 0, sysclk, SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;

	return 0;
}

static struct snd_soc_ops evm_ops = {
	.hw_params = evm_hw_params,
};

/* davinci-evm machine dapm widgets */
static const struct snd_soc_dapm_widget aic3x_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_LINE("Line Out", NULL),
	SND_SOC_DAPM_MIC("Mic Jack", NULL),
	SND_SOC_DAPM_LINE("Line In", NULL),
};

/* davinci-evm machine audio_mapnections to the codec pins */
static const struct snd_soc_dapm_route audio_map[] = {
	/* Headphone connected to HPLOUT, HPROUT */
	{"Headphone Jack", NULL, "HPLOUT"},
	{"Headphone Jack", NULL, "HPROUT"},

	/* Line Out connected to LLOUT, RLOUT */
	{"Line Out", NULL, "LLOUT"},
	{"Line Out", NULL, "RLOUT"},

	/* Mic connected to (MIC3L | MIC3R) */
	{"MIC3L", NULL, "Mic Bias 2V"},
	{"MIC3R", NULL, "Mic Bias 2V"},
	{"Mic Bias 2V", NULL, "Mic Jack"},

	/* Line In connected to (LINE1L | LINE2L), (LINE1R | LINE2R) */
	{"LINE1L", NULL, "Line In"},
	{"LINE2L", NULL, "Line In"},
	{"LINE1R", NULL, "Line In"},
	{"LINE2R", NULL, "Line In"},
};

/* Logic for a aic3x as connected on a davinci-evm */
static int evm_aic3x_init(struct snd_soc_codec *codec)
{
	/* Add davinci-evm specific widgets */
	snd_soc_dapm_new_controls(codec, aic3x_dapm_widgets,
				  ARRAY_SIZE(aic3x_dapm_widgets));

	/* Set up davinci-evm specific audio path audio_map */
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

	/* not connected */
	snd_soc_dapm_disable_pin(codec, "MONO_LOUT");
	snd_soc_dapm_disable_pin(codec, "HPLCOM");
	snd_soc_dapm_disable_pin(codec, "HPRCOM");

	/* always connected */
	snd_soc_dapm_enable_pin(codec, "Headphone Jack");
	snd_soc_dapm_enable_pin(codec, "Line Out");
	snd_soc_dapm_enable_pin(codec, "Mic Jack");
	snd_soc_dapm_enable_pin(codec, "Line In");

	snd_soc_dapm_sync(codec);

	return 0;
}

/* davinci-evm digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link evm_dai = {
	.name = "TLV320AIC3X",
	.stream_name = "AIC3X",
	.cpu_dai = &davinci_i2s_dai,
	.codec_dai = &aic3x_dai,
	.init = evm_aic3x_init,
	.ops = &evm_ops,
};

static struct snd_soc_dai_link da850_evm_dai[] = {
	{
		.name		= "TLV320AIC3X",
		.stream_name	= "AIC3X",
		.cpu_dai	= davinci_iis_mcasp_dai,
		.codec_dai	= &aic3x_dai,
		.init		= evm_aic3x_init,
		.ops		= &evm_ops,
	},
};

static struct snd_soc_dai_link da830_evm_dai[] = {
	{
		.name		= "TLV320AIC3X",
		.stream_name	= "AIC3X",
		.cpu_dai	= davinci_iis_mcasp_dai,
		.codec_dai	= &aic3x_dai,
		.init		= evm_aic3x_init,
		.ops		= &evm_ops,
	},
};

static struct snd_soc_dai_link dm6467_evm_dai[] = {
	{
		.name = "TLV320AIC3X",
		.stream_name = "AIC3X",
		.cpu_dai = davinci_iis_mcasp_dai,
		.codec_dai = &aic3x_dai,
		.init = evm_aic3x_init,
		.ops = &evm_ops,
	},
	{
		.name = "MCASP SPDIF",
		.stream_name = "spdif",
		.cpu_dai = &davinci_iis_mcasp_dai[1],
		.codec_dai = dit_stub_dai,
		.ops = &evm_ops,
	},
};


/* davinci-evm audio machine driver */
static struct snd_soc_card snd_soc_card_evm = {
	.name = "DaVinci EVM",
	.platform = &davinci_soc_platform,
	.dai_link = &evm_dai,
	.num_links = 1,
};

/* davinci dm6467 evm audio machine driver */
static struct snd_soc_card dm6467_snd_soc_card_evm = {
		.name = "DaVinci DM6467 EVM",
		.platform = &davinci_soc_platform,
		.dai_link = dm6467_evm_dai,
		.num_links = 2,
};

/* evm audio private data */
static struct aic3x_setup_data evm_aic3x_setup = {
	.i2c_bus = 1,
	.i2c_address = 0x1b,
};

/* dm6467 evm audio private data */
static struct aic3x_setup_data dm6467_evm_aic3x_setup = {
       .i2c_bus = 1,
       .i2c_address = 0x18,
};


/* evm audio subsystem */
static struct snd_soc_device evm_snd_devdata = {
	.card = &snd_soc_card_evm,
	.codec_dev = &soc_codec_dev_aic3x,
	.codec_data = &evm_aic3x_setup,
};

/* evm audio subsystem */
static struct snd_soc_device dm6467_evm_snd_devdata = {
		.card = &dm6467_snd_soc_card_evm,
       .codec_dev = &soc_codec_dev_aic3x,
       .codec_data = &dm6467_evm_aic3x_setup,
};


/* DM6446 EVM uses ASP0; line-out is a pair of RCA jacks */
static struct resource evm_snd_resources[] = {
	{
		.start = DAVINCI_ASP0_BASE,
		.end = DAVINCI_ASP0_BASE + SZ_8K - 1,
		.flags = IORESOURCE_MEM,
	},
};

static struct resource dm6467_evm_snd_resources[] = {
	{
		.start = DAVINCI_DM646X_MCASP0_REG_BASE,
		.end = DAVINCI_DM646X_MCASP0_REG_BASE + (SZ_1K << 1) - 1,
		.flags = IORESOURCE_MEM,
	},
	{
		.start = DAVINCI_DM646X_MCASP1_REG_BASE,
		.end = DAVINCI_DM646X_MCASP1_REG_BASE + (SZ_1K << 1) - 1,
		.flags = IORESOURCE_MEM,
	},
};

static u8 dm6467_iis_serializer_direction[] = {
       TX_MODE, RX_MODE, INACTIVE_MODE, INACTIVE_MODE,
};

static u8 dm6467_dit_serializer_direction[] = {
       TX_MODE, INACTIVE_MODE, INACTIVE_MODE, INACTIVE_MODE,
};

static struct evm_snd_platform_data evm_snd_data = {
	.tx_dma_ch	= EDMA_CTLR_CHAN(0, DAVINCI_DMA_ASP0_TX),
	.rx_dma_ch	= EDMA_CTLR_CHAN(0, DAVINCI_DMA_ASP0_RX),
};

static struct evm_snd_platform_data dm6467_evm_snd_data[] = {
	{
		.clk_name       = "McASPCLK0",
		.tx_dma_ch      = DAVINCI_DM646X_DMA_MCASP0_AXEVT0,
		.rx_dma_ch      = DAVINCI_DM646X_DMA_MCASP0_AREVT0,
		.tx_dma_offset  = 0x400,
		.rx_dma_offset  = 0x400,
		.op_mode        = DAVINCI_MCASP_IIS_MODE,
		.num_serializer = 4,
		.tdm_slots      = 2,
		.serial_dir     = dm6467_iis_serializer_direction,
		.eventq_no      = EVENTQ_0,
		.codec_fmt      = SND_SOC_DAIFMT_CBM_CFM | SND_SOC_DAIFMT_IB_NF,
		.cc_inst		= 0,
	},
	{
		.clk_name       = "McASPCLK1",
		.tx_dma_ch      = DAVINCI_DM646X_DMA_MCASP1_AXEVT1,
		.rx_dma_ch      = -1,
		.tx_dma_offset  = 0x400,
		.rx_dma_offset  = 0,
		.op_mode        = DAVINCI_MCASP_DIT_MODE,
		.num_serializer = 4,
		.tdm_slots      = 32,
		.serial_dir     = dm6467_dit_serializer_direction,
		.eventq_no      = EVENTQ_0,
		.codec_fmt      = SND_SOC_DAIFMT_CBS_CFS | SND_SOC_DAIFMT_IB_NF,
		.cc_inst		= 0,
	},
};

/* DM335 EVM uses ASP1; line-out is a stereo mini-jack */
static struct resource dm335evm_snd_resources[] = {
	{
		.start = DAVINCI_ASP1_BASE,
		.end = DAVINCI_ASP1_BASE + SZ_8K - 1,
		.flags = IORESOURCE_MEM,
	},
};

static struct evm_snd_platform_data dm335evm_snd_data = {
	.tx_dma_ch	= EDMA_CTLR_CHAN(0, DAVINCI_DMA_ASP1_TX),
	.rx_dma_ch	= EDMA_CTLR_CHAN(0, DAVINCI_DMA_ASP1_RX),
};

/* davinci da830 evm audio machine driver */
static u8 da830_iis_serializer_direction[] = {
	RX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	TX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
};

static struct snd_soc_card da830_snd_soc_card = {
	.name		= "DA830 EVM",
	.dai_link	= da830_evm_dai,
	.platform	= &davinci_soc_platform,
	.num_links	= 1,
};

static struct aic3x_setup_data da830_evm_aic3x_setup = {
	.i2c_bus	= 1,
	.i2c_address	= 0x18,
	.variant	= AIC3106_CODEC,
};

/* evm audio subsystem */
static struct snd_soc_device da830_evm_snd_devdata = {
	.card		= &da830_snd_soc_card,
	.codec_dev	= &soc_codec_dev_aic3x,
	.codec_data	= &da830_evm_aic3x_setup,
};

static struct resource da830_evm_aic_snd_resources[] = {
	{
		.start	= DA830_MCASP1_CNTRL_BASE,
		.end	= DA830_MCASP1_CNTRL_BASE + (SZ_1K << 1) - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct evm_snd_platform_data da830_evm_snd_data[] = {
	{
		.clk_name	= "mcasp1",
		.tx_dma_ch	= 3,
		.rx_dma_ch	= 2,
		.tx_dma_offset	= 0x2000,
		.rx_dma_offset	= 0x2000,
		.op_mode	= DAVINCI_MCASP_IIS_MODE,
		.num_serializer	= 12,
		.tdm_slots	= 2,
		.serial_dir	= da830_iis_serializer_direction,
		.eventq_no	= EVENTQ_1,
		.codec_fmt	= SND_SOC_DAIFMT_CBM_CFM | SND_SOC_DAIFMT_IB_NF,
		.cc_inst	= 0,
		.version	= MCASP_VERSION_2,
		.txnumevt	= 8,
		.rxnumevt	= 0,
	},
};

/* davinci da850 evm audio machine driver */
static u8 da850_iis_serializer_direction[] = {
	TX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	RX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
};

static struct snd_soc_card da850_snd_soc_card = {
	.name		= "DA850 EVM",
	.dai_link	= da850_evm_dai,
	.platform	= &davinci_soc_platform,
	.num_links	= 1,
};

static struct aic3x_setup_data da850_evm_aic3x_setup = {
	.i2c_bus	= 1,
	.i2c_address	= 0x18,
	.variant	= AIC3106_CODEC,
};

/* evm audio subsystem */
static struct snd_soc_device da850_evm_snd_devdata = {
	.card		= &da850_snd_soc_card,
	.codec_dev	= &soc_codec_dev_aic3x,
	.codec_data	= &da850_evm_aic3x_setup,
};

static struct resource da850_evm_aic_snd_resources[] = {
	{
		.start	= DA850_MCASP_CNTRL_BASE,
		.end	= DA850_MCASP_CNTRL_BASE + (SZ_1K << 1) - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct evm_snd_platform_data da850_evm_snd_data[] = {
	{
		.clk_name	= "mcasp",
		.tx_dma_ch	= 1,
		.rx_dma_ch	= 0,
		.tx_dma_offset	= 0x2000,
		.rx_dma_offset	= 0x2000,
		.op_mode	= DAVINCI_MCASP_IIS_MODE,
		.num_serializer	= 12,
		.tdm_slots	= 2,
		.serial_dir	= da850_iis_serializer_direction,
		.eventq_no	= EVENTQ_1,
		.codec_fmt	= SND_SOC_DAIFMT_CBM_CFM | SND_SOC_DAIFMT_IB_NF,
		.cc_inst	= 0,
		.txnumevt	= 0,
		.rxnumevt	= 0,
		.version	= MCASP_VERSION_2,
	},
};

static struct platform_device *evm_snd_device;

static int __init evm_init(void)
{
	struct resource *resources;
	struct evm_snd_platform_data *data;
	struct snd_soc_device *evm_snd_dev_data;
	int index;
	int ret;
	int res_size;

	if (cpu_is_davinci_dm644x()) {
		davinci_cfg_reg(DM644X_MCBSP);
		evm_snd_dev_data = &evm_snd_devdata;
		resources = evm_snd_resources;
		data = &evm_snd_data;
		res_size = ARRAY_SIZE(evm_snd_resources);
		index = 0;
	} else if (cpu_is_davinci_dm355()) {
		/* we don't use ASP1 IRQs, or we'd need to mux them ... */
		davinci_cfg_reg(DM355_EVT8_ASP1_TX);
		davinci_cfg_reg(DM355_EVT9_ASP1_RX);
		evm_snd_dev_data = &evm_snd_devdata;
		resources = dm335evm_snd_resources;
		res_size = ARRAY_SIZE(dm335evm_snd_resources);
		data = &dm335evm_snd_data;
		index = 1;
	} else if (cpu_is_davinci_dm646x()) {
		evm_snd_dev_data = &dm6467_evm_snd_devdata;
		resources = dm6467_evm_snd_resources;
		res_size = ARRAY_SIZE(dm6467_evm_snd_resources);
		data = dm6467_evm_snd_data;
		index = 0;
	} else if (cpu_is_da830()) {
		evm_snd_dev_data = &da830_evm_snd_devdata;
		resources = da830_evm_aic_snd_resources;
		res_size = ARRAY_SIZE(da830_evm_aic_snd_resources);
		data = da830_evm_snd_data;
		index = -1;
	} else if (cpu_is_da850()) {
		evm_snd_dev_data = &da850_evm_snd_devdata;
		resources = da850_evm_aic_snd_resources;
		res_size = ARRAY_SIZE(da850_evm_aic_snd_resources);
		data = da850_evm_snd_data;
		index = -1;
	} else
		return -EINVAL;

	evm_snd_device = platform_device_alloc("soc-audio", index);
	if (!evm_snd_device)
		return -ENOMEM;

	platform_device_add_data(evm_snd_device, data, sizeof(*data));
	platform_set_drvdata(evm_snd_device, evm_snd_dev_data);
	evm_snd_dev_data->dev = &evm_snd_device->dev;

	ret = platform_device_add_resources(evm_snd_device,
						resources, res_size);
	if (ret) {
		platform_device_put(evm_snd_device);
		return ret;
	}

	ret = platform_device_add(evm_snd_device);
	if (ret)
		platform_device_put(evm_snd_device);

	return ret;
}

static void __exit evm_exit(void)
{
	platform_device_unregister(evm_snd_device);
}

module_init(evm_init);
module_exit(evm_exit);

MODULE_AUTHOR("Vladimir Barinov");
MODULE_DESCRIPTION("TI DAVINCI EVM ASoC driver");
MODULE_LICENSE("GPL");
