/*
 * Driver for TI DA8xx SoC's RTC.
 *
 * Author: Mark A. Greer <mgreer@mvista.com>
 *
 * 2008 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/ioctl.h>
#include <linux/platform_device.h>
#include <linux/io.h>

#define OMAPL1X7_RTC_SECONDS		0x00
#define		OMAPL1X7_RTC_SECONDS_SEC_0		0x0000000f
#define		OMAPL1X7_RTC_SECONDS_SEC_1		0x00000070
#define		OMAPL1X7_RTC_SECONDS_MASK \
			(OMAPL1X7_RTC_SECONDS_SEC_0 | OMAPL1X7_RTC_SECONDS_SEC_1)
#define OMAPL1X7_RTC_MINUTES		0x04
#define		OMAPL1X7_RTC_MINUTES_MIN_0		0x0000000f
#define		OMAPL1X7_RTC_MINUTES_MIN_1		0x00000070
#define		OMAPL1X7_RTC_MINUTES_MASK \
			(OMAPL1X7_RTC_MINUTES_MIN_0 | OMAPL1X7_RTC_MINUTES_MIN_1)
#define OMAPL1X7_RTC_HOURS			0x08
#define		OMAPL1X7_RTC_HOURS_HOUR_0		0x0000000f
#define		OMAPL1X7_RTC_HOURS_HOUR_1		0x00000030
#define		OMAPL1X7_RTC_HOURS_HOUR_PM_AM	0x00000080
#define		OMAPL1X7_RTC_HOURS_MASK \
			(OMAPL1X7_RTC_HOURS_HOUR_0 | OMAPL1X7_RTC_HOURS_HOUR_1)
#define OMAPL1X7_RTC_DAYS			0x0c
#define		OMAPL1X7_RTC_DAYS_DAY_0		0x0000000f
#define		OMAPL1X7_RTC_DAYS_DAY_1		0x00000030
#define		OMAPL1X7_RTC_DAYS_MASK \
			(OMAPL1X7_RTC_DAYS_DAY_0 | OMAPL1X7_RTC_DAYS_DAY_1)
#define OMAPL1X7_RTC_MONTHS		0x10
#define		OMAPL1X7_RTC_MONTHS_MONTH_0	0x0000000f
#define		OMAPL1X7_RTC_MONTHS_MONTH_1	0x00000010
#define		OMAPL1X7_RTC_MONTHS_MASK \
			(OMAPL1X7_RTC_MONTHS_MONTH_0 | OMAPL1X7_RTC_MONTHS_MONTH_1)
#define OMAPL1X7_RTC_YEARS			0x14
#define		OMAPL1X7_RTC_YEARS_YEAR_0		0x0000000f
#define		OMAPL1X7_RTC_YEARS_YEAR_1		0x000000f0
#define		OMAPL1X7_RTC_YEARS_MASK \
			(OMAPL1X7_RTC_YEARS_YEAR_0 | OMAPL1X7_RTC_YEARS_YEAR_1)
/* Not bothering with day of week reg */

/*
 * Alarm regs are identical to clock regs above but 0x20 higher up in the
 * register block.  So, instead of duplicating #defines, will use an offset
 * of 0x20 to access alarm regs.
 */
#define OMAPL1X7_RTC_ALARM_REG_OFFSET	0x20

#define OMAPL1X7_RTC_CTRL			0x40
#define		OMAPL1X7_RTC_CTRL_STOP_RTC		0x00000001
#define		OMAPL1X7_RTC_CTRL_ROUND_30S	0x00000002
#define		OMAPL1X7_RTC_CTRL_AUTO_COMP	0x00000004
#define		OMAPL1X7_RTC_CTRL_MODE_12_24	0x00000008
#define		OMAPL1X7_RTC_CTRL_TEST_MODE	0x00000010
#define		OMAPL1X7_RTC_CTRL_SET_32_COUNTER	0x00000020
#define		OMAPL1X7_RTC_CTRL_RTC_DISABLE	0x00000040
#define		OMAPL1X7_RTC_CTRL_SPLIT_POWER	0x00000080
#define		OMAPL1X7_RTC_CTRL_IO_TEST		0x80000000
#define OMAPL1X7_RTC_STATUS		0x44
#define		OMAPL1X7_RTC_STATUS_BUSY		0x00000001
#define		OMAPL1X7_RTC_STATUS_RUN		0x00000002
#define		OMAPL1X7_RTC_STATUS_1S_EVENT	0x00000004
#define		OMAPL1X7_RTC_STATUS_1M_EVENT	0x00000008
#define		OMAPL1X7_RTC_STATUS_1H_EVENT	0x00000010
#define		OMAPL1X7_RTC_STATUS_1D_EVENT	0x00000020
#define		OMAPL1X7_RTC_STATUS_ALARM		0x00000040
#define		OMAPL1X7_RTC_STATUS_POWER_UP	0x00000080
#define		OMAPL1X7_RTC_STATUS_GOOD_INTRS \
			(OMAPL1X7_RTC_STATUS_1S_EVENT | OMAPL1X7_RTC_STATUS_ALARM)
#define		OMAPL1X7_RTC_STATUS_BAD_INTRS \
			(OMAPL1X7_RTC_STATUS_1M_EVENT | OMAPL1X7_RTC_STATUS_1H_EVENT \
			 | OMAPL1X7_RTC_STATUS_1D_EVENT)
#define		OMAPL1X7_RTC_STATUS_INTR_MASK \
			(OMAPL1X7_RTC_STATUS_GOOD_INTRS|OMAPL1X7_RTC_STATUS_BAD_INTRS)
#define OMAPL1X7_RTC_INTR			0x48
#define		OMAPL1X7_RTC_INTR_EVERY		0x00000003
#define		OMAPL1X7_RTC_INTR_IT_TIMER		0x00000004
#define		OMAPL1X7_RTC_INTR_IT_ALARM		0x00000008
#define		OMAPL1X7_RTC_INTR_MASK \
			(OMAPL1X7_RTC_INTR_EVERY | OMAPL1X7_RTC_INTR_IT_TIMER \
			 | OMAPL1X7_RTC_INTR_IT_ALARM)
#define OMAPL1X7_RTC_COMP_LSB		0x4c
#define OMAPL1X7_RTC_COMP_MSB		0x50
#define OMAPL1X7_RTC_OSC			0x54
#define		OMAPL1X7_RTC_OSC_SW_RES_PRO	0x0000000f
#define		OMAPL1X7_RTC_OSC_OSC32K_PWRDN_R	0x00000010
#define		OMAPL1X7_RTC_OSC_SW_RESET		0x00000020
#define OMAPL1X7_RTC_SCRATCH_0		0x60
#define OMAPL1X7_RTC_SCRATCH_1		0x64
#define OMAPL1X7_RTC_SCRATCH_2		0x68
#define OMAPL1X7_RTC_KICK_0		0x6c
#define OMAPL1X7_RTC_KICK_1		0x70

#define	OMAPL1X7_RTC_KICK_0_VAL		0x83e70b13 /* Magic value from doc */
#define	OMAPL1X7_RTC_KICK_1_VAL		0x95a4f1e0 /* Magic value from doc */


#define	omapl1x7_rtc_read(addr)		__raw_readl( \
		(unsigned int *) ((u32)omapl1x7_rtc_base + (u32)(addr)))
#define	omapl1x7_rtc_write(val, addr)	__raw_writel(val, \
		(unsigned int *)((u32)omapl1x7_rtc_base + (u32)(addr)))

static DEFINE_SPINLOCK(omapl1x7_rtc_lock);
static void __iomem	*omapl1x7_rtc_base;
static resource_size_t	omapl1x7_rtc_pbase;
static size_t		omapl1x7_rtc_base_size;
static u8		omapl1x7_rtc_irq;

/*
 * Most regs can only be accessed within 15us of BUSY bit clearing in the
 * STATUS reg.  Can actually read the clock regs (but not alarm regs) without
 * doing this but in interest of code compactness, using same access
 * routine for both.
 */
static void omapl1x7_rtc_wait_not_busy(void)
{
	u32 i, v;

	for (i = 0; i < 10000; i++) {
		v = omapl1x7_rtc_read(OMAPL1X7_RTC_STATUS);
		if (!(v & OMAPL1X7_RTC_STATUS_BUSY))
			return; /* Now have 15us to complete I/O */
	}

	pr_debug("rtc-omapl1x7: RTC always busy.\n");
}

static void omapl1x7_rtc_bcd2bin(struct rtc_time *tm)
{
	tm->tm_sec = bcd2bin(tm->tm_sec);
	tm->tm_min = bcd2bin(tm->tm_min);
	tm->tm_hour = bcd2bin(tm->tm_hour);
	tm->tm_mday = bcd2bin(tm->tm_mday);
	tm->tm_mon = bcd2bin(tm->tm_mon) - 1;
	tm->tm_year = bcd2bin(tm->tm_year) + 100;
}

static int omapl1x7_rtc_bin2bcd(struct rtc_time *tm)
{
	if (rtc_valid_tm(tm) != 0)
		return -EINVAL;

	tm->tm_sec = bin2bcd(tm->tm_sec);
	tm->tm_min = bin2bcd(tm->tm_min);
	tm->tm_hour = bin2bcd(tm->tm_hour);
	tm->tm_mday = bin2bcd(tm->tm_mday);
	tm->tm_mon = bin2bcd(tm->tm_mon + 1);
	tm->tm_year = bin2bcd(tm->tm_year % 100);

	return 0;
}

static int _omapl1x7_rtc_read_time(struct device *dev, struct rtc_time *tm,
		int is_alarm)
{
	int offset;
	unsigned long flags;

	offset = (is_alarm) ? OMAPL1X7_RTC_ALARM_REG_OFFSET : 0;

	spin_lock_irqsave(&omapl1x7_rtc_lock, flags);
	omapl1x7_rtc_wait_not_busy();

	tm->tm_sec = omapl1x7_rtc_read(OMAPL1X7_RTC_SECONDS + offset)
		& OMAPL1X7_RTC_SECONDS_MASK;
	tm->tm_min = omapl1x7_rtc_read(OMAPL1X7_RTC_MINUTES + offset)
		& OMAPL1X7_RTC_MINUTES_MASK;
	tm->tm_hour = omapl1x7_rtc_read(OMAPL1X7_RTC_HOURS + offset)
		& OMAPL1X7_RTC_HOURS_MASK;
	tm->tm_mday = omapl1x7_rtc_read(OMAPL1X7_RTC_DAYS + offset)
		& OMAPL1X7_RTC_DAYS_MASK;
	tm->tm_mon = omapl1x7_rtc_read(OMAPL1X7_RTC_MONTHS + offset)
		& OMAPL1X7_RTC_MONTHS_MASK;
	tm->tm_year = omapl1x7_rtc_read(OMAPL1X7_RTC_YEARS + offset)
		& OMAPL1X7_RTC_YEARS_MASK;

	spin_unlock_irqrestore(&omapl1x7_rtc_lock, flags);

	omapl1x7_rtc_bcd2bin(tm);
	return 0;
}

static int _omapl1x7_rtc_set_time(struct device *dev, struct rtc_time *tm,
		int is_alarm)
{
	int offset, ret;
	unsigned long flags;

	ret = omapl1x7_rtc_bin2bcd(tm);
	if (ret < 0)
		return ret;

	offset = (is_alarm) ? OMAPL1X7_RTC_ALARM_REG_OFFSET : 0;

	spin_lock_irqsave(&omapl1x7_rtc_lock, flags);
	omapl1x7_rtc_wait_not_busy();

	omapl1x7_rtc_write(tm->tm_sec, OMAPL1X7_RTC_SECONDS + offset);
	omapl1x7_rtc_write(tm->tm_min, OMAPL1X7_RTC_MINUTES + offset);
	omapl1x7_rtc_write(tm->tm_hour, OMAPL1X7_RTC_HOURS + offset);
	omapl1x7_rtc_write(tm->tm_mday, OMAPL1X7_RTC_DAYS + offset);
	omapl1x7_rtc_write(tm->tm_mon, OMAPL1X7_RTC_MONTHS + offset);
	omapl1x7_rtc_write(tm->tm_year, OMAPL1X7_RTC_YEARS + offset);

	spin_unlock_irqrestore(&omapl1x7_rtc_lock, flags);

	return 0;
}

static int omapl1x7_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	return _omapl1x7_rtc_read_time(dev, tm, 0);
}

static int omapl1x7_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	return _omapl1x7_rtc_set_time(dev, tm, 0);
}

static int omapl1x7_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	return _omapl1x7_rtc_read_time(dev, &alrm->time, 1);
}

static int omapl1x7_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	return _omapl1x7_rtc_set_time(dev, &alrm->time, 1);
}

#ifdef CONFIG_RTC_INTF_DEV
static int omapl1x7_rtc_ioctl(struct device *dev, unsigned int cmd,
		unsigned long arg)
{
	int ret = 0;
	u32 v;
	unsigned long flags;

	if (!omapl1x7_rtc_irq) /* Intr handler not registered */
		return -ENOIOCTLCMD;

	switch (cmd) {
	case RTC_AIE_OFF:
	case RTC_AIE_ON:
	case RTC_UIE_OFF:
	case RTC_UIE_ON:
		break;
	default:
		return -ENOIOCTLCMD;
	}

	spin_lock_irqsave(&omapl1x7_rtc_lock, flags);
	omapl1x7_rtc_wait_not_busy();

	v = omapl1x7_rtc_read(OMAPL1X7_RTC_INTR);

	switch (cmd) {
	case RTC_AIE_OFF: /* Disable alarm */
		v &= ~OMAPL1X7_RTC_INTR_IT_ALARM;
		break;
	case RTC_AIE_ON: /* Enable alarm */
		v |= OMAPL1X7_RTC_INTR_IT_ALARM;
		break;
	case RTC_UIE_OFF: /* Disable Update Interrupt Enable (1 second timer) */
		v &= ~OMAPL1X7_RTC_INTR_IT_TIMER;
		break;
	case RTC_UIE_ON: /* Enable Update Interrupt Enable (1 second timer) */
		v |= OMAPL1X7_RTC_INTR_IT_TIMER;
		break;
	}

	omapl1x7_rtc_write(v, OMAPL1X7_RTC_INTR);
	spin_unlock_irqrestore(&omapl1x7_rtc_lock, flags);

	return ret;
}
#endif

static int omapl1x7_rtc_proc(struct device *dev, struct seq_file *seq)
{
	u32 v;
	unsigned long flags;

	spin_lock_irqsave(&omapl1x7_rtc_lock, flags);
	omapl1x7_rtc_wait_not_busy();

	v = omapl1x7_rtc_read(OMAPL1X7_RTC_INTR);

	spin_unlock_irqrestore(&omapl1x7_rtc_lock, flags);

	seq_printf(seq, "alarm irq\t: %s\n",
			(v & OMAPL1X7_RTC_INTR_IT_ALARM) ? "yes" : "no");
	seq_printf(seq, "update (1s) irq\t: %s\n",
			(v & OMAPL1X7_RTC_INTR_IT_TIMER) ? "yes" : "no");
	return 0;
}

static struct rtc_class_ops omapl1x7_rtc_ops = {
#ifdef CONFIG_RTC_INTF_DEV
	.ioctl		= omapl1x7_rtc_ioctl,
#endif
	.read_time	= omapl1x7_rtc_read_time,
	.set_time	= omapl1x7_rtc_set_time,
	.read_alarm	= omapl1x7_rtc_read_alarm,
	.set_alarm	= omapl1x7_rtc_set_alarm,
	.proc		= omapl1x7_rtc_proc,
};


/* Interrupt handling for alarms and periodic 1 second interrupts */
static void omapl1x7_rtc_clear_alarm_intr(void)
{
	u32 v;

	v = omapl1x7_rtc_read(OMAPL1X7_RTC_STATUS);
	omapl1x7_rtc_write(v | OMAPL1X7_RTC_STATUS_ALARM, OMAPL1X7_RTC_STATUS);
}

static irqreturn_t omapl1x7_rtc_intr(int irq, void *dev_id)
{
	struct platform_device *pdev = dev_id;
	struct rtc_device *rtc = platform_get_drvdata(pdev);
	unsigned long events = RTC_IRQF;
	u32 v;
	static u8 first_time = 1;

	v = omapl1x7_rtc_read(OMAPL1X7_RTC_STATUS);

	if ((v & OMAPL1X7_RTC_STATUS_INTR_MASK)) {
		if (likely((v & OMAPL1X7_RTC_STATUS_GOOD_INTRS) != 0)) {
			if ((v & OMAPL1X7_RTC_STATUS_ALARM)) {
				events |= RTC_AF;
				omapl1x7_rtc_clear_alarm_intr();
			} else /* must be 1 second timer intr */
				events |= RTC_UF;

			rtc_update_irq(rtc, 1, events);
		} else if (first_time) {
			dev_warn(&pdev->dev, "Bogus interrupt: 0x%08x\n", v);
			first_time = 0;
		}

		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}


/* Driver & hardware init/exit code */
static void __init omapl1x7_rtc_mmr_unlock(void)
{
	/* Regs may be locked; this is how they're unlocked */
	omapl1x7_rtc_write(OMAPL1X7_RTC_KICK_0_VAL, OMAPL1X7_RTC_KICK_0);
	omapl1x7_rtc_write(OMAPL1X7_RTC_KICK_1_VAL, OMAPL1X7_RTC_KICK_1);
}

static void __init omapl1x7_rtc_start(void)
{
	u32 v;

	v = omapl1x7_rtc_read(OMAPL1X7_RTC_CTRL);
	if (!(v & OMAPL1X7_RTC_CTRL_STOP_RTC)) /* Set bit to start */
		omapl1x7_rtc_write(v | OMAPL1X7_RTC_CTRL_STOP_RTC, OMAPL1X7_RTC_CTRL);
}

static int __init omapl1x7_rtc_is_enabled(void)
{
	u32 v;

	v = omapl1x7_rtc_read(OMAPL1X7_RTC_CTRL);
	return (v & OMAPL1X7_RTC_CTRL_RTC_DISABLE) ? 0 : 1;
}

static void __init omapl1x7_rtc_config(void)
{
	u32 v;
	unsigned long flags;

	/*
	 * Put clock & alarm in 24 hour mode, don't round.
	 * Leave compensation alone in case firmware sets it up.
	 */
	v = omapl1x7_rtc_read(OMAPL1X7_RTC_CTRL);
	v &= ~(OMAPL1X7_RTC_CTRL_ROUND_30S | OMAPL1X7_RTC_CTRL_MODE_12_24
			| OMAPL1X7_RTC_CTRL_TEST_MODE
			| OMAPL1X7_RTC_CTRL_SET_32_COUNTER);
	omapl1x7_rtc_write(v, OMAPL1X7_RTC_CTRL);

	/* Turn off timer & alarm interrupts */
	spin_lock_irqsave(&omapl1x7_rtc_lock, flags);
	omapl1x7_rtc_wait_not_busy();

	v = omapl1x7_rtc_read(OMAPL1X7_RTC_INTR);
	v &= ~OMAPL1X7_RTC_INTR_MASK;
	omapl1x7_rtc_write(v, OMAPL1X7_RTC_INTR);

	spin_unlock_irqrestore(&omapl1x7_rtc_lock, flags);
}

static int __devinit omapl1x7_rtc_probe(struct platform_device *pdev)
{
	struct resource *res, *mem = NULL;
	struct rtc_device *rtc = NULL;
	int irq, ret;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		pr_debug("%s: No MEM resource in platform data.\n", pdev->name);
		ret = -ENOENT;
		goto err_out;
	}

	omapl1x7_rtc_pbase = res->start;
	omapl1x7_rtc_base_size = res->start - res->start + 1;

	mem = request_mem_region(res->start, omapl1x7_rtc_base_size, pdev->name);
	if (!mem) {
		pr_debug("%s: Can't reserve MEM resource.\n", pdev->name);
		ret = -EBUSY;
		goto err_out;
	}

	omapl1x7_rtc_base = ioremap(res->start, omapl1x7_rtc_base_size);
	if (omapl1x7_rtc_base == NULL) {
		pr_debug("%s: Can't ioremap MEM resource.\n", pdev->name);
		ret = -ENOMEM;
		goto err_out0;
	}

	rtc = rtc_device_register(pdev->name, &pdev->dev, &omapl1x7_rtc_ops,
			THIS_MODULE);
	if (IS_ERR(rtc)) {
		pr_debug("%s: Can't register device.\n", pdev->name);
		ret = PTR_ERR(rtc);
		goto err_out1;
	}

	platform_set_drvdata(pdev, rtc);

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0)
		dev_warn(&pdev->dev,
				"No IRQ in platform data. Alarms disabled.\n");
	else {
		ret = request_irq(irq, omapl1x7_rtc_intr, IRQF_DISABLED,
				pdev->name, pdev);
		if (ret) {
			pr_debug("%s: Failed to register handler for irq %d.\n",
					pdev->name, irq);
			ret = -EIO;
			goto err_out2;
		}

		omapl1x7_rtc_irq = irq;
	}

	omapl1x7_rtc_mmr_unlock();
	if (!omapl1x7_rtc_is_enabled()) {
		pr_debug("%s: RTC disabled and can't be enabled.\n",
				pdev->name);
		ret = -EIO;
		goto err_out3;
	}
#if 0 /* XXXX */
	omapl1x7_rtc_sw_reset();
#endif
	omapl1x7_rtc_config();
	omapl1x7_rtc_clear_alarm_intr();
	omapl1x7_rtc_start();

	dev_info(&pdev->dev, "TI DA8xx Real Time Clock driver.\n");
	return 0;

err_out3:
	if (omapl1x7_rtc_irq)
		free_irq(omapl1x7_rtc_irq, pdev);
err_out2:
	platform_set_drvdata(pdev, NULL);
	rtc_device_unregister(rtc);
err_out1:
	iounmap(omapl1x7_rtc_base);
err_out0:
	release_mem_region(omapl1x7_rtc_pbase, omapl1x7_rtc_base_size);
err_out:
	dev_err(&pdev->dev, "Unable to register RTC.\n");
	return ret;
}

static int __devexit omapl1x7_rtc_remove(struct platform_device *pdev)
{
	struct rtc_device *rtc = platform_get_drvdata(pdev);

	/* Put RTC into known state with intrs disabled */
	omapl1x7_rtc_config();
	omapl1x7_rtc_clear_alarm_intr();

	if (omapl1x7_rtc_irq)
		free_irq(omapl1x7_rtc_irq, pdev);

	platform_set_drvdata(pdev, NULL);
	rtc_device_unregister(rtc);
	iounmap(omapl1x7_rtc_base);
	release_mem_region(omapl1x7_rtc_pbase, omapl1x7_rtc_base_size);

	return 0;
}

/*
 * Suspend/Resume are not supported by the hardware because once you
 * turn the RTC off, you can't turn it back on (section 2.2.4.1).
 */
static struct platform_driver omapl1x7_rtc_driver = {
	.probe		= omapl1x7_rtc_probe,
	.remove		= omapl1x7_rtc_remove,
	.driver		= {
		.name	= "rtc-omapl1x7",
		.owner	= THIS_MODULE,
	},
};

static int __init omapl1x7_rtc_init(void)
{
	return platform_driver_register(&omapl1x7_rtc_driver);
}

static void __exit omapl1x7_rtc_exit(void)
{
	platform_driver_unregister(&omapl1x7_rtc_driver);
}

module_init(omapl1x7_rtc_init);
module_exit(omapl1x7_rtc_exit);

MODULE_AUTHOR("Mark A. Greer");
MODULE_DESCRIPTION("RTC driver for TI DA8xx SoC");
MODULE_LICENSE("GPL");
