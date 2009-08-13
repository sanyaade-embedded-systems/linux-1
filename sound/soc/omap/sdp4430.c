/*
 * sdp4430.c  --  SoC audio for TI OMAP4430 SDP
 *
 * Author: Misael Lopez Cruz <x0052729@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <linux/clk.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <mach/mux.h>

#include "mcpdm.h"
#include "omap-abe.h"
#include "omap-pcm.h"
#include "../codecs/twl6030.h"
#include "../codecs/abe-twl6030.h"

#define TWL6030_AUDPWRON_GPIO	127

static struct snd_soc_card snd_soc_sdp4430;

static int sdp4430_hw_params(struct snd_pcm_substream *substream,
			struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	int rate, clk_id, pll_id, freq_in, freq_out;
	int ret;

	rate = params_rate(params);
	switch (rate) {
	case 44100:
		clk_id = TWL6030_SYSCLK_SEL_LPPLL;
		pll_id = TWL6030_LPPLL_ID;
		freq_in = 32768;
		freq_out = 17640000;
		break;
	case 48000:
		clk_id = TWL6030_SYSCLK_SEL_HPPLL;
		pll_id = TWL6030_HPPLL_ID;
		freq_in = 12000000;
		freq_out = 19200000;
		break;
	default:
		printk(KERN_ERR "unknown rate %d\n", rate);
		return -EINVAL;
	}

	/* Set the codec pll divider */
	ret = snd_soc_dai_set_pll(codec_dai, pll_id, freq_in, freq_out);
	if (ret < 0) {
		printk(KERN_ERR "can't set codec pll frequency\n");
		return ret;
	}

	/* Set the codec system clock for DAC and ADC */
	ret = snd_soc_dai_set_sysclk(codec_dai, clk_id, freq_in, SND_SOC_CLOCK_IN);
	if (ret < 0) {
		printk(KERN_ERR "can't set codec system clock\n");
		return ret;
	}

	return 0;
}

static struct snd_soc_ops sdp4430_ops = {
	.hw_params = sdp4430_hw_params,
};

/* SDP4430 machine DAPM */
static const struct snd_soc_dapm_widget sdp4430_twl6030_dapm_widgets[] = {
	SND_SOC_DAPM_MIC("Ext Mic", NULL),
	SND_SOC_DAPM_SPK("Ext Spk", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
	SND_SOC_DAPM_HP("Headset Stereophone", NULL),
};

static const struct snd_soc_dapm_route audio_map[] = {
	/* External Mics: MAINMIC, SUBMIC with bias*/
	{"MAINMIC", NULL, "Main Mic Bias"},
	{"SUBMIC", NULL, "Main Mic Bias"},
	{"Main Mic Bias", NULL, "Ext Mic"},

	/* External Speakers: HFL, HFR */
	{"Ext Spk", NULL, "HFL"},
	{"Ext Spk", NULL, "HFR"},

	/* Headset Mic: HSMIC with bias */
	{"HSMIC", NULL, "Headset Mic Bias"},
	{"Headset Mic Bias", NULL, "Headset Mic"},

	/* Headset Stereophone (Headphone): HSOL, HSOR */
	{"Headset Stereophone", NULL, "HSOL"},
	{"Headset Stereophone", NULL, "HSOR"},
};

static int sdp4430_twl6030_init(struct snd_soc_codec *codec)
{
	int ret;

	/* Add SDP4430 specific widgets */
	ret = snd_soc_dapm_new_controls(codec, sdp4430_twl6030_dapm_widgets,
				ARRAY_SIZE(sdp4430_twl6030_dapm_widgets));
	if (ret)
		return ret;

	/* Set up SDP4430 specific audio path audio_map */
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

	/* SDP4430 connected pins */
	snd_soc_dapm_enable_pin(codec, "Ext Mic");
	snd_soc_dapm_enable_pin(codec, "Ext Spk");
	snd_soc_dapm_enable_pin(codec, "Headset Mic");
	snd_soc_dapm_enable_pin(codec, "Headset Stereophone");

	/* TWL6030 not connected pins */
	snd_soc_dapm_nc_pin(codec, "AFML");
	snd_soc_dapm_nc_pin(codec, "AFMR");

	ret = snd_soc_dapm_sync(codec);

	return ret;
}

/* Digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link sdp4430_dai[] = {
	{
		.name = "abe-twl6030",
		.stream_name = "Multimedia",
		.cpu_dai = &omap_abe_dai[OMAP_ABE_MM_DAI],
		.codec_dai = &abe_dai[0],
		.init = sdp4430_twl6030_init,
		.ops = &sdp4430_ops,
	},
	{
		.name = "abe-twl6030",
		.stream_name = "Multimedia Extended",
		.cpu_dai = &omap_abe_dai[OMAP_ABE_MM_EXT_DAI],
		.codec_dai = &abe_dai[1],
		.ops = &sdp4430_ops,
	},
	{
		.name = "abe-twl6030",
		.stream_name = "Voice",
		.cpu_dai = &omap_abe_dai[OMAP_ABE_VOICE_DAI],
		.codec_dai = &abe_dai[2],
		.ops = &sdp4430_ops,
	},
	{
		.name = "abe-twl6030",
		.stream_name = "Digital Uplink",
		.cpu_dai = &omap_abe_dai[OMAP_ABE_DIG_UPLINK_DAI],
		.codec_dai = &abe_dai[3],
	},
	{
		.name = "abe-twl6030",
		.stream_name = "Vibrator",
		.cpu_dai = &omap_abe_dai[OMAP_ABE_VIB_DAI],
		.codec_dai = &abe_dai[4],
	},
};

static void twl6030_codec_enable(int enable)
{
	gpio_set_value(TWL6030_AUDPWRON_GPIO, enable);
}

static struct twl6030_setup_data twl6030_setup = {
	.codec_enable = twl6030_codec_enable,
};

/* Audio machine driver */
static struct snd_soc_card snd_soc_sdp4430 = {
	.name = "SDP4430",
	.platform = &omap_soc_platform,
	.dai_link = sdp4430_dai,
	.num_links = ARRAY_SIZE(sdp4430_dai),
};

/* Audio subsystem */
static struct snd_soc_device sdp4430_snd_devdata = {
	.card = &snd_soc_sdp4430,
	.codec_dev = &soc_codec_dev_abe_twl6030,
	.codec_data = &twl6030_setup,
};

static struct platform_device *sdp4430_snd_device;

static int __init sdp4430_soc_init(void)
{
	int ret;

	if (!machine_is_omap_4430sdp()) {
		pr_debug("Not SDP4430!\n");
		return -ENODEV;
	}
	printk(KERN_INFO "SDP4430 SoC init\n");

	sdp4430_snd_device = platform_device_alloc("soc-audio", -1);
	if (!sdp4430_snd_device) {
		printk(KERN_ERR "Platform device allocation failed\n");
		return -ENOMEM;
	}

	platform_set_drvdata(sdp4430_snd_device, &sdp4430_snd_devdata);
	sdp4430_snd_devdata.dev = &sdp4430_snd_device->dev;

	/* twl6030 audio power-on gpio */
	ret = gpio_request(TWL6030_AUDPWRON_GPIO, "audpwron");
	if (ret)
		goto err1;

	gpio_direction_output(TWL6030_AUDPWRON_GPIO, 0);

	ret = platform_device_add(sdp4430_snd_device);
	if (ret)
		goto err2;

	return 0;

err2:
	gpio_free(TWL6030_AUDPWRON_GPIO);
err1:
	printk(KERN_ERR "Unable to add platform device\n");
	platform_device_put(sdp4430_snd_device);

	return ret;
}
module_init(sdp4430_soc_init);

static void __exit sdp4430_soc_exit(void)
{
	gpio_free(TWL6030_AUDPWRON_GPIO);
	platform_device_unregister(sdp4430_snd_device);
}
module_exit(sdp4430_soc_exit);

MODULE_AUTHOR("Misael Lopez Cruz <x0052729@ti.com>");
MODULE_DESCRIPTION("ALSA SoC SDP4430");
MODULE_LICENSE("GPL");

