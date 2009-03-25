#
# Makefile for the linux kernel.
#

# Common support
obj-y := irq.o io.o sdrc.o control.o mux.o \
		serial.o timer-gp.o prcm.o clock.o clockdomain.o powerdomain.o \
			#!@@ NETRA !-> prcm.o clock.o \
			clockdomain.o devices.o id.o gpmc.o powerdomain.o

obj-$(CONFIG_OMAP_MCBSP) += mcbsp.o

# Functions loaded to SRAM
obj-$(CONFIG_ARCH_OMAP3)		+= sram34xx.o

# SMS/SDRC
# obj-$(CONFIG_ARCH_OMAP3)		+= sdrc3xxx.o

# SmartReflex driver
obj-$(CONFIG_OMAP_SMARTREFLEX)		+= smartreflex.o

# Clock framework
obj-$(CONFIG_ARCH_OMAP3)		+= clock34xx.o
obj-$(CONFIG_ARCH_NETRA)		+= clock34xx.o

# DSP
obj-$(CONFIG_OMAP_MBOX_FWK)	+= mailbox_mach.o
mailbox_mach-objs		:= mailbox.o

# Specific board support
obj-$(CONFIG_MACH_NETRA_SIM)		+= board-generic.o
obj-$(CONFIG_MACH_OMAP3EVM)		+= board-omap3evm.o \
					   mmc-twl4030.o \
					   usb-musb.o usb-ehci.o \
					   board-omap3evm-flash.o \
					   twl4030-generic-scripts.o
