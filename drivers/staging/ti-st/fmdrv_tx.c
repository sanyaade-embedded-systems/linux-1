/*
 *  FM Driver for Connectivity chip of Texas Instruments.
 *  This sub-module of FM driver implements FM TX functionality.
 *
 *  Copyright (C) 2010 Texas Instruments
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/delay.h>
#include "fmdrv.h"
#include "fmdrv_common.h"
#include "fmdrv_tx.h"

int fm_tx_set_stereo_mono(struct fmdrv_ops *fmdev, unsigned short mode)
{
	unsigned short payload;
	int ret = 0;

	if (fmdev->curr_fmmode != FM_MODE_TX)
		return -EPERM;

	if (fmdev->tx_data.aud_mode == mode)
		return ret;

	pr_debug("stereo mode: %d\n", mode);

	/* Set Stereo/Mono mode */
	payload = (1 - mode);
	ret = fmc_send_cmd(fmdev, MONO_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	fmdev->tx_data.aud_mode = mode;

	return ret;
}

static int __set_rds_text(struct fmdrv_ops *fmdev, unsigned char *rds_text)
{
	unsigned short payload;
	int ret;

	ret = fmc_send_cmd(fmdev, RDS_DATA_SET, REG_WR, rds_text,
			strlen(rds_text), NULL, NULL);
	if (ret < 0)
		return ret;

	/* Scroll mode */
	payload = (unsigned short)0x1;
	ret = fmc_send_cmd(fmdev, DISPLAY_MODE, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	return 0;
}

static int __set_rds_data_mode(struct fmdrv_ops *fmdev, unsigned char mode)
{
	unsigned short payload;
	int ret;

	/* Setting unique PI TODO: how unique? */
	payload = (unsigned short)0xcafe;
	ret = fmc_send_cmd(fmdev, PI_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	/* Set decoder id */
	payload = (unsigned short)0xa;
	ret = fmc_send_cmd(fmdev, DI_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	/* TODO: RDS_MODE_GET? */
	return 0;
}

static int __set_rds_len(struct fmdrv_ops *fmdev, unsigned char type,
				unsigned short len)
{
	unsigned short payload;
	int ret;

	len |= type << 8;
	payload = len;
	ret = fmc_send_cmd(fmdev, RDS_CONFIG_DATA_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	/* TODO: LENGTH_GET? */
	return 0;
}

int fm_tx_set_rds_mode(struct fmdrv_ops *fmdev, unsigned char rds_en_dis)
{
	unsigned short payload;
	int ret;
	unsigned char rds_text[] = "Zoom2\n";

	if (fmdev->curr_fmmode != FM_MODE_TX)
		return -EPERM;

	pr_debug("rds_en_dis:%d(E:%d, D:%d)\n", rds_en_dis,
		   FM_RDS_ENABLE, FM_RDS_DISABLE);

	if (rds_en_dis == FM_RDS_ENABLE) {
		/* Set RDS length */
		__set_rds_len(fmdev, 0, strlen(rds_text));

		/* Set RDS text */
		__set_rds_text(fmdev, rds_text);

		/* Set RDS mode */
		__set_rds_data_mode(fmdev, 0x0);
	}

	/* Send command to enable RDS */
	if (rds_en_dis == FM_RDS_ENABLE)
		payload = 0x01;
	else
		payload = 0x00;

	ret = fmc_send_cmd(fmdev, RDS_DATA_ENB, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	if (rds_en_dis == FM_RDS_ENABLE) {
		/* Set RDS length */
		__set_rds_len(fmdev, 0, strlen(rds_text));

		/* Set RDS text */
		__set_rds_text(fmdev, rds_text);
	}
	fmdev->tx_data.rds.flag = rds_en_dis;

	return 0;
}

int fm_tx_set_radio_text(struct fmdrv_ops *fmdev,
				unsigned char *rds_text,
				unsigned char rds_type)
{
	unsigned short payload;
	int ret;

	if (fmdev->curr_fmmode != FM_MODE_TX)
		return -EPERM;

	fm_tx_set_rds_mode(fmdev, 0);

	/* Set RDS length */
	__set_rds_len(fmdev, rds_type, strlen(rds_text));

	/* Set RDS text */
	__set_rds_text(fmdev, rds_text);

	/* Set RDS mode */
	__set_rds_data_mode(fmdev, 0x0);

	payload = 1;
	ret = fmc_send_cmd(fmdev, RDS_DATA_ENB, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	return 0;
}

int fm_tx_set_af(struct fmdrv_ops *fmdev, unsigned int af)
{
	unsigned short payload;
	int ret;

	if (fmdev->curr_fmmode != FM_MODE_TX)
		return -EPERM;
	pr_debug("AF: %d\n", af);

	af = (af - 87500) / 100;
	payload = (unsigned short)af;
	ret = fmc_send_cmd(fmdev, TA_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	return 0;
}

int fm_tx_set_region(struct fmdrv_ops *fmdev,
				unsigned char region_to_set)
{
	unsigned short payload;
	int ret;

	if (region_to_set != FM_BAND_EUROPE_US &&
	    region_to_set != FM_BAND_JAPAN) {
		pr_err("Invalid band\n");
		return -EINVAL;
	}

	/* Send command to set the band */
	payload = (unsigned short)region_to_set;
	ret = fmc_send_cmd(fmdev, TX_BAND_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	return 0;
}

int fm_tx_set_mute_mode(struct fmdrv_ops *fmdev,
				unsigned char mute_mode_toset)
{
	unsigned short payload;
	int ret;

	if (fmdev->curr_fmmode != FM_MODE_TX)
		return -EPERM;
	pr_debug("tx: mute mode %d\n", mute_mode_toset);

	payload = mute_mode_toset;
	ret = fmc_send_cmd(fmdev, MUTE, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	return 0;
}

/* Set TX Audio I/O */
static int __set_audio_io(struct fmdrv_ops *fmdev)
{
	struct fmtx_data *tx = &fmdev->tx_data;
	unsigned short payload;
	int ret;

	/* Set Audio I/O Enable */
	payload = tx->audio_io;
	ret = fmc_send_cmd(fmdev, AUDIO_IO_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	/* TODO: is audio set? */
	return 0;
}

/* Start TX Transmission */
static int __enable_xmit(struct fmdrv_ops *fmdev, unsigned char new_xmit_state)
{
	struct fmtx_data *tx = &fmdev->tx_data;
	unsigned short payload;
	unsigned long timeleft;
	int ret;

	/* Enable POWER_ENB interrupts */
	payload = FM_POW_ENB_EVENT;
	ret = fmc_send_cmd(fmdev, INT_MASK_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	/* Set Power Enable */
	payload = new_xmit_state;
	ret = fmc_send_cmd(fmdev, POWER_ENB_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	/* Wait for Power Enabled */
	init_completion(&fmdev->maintask_completion);
	timeleft = wait_for_completion_timeout(&fmdev->maintask_completion,
					       FM_DRV_TX_TIMEOUT);
	if (!timeleft) {
		pr_err("Timeout(%d sec),didn't get tune ended interrupt\n",
			   jiffies_to_msecs(FM_DRV_TX_TIMEOUT) / 1000);
		return -ETIMEDOUT;
	}

	set_bit(FM_CORE_TX_XMITING, &fmdev->flag);
	tx->xmit_state = new_xmit_state;

	return 0;
}

/* Set TX power level */
int fm_tx_set_pwr_lvl(struct fmdrv_ops *fmdev, unsigned char new_pwr_lvl)
{
	unsigned short payload;
	struct fmtx_data *tx = &fmdev->tx_data;
	int ret;

	if (fmdev->curr_fmmode != FM_MODE_TX)
		return -EPERM;
	pr_debug("tx: pwr_level_to_set %ld\n", (long int)new_pwr_lvl);

	/* If the core isn't ready update global variable */
	if (!test_bit(FM_CORE_READY, &fmdev->flag)) {
		tx->pwr_lvl = new_pwr_lvl;
		return 0;
	}

	/* Set power level */
	payload = new_pwr_lvl;
	ret = fmc_send_cmd(fmdev, POWER_LEV_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	/* TODO: is the power level set? */
	tx->pwr_lvl = new_pwr_lvl;

	return 0;
}

/*
 * Sets FM TX pre-emphasis filter value (OFF, 50us, or 75us)
 * Convert V4L2 specified filter values to chip specific filter values.
 */
int fm_tx_set_preemph_filter(struct fmdrv_ops *fmdev, unsigned int filter)
{
	struct fmtx_data *tx = &fmdev->tx_data;
	unsigned short payload;
	int ret;

	if (fmdev->curr_fmmode != FM_MODE_TX)
		return -EPERM;

	payload = filter;
	ret = fmc_send_cmd(fmdev, PREMPH_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	tx->preemph = filter;

	return ret;
}

/* Sets FM TX antenna impedance value */
int fm_tx_set_ant_imp(struct fmdrv_ops *fmdev, unsigned char imp)
{
	unsigned short payload;
	int ret;

	if (fmdev->curr_fmmode != FM_MODE_TX)
		return -EPERM;

	payload = imp;
	ret = fmc_send_cmd(fmdev, RX_ANTENNA_SELECT, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	return ret;
}

/* Set TX Frequency */
int fm_tx_set_frequency(struct fmdrv_ops *fmdev, unsigned int freq_to_set)
{
	struct fmtx_data *tx = &fmdev->tx_data;
	unsigned short payload, chanl_index;
	int ret;

	if (test_bit(FM_CORE_TX_XMITING, &fmdev->flag)) {
		__enable_xmit(fmdev, 0);
		clear_bit(FM_CORE_TX_XMITING, &fmdev->flag);
	}

	/* Enable FR, BL interrupts */
	payload = (FM_FR_EVENT | FM_BL_EVENT);
	ret = fmc_send_cmd(fmdev, INT_MASK_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	tx->tx_frq = (unsigned long)freq_to_set;
	pr_debug("tx: freq_to_set %ld\n", (long int)tx->tx_frq);

	chanl_index = freq_to_set / 10;

	/* Set current tuner channel */
	payload = chanl_index;
	ret = fmc_send_cmd(fmdev, CHANL_SET, REG_WR, &payload,
			sizeof(payload), NULL, NULL);
	if (ret < 0)
		return ret;

	fm_tx_set_pwr_lvl(fmdev, tx->pwr_lvl);
	fm_tx_set_preemph_filter(fmdev, tx->preemph);

	tx->audio_io = 0x01;	/* I2S */
	__set_audio_io(fmdev);

	__enable_xmit(fmdev, 0x01);	/* Enable transmission */

	tx->aud_mode = FM_STEREO_MODE;
	tx->rds.flag = FM_RDS_DISABLE;

	return 0;
}

