/*
 *  FM Driver for Connectivity chip of Texas Instruments.
 *  FM TX module header.
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

#ifndef _FMDRV_TX_H
#define _FMDRV_TX_H

int fm_tx_set_frequency(struct fmdrv_ops *, unsigned int);
int fm_tx_set_pwr_lvl(struct fmdrv_ops *, unsigned char);
int fm_tx_set_region(struct fmdrv_ops *, unsigned char);
int fm_tx_set_mute_mode(struct fmdrv_ops *, unsigned char);
int fm_tx_set_stereo_mono(struct fmdrv_ops *, unsigned short);
int fm_tx_set_rds_mode(struct fmdrv_ops *, unsigned char);
int fm_tx_set_radio_text(struct fmdrv_ops *, unsigned char *, unsigned char);
int fm_tx_set_af(struct fmdrv_ops *, unsigned int);
int fm_tx_set_preemph_filter(struct fmdrv_ops *, unsigned int);
int fm_tx_set_ant_imp(struct fmdrv_ops *, unsigned char);

#endif

