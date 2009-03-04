#
# Makefile for the linux kernel.
#
#

# Common objects
obj-y 			:= time.o irq.o clock.o serial.o io.o id.o psc.o \
			   gpio.o devices.o usb.o dma.o iram.o

obj-$(CONFIG_DAVINCI_MUX)		+= mux.o

# Chip specific
obj-$(CONFIG_ARCH_DAVINCI_DM644x)       += dm644x.o
obj-$(CONFIG_ARCH_DAVINCI_DM646x)       += dm646x.o
obj-$(CONFIG_ARCH_DAVINCI_DM355)        += dm355.o
obj-$(CONFIG_ARCH_OMAPL1x7)		+= omapl1x7.o

obj-$(CONFIG_DAVINCI_SOC_USING_AINTC)	+= irq.o
obj-$(CONFIG_DAVINCI_SOC_USING_CP_INTC)	+= irq_cp_intc.o

# Board specific
obj-$(CONFIG_MACH_DAVINCI_EVM)  	+= board-dm644x-evm.o
obj-$(CONFIG_MACH_DAVINCI_DM646X_EVM)	+= board-dm646x-evm.o
obj-$(CONFIG_MACH_DAVINCI_DM355_EVM)	+= board-dm355-evm.o
obj-$(CONFIG_MACH_SFFSDR)		+= board-sffsdr.o
obj-$(CONFIG_ARCH_OMAPL1x7)		+= soc-omapl1x7.o
obj-$(CONFIG_MACH_OMAPL1X7_EVM)		+= board-omapl1x7-evm.o
obj-$(CONFIG_SPI_DAVINCI)		+= spi.o
