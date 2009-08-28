/*
 * ALSA SoC ABE-TWL6030 codec driver
 *
 * Author:      Misael Lopez Cruz <x0052729@ti.com>
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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/i2c/twl.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "twl6030.h"
#include "abe-twl6030.h"
#include "abe/abe_main.h"

#define ABE_FORMATS	 (SNDRV_PCM_FMTBIT_S32_LE)

/* codec private data */
struct twl6030_priv_data {
	int codec_powered;
	unsigned int sysclk;
};

/*
 * twl6030 register cache & default register settings
 */
static const u8 twl6030_reg[TWL6030_CACHEREGNUM] = {
	0x00, /* not used		0x00	*/
	0x4B, /* TWL6030_ASICID (ro)	0x01	*/
	0x00, /* TWL6030_ASICREV (ro)	0x02	*/
	0x00, /* TWL6030_INTID		0x03	*/
	0x00, /* TWL6030_INTMR		0x04	*/
	0x00, /* TWL6030_NCPCTRL	0x05	*/
	0x00, /* TWL6030_LDOCTL		0x06	*/
	0x00, /* TWL6030_HPPLLCTL	0x07	*/
	0x00, /* TWL6030_LPPLLCTL	0x08	*/
	0x00, /* TWL6030_LPPLLDIV	0x09	*/
	0x00, /* TWL6030_AMICBCTL	0x0A	*/
	0x00, /* TWL6030_DMICBCTL	0x0B	*/
	0x00, /* TWL6030_MICLCTL	0x0C	*/
	0x00, /* TWL6030_MICRCTL	0x0D	*/
	0x00, /* TWL6030_MICGAIN	0x0E	*/
	0x1B, /* TWL6030_LINEGAIN	0x0F	*/
	0x00, /* TWL6030_HSLCTL		0x10	*/
	0x00, /* TWL6030_HSRCTL		0x11	*/
	0x00, /* TWL6030_HSGAIN		0x12	*/
	0x06, /* TWL6030_EARCTL		0x13	*/
	0x00, /* TWL6030_HFLCTL		0x14	*/
	0x03, /* TWL6030_HFLGAIN	0x15	*/
	0x00, /* TWL6030_HFRCTL		0x16	*/
	0x03, /* TWL6030_HFRGAIN	0x17	*/
	0x00, /* TWL6030_VIBCTLL	0x18	*/
	0x00, /* TWL6030_VIBDATL	0x19	*/
	0x00, /* TWL6030_VIBCTLR	0x1A	*/
	0x00, /* TWL6030_VIBDATR	0x1B	*/
	0x00, /* TWL6030_HKCTL1		0x1C	*/
	0x00, /* TWL6030_HKCTL2		0x1D	*/
	0x00, /* TWL6030_GPOCTL		0x1E	*/
	0x00, /* TWL6030_ALB		0x1F	*/
	0x00, /* TWL6030_DLB		0x20	*/
	0x00, /* not used		0x21	*/
	0x00, /* not used		0x22	*/
	0x00, /* not used		0x23	*/
	0x00, /* not used		0x24	*/
	0x00, /* not used		0x25	*/
	0x00, /* not used		0x26	*/
	0x00, /* not used		0x27	*/
	0x00, /* TWL6030_TRIM1		0x28	*/
	0x00, /* TWL6030_TRIM2		0x29	*/
	0x00, /* TWL6030_TRIM3		0x2A	*/
	0x00, /* TWL6030_HSOTRIM	0x2B	*/
	0x00, /* TWL6030_HFOTRIM	0x2C	*/
	0x09, /* TWL6030_ACCCTL		0x2D	*/
	0x00, /* TWL6030_STATUS (ro)	0x2E	*/
};

/*
 * read twl6030 register cache
 */
static inline unsigned int twl6030_read_reg_cache(struct snd_soc_codec *codec,
						unsigned int reg)
{
	u8 *cache = codec->reg_cache;

	if (reg >= TWL6030_CACHEREGNUM)
		return -EIO;

	return cache[reg];
}

/*
 * write twl6030 register cache
 */
static inline void twl6030_write_reg_cache(struct snd_soc_codec *codec,
						u8 reg, u8 value)
{
	u8 *cache = codec->reg_cache;

	if (reg >= TWL6030_CACHEREGNUM)
		return;
	cache[reg] = value;
}

/*
 * read from twl6030 hardware register
 */
static int twl6030_read(struct snd_soc_codec *codec,
			unsigned int reg)
{
	u8 value;

	if (reg > TWL6030_CACHEREGNUM)
		return -EIO;

	twl_i2c_read_u8(TWL6030_MODULE_AUDIO, &value, reg);
	twl6030_write_reg_cache(codec, reg, value);

	return value;
}

/*
 * write to the twl6030 register space
 */
static int twl6030_write(struct snd_soc_codec *codec,
			unsigned int reg, unsigned int value)
{
	if ((reg < TWL6030_REG_INTID) || (reg > TWL6030_REG_ACCCTL))
		/* read-only registers */
		return -EIO;

	twl6030_write_reg_cache(codec, reg, value);
	return twl_i2c_write_u8(TWL6030_MODULE_AUDIO, value, reg);
}

static void abe_init_chip(struct snd_soc_codec *codec)
{
	abe_reset_hal();
}

static void twl6030_init_chip(struct snd_soc_codec *codec)
{
	u8 *cache = codec->reg_cache;
	int i;

	/* allow registers to be accessed by i2c */
	twl6030_write(codec, TWL6030_REG_ACCCTL, cache[TWL6030_REG_ACCCTL]);

	/* set all audio section registers to reasonable defaults */
	/* avoid read-only (ASICID, ASICREV, STATUS) and non-used registers */
	for (i = TWL6030_REG_INTMR; i <= TWL6030_REG_DLB; i++)
		twl6030_write(codec, i, cache[i]);
	for (i = TWL6030_REG_TRIM1; i < TWL6030_REG_ACCCTL; i++)
		twl6030_write(codec, i, cache[i]);
}

static void twl6030_power_up(struct snd_soc_codec *codec)
{
	struct snd_soc_device *socdev = codec->socdev;
	struct twl6030_priv_data *priv = codec->private_data;
	struct twl6030_setup_data *setup = socdev->codec_data;

	if (priv->codec_powered)
		return;

	setup->codec_enable(1);

	/* wait for ready interrupt */
	wait_for_completion(&setup->ready_completion);

	/* sync registers updated during power-up sequence */
	twl6030_read(codec, TWL6030_REG_NCPCTL);
	twl6030_read(codec, TWL6030_REG_LDOCTL);
	twl6030_read(codec, TWL6030_REG_LPPLLCTL);
}

static void twl6030_power_down(struct snd_soc_codec *codec)
{
	struct snd_soc_device *socdev = codec->socdev;
	struct twl6030_priv_data *priv = codec->private_data;
	struct twl6030_setup_data *setup = socdev->codec_data;

	setup->codec_enable(0);
	udelay(500);

	/* sync registers updated during power-down sequence */
	twl6030_read(codec, TWL6030_REG_NCPCTL);
	twl6030_read(codec, TWL6030_REG_LDOCTL);
	twl6030_read(codec, TWL6030_REG_LPPLLCTL);

	priv->codec_powered = 0;
}

/* audio interrupt handler */
irqreturn_t twl6030_naudint_handler(int irq, void *data)
{
	struct twl6030_setup_data *setup = data;

	schedule_work(&setup->audint_work);

	/* disable audint irq to let workqueue to execute */
	disable_irq_nosync(irq);

	return IRQ_HANDLED;
}

void twl6030_naudint_work(struct work_struct *work)
{
	struct snd_soc_codec *codec;
	struct twl6030_setup_data *setup;
	struct twl6030_priv_data *priv;
	u8 intid;

	setup = container_of(work, struct twl6030_setup_data, audint_work);
	codec = setup->codec;
	priv = codec->private_data;

	twl_i2c_read_u8(TWL6030_MODULE_AUDIO, &intid, TWL6030_REG_INTID);

	switch (intid) {
	case TWL6030_THINT:
		dev_alert(codec->dev, "die temperature over-limit detection\n");
		break;
	case TWL6030_PLUGINT:
	case TWL6030_UNPLUGINT:
	case TWL6030_HOOKINT:
		break;
	case TWL6030_HFINT:
		dev_alert(codec->dev, "hf drivers over current detection\n");
		break;
	case TWL6030_VIBINT:
		dev_alert(codec->dev, "vib drivers over current detection\n");
		break;
	case TWL6030_READYINT:
		priv->codec_powered = 1;
		complete(&setup->ready_completion);
		break;
	default:
		dev_err(codec->dev, "unknown twl6030 audio interrupt\n");
	}

	enable_irq(setup->irq);
}

/* set headset dac and driver power mode */
static int headset_power_mode(struct snd_soc_codec *codec, int high_perf)
{
	int hslctl, hsrctl;
	int  mask = TWL6030_HSDRVMODEL | TWL6030_HSDACMODEL;

	hslctl = twl6030_read_reg_cache(codec, TWL6030_REG_HSLCTL);
	hsrctl = twl6030_read_reg_cache(codec, TWL6030_REG_HSRCTL);

	if (high_perf) {
		hslctl &= ~mask;
		hsrctl &= ~mask;
	} else {
		hslctl |= mask;
		hsrctl |= mask;
	}

	twl6030_write(codec, TWL6030_REG_HSLCTL, hslctl);
	twl6030_write(codec, TWL6030_REG_HSRCTL, hsrctl);

	return 0;
}

/*
 * MICATT volume control:
 * from -6 to 0 dB in 6 dB steps
 */
static DECLARE_TLV_DB_SCALE(mic_preamp_tlv, -600, 600, 0);

/*
 * MICGAIN volume control:
 * from 6 to 30 dB in 6 dB steps
 */
static DECLARE_TLV_DB_SCALE(mic_amp_tlv, 600, 600, 0);

/*
 * HSGAIN volume control:
 * from -30 to 0 dB in 2 dB steps
 */
static DECLARE_TLV_DB_SCALE(hs_tlv, -3000, 200, 0);

/*
 * HFGAIN volume control:
 * from -52 to 6 dB in 2 dB steps
 */
static DECLARE_TLV_DB_SCALE(hf_tlv, -5200, 200, 0);

/* Left analog microphone selection */
static const char *twl6030_amicl_texts[] =
	{"Headset Mic", "Main Mic", "Aux/FM Left", "Off"};

/* Right analog microphone selection */
static const char *twl6030_amicr_texts[] =
	{"Headset Mic", "Sub Mic", "Aux/FM Right", "Off"};

static const struct soc_enum twl6030_enum[] = {
	SOC_ENUM_SINGLE(TWL6030_REG_MICLCTL, 3, 3, twl6030_amicl_texts),
	SOC_ENUM_SINGLE(TWL6030_REG_MICRCTL, 3, 3, twl6030_amicr_texts),
};

static const struct snd_kcontrol_new amicl_control =
	SOC_DAPM_ENUM("Route", twl6030_enum[0]);

static const struct snd_kcontrol_new amicr_control =
	SOC_DAPM_ENUM("Route", twl6030_enum[1]);

/* Headset DAC playback switches */
static const struct snd_kcontrol_new hsdacl_switch_controls =
	SOC_DAPM_SINGLE("Switch", TWL6030_REG_HSLCTL, 5, 1, 0);

static const struct snd_kcontrol_new hsdacr_switch_controls =
	SOC_DAPM_SINGLE("Switch", TWL6030_REG_HSRCTL, 5, 1, 0);

/* Handsfree DAC playback switches */
static const struct snd_kcontrol_new hfdacl_switch_controls =
	SOC_DAPM_SINGLE("Switch", TWL6030_REG_HFLCTL, 2, 1, 0);

static const struct snd_kcontrol_new hfdacr_switch_controls =
	SOC_DAPM_SINGLE("Switch", TWL6030_REG_HFRCTL, 2, 1, 0);

/* Headset driver switches */
static const struct snd_kcontrol_new hsl_driver_switch_controls =
	SOC_DAPM_SINGLE("Switch", TWL6030_REG_HSLCTL, 2, 1, 0);

static const struct snd_kcontrol_new hsr_driver_switch_controls =
	SOC_DAPM_SINGLE("Switch", TWL6030_REG_HSRCTL, 2, 1, 0);

/* Handsfree driver switches */
static const struct snd_kcontrol_new hfl_driver_switch_controls =
	SOC_DAPM_SINGLE("Switch", TWL6030_REG_HFLCTL, 4, 1, 0);

static const struct snd_kcontrol_new hfr_driver_switch_controls =
	SOC_DAPM_SINGLE("Switch", TWL6030_REG_HFRCTL, 4, 1, 0);

static const struct snd_kcontrol_new twl6030_snd_controls[] = {
	/* Capture gains */
	SOC_DOUBLE_TLV("Capture Preamplifier (Attenuator) Volume",
		TWL6030_REG_MICGAIN, 6, 7, 1, 1, mic_preamp_tlv),
	SOC_DOUBLE_TLV("Capture Amplifier Volume",
		TWL6030_REG_MICGAIN, 0, 3, 4, 0, mic_amp_tlv),

	/* Playback gains */
	SOC_DOUBLE_TLV("Headset Playback Volume",
		TWL6030_REG_HSGAIN, 0, 4, 0xF, 1, hs_tlv),
	SOC_DOUBLE_R_TLV("Handsfree Playback Volume",
		TWL6030_REG_HFLGAIN, TWL6030_REG_HFRGAIN, 0, 0x1D, 1, hf_tlv),

};

static const struct snd_soc_dapm_widget twl6030_dapm_widgets[] = {
	/* Inputs */
	SND_SOC_DAPM_INPUT("MAINMIC"),
	SND_SOC_DAPM_INPUT("HSMIC"),
	SND_SOC_DAPM_INPUT("SUBMIC"),
	SND_SOC_DAPM_INPUT("AFML"),
	SND_SOC_DAPM_INPUT("AFMR"),

	/* Outputs */
	SND_SOC_DAPM_OUTPUT("HSOL"),
	SND_SOC_DAPM_OUTPUT("HSOR"),
	SND_SOC_DAPM_OUTPUT("HFL"),
	SND_SOC_DAPM_OUTPUT("HFR"),

	/* Analog input muxers for the capture amplifiers */
	SND_SOC_DAPM_MUX("Analog Left Capture Route",
			SND_SOC_NOPM, 0, 0, &amicl_control),
	SND_SOC_DAPM_MUX("Analog Right Capture Route",
			SND_SOC_NOPM, 0, 0, &amicr_control),

	/* Analog capture PGAs */
	SND_SOC_DAPM_PGA("MicAmpL",
			TWL6030_REG_MICLCTL, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("MicAmpR",
			TWL6030_REG_MICRCTL, 0, 0, NULL, 0),

	/* ADCs */
	SND_SOC_DAPM_ADC("ADC Left", "Left Front Capture",
			TWL6030_REG_MICLCTL, 2, 0),
	SND_SOC_DAPM_ADC("ADC Right", "Right Front Capture",
			TWL6030_REG_MICLCTL, 2, 0),

	/* Microphone bias */
	SND_SOC_DAPM_MICBIAS("Headset Mic Bias",
			TWL6030_REG_AMICBCTL, 0, 0),
	SND_SOC_DAPM_MICBIAS("Main Mic Bias",
			TWL6030_REG_AMICBCTL, 4, 0),
	SND_SOC_DAPM_MICBIAS("Digital Mic1 Bias",
			TWL6030_REG_DMICBCTL, 0, 0),
	SND_SOC_DAPM_MICBIAS("Digital Mic2 Bias",
			TWL6030_REG_DMICBCTL, 4, 0),

	/* DACs */
	SND_SOC_DAPM_DAC("HSDAC Left", "Headset Playback",
			TWL6030_REG_HSLCTL, 0, 0),
	SND_SOC_DAPM_DAC("HSDAC Right", "Headset Playback",
			TWL6030_REG_HSRCTL, 0, 0),
	SND_SOC_DAPM_DAC("HFDAC Left", "Handsfree Playback",
			TWL6030_REG_HFLCTL, 0, 0),
	SND_SOC_DAPM_DAC("HFDAC Right", "Handsfree Playback",
			TWL6030_REG_HFRCTL, 0, 0),

	/* Analog playback switches */
	SND_SOC_DAPM_SWITCH("HSDAC Left Playback",
			SND_SOC_NOPM, 0, 0, &hsdacl_switch_controls),
	SND_SOC_DAPM_SWITCH("HSDAC Right Playback",
			SND_SOC_NOPM, 0, 0, &hsdacr_switch_controls),
	SND_SOC_DAPM_SWITCH("HFDAC Left Playback",
			SND_SOC_NOPM, 0, 0, &hfdacl_switch_controls),
	SND_SOC_DAPM_SWITCH("HFDAC Right Playback",
			SND_SOC_NOPM, 0, 0, &hfdacr_switch_controls),

	SND_SOC_DAPM_SWITCH("Headset Left Driver",
			SND_SOC_NOPM, 0, 0, &hsl_driver_switch_controls),
	SND_SOC_DAPM_SWITCH("Headset Right Driver",
			SND_SOC_NOPM, 0, 0, &hsr_driver_switch_controls),
	SND_SOC_DAPM_SWITCH("Handsfree Left Driver",
			SND_SOC_NOPM, 0, 0, &hfl_driver_switch_controls),
	SND_SOC_DAPM_SWITCH("Handsfree Right Driver",
			SND_SOC_NOPM, 0, 0, &hfr_driver_switch_controls),

	/* Analog playback PGAs */
	SND_SOC_DAPM_PGA("HFDAC Left PGA",
			TWL6030_REG_HFLCTL, 1, 0, NULL, 0),
	SND_SOC_DAPM_PGA("HFDAC Right PGA",
			TWL6030_REG_HFRCTL, 1, 0, NULL, 0),

};

static const struct snd_soc_dapm_route intercon[] = {
	/* Capture path */
	{"Analog Left Capture Route", "Headset Mic", "HSMIC"},
	{"Analog Left Capture Route", "Main Mic", "MAINMIC"},
	{"Analog Left Capture Route", "Aux/FM Left", "AFML"},

	{"Analog Right Capture Route", "Headset Mic", "HSMIC"},
	{"Analog Right Capture Route", "Sub Mic", "SUBMIC"},
	{"Analog Right Capture Route", "Aux/FM Right", "AFMR"},

	{"MicAmpL", NULL, "Analog Left Capture Route"},
	{"MicAmpR", NULL, "Analog Right Capture Route"},

	{"ADC Left", NULL, "MicAmpL"},
	{"ADC Right", NULL, "MicAmpR"},

	/* Headset playback path */
	{"HSDAC Left Playback", "Switch", "HSDAC Left"},
	{"HSDAC Right Playback", "Switch", "HSDAC Right"},

	{"Headset Left Driver", "Switch", "HSDAC Left Playback"},
	{"Headset Right Driver", "Switch", "HSDAC Right Playback"},

	{"HSOL", NULL, "Headset Left Driver"},
	{"HSOR", NULL, "Headset Right Driver"},

	/* Handsfree playback path */
	{"HFDAC Left Playback", "Switch", "HFDAC Left"},
	{"HFDAC Right Playback", "Switch", "HFDAC Right"},

	{"HFDAC Left PGA", NULL, "HFDAC Left Playback"},
	{"HFDAC Right PGA", NULL, "HFDAC Right Playback"},

	{"Handsfree Left Driver", "Switch", "HFDAC Left PGA"},
	{"Handsfree Right Driver", "Switch", "HFDAC Right PGA"},

	{"HFL", NULL, "Handsfree Left Driver"},
	{"HFR", NULL, "Handsfree Right Driver"},
};

static int abe_twl6030_add_widgets(struct snd_soc_codec *codec)
{
	snd_soc_dapm_new_controls(codec, twl6030_dapm_widgets,
				 ARRAY_SIZE(twl6030_dapm_widgets));

	snd_soc_dapm_add_routes(codec, intercon, ARRAY_SIZE(intercon));

	snd_soc_dapm_new_widgets(codec);
	return 0;
}

static int abe_twl6030_set_bias_level(struct snd_soc_codec *codec,
				enum snd_soc_bias_level level)
{
	switch (level) {
	case SND_SOC_BIAS_ON:
		twl6030_power_up(codec);
		break;
	case SND_SOC_BIAS_PREPARE:
		twl6030_power_up(codec);
		break;
	case SND_SOC_BIAS_STANDBY:
		twl6030_power_up(codec);
		break;
	case SND_SOC_BIAS_OFF:
		twl6030_power_down(codec);
		break;
	}
	codec->bias_level = level;

	return 0;
}

static int twl6030_set_dai_sysclk(struct snd_soc_dai *codec_dai,
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct twl6030_priv_data *twl6030_priv = codec->private_data;
	u8 hppll, lppll;

	lppll = twl6030_read_reg_cache(codec, TWL6030_REG_LPPLLCTL);
	hppll = twl6030_read_reg_cache(codec, TWL6030_REG_HPPLLCTL);

	switch (clk_id) {
	case TWL6030_SYSCLK_SEL_LPPLL:
		if (freq != 32768) {
			dev_err(codec->dev, "invalid sysclk freq %d\n", freq);
			return -EINVAL;
		}

		/* CLK32K input requires low-power PLL */
		lppll |= TWL6030_LPLLENA | TWL6030_LPLLSEL;
		twl6030_write(codec, TWL6030_REG_LPPLLCTL, lppll);
		mdelay(5);
		lppll &= ~TWL6030_HPLLSEL;
		twl6030_write(codec, TWL6030_REG_LPPLLCTL, lppll);

		/* headset dac and driver must be in low-power mode */
		headset_power_mode(codec, 0);

		hppll &= ~TWL6030_HPLLENA;
		twl6030_write(codec, TWL6030_REG_HPPLLCTL, hppll);
		break;
	case TWL6030_SYSCLK_SEL_HPPLL:
		switch (freq) {
		case 12000000:
			hppll = TWL6030_MCLK_12000KHZ;
			break;
		case 26000000:
			hppll = TWL6030_MCLK_26000KHZ;
			break;
		default:
			dev_err(codec->dev, "invalid sysclk freq %d\n", freq);
			return -EINVAL;
		}

		/* 12 and 26 MHz freqs require high-performance PLL */
		hppll |= TWL6030_HPLLSQRBP | TWL6030_HPLLENA;
		twl6030_write(codec, TWL6030_REG_HPPLLCTL, hppll);
		udelay(500);

		/* headset dac and driver must be in high-performance mode */
		headset_power_mode(codec, 1);

		lppll |= TWL6030_HPLLSEL;
		twl6030_write(codec, TWL6030_REG_LPPLLCTL, lppll);
		lppll &= ~TWL6030_LPLLENA;
		twl6030_write(codec, TWL6030_REG_LPPLLCTL, lppll);

		/* only 19.2 MHz can be generated by HPPLL */
		twl6030_priv->sysclk = 19200000;
		break;
	case TWL6030_SYSCLK_SEL_MCLK:
		switch (freq) {
		case 19200000:
			hppll = TWL6030_MCLK_19200KHZ | TWL6030_HPLLSQRBP;
			break;
		case 38400000:
			hppll = TWL6030_MCLK_38400KHZ;
			break;
		default:
			dev_err(codec->dev, "invalid sysclk freq %d\n", freq);
			return -EINVAL;
		}

		/* 19.2 and 38.4 MHz freqs don't require PLL */
		hppll |= TWL6030_HPLLBP;
		twl6030_write(codec, TWL6030_REG_HPPLLCTL, hppll);
		udelay(500);

		/* headset dac and driver must be in high-performance mode */
		headset_power_mode(codec, 1);

		lppll |= TWL6030_HPLLSEL;
		twl6030_write(codec, TWL6030_REG_LPPLLCTL, lppll);
		lppll &= ~TWL6030_LPLLENA;
		twl6030_write(codec, TWL6030_REG_LPPLLCTL, lppll);

		/* only 19.2 MHz can be generated by MCLK */
		twl6030_priv->sysclk = 19200000;
		break;
	default:
		dev_err(codec->dev, "unknown sysclk rate %d\n", freq);
		return -EINVAL;
	}

	return 0;
}

static int twl6030_set_dai_pll(struct snd_soc_dai *codec_dai, int pll_id,
				unsigned int freq_in, unsigned int freq_out)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct twl6030_priv_data *twl6030_priv = codec->private_data;
	int div;

	if (!freq_in || !freq_out)
		return -EINVAL;

	/* div = round(freq_out / freq_in) */
	div = (freq_out + (freq_in >> 2)) / freq_in;

	if (pll_id == TWL6030_LPPLL_ID) {
		if (div < 512) {
			dev_err(codec->dev, "invalid pll divider %d\n",
				div);
			return -EINVAL;
		}

		twl6030_write(codec, TWL6030_REG_LPPLLDIV, div - 512);
		twl6030_priv->sysclk = freq_out;
	}

	return 0;
}

static int abe_mm_startup(struct snd_pcm_substream *substream,
			struct snd_soc_dai *dai)
{
	abe_default_configuration(UC2_VOICE_CALL_AND_IHF_MMDL);
	return 0;
}

static int abe_mm_hw_params(struct snd_pcm_substream *substream,
			   struct snd_pcm_hw_params *params,
			   struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	unsigned int *dst_ptr;
	int rate, format;

	rate = params_rate(params);
	switch (rate) {
	case 48000:
		break;
	default:
		dev_err(codec->dev, "hw params: unknown rate %d\n", rate);
		return -EINVAL;
	}

	if (!substream->stream) {
		/* Circular buffer 0 (MM_DL path) write pointer address
		 * increases by 2 to avoid loosing DMA request
	 	 */
		dst_ptr = (unsigned int*) (ABE_DMEM_BASE_ADDRESS_MPU + 0x100);
		*dst_ptr = 0x1a004800;

		dst_ptr = (unsigned int*) (ABE_DMEM_BASE_ADDRESS_MPU + 0x108);
		*dst_ptr = 0x04000c00;
	}

	format = params_format(params);
	if (format != SNDRV_PCM_FORMAT_S32_LE) {
		dev_err(codec->dev, "hw params: unknown format %d\n", format);
		return -EINVAL;
	}

	return 0;
}

static struct snd_soc_dai_ops abe_mm_dai_ops = {
	.startup	= abe_mm_startup,
	.hw_params	= abe_mm_hw_params,
	.set_sysclk	= twl6030_set_dai_sysclk,
	.set_pll	= twl6030_set_dai_pll,
};

static int abe_voice_hw_params(struct snd_pcm_substream *substream,
			   struct snd_pcm_hw_params *params,
			   struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	int rate, format;

	rate = params_rate(params);
	switch (rate) {
	case 8000:
		break;
	case 16000:
		break;
	default:
		dev_err(codec->dev, "hw params: unknown rate %d\n", rate);
		return -EINVAL;
	}

	format = params_format(params);
	if (format != SNDRV_PCM_FORMAT_S32_LE) {
		dev_err(codec->dev, "hw params: unknown format %d\n", format);
		return -EINVAL;
	}

	return 0;
}

static struct snd_soc_dai_ops abe_voice_dai_ops = {
	.startup	= abe_mm_startup,
	.hw_params	= abe_voice_hw_params,
	.set_sysclk	= twl6030_set_dai_sysclk,
	.set_pll	= twl6030_set_dai_pll,
};

/* Audio Backend DAIs */
struct snd_soc_dai abe_dai[] = {
	/* Multimedia: MM-UL2, MM-DL */
	{
		.name = "Multimedia",
		.playback = {
			.stream_name = "Playback",
			.channels_min = 2,
			.channels_max = 2,
			.rates = SNDRV_PCM_RATE_48000,
			.formats = ABE_FORMATS,
		},
		.capture = {
			.stream_name = "Capture",
			.channels_min = 2,
			.channels_max = 2,
			.rates = SNDRV_PCM_RATE_48000,
			.formats = ABE_FORMATS,
		},
		.ops = &abe_mm_dai_ops,
	},
	/* Multimedia Extended: MM-DL2 */
	{
		.name = "Multimedia Ext",
		.playback = {
			.stream_name = "Playback",
			.channels_min = 2,
			.channels_max = 2,
			.rates = SNDRV_PCM_RATE_48000,
			.formats = ABE_FORMATS,
		},
		.ops = &abe_mm_dai_ops,
	},
	/* Voice: VX-UL, VX-DL */
	{
		.name = "Voice",
		.playback = {
			.stream_name = "Playback",
			.channels_min = 2,
			.channels_max = 2,
			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
			.formats = ABE_FORMATS,
		},
		.capture = {
			.stream_name = "Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
			.formats = ABE_FORMATS,
		},
		.ops = &abe_voice_dai_ops,
	},
	/* Digital Uplink: MM-UL */
	{
		.name = "Digital Uplink",
		.capture = {
			.stream_name = "Capture",
			.channels_min = 2,
			.channels_max = 8,
			.rates = SNDRV_PCM_RATE_48000,
			.formats = ABE_FORMATS,
		},
	},
	/* Vibrator: VIB-DL */
	{
		.name = "Vibrator",
		.playback = {
			.stream_name = "Playback",
			.channels_min = 2,
			.channels_max = 2,
			.rates = SNDRV_PCM_RATE_48000,
			.formats = ABE_FORMATS,
		},
	},
 };

static int abe_twl6030_suspend(struct platform_device *pdev,
				pm_message_t state)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	abe_twl6030_set_bias_level(codec, SND_SOC_BIAS_OFF);

	return 0;
}

static int abe_twl6030_resume(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	abe_twl6030_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	abe_twl6030_set_bias_level(codec, codec->suspend_bias_level);
	return 0;
}

/*
 * initialize the driver
 */
static int abe_twl6030_init(struct snd_soc_device *socdev)
{
	struct snd_soc_codec *codec = socdev->card->codec;
	struct twl6030_setup_data *setup = socdev->codec_data;
	int ret = 0;

	dev_info(codec->dev, "ABE-TWL6030 Audio Codec init\n");

	codec->name = "abe-wl6030";
	codec->owner = THIS_MODULE;
	codec->read = twl6030_read_reg_cache;
	codec->write = twl6030_write;
	codec->set_bias_level = abe_twl6030_set_bias_level;
	codec->dai = abe_dai;
	codec->num_dai = ARRAY_SIZE(abe_dai);
	codec->reg_cache_size = ARRAY_SIZE(twl6030_reg);
	codec->reg_cache = kmemdup(twl6030_reg, sizeof(twl6030_reg),
					GFP_KERNEL);
	if (codec->reg_cache == NULL)
		return -ENOMEM;

	/* register pcms */
	ret = snd_soc_new_pcms(socdev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1);
	if (ret < 0) {
		dev_err(codec->dev, "failed to create pcms\n");
		goto pcm_err;
	}

	/* platform setup data is required for power-off/off the device */
	if (setup == NULL) {
		dev_err(codec->dev, "platform setup data missing\n");
		goto pcm_err;
	}

	setup->codec = codec;

	/* power on device */
	abe_twl6030_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	abe_init_chip(codec);
	twl6030_init_chip(codec);

	snd_soc_add_controls(codec, twl6030_snd_controls,
				ARRAY_SIZE(twl6030_snd_controls));
	abe_twl6030_add_widgets(codec);

	ret = snd_soc_init_card(socdev);
	if (ret < 0) {
		dev_err(codec->dev, "failed to register card\n");
		goto card_err;
	}

	return ret;

card_err:
	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);
pcm_err:
	kfree(codec->reg_cache);
	return ret;
}

static struct snd_soc_device *abe_twl6030_socdev;

static int abe_twl6030_probe(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec;
	struct twl6030_priv_data *twl6030_priv;

	codec = kzalloc(sizeof(struct snd_soc_codec), GFP_KERNEL);
	if (codec == NULL)
		return -ENOMEM;

	twl6030_priv = kzalloc(sizeof(struct snd_soc_codec), GFP_KERNEL);
	if (twl6030_priv == NULL)
		return -ENOMEM;

	codec->private_data = twl6030_priv;
	codec->dev = &pdev->dev;
	socdev->card->codec = codec;
	mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);

	abe_twl6030_socdev = socdev;
	abe_twl6030_init(socdev);

	return 0;
}

static int abe_twl6030_remove(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	dev_info(codec->dev, "ABE-TWL6030 Audio Codec remove\n");
	abe_twl6030_set_bias_level(codec, SND_SOC_BIAS_OFF);
	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);
	kfree(codec->private_data);
	kfree(codec);

	return 0;
}

struct snd_soc_codec_device soc_codec_dev_abe_twl6030 = {
	.probe = abe_twl6030_probe,
	.remove = abe_twl6030_remove,
	.suspend = abe_twl6030_suspend,
	.resume = abe_twl6030_resume,
};
EXPORT_SYMBOL_GPL(soc_codec_dev_abe_twl6030);

static int __init abe_twl6030_modinit(void)
{
	return snd_soc_register_dais(abe_dai, ARRAY_SIZE(abe_dai));
}
module_init(abe_twl6030_modinit);

static void __exit abe_twl6030_exit(void)
{
	snd_soc_unregister_dais(abe_dai, ARRAY_SIZE(abe_dai));
}
module_exit(abe_twl6030_exit);

MODULE_DESCRIPTION("ASoC ABE-TWL6030 codec driver");
MODULE_AUTHOR("Misael Lopez Cruz");
MODULE_LICENSE("GPL");
