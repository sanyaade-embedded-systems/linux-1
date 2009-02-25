/*
 * TI CP INTC (common platform interrupt controller) header file
 *
 * 2008, MontaVista Software, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 */
#ifndef IRQ_CP_INTC_H
#define IRQ_CP_INTC_H

struct cp_intc_irq_data {
       u32 base;
       u8 num_host_irqs;
       u8 host_irq;
       u8 max_irq;
};

void __init cp_intc_init(struct cp_intc_irq_data *irq_data);

#endif /* IRQ_CP_INTC_H */
