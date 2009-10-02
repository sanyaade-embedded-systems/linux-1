/*
 * board-3630sdp-camera.c - Camera support for 3630SDP board
 *
 * Copyright (C) 2009 Texas Instruments
 *
 * Modified from mach-omap2/board-3430sdp-camera.c
 *
 * Initial code: Sergio Aguirre <saaguirre@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/mm.h>

#include <linux/regulator/consumer.h>

#include <asm/io.h>

#include <mach/mux.h>
#include <mach/gpio.h>
#ifdef CONFIG_OMAP_PM_SRF
#include <mach/omap-pm.h>
#endif

static int cam_inited;

static struct device *camkit_v9_1_dev;

#include <media/v4l2-int-device.h>
#include <../drivers/media/video/omap34xxcam.h>
#include <../drivers/media/video/isp/ispreg.h>

#define CAMKITV9_1_USE_XCLKA		0
#define CAMKITV9_1_USE_XCLKB		1

#define CAMKITV9_1_RESET_GPIO		98

/* Sensor specific GPIO signals */
#define IMX046_STANDBY_GPIO		4

static struct regulator *sdp3630_imx046_reg1;
static struct regulator *sdp3630_imx046_reg2;

#if defined(CONFIG_VIDEO_IMX046) || defined(CONFIG_VIDEO_IMX046_MODULE)
#include <media/imx046.h>
#include <../drivers/media/video/isp/ispcsi2.h>
#define IMX046_CSI2_CLOCK_POLARITY	0	/* +/- pin order */
#define IMX046_CSI2_DATA0_POLARITY	0	/* +/- pin order */
#define IMX046_CSI2_DATA1_POLARITY	0	/* +/- pin order */
#define IMX046_CSI2_CLOCK_LANE		1	 /* Clock lane position: 1 */
#define IMX046_CSI2_DATA0_LANE		2	 /* Data0 lane position: 2 */
#define IMX046_CSI2_DATA1_LANE		3	 /* Data1 lane position: 3 */
#define IMX046_CSI2_PHY_THS_TERM	2
#define IMX046_CSI2_PHY_THS_SETTLE	23
#define IMX046_CSI2_PHY_TCLK_TERM	0
#define IMX046_CSI2_PHY_TCLK_MISS	1
#define IMX046_CSI2_PHY_TCLK_SETTLE	14
#define IMX046_BIGGEST_FRAME_BYTE_SIZE	PAGE_ALIGN(3280 * 2464 * 2)
#endif

#if defined(CONFIG_VIDEO_IMX046) || defined(CONFIG_VIDEO_IMX046_MODULE)

static struct omap34xxcam_sensor_config imx046_hwc = {
	.sensor_isp  = 0,
	.capture_mem = IMX046_BIGGEST_FRAME_BYTE_SIZE * 2,
	.ival_default	= { 1, 10 },
};

static int imx046_sensor_set_prv_data(struct v4l2_int_device *s, void *priv)
{
	struct omap34xxcam_hw_config *hwc = priv;

	hwc->u.sensor.sensor_isp = imx046_hwc.sensor_isp;
	hwc->dev_index		= 2;
	hwc->dev_minor		= 5;
	hwc->dev_type		= OMAP34XXCAM_SLAVE_SENSOR;

	return 0;
}

static struct isp_interface_config imx046_if_config = {
	.ccdc_par_ser 		= ISP_CSIA,
	.dataline_shift 	= 0x0,
	.hsvs_syncdetect 	= ISPCTRL_SYNC_DETECT_VSRISE,
	.strobe 		= 0x0,
	.prestrobe 		= 0x0,
	.shutter 		= 0x0,
	.wenlog 		= ISPCCDC_CFG_WENLOG_AND,
	.wait_hs_vs		= 2,
	.u.csi.crc 		= 0x0,
	.u.csi.mode 		= 0x0,
	.u.csi.edge 		= 0x0,
	.u.csi.signalling 	= 0x0,
	.u.csi.strobe_clock_inv = 0x0,
	.u.csi.vs_edge 		= 0x0,
	.u.csi.channel 		= 0x0,
	.u.csi.vpclk 		= 0x2,
	.u.csi.data_start 	= 0x0,
	.u.csi.data_size 	= 0x0,
	.u.csi.format 		= V4L2_PIX_FMT_SGRBG10,
};


static int imx046_sensor_power_set(struct v4l2_int_device *s, enum v4l2_power power)
{
	struct omap34xxcam_videodev *vdev = s->u.slave->master->priv;
	struct isp_csi2_lanes_cfg lanecfg;
	struct isp_csi2_phy_cfg phyconfig;
	static enum v4l2_power previous_power = V4L2_POWER_OFF;
	int err = 0;

	if (!cam_inited) {
		printk(KERN_ERR "MT9P012: Unable to control board GPIOs!\n");
		return -EFAULT;
	}

	/*
	 * Plug regulator consumer to respective VAUX supply
	 * if not done before.
	 */
	if (!sdp3630_imx046_reg1 && !sdp3630_imx046_reg2) {
		sdp3630_imx046_reg1 = regulator_get(camkit_v9_1_dev, "vaux2_1");
		if (IS_ERR(sdp3630_imx046_reg1)) {
			dev_err(camkit_v9_1_dev, "vaux2_1 regulator missing\n");
			return PTR_ERR(sdp3630_imx046_reg1);
		}
		sdp3630_imx046_reg2 = regulator_get(camkit_v9_1_dev, "vaux4_1");
		if (IS_ERR(sdp3630_imx046_reg2)) {
			dev_err(camkit_v9_1_dev, "vaux4_1 regulator missing\n");
			regulator_put(sdp3630_imx046_reg1);
			return PTR_ERR(sdp3630_imx046_reg2);
		}
	}

	switch (power) {
	case V4L2_POWER_ON:
		/* Power Up Sequence */
		printk(KERN_DEBUG "imx046_sensor_power_set(ON)\n");

		/* Through-put requirement:
		 * 3280 x 2464 x 2Bpp x 7.5fps x 3 memory ops = 355163 KByte/s
		 */
#ifdef CONFIG_OMAP_PM_SRF
		omap_pm_set_min_bus_tput(vdev->cam->isp, OCP_INITIATOR_AGENT, 355163);
#endif

		isp_csi2_reset();

		lanecfg.clk.pol = IMX046_CSI2_CLOCK_POLARITY;
		lanecfg.clk.pos = IMX046_CSI2_CLOCK_LANE;
		lanecfg.data[0].pol = IMX046_CSI2_DATA0_POLARITY;
		lanecfg.data[0].pos = IMX046_CSI2_DATA0_LANE;
		lanecfg.data[1].pol = IMX046_CSI2_DATA1_POLARITY;
		lanecfg.data[1].pos = IMX046_CSI2_DATA1_LANE;
		lanecfg.data[2].pol = 0;
		lanecfg.data[2].pos = 0;
		lanecfg.data[3].pol = 0;
		lanecfg.data[3].pos = 0;
		isp_csi2_complexio_lanes_config(&lanecfg);
		isp_csi2_complexio_lanes_update(true);

		isp_csi2_ctrl_config_ecc_enable(true);

		phyconfig.ths_term = IMX046_CSI2_PHY_THS_TERM;
		phyconfig.ths_settle = IMX046_CSI2_PHY_THS_SETTLE;
		phyconfig.tclk_term = IMX046_CSI2_PHY_TCLK_TERM;
		phyconfig.tclk_miss = IMX046_CSI2_PHY_TCLK_MISS;
		phyconfig.tclk_settle = IMX046_CSI2_PHY_TCLK_SETTLE;
		isp_csi2_phy_config(&phyconfig);
		isp_csi2_phy_update(true);

		isp_configure_interface(vdev->cam->isp, &imx046_if_config);

		if (previous_power == V4L2_POWER_OFF) {
			/* nRESET is active LOW. set HIGH to release reset */
			gpio_set_value(CAMKITV9_1_RESET_GPIO, 1);


			/* turn on analog power */
			regulator_enable(sdp3630_imx046_reg1);
			regulator_enable(sdp3630_imx046_reg2);
			udelay(100);

			/* have to put sensor to reset to guarantee detection */
			gpio_set_value(CAMKITV9_1_RESET_GPIO, 0);
			udelay(1500);

			/* nRESET is active LOW. set HIGH to release reset */
			gpio_set_value(CAMKITV9_1_RESET_GPIO, 1);
			udelay(300);
		}
		break;
	case V4L2_POWER_OFF:
		printk(KERN_DEBUG "imx046_sensor_power_set(OFF)\n");
		/* Power Down Sequence */
		isp_csi2_complexio_power(ISP_CSI2_POWER_OFF);

		if (regulator_is_enabled(sdp3630_imx046_reg1))
			regulator_disable(sdp3630_imx046_reg1);
		if (regulator_is_enabled(sdp3630_imx046_reg2))
			regulator_disable(sdp3630_imx046_reg2);

#ifdef CONFIG_OMAP_PM_SRF
		omap_pm_set_min_bus_tput(vdev->cam->isp, OCP_INITIATOR_AGENT, 0);
#endif
		break;
	case V4L2_POWER_STANDBY:
		printk(KERN_DEBUG "imx046_sensor_power_set(STANDBY)\n");
		isp_csi2_complexio_power(ISP_CSI2_POWER_OFF);
		/*TODO*/
#ifdef CONFIG_OMAP_PM_SRF
		omap_pm_set_min_bus_tput(vdev->cam->isp, OCP_INITIATOR_AGENT, 0);
#endif
		break;
	}

	/* Save powerstate to know what was before calling POWER_ON. */
	previous_power = power;
	return err;
}

static u32 imx046_sensor_set_xclk(struct v4l2_int_device *s, u32 xclkfreq)
{
	struct omap34xxcam_videodev *vdev = s->u.slave->master->priv;

	return isp_set_xclk(vdev->cam->isp, xclkfreq, CAMKITV9_1_USE_XCLKB);
}

struct imx046_platform_data sdp3630_imx046_platform_data = {
	.power_set            = imx046_sensor_power_set,
	.priv_data_set        = imx046_sensor_set_prv_data,
	.set_xclk             = imx046_sensor_set_xclk,
	.csi2_lane_count      = isp_csi2_complexio_lanes_count,
	.csi2_cfg_vp_out_ctrl = isp_csi2_ctrl_config_vp_out_ctrl,
	.csi2_ctrl_update     = isp_csi2_ctrl_update,
	.csi2_cfg_virtual_id  = isp_csi2_ctx_config_virtual_id,
	.csi2_ctx_update      = isp_csi2_ctx_update,
	.csi2_calc_phy_cfg0   = isp_csi2_calc_phy_cfg0,
};
#endif

static int sdp3630_camkit_probe(struct platform_device *pdev)
{
	int ret = 0;

	/* Request and configure gpio pins */
	if (gpio_request(IMX046_STANDBY_GPIO, "imx046_standby_gpio") != 0) {
		dev_err(&pdev->dev, "Could not request GPIO %d",
			IMX046_STANDBY_GPIO);
		ret = -ENODEV;
		goto err;
	}

	if (gpio_request(CAMKITV9_1_RESET_GPIO,
			 "camkitv9_1_reset_gpio") != 0) {
		dev_err(&pdev->dev, "Could not request GPIO %d",
			CAMKITV9_1_RESET_GPIO);
		ret = -ENODEV;
		goto err_freegpio1;
	}

	/* set to output mode */
	gpio_direction_output(IMX046_STANDBY_GPIO, true);
	gpio_direction_output(CAMKITV9_1_RESET_GPIO, true);

	cam_inited = 1;
	camkit_v9_1_dev = &pdev->dev;
	return 0;

err_freegpio1:
	gpio_free(IMX046_STANDBY_GPIO);
err:
	cam_inited = 0;
	return ret;
}

static int sdp3630_camkit_remove(struct platform_device *pdev)
{
	if (regulator_is_enabled(sdp3630_imx046_reg1))
		regulator_disable(sdp3630_imx046_reg1);
	regulator_put(sdp3630_imx046_reg1);
	if (regulator_is_enabled(sdp3630_imx046_reg2))
		regulator_disable(sdp3630_imx046_reg2);
	regulator_put(sdp3630_imx046_reg2);

	gpio_free(IMX046_STANDBY_GPIO);
	gpio_free(CAMKITV9_1_RESET_GPIO);
	return 0;
}

static int sdp3630_camkit_suspend(struct device *dev)
{
	return 0;
}

static int sdp3630_camkit_resume(struct device *dev)
{
	return 0;
}

static struct dev_pm_ops sdp3630_camkit_pm_ops = {
	.suspend = sdp3630_camkit_suspend,
	.resume  = sdp3630_camkit_resume,
};

static struct platform_driver sdp3630_camkit_driver = {
	.probe		= sdp3630_camkit_probe,
	.remove		= sdp3630_camkit_remove,
	.driver		= {
		.name	= "sdp3630_camkit",
		.pm	= &sdp3630_camkit_pm_ops,
	},
};

void __init sdp3630_cam_init(void)
{
	cam_inited = 0;
	platform_driver_register(&sdp3630_camkit_driver);
}

