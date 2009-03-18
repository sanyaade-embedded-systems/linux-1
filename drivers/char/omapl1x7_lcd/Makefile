#
# Makefile for the Linux lcd driver.
#

obj-$(CONFIG_DA830_LCD) += da830_lcd.o

da830_lcd-objs := lcd_drv.o  lidd_hal.o  hd44780_hal.o

da830_lcd.o:      $(da830_lcd-objs)
	$(LD) -r -o $@ $(da830_lcd-objs)
