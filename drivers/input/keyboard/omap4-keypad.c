/*
 * omap4-keypad.c
 *
 * OMAP4 Keypad Driver
 *
 * Copyright (C) 2010 Texas Instruments
 *
 * Author: Abraham Arce <x0066660@ti.com>
 * Initial Code: Syed Rafiuddin <rafiuddin.syed@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/input.h>
#include <linux/input/matrix_keypad.h>
#include <linux/slab.h>

/* OMAP4 registers */
#define OMAP4_KBD_REVISION		0x00
#define OMAP4_KBD_SYSCONFIG		0x10
#define OMAP4_KBD_SYSSTATUS		0x14
#define OMAP4_KBD_IRQSTATUS		0x18
#define OMAP4_KBD_IRQENABLE		0x1C
#define OMAP4_KBD_WAKEUPENABLE		0x20
#define OMAP4_KBD_PENDING		0x24
#define OMAP4_KBD_CTRL			0x28
#define OMAP4_KBD_DEBOUNCINGTIME	0x2C
#define OMAP4_KBD_LONGKEYTIME		0x30
#define OMAP4_KBD_TIMEOUT		0x34
#define OMAP4_KBD_STATEMACHINE		0x38
#define OMAP4_KBD_ROWINPUTS		0x3C
#define OMAP4_KBD_COLUMNOUTPUTS		0x40
#define OMAP4_KBD_FULLCODE31_0		0x44
#define OMAP4_KBD_FULLCODE63_32		0x48

/* OMAP4 bit definitions */
#define OMAP4_DEF_SYSCONFIG_SOFTRST	(1 << 1)
#define OMAP4_DEF_SYSCONFIG_ENAWKUP	(1 << 2)
#define OMAP4_DEF_IRQENABLE_EVENTEN	(1 << 0)
#define OMAP4_DEF_IRQENABLE_LONGKEY	(1 << 1)
#define OMAP4_DEF_IRQENABLE_TIMEOUTEN	(1 << 2)
#define OMAP4_DEF_CTRL_NOSOFTMODE	(1 << 1)
#define OMAP4_DEF_CTRLPTVVALUE		(1 << 2)
#define OMAP4_DEF_CTRLPTV		(1 << 1)
#define OMAP4_DEF_IRQDISABLE		0x00

/* OMAP4 values */
#define OMAP4_VAL_DEBOUNCINGTIME	0x07
#define OMAP4_VAL_FUNCTIONALCFG		0x1E

#define OMAP4_MASK_IRQSTATUSDISABLE	0xFFFF

struct omap_keypad {
	struct input_dev *input;
	struct platform_device *pdev;

	void __iomem    *base;
	int irq;

	unsigned short *keycodes;
	unsigned int rows;
	unsigned int cols;
	unsigned int row_shift;
	unsigned char key_state[8];
	unsigned short keymap[];
};

static void __init omap_keypad_config(struct omap_keypad *keypad_data)
{
	__raw_writel(OMAP4_DEF_SYSCONFIG_SOFTRST | OMAP4_DEF_SYSCONFIG_ENAWKUP,
			keypad_data->base + OMAP4_KBD_SYSCONFIG);
	__raw_writel(OMAP4_VAL_FUNCTIONALCFG,
			keypad_data->base + OMAP4_KBD_CTRL);
	__raw_writel(OMAP4_VAL_DEBOUNCINGTIME,
			keypad_data->base + OMAP4_KBD_DEBOUNCINGTIME);
	__raw_writel(OMAP4_DEF_IRQDISABLE,
			keypad_data->base + OMAP4_KBD_IRQSTATUS);
	__raw_writel(OMAP4_DEF_IRQENABLE_EVENTEN | OMAP4_DEF_IRQENABLE_LONGKEY,
			keypad_data->base + OMAP4_KBD_IRQENABLE);
}

/* Interrupt handler */
static irqreturn_t omap_keypad_interrupt(int irq, void *dev_id)
{
	struct omap_keypad *keypad_data = dev_id;
	struct input_dev *input_dev = keypad_data->input;
	unsigned char key_state[8];
	int col, row, code;
	u32 *new_state = (u32 *) key_state;

	/* Disable interrupts */
	__raw_writel(OMAP4_DEF_IRQDISABLE, keypad_data->base +
			OMAP4_KBD_IRQENABLE);

	*new_state = __raw_readl(keypad_data->base +
			OMAP4_KBD_FULLCODE31_0);
	*(new_state + 1) = __raw_readl(keypad_data->base +
			OMAP4_KBD_FULLCODE63_32);

	for (col = 0; col < keypad_data->cols; col++) {
		for (row = 0; row < keypad_data->rows; row++) {
			code = MATRIX_SCAN_CODE(row, col,
					keypad_data->row_shift);
			input_report_key(input_dev, keypad_data->keymap[code],
					key_state[row] & (1 << col));
			input_sync(input_dev);
		}
	}

	memcpy(keypad_data->key_state, &key_state,
			sizeof(keypad_data->key_state));

	/* clear pending interrupts */
	__raw_writel(__raw_readl(keypad_data->base + OMAP4_KBD_IRQSTATUS),
			keypad_data->base + OMAP4_KBD_IRQSTATUS);

	/* enable interrupts */
	__raw_writel(OMAP4_DEF_IRQENABLE_EVENTEN | OMAP4_DEF_IRQENABLE_LONGKEY,
			keypad_data->base + OMAP4_KBD_IRQENABLE);

	return IRQ_HANDLED;

}


static int __devinit omap_keypad_probe(struct platform_device *pdev)
{
	struct matrix_keypad_platform_data *pdata;
	struct input_dev *input_dev;
	const struct matrix_keymap_data *keymap_data;
	struct omap_keypad *keypad_data;

	unsigned int status = 0, row_shift = 0, error = 0;

	/* platform data */
	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data defined\n");
		return -EINVAL;
	}

	/* keymap data */
	keymap_data = pdata->keymap_data;
	if (!keymap_data) {
		dev_err(&pdev->dev, "no keymap data defined\n");
		error = -EINVAL;
		goto failed_free_pdata;
	}

	/* omap keypad data and keycodes */
	row_shift = get_count_order(pdata->num_col_gpios);
	keypad_data = kzalloc(sizeof(struct omap_keypad) +
			(pdata->num_row_gpios << row_shift) *
			sizeof(unsigned short), GFP_KERNEL);
	if (!keypad_data) {
		dev_err(&pdev->dev, "keypad_data memory allocation failed\n");
		error = -ENOMEM;
		goto failed_free_keymap;
	}

	memcpy(keypad_data->keymap, pdata->keymap_data->keymap,
			sizeof(pdata->keymap_data->keymap_size));

	/* omap keyboard base resource */
	keypad_data->base = pdata->base;
	if (!keypad_data->base) {
		error = -ENOMEM;
		goto failed_free_keypad;
	}

	/* omap keyboard irq */
	keypad_data->irq = pdata->irq;
	if (keypad_data->irq < 0) {
		dev_err(&pdev->dev, "no keyboard irq assigned\n");
		error = keypad_data->irq;
		goto failed_free_base;
	}

	/* input device allocation */
	input_dev = input_allocate_device();
	if (!input_dev) {
		error = -ENOMEM;
		goto failed_free_base;
	}

	keypad_data->input = input_dev;
	keypad_data->row_shift = row_shift;
	keypad_data->rows = pdata->num_row_gpios;
	keypad_data->cols = pdata->num_col_gpios;

	input_dev->name = pdev->name;
	input_dev->dev.parent = &pdev->dev;
	input_dev->id.bustype = BUS_HOST;
	input_dev->id.vendor = 0x0001;
	input_dev->id.product = 0x0001;
	input_dev->id.version = 0x0001;
	input_dev->keycode	= keypad_data->keymap;
	input_dev->keycodesize	= sizeof(keypad_data->keymap[0]);
	input_dev->keycodemax	= pdata->num_row_gpios << row_shift;

	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(EV_REP, input_dev->evbit);

	input_set_drvdata(input_dev, keypad_data);

	matrix_keypad_build_keymap(keymap_data, row_shift,
			input_dev->keycode, input_dev->keybit);

	status = input_register_device(keypad_data->input);
	if (status < 0) {
		dev_err(&pdev->dev, "failed to register input device\n");
		goto failed_free_input;
	}

	omap_keypad_config(keypad_data);

	status = request_irq(keypad_data->irq, omap_keypad_interrupt,
			IRQF_TRIGGER_FALLING,
			"omap4-keypad", keypad_data);
	if (status) {
		dev_err(&pdev->dev, "failed to register interrupt\n");
		error = -ENOMEM;
		goto failed_unregister_device;
	}

	platform_set_drvdata(pdev, keypad_data);

	return 0;

failed_unregister_device:
	input_unregister_device(keypad_data->input);
	input_dev = NULL;
failed_free_input:
	input_free_device(input_dev);
failed_free_base:
	keypad_data->base = NULL;
failed_free_keypad:
	kfree(keypad_data);
failed_free_keymap:
	kfree(keymap_data);
failed_free_pdata:
	kfree(pdata);
	return error;
}

static int __devexit omap_keypad_remove(struct platform_device *pdev)
{
	struct omap_keypad *keypad_data = platform_get_drvdata(pdev);

	free_irq(keypad_data->irq, keypad_data);
	input_unregister_device(keypad_data->input);
	keypad_data->input = NULL;
	platform_set_drvdata(pdev, NULL);
	kfree(keypad_data);

	return 0;
}

static struct platform_driver omap_keypad_driver = {
	.probe		= omap_keypad_probe,
	.remove		= __devexit_p(omap_keypad_remove),
	.driver		= {
		.name	= "omap4-keypad",
		.owner	= THIS_MODULE,
	},
};

static int __init omap_keypad_init(void)
{
	return platform_driver_register(&omap_keypad_driver);
}

static void __exit omap_keypad_exit(void)
{
	platform_driver_unregister(&omap_keypad_driver);
}

module_init(omap_keypad_init);
module_exit(omap_keypad_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("OMAP4 Keypad Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:omap4-keypad");
