#
# Makefile for the linux kernel.
#

# Common support
obj-y := irq.o io.o sdrc.o control.o mux.o \
		timer16.o intc.o serial.o prcm.o clock.o clockdomain.o powerdomain.o \
			#!@@ NETRA !-> 	devices.o id.o gpmc.o
			#!@@ HAPS54 !-> timer-gp.o 

# Clock framework
obj-y					+= usb-musb.o
obj-$(CONFIG_ARCH_NETRA)		+= clock34xx.o

# DSP
# obj-$(CONFIG_OMAP_MBOX_FWK)	+= mailbox_mach.o
# mailbox_mach-objs		:= mailbox.o

# Specific board support
obj-$(CONFIG_MACH_NETRA_SIM)		+= board-generic.o
