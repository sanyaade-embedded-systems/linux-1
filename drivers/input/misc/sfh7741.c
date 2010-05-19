/*
 * sfh7741.c
 *
 * SFH7741 Proximity Driver
 *
 * Copyright (C) 2010 Texas Instruments
 *
 * Author: Shubhrajyoti Datta <a0393217@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
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

#include <linux/interrupt.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/input/sfh7741.h>

struct sfh7741_drvdata {
	struct input_dev *input;
	int proximity_out;
	int gpio_en;
};

static DEFINE_MUTEX(prox_enable_mutex);
static int prox_enable = 1;

static irqreturn_t sfh7741_isr(int irq, void *dev_id)
{
	struct sfh7741_drvdata *ddata = dev_id;

	int proximity = gpio_get_value(ddata->proximity_out);
	input_report_abs(ddata->input, ABS_DISTANCE, proximity);
	input_sync(ddata->input);

	return IRQ_HANDLED;
}

static int __devinit sfh7741_setup(struct device *dev,
					struct sfh7741_drvdata *ddata
					 )
{
	int  error;
	char *desc = "sfh7741";

	error = gpio_request(ddata->proximity_out, desc);
	if (error < 0) {
		dev_err(dev, "failed to request GPIO %d, error %d\n",
			ddata->proximity_out, error);
		return error;
	}

	error = gpio_direction_input(ddata->proximity_out);
	if (error < 0) {
		dev_err(dev, "failed to configure direction for GPIO %d,\
				error %d\n", ddata->proximity_out, error);
		goto fail1;
	}

	error = request_threaded_irq(gpio_to_irq(ddata->proximity_out) , NULL ,
				sfh7741_isr,
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING
				| IRQF_ONESHOT,
				desc, ddata);
	if (error) {
		dev_err(dev, "Unable to claim irq %d; error %d\n",
			gpio_to_irq(ddata->proximity_out), error);
		goto fail1;
	}

	error = gpio_request(ddata->gpio_en, desc);
	if (error < 0) {
		dev_err(dev, "failed to request GPIO %d, error %d\n",
			ddata->gpio_en, error);
		goto fail2;
	}

	error = gpio_direction_output(ddata->gpio_en , 1);
	if (error < 0) {
		dev_err(dev, "%s: GPIO configuration failed: GPIO %d,\
			error %d\n",__func__, ddata->gpio_en, error);
		goto fail3;
	}
	return 0;

fail3:
	gpio_free(ddata->gpio_en);
fail2:
	free_irq(gpio_to_irq(ddata->proximity_out), &ddata);
fail1:
	gpio_free(ddata->proximity_out);
	return error;
}

static ssize_t set_prox_state(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	int state , error;
	struct platform_device *pdev = to_platform_device(dev);
	struct sfh7741_drvdata *ddata = platform_get_drvdata(pdev);

	if (sscanf(buf, "%u", &state) != 1)
		return -EINVAL;

	if ((state != 1) && (state != 0))
		return -EINVAL;

	error = gpio_direction_output(ddata->gpio_en , state);
	if (error < 0) {
		dev_err(dev, "%s: GPIO configuration failed: GPIO %d,\
			error %d\n",__func__, ddata->gpio_en, error);
	}
	mutex_lock(&prox_enable_mutex);
	if (state != prox_enable) {
		if (state)
			enable_irq(gpio_to_irq(ddata->proximity_out));
		else
			disable_irq(gpio_to_irq(ddata->proximity_out));
		prox_enable = state;
	}
	mutex_unlock(&prox_enable_mutex);
	return strnlen(buf, count);
}

static ssize_t show_prox_state(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%u\n", prox_enable);
}
static DEVICE_ATTR(state, S_IWUSR | S_IRUGO, show_prox_state, set_prox_state);

static ssize_t show_proximity(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	int proximity;
	struct platform_device *pdev = to_platform_device(dev);
	struct sfh7741_drvdata *ddata = platform_get_drvdata(pdev);

	proximity = gpio_get_value(ddata->proximity_out);
	return sprintf(buf, "%u\n", proximity);
}
static DEVICE_ATTR(proximity, S_IRUGO, show_proximity, NULL);

static struct attribute *sfh7741_attributes[] = {
	&dev_attr_state.attr,
	&dev_attr_proximity.attr,
	NULL
};

static const struct attribute_group sfh7741_attr_group = {
	.attrs = sfh7741_attributes,
};

static int __devinit sfh7741_probe(struct platform_device *pdev)
{
	struct sfh7741_platform_data *pdata = pdev->dev.platform_data;
	struct sfh7741_drvdata *ddata;
	struct device *dev = &pdev->dev;
	struct input_dev *input;
	int  error;

	error = device_create_file(&pdev->dev, &dev_attr_state);
	if (error < 0)
		return error;
	error = device_create_file(&pdev->dev, &dev_attr_proximity);
	if (error < 0)
		goto fail1;

	pr_info("SFH7741: Proximity sensor\n");

	ddata = kzalloc(sizeof(struct sfh7741_drvdata),
			GFP_KERNEL);
	input = input_allocate_device();
	if (!ddata || !input) {
		dev_err(dev, "failed to allocate state\n");
		error = -ENOMEM;
		goto fail2;
	}

	input->name = pdev->name;
	input->phys = "sfh7741/input0";
	input->dev.parent = &pdev->dev;

	input->id.bustype = BUS_HOST;
	ddata->proximity_out = pdata->gpio_intr;
	ddata->gpio_en = pdata->gpio_en;


	ddata->input = input;
	__set_bit(EV_ABS, input->evbit);

	input_set_abs_params(input, ABS_DISTANCE, 0, 1, 0, 0);

	error = input_register_device(input);
	if (error) {
		dev_err(dev, "Unable to register input device,error: %d\n"
				, error);
		goto fail3;
	}

	platform_set_drvdata(pdev, ddata);
	error = sfh7741_setup(dev, ddata);
	if (error)
		goto fail4;
	return 0;

fail4:
	platform_set_drvdata(pdev, NULL);
fail3:
	input_free_device(input);
fail2:
	device_remove_file(&pdev->dev, &dev_attr_proximity);
fail1:
	dev_err(dev, "failed to create sysfs entries\n");
	device_remove_file(&pdev->dev, &dev_attr_state);
	return error;

}

static int __devexit sfh7741_remove(struct platform_device *pdev)
{
	struct sfh7741_drvdata *ddata = platform_get_drvdata(pdev);
	int irq ;

	device_remove_file(&pdev->dev, &dev_attr_state);
	device_remove_file(&pdev->dev, &dev_attr_proximity);
	gpio_free(ddata->gpio_en);

	irq = gpio_to_irq(ddata->proximity_out);
	free_irq(irq, (void *)ddata);
	gpio_free(ddata->proximity_out);

	input_unregister_device(ddata->input);
	kfree(ddata);
	return 0;
}

#ifdef CONFIG_PM
static int sfh7741_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct sfh7741_drvdata *ddata = platform_get_drvdata(pdev);
	int error;

	error = gpio_direction_output(ddata->gpio_en , 0);
	if (error < 0) {
		dev_err(dev, "%s: GPIO configuration failed: GPIO %d,\
			error %d\n",__func__, ddata->gpio_en, error);
	}
	return 0;
}

static int sfh7741_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct sfh7741_drvdata *ddata = platform_get_drvdata(pdev);
	int error;

	error = gpio_direction_output(ddata->gpio_en , 1);
	if (error < 0) {
		dev_err(dev, "%s: GPIO configuration failed: GPIO %d,\
			error %d\n",__func__, ddata->gpio_en, error);
	}
	return 0;
}

static const struct dev_pm_ops sfh7741_pm_ops = {
	.suspend	= sfh7741_suspend,
	.resume		= sfh7741_resume,
};
#endif

static struct platform_driver sfh7741_device_driver = {
	.probe		= sfh7741_probe,
	.remove		= __devexit_p(sfh7741_remove),
	.driver		= {
		.name	= "sfh7741",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &sfh7741_pm_ops,
#endif
	}
};

static int __init sfh7741_init(void)
{
	return platform_driver_register(&sfh7741_device_driver);
}

static void __exit sfh7741_exit(void)
{
	platform_driver_unregister(&sfh7741_device_driver);
}

module_init(sfh7741_init);
module_exit(sfh7741_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("Proximity driver ");
MODULE_ALIAS("platform:sfh7741");

