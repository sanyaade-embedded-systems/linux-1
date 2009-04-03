/*
 * <mach/asp.h> - DaVinci Audio Serial Port support
 */
#ifndef __ASM_ARCH_DAVINCI_ASP_H
#define __ASM_ARCH_DAVINCI_ASP_H

#include <mach/irqs.h>

/* Bases of dm644x and dm355 register banks */
#define DAVINCI_ASP0_BASE	0x01E02000
#define DAVINCI_ASP1_BASE	0x01E04000


/* Bases of dm646x register banks */
#define	DAVINCI_DM646X_MCASP0_REG_BASE		0x01D01000
#define DAVINCI_DM646X_MCASP1_REG_BASE		0x01D01800

/* EDMA channels of dm644x and dm355 */
#define DAVINCI_DMA_ASP0_TX	2
#define DAVINCI_DMA_ASP0_RX	3
#define DAVINCI_DMA_ASP1_TX	8
#define DAVINCI_DMA_ASP1_RX	9


/* EDMA channels of dm646x */
#define	DAVINCI_DM646X_DMA_MCASP0_AXEVT0	6
#define	DAVINCI_DM646X_DMA_MCASP0_AREVT0	9
#define	DAVINCI_DM646X_DMA_MCASP1_AXEVT1	12
/* Interrupts */
#define DAVINCI_ASP0_RX_INT	IRQ_MBRINT
#define DAVINCI_ASP0_TX_INT	IRQ_MBXINT
#define DAVINCI_ASP1_RX_INT	IRQ_MBRINT
#define DAVINCI_ASP1_TX_INT	IRQ_MBXINT

#endif /* __ASM_ARCH_DAVINCI_ASP_H */
