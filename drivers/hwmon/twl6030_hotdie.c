/*
 *
 * TWL6030 Hotdie
 * supports configuration of temperature threshold
 * handles interrupt when temperature crosses the threshold
 *
 * Copyright (C) 2010 Texas Instruments, Inc.
 * Author: Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/i2c/twl.h>
#include <linux/i2c/twl6030-gpadc.h>

#define TMP_CFG			0xE7
#define THERM_HD		(1 << 4)

struct twl6030_hotdie {
	struct device		*dev;
	struct mutex            lock;
	int			state;
};

static irqreturn_t twl6030_hotdie_interrupt(int irq, void *_hd)
{
	struct twl6030_hotdie *hd = _hd;
	u8 read_reg = 0;

	twl_i2c_read_u8(TWL6030_MODULE_ID0, &read_reg, TMP_CFG);
	hd->state = read_reg & THERM_HD;
	dev_err(hd->dev, "TWL6030 Hotdie status %d\n", hd->state);
	sysfs_notify(&hd->dev->kobj, NULL, "hotdie");
	kobject_uevent(&hd->dev->kobj, KOBJ_CHANGE);

	return IRQ_HANDLED;
}

/*
 * Return temperature
 * Or < 0 on failure.
 */
static int twl6030_get_gpadc_hotdie(int channel_no)
{
	struct twl6030_gpadc_request req;
	int temp = 0;
	int ret;
	u32 temperature = 0;

	req.channels = (1 << channel_no);
	req.method = TWL6030_GPADC_SW2;
	req.active = 0;
	req.func_cb = NULL;
	ret = twl6030_gpadc_conversion(&req);
	if (ret < 0)
		return ret;

	if (req.rbuf[channel_no] > 0) {
		temp = req.rbuf[channel_no];
		temperature = (((temp - 671) * 465) / 1000) + 27;
	}

	return  temperature;
}

static ssize_t set_hotdie_threshold(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	long val;
	u8 threshold_sel = 0;

	if (strict_strtol(buf, 10, &val))
		return -EINVAL;

	if (val == 117 || val == 106)
		threshold_sel = 0;
	else if (val == 121 || val == 112)
		threshold_sel = 1;
	else if (val == 125 || val == 116)
		threshold_sel = 2;
	else if (val == 130 || val == 120)
		threshold_sel = 3;
	else
		return -EINVAL;

	twl6030_interrupt_mask(TWL6030_HOTDIE_INT_MASK, REG_INT_MSK_LINE_C);
	twl6030_interrupt_mask(TWL6030_HOTDIE_INT_MASK,	REG_INT_MSK_STS_C);
	twl_i2c_write_u8(TWL6030_MODULE_ID0, threshold_sel, TMP_CFG);
	twl6030_interrupt_unmask(TWL6030_HOTDIE_INT_MASK, REG_INT_MSK_LINE_C);
	twl6030_interrupt_unmask(TWL6030_HOTDIE_INT_MASK, REG_INT_MSK_STS_C);

	return count;
}

static ssize_t show_hotdie_threshold(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int val;
	u8 threshold_sel = 0;

	twl_i2c_read_u8(TWL6030_MODULE_ID0, &threshold_sel, TMP_CFG);
	threshold_sel &= 3;

	if (threshold_sel == 0)
		val = 117;
	else if (threshold_sel == 1)
		val = 121;
	else if (threshold_sel == 2)
		val = 125;
	else if (threshold_sel == 3)
		val = 130;

	return sprintf(buf, "%d\n", val);
}

static ssize_t show_hotdie2_temp(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int val;

	val = twl6030_get_gpadc_hotdie(13);
	return sprintf(buf, "%d\n", val);
}

static ssize_t show_hotdie1_temp(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int val;

	val = twl6030_get_gpadc_hotdie(12);
	return sprintf(buf, "%d\n", val);
}

static ssize_t show_hotdie(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int val;
	struct twl6030_hotdie *hd = dev_get_drvdata(dev);

	val = hd->state;
	return sprintf(buf, "%d\n", val);
}

static DEVICE_ATTR(hotdie, S_IRUGO, show_hotdie, NULL);
static DEVICE_ATTR(hotdie1_temp, S_IRUGO, show_hotdie1_temp, NULL);
static DEVICE_ATTR(hotdie2_temp, S_IRUGO, show_hotdie2_temp, NULL);
static DEVICE_ATTR(hotdie_threshold, S_IWUSR | S_IRUGO, show_hotdie_threshold,
				set_hotdie_threshold);

static struct attribute *twl6030_hotdie_attributes[] = {
	&dev_attr_hotdie.attr,
	&dev_attr_hotdie1_temp.attr,
	&dev_attr_hotdie2_temp.attr,
	&dev_attr_hotdie_threshold.attr,
	NULL,
};

static const struct attribute_group twl6030_hotdie_attr_group = {
	.attrs = twl6030_hotdie_attributes,
};

static int __devinit twl6030_hotdie_probe(struct platform_device *pdev)
{
	struct twl6030_hotdie *hd;
	int ret;
	int irq;

	hd = kzalloc(sizeof(*hd), GFP_KERNEL);
	if (!hd)
		return -ENOMEM;

	irq = platform_get_irq(pdev, 0);
	ret = request_threaded_irq(irq, NULL, twl6030_hotdie_interrupt,
		0, "twl6030_hotdie", hd);
	if (ret) {
		dev_dbg(&pdev->dev, "could not request irq %d, status %d\n",
			irq, ret);
		goto exit_irq;
	}

	hd->dev = hwmon_device_register(&pdev->dev);
	if (ret) {
		dev_dbg(&pdev->dev, "failed to register twl6030 hotdie\n");
		goto exit_register;
	}
	platform_set_drvdata(pdev, hd);
	dev_set_drvdata(hd->dev, hd);

	ret = sysfs_create_group(&hd->dev->kobj, &twl6030_hotdie_attr_group);
	kobject_uevent(&hd->dev->kobj, KOBJ_ADD);
	if (ret) {
		dev_dbg(&pdev->dev, "could not create sysfs files\n");
		goto exit_sysfs;
	}

	twl6030_interrupt_unmask(TWL6030_HOTDIE_INT_MASK, REG_INT_MSK_LINE_C);
	twl6030_interrupt_unmask(TWL6030_HOTDIE_INT_MASK, REG_INT_MSK_STS_C);

	return 0;
exit_sysfs:
	dev_set_drvdata(hd->dev, NULL);
	platform_set_drvdata(pdev, NULL);
	hwmon_device_unregister(hd->dev);
exit_register:
	free_irq(irq, hd);
exit_irq:
	kfree(hd);

	return ret;
}

static int __devexit twl6030_hotdie_remove(struct platform_device *pdev)
{
	struct twl6030_hotdie *hd = platform_get_drvdata(pdev);
	int irq;

	twl6030_interrupt_mask(TWL6030_HOTDIE_INT_MASK, REG_INT_MSK_LINE_C);
	twl6030_interrupt_mask(TWL6030_HOTDIE_INT_MASK,	REG_INT_MSK_STS_C);

	irq = platform_get_irq(pdev, 0);
	free_irq(irq, hd);

	sysfs_remove_group(&hd->dev->kobj, &twl6030_hotdie_attr_group);
	dev_set_drvdata(hd->dev, NULL);
	platform_set_drvdata(pdev, NULL);
	hwmon_device_unregister(hd->dev);
	kfree(hd);

	return 0;
}

#ifdef CONFIG_PM
static int twl6030_hotdie_suspend(struct platform_device *pdev,
	pm_message_t state)
{
	twl6030_interrupt_mask(TWL6030_HOTDIE_INT_MASK, REG_INT_MSK_LINE_C);
	twl6030_interrupt_mask(TWL6030_HOTDIE_INT_MASK,	REG_INT_MSK_STS_C);

	return 0;
}

static int twl6030_hotdie_resume(struct platform_device *pdev)
{
/*	struct twl6030_hotdie *hd = platform_get_drvdata(pdev);*/

	twl6030_interrupt_unmask(TWL6030_HOTDIE_INT_MASK, REG_INT_MSK_LINE_C);
	twl6030_interrupt_unmask(TWL6030_HOTDIE_INT_MASK, REG_INT_MSK_STS_C);

	return 0;
}
#else
#define twl6030_hotdie_suspend	NULL
#define twl6030_hotdie_resume	NULL
#endif /* CONFIG_PM */

static struct platform_driver twl6030_hotdie_driver = {
	.probe		= twl6030_hotdie_probe,
	.remove		= __devexit_p(twl6030_hotdie_remove),
	.suspend	= twl6030_hotdie_suspend,
	.resume		= twl6030_hotdie_resume,
	.driver		= {
		.name	= "twl6030_hotdie",
	},
};

static int __init twl6030_hotdie_init(void)
{
	return platform_driver_register(&twl6030_hotdie_driver);
}
module_init(twl6030_hotdie_init);

static void __exit twl6030_hotdie_exit(void)
{
	platform_driver_unregister(&twl6030_hotdie_driver);
}
module_exit(twl6030_hotdie_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TWL6030 Hotdie");
MODULE_ALIAS("platform:twl6030_hotdie");
MODULE_AUTHOR("Texas Instruments Inc");
