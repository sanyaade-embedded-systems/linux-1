#
# Makefile for the linux kernel.
#

# Common support
obj-y := irq.o io.o sdrc.o control.o mux.o \
		serial.o timer-gp.o prcm.o clock.o clockdomain.o powerdomain.o \
			#!@@ NETRA !-> 	devices.o id.o gpmc.o

# Clock framework
obj-$(CONFIG_ARCH_NETRA)		+= clock34xx.o

# DSP
# obj-$(CONFIG_OMAP_MBOX_FWK)	+= mailbox_mach.o
# mailbox_mach-objs		:= mailbox.o

# Specific board support
obj-$(CONFIG_MACH_NETRA_SIM)		+= board-generic.o
