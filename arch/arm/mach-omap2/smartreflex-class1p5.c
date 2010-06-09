#include "smartreflex.h"
#include "voltage.h"

static int sr_class1p5_enable(int id)
{
	int target_opp_no = 0;
	if (id == SR1)
		target_opp_no = get_vdd1_opp();
	else if (id == SR2)
		target_opp_no = get_vdd2_opp();
	if (!target_opp_no) {
		pr_warning("Targetopp not known.Cannot enable SR%d\n", id);
		return false;
	}
	omap_voltageprocessor_enable(id);
	return sr_enable(id, target_opp_no);
}

static int sr_class1p5_disable(int id, int is_volt_reset)
{
	int target_opp_no = 0;
	if (id == SR1)
		target_opp_no = get_vdd1_opp();
	else if (id == SR2)
		target_opp_no = get_vdd2_opp();
	omap_voltageprocessor_disable(id);
	sr_disable(id);
	if (is_volt_reset)
		omap_reset_voltage(id);
	return true;
}

/* SR class3 structure */
struct omap_smartreflex_class_data class1p5_data = {
	.enable = sr_class1p5_enable,
	.disable = sr_class1p5_disable,
	.class_type = SR_CLASS1P5,
};

static int __init sr_class1p5_init(void)
{
	omap_sr_register_class(&class1p5_data);
	return 0;
}
late_initcall(sr_class1p5_init);
