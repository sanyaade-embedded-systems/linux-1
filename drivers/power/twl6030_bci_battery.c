/*
 * linux/drivers/power/twl6030_bci_battery.c
 *
 * OMAP2430/3430 BCI battery driver for Linux
 *
 * Copyright (C) 2008-2009 Texas Instruments, Inc.
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
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/i2c/twl6030.h>
#include <linux/power_supply.h>
#include <linux/i2c/twl6030-madc.h>

#define T2_BATTERY_VOLT		0x04
#define T2_BATTERY_TEMP		0x06
#define T2_BATTERY_CUR		0x08

/* charger constants */
#define NO_PW_CONN		0
#define AC_PW_CONN		0x01
#define USB_PW_CONN		0x02

#define CONTROLLER_INT_MASK	(0x0)
#define CONTROLLER_CTRL1	(0x1)
#define CONTROLLER_WDG		(0x2)
#define CONTROLLER_STAT1	(0x3)
#define CHARGERUSB_INT_STATUS	(0x4)
#define CHARGERUSB_INT_MASK	(0x5)
#define CHARGERUSB_STATUS_INT1	(0x6)
#define CHARGERUSB_STATUS_INT2	(0x7)
#define CHARGERUSB_CTRL1	(0x8)
#define CHARGERUSB_CTRL2	(0x9)
#define CHARGERUSB_CTRL3	(0xA)
#define CHARGERUSB_STAT1	(0xB)
#define CHARGERUSB_VOREG	(0xC)
#define CHARGERUSB_VICHRG	(0xD)
#define CHARGERUSB_CINLIMIT	(0xE)
#define CHARGERUSB_CTRLLIMIT1	(0xF)
#define CHARGERUSB_CTRLLIMIT2	(0x10)

#define CONTROLLER_CTRL1_EN_CHARGER	(1 << 4)
#define CONTROLLER_CTRL1_SEL_CHARGER	(1 << 3)

#define CONTROLLER_STAT1_EXTCHRG_STATZ	(1 << 7)
#define CONTROLLER_STAT1_CHRG_DET_N	(1 << 5)
#define CONTROLLER_STAT1_FAULT_WDG	(1 << 4)
#define CONTROLLER_STAT1_VAC_DET	(1 << 3)
#define CONTROLLER_STAT1_VBUS_DET	(1 << 2)
#define CONTROLLER_STAT1_BAT_REMOVED	(1 << 1)
#define CONTROLLER_STAT1_BAT_TEMP_OVRANGE (1 << 0)

#define CHARGERUSB_STATUS_INT1_TMREG	(1 << 7)
#define CHARGERUSB_STATUS_INT1_NO_BAT	(1 << 6)
#define CHARGERUSB_STATUS_INT1_BST_OCP	(1 << 5)
#define CHARGERUSB_STATUS_INT1_TH_SHUTD	(1 << 4)
#define CHARGERUSB_STATUS_INT1_BAT_OVP	(1 << 3)
#define CHARGERUSB_STATUS_INT1_POOR_SRC	(1 << 2)
#define CHARGERUSB_STATUS_INT1_SLP_MODE	(1 << 1)
#define CHARGERUSB_STATUS_INT1_VBUS_OVP	(1 << 0)

/* Ptr to thermistor table */
int *therm_tbl;

struct twl6030_bci_device_info {
	struct device		*dev;

	unsigned long		update_time;
	int			voltage_uV;
	int			bk_voltage_uV;
	int			current_uA;
	int			temp_C;
	int			charge_rsoc;
	int			charge_status;

	struct power_supply	bat;
	struct power_supply	bk_bat;
	struct delayed_work	twl6030_bci_monitor_work;
	struct delayed_work	twl6030_bk_bci_monitor_work;
};

static int usb_charger_flag;
static int LVL_1, LVL_2, LVL_3, LVL_4;

static int read_bci_val(u8 reg_1);
static inline int clear_n_set(u8 mod_no, u8 clear, u8 set, u8 reg);

/*
 * Interrupt service routine
 *
 * Attends to TWL 6030 power module interruptions events, specifically
 * USB_PRES (USB charger presence) CHG_PRES (AC charger presence) events
 *
 */
static irqreturn_t twl6030charger_ctrl_interrupt(int irq, void *_di)
{
	struct twl6030_bci_device_info *di = _di;
	int ret; u8 chg_sts = 0;

#ifdef CONFIG_LOCKDEP
	/* WORKAROUND for lockdep forcing IRQF_DISABLED on us, which
	 * we don't want and can't tolerate.  Although it might be
	 * friendlier not to borrow this thread context...
	 */
	local_irq_enable();
#endif

	/* read charger controller_stat1 */
	ret = twl_i2c_read_u8(TWL6030_MODULE_CHARGER, &chg_sts,
		CONTROLLER_STAT1);
	if (ret)
		return IRQ_NONE;

	if (chg_sts & CONTROLLER_STAT1_VBUS_DET) {
		printk(KERN_INFO "USB charger detected\n");
		ret = twl_i2c_write_u8(TWL6030_MODULE_CHARGER,
				CONTROLLER_CTRL1_EN_CHARGER
				CONTROLLER_CTRL1);
		if (ret)
			    return IRQ_NONE;
	}
	if (chg_sts & CONTROLLER_STAT1_VAC_DET) {
		printk(KERN_INFO "A/C charger detected\n");
		ret = twl_i2c_write_u8(TWL6030_MODULE_CHARGER,
				CONTROLLER_CTRL1_EN_CHARGER |
				CONTROLLER_CTRL1_SEL_CHARGER ,
				CONTROLLER_CTRL1);
		if (ret)
			    return IRQ_NONE;
	}
	if (chg_sts & CONTROLLER_STAT1_FAULT_WDG) {
		printk(KERN_INFO "Fault watchdog fired\n");
	}
	if (chg_sts & CONTROLLER_STAT1_BAT_REMOVED) {
		printk(KERN_INFO "Battery removed\n");
	}
	if (chg_sts & CONTROLLER_STAT1_BAT_TEMP_OVRANGE) {
		printk(KERN_INFO "Battery temperature overrange\n");
	}

	power_supply_changed(&di->bat);

	return IRQ_HANDLED;
}

static irqreturn_t twl6030charger_fault_interrupt(int irq, void *_di)
{
	struct twl6030_bci_device_info *di = _di;

	printk(KERN_INFO "Charger fault detected\n");
	power_supply_changed(&di->bat);
	return IRQ_HANDLED;
}


/*
 * This function handles the twl6030 battery voltage level interrupt.
 */
static int twl6030battery_level_evt(void)
{
}


/*
 * Enable/Disable hardware battery level event notifications.
 */
static int twl6030battery_hw_level_en(int enable)
{
}

/*
 * Return battery temperature
 * Or < 0 on failure.
 */
static int twl6030battery_temperature(void)
{
}

/*
 * Return battery voltage
 * Or < 0 on failure.
 */
static int twl6030battery_voltage(void)
{
}

/*
 * Return the battery current
 * Or < 0 on failure.
 */
static int twl6030battery_current(void)
{
}

/*
 * Return the battery backup voltage
 * Or < 0 on failure.
 */
static int twl6030backupbatt_voltage(void)
{
}

/*
 * Returns the main charge FSM status
 * Or < 0 on failure.
 */
static int twl6030bci_status(void)
{
}


/*
 * Settup the twl6030 BCI module to enable backup
 * battery charging.
 */
static int twl6030backupbatt_voltage_setup(void)
{
}

/*
 * Settup the twl6030 BCI module to measure battery
 * temperature
 */
static int twl6030battery_temp_setup(void)
{
}

static enum power_supply_property twl6030_bci_battery_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP,
};

static enum power_supply_property twl6030_bk_bci_battery_props[] = {
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
};

static void
twl6030_bk_bci_battery_read_status(struct twl6030_bci_device_info *di)
{
	di->bk_voltage_uV = twl6030backupbatt_voltage();
}

static void twl6030_bk_bci_battery_work(struct work_struct *work)
{
	struct twl6030_bci_device_info *di = container_of(work,
		struct twl6030_bci_device_info,
		twl6030_bk_bci_monitor_work.work);

	twl6030_bk_bci_battery_read_status(di);
	schedule_delayed_work(&di->twl6030_bk_bci_monitor_work, 500);
}

static void twl6030_bci_battery_read_status(struct twl6030_bci_device_info *di)
{
	di->temp_C = twl6030battery_temperature();
	di->voltage_uV = twl6030battery_voltage();
	di->current_uA = twl6030battery_current();
}

static void
twl6030_bci_battery_update_status(struct twl6030_bci_device_info *di)
{
	twl6030_bci_battery_read_status(di);
	di->charge_status = POWER_SUPPLY_STATUS_UNKNOWN;

	if (power_supply_am_i_supplied(&di->bat))
		di->charge_status = POWER_SUPPLY_STATUS_CHARGING;
	else
		di->charge_status = POWER_SUPPLY_STATUS_DISCHARGING;
}

static void twl6030_bci_battery_work(struct work_struct *work)
{
	struct twl6030_bci_device_info *di = container_of(work,
		struct twl6030_bci_device_info, twl6030_bci_monitor_work.work);

	twl6030_bci_battery_update_status(di);
	schedule_delayed_work(&di->twl6030_bci_monitor_work, 100);
}


#define to_twl6030_bci_device_info(x) container_of((x), \
			struct twl6030_bci_device_info, bat);

static void twl6030_bci_battery_external_power_changed(struct power_supply *psy)
{
	struct twl6030_bci_device_info *di = to_twl6030_bci_device_info(psy);

	cancel_delayed_work(&di->twl6030_bci_monitor_work);
	schedule_delayed_work(&di->twl6030_bci_monitor_work, 0);
}

#define to_twl6030_bk_bci_device_info(x) container_of((x), \
		struct twl6030_bci_device_info, bk_bat);

static int twl6030_bk_bci_battery_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	struct twl6030_bci_device_info *di = to_twl6030_bk_bci_device_info(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = di->bk_voltage_uV;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int twl6030_bci_battery_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	struct twl6030_bci_device_info *di;
	int status = 0;

	di = to_twl6030_bci_device_info(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = di->charge_status;
		return 0;
	default:
		break;
	}

	switch (psp) {
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = di->voltage_uV;
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		val->intval = di->current_uA;
		break;
	case POWER_SUPPLY_PROP_TEMP:
		val->intval = di->temp_C;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		status = twl6030bci_status();
		if ((status & AC_STATEC) == AC_STATEC)
			val->intval = POWER_SUPPLY_TYPE_MAINS;
		else if (usb_charger_flag)
			val->intval = POWER_SUPPLY_TYPE_USB;
		else
			val->intval = 0;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		/*
		 * need to get the correct percentage value per the
		 * battery characteristics. Approx values for now.
		 */
		if (di->voltage_uV < 2894 || LVL_1) {
			val->intval = 5;
			LVL_1 = 0;
		} else if ((di->voltage_uV < 3451 && di->voltage_uV > 2894)
			|| LVL_2) {
			val->intval = 20;
			LVL_2 = 0;
		} else if ((di->voltage_uV < 3902 && di->voltage_uV > 3451)
			|| LVL_3) {
			val->intval = 50;
			LVL_3 = 0;
		} else if ((di->voltage_uV < 3949 && di->voltage_uV > 3902)
			|| LVL_4) {
			val->intval = 75;
			LVL_4 = 0;
		} else if (di->voltage_uV > 3949)
			val->intval = 90;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static char *twl6030_bci_supplied_to[] = {
	"twl6030_bci_battery",
};

static int __init twl6030_bci_battery_probe(struct platform_device *pdev)
{
	struct twl6030_bci_platform_data *pdata = pdev->dev.platform_data;
	struct twl6030_bci_device_info *di;
	int irq;
	int ret;

	therm_tbl = pdata->battery_tmp_tbl;

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	di->dev = &pdev->dev;
	di->bat.name = "twl6030_bci_battery";
	di->bat.supplied_to = twl6030_bci_supplied_to;
	di->bat.num_supplicants = ARRAY_SIZE(twl6030_bci_supplied_to);
	di->bat.type = POWER_SUPPLY_TYPE_BATTERY;
	di->bat.properties = twl6030_bci_battery_props;
	di->bat.num_properties = ARRAY_SIZE(twl6030_bci_battery_props);
	di->bat.get_property = twl6030_bci_battery_get_property;
	di->bat.external_power_changed =
			twl6030_bci_battery_external_power_changed;

	di->charge_status = POWER_SUPPLY_STATUS_UNKNOWN;

	di->bk_bat.name = "twl6030_bci_bk_battery";
	di->bk_bat.type = POWER_SUPPLY_TYPE_BATTERY;
	di->bk_bat.properties = twl6030_bk_bci_battery_props;
	di->bk_bat.num_properties = ARRAY_SIZE(twl6030_bk_bci_battery_props);
	di->bk_bat.get_property = twl6030_bk_bci_battery_get_property;
	di->bk_bat.external_power_changed = NULL;

	platform_set_drvdata(pdev, di);

	/* settings for temperature sensing */
	ret = twl6030battery_temp_setup();
	if (ret)
		goto temp_setup_fail;

	/* request charger fault interruption */
	irq = platform_get_irq(pdev, 1);
	ret = request_irq(irq, twl6030charger_fault_interrupt,
		0, "twl_charger_fault", di);
	if (ret) {
		dev_dbg(&pdev->dev, "could not request irq %d, status %d\n",
			irq, ret);
		goto batt_irq_fail;
	}

	/* request charger ctrl interruption */
	irq = platform_get_irq(pdev, 0);
	ret = request_irq(irq, twl6030charger_ctrl_interrupt,
		0, "twl_charger_ctrl", di);

	if (ret) {
		dev_dbg(&pdev->dev, "could not request irq %d, status %d\n",
			irq, ret);
		goto chg_irq_fail;
	}

	twl_int_mask_reset(TWL6030_CHARGER_CTRL_INT_MASK, REG_INT_MSK_LINE_C);
	twl_int_mask_reset(TWL6030_CHARGER_FAULT_INT_MASK, REG_INT_MSK_LINE_C);

	ret = power_supply_register(&pdev->dev, &di->bat);
	if (ret) {
		dev_dbg(&pdev->dev, "failed to register main battery\n");
		goto batt_failed;
	}

	INIT_DELAYED_WORK_DEFERRABLE(&di->twl6030_bci_monitor_work,
				twl6030_bci_battery_work);
	schedule_delayed_work(&di->twl6030_bci_monitor_work, 0);

	ret = power_supply_register(&pdev->dev, &di->bk_bat);
	if (ret) {
		dev_dbg(&pdev->dev, "failed to register backup battery\n");
		goto bk_batt_failed;
	}

	INIT_DELAYED_WORK_DEFERRABLE(&di->twl6030_bk_bci_monitor_work,
				twl6030_bk_bci_battery_work);
	schedule_delayed_work(&di->twl6030_bk_bci_monitor_work, 500);

	return 0;

bk_batt_failed:
	power_supply_unregister(&di->bat);
batt_failed:
	free_irq(irq, di);
chg_irq_fail:
	irq = platform_get_irq(pdev, 1);
	free_irq(irq, NULL);
batt_irq_fail:
voltage_setup_fail:
temp_setup_fail:
	kfree(di);

	return ret;
}

static int __exit twl6030_bci_battery_remove(struct platform_device *pdev)
{
	struct twl6030_bci_device_info *di = platform_get_drvdata(pdev);
	int irq;

	irq = platform_get_irq(pdev, 0);
	free_irq(irq, di);

	irq = platform_get_irq(pdev, 1);
	free_irq(irq, NULL);

	flush_scheduled_work();
	power_supply_unregister(&di->bat);
	power_supply_unregister(&di->bk_bat);
	platform_set_drvdata(pdev, NULL);
	kfree(di);

	return 0;
}

#ifdef CONFIG_PM
static int twl6030_bci_battery_suspend(struct platform_device *pdev,
	pm_message_t state)
{
	struct twl6030_bci_device_info *di = platform_get_drvdata(pdev);

	di->charge_status = POWER_SUPPLY_STATUS_UNKNOWN;
	cancel_delayed_work(&di->twl6030_bci_monitor_work);
	cancel_delayed_work(&di->twl6030_bk_bci_monitor_work);
	return 0;
}

static int twl6030_bci_battery_resume(struct platform_device *pdev)
{
	struct twl6030_bci_device_info *di = platform_get_drvdata(pdev);

	schedule_delayed_work(&di->twl6030_bci_monitor_work, 0);
	schedule_delayed_work(&di->twl6030_bk_bci_monitor_work, 50);
	return 0;
}
#else
#define twl6030_bci_battery_suspend	NULL
#define twl6030_bci_battery_resume	NULL
#endif /* CONFIG_PM */

static struct platform_driver twl6030_bci_battery_driver = {
	.probe		= twl6030_bci_battery_probe,
	.remove		= __exit_p(twl6030_bci_battery_remove),
	.suspend	= twl6030_bci_battery_suspend,
	.resume		= twl6030_bci_battery_resume,
	.driver		= {
		.name	= "twl_bci",
	},
};

MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:twl6030_bci");
MODULE_AUTHOR("Texas Instruments Inc");

static int __init twl6030_battery_init(void)
{
	return platform_driver_register(&twl6030_bci_battery_driver);
}
module_init(twl6030_battery_init);

static void __exit twl6030_battery_exit(void)
{
	platform_driver_unregister(&twl6030_bci_battery_driver);
}
module_exit(twl6030_battery_exit);

