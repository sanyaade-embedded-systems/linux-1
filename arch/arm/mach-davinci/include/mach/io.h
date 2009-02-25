/*
 * DaVinci IO address definitions
 *
 * Copied from include/asm/arm/arch-omap/io.h
 *
 * 2007 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */
#ifndef __ASM_ARCH_IO_H
#define __ASM_ARCH_IO_H

#define IO_SPACE_LIMIT 0xffffffff

/*
 * ----------------------------------------------------------------------------
 * I/O mapping
 * ----------------------------------------------------------------------------
 */
#define IO_PHYS		0x01c00000
#define IO_OFFSET	0xfd000000 /* Virtual IO = 0xfec00000 */
#define IO_SIZE		0x00400000
#define IO_VIRT		(IO_PHYS + IO_OFFSET)
#define io_v2p(va)	((va) - IO_OFFSET)
#define __IO_ADDRESS(x)	((x) + IO_OFFSET)

/*
 * Some platforms have the interrupt controller mapped at a different physical
 * address.  This I/O map creates a virtual mapping of the interrupt controller
 * that is common across platforms; it needs to apply to all platform variants.
 * The interrupt controller address mapped in the IO_VIRT range should not be
 * used.  The address is determined by the size of CP INTC address space (which
 * is 4KB or 1 page) and a single page hole to reduce the chance for invalid
 * memory access.
 */
#define IO_Dx_INTC_SIZE SZ_64K
#define IO_Dx_INTC_VIRT 0xfebd0000

/*
 * We don't actually have real ISA nor PCI buses, but there is so many
 * drivers out there that might just work if we fake them...
 */
#define __io(a)			__typesafe_io(a)
#define __mem_pci(a)		(a)
#define __mem_isa(a)		(a)

#define IO_ADDRESS(pa)          IOMEM(__IO_ADDRESS(pa))

#ifdef __ASSEMBLER__
#define IOMEM(x)                x
#else
#define IOMEM(x)                ((void __force __iomem *)(x))

#define __arch_ioremap(p, s, t)	davinci_ioremap(p, s, t)
#define __arch_iounmap(v)	davinci_iounmap(v)

void __iomem *davinci_ioremap(unsigned long phys, size_t size,
			      unsigned int type);
void davinci_iounmap(volatile void __iomem *addr);

#endif /* __ASSEMBLER__ */
#endif /* __ASM_ARCH_IO_H */
