#
# Makefile for the Linux lcd driver.
#

obj-$(CONFIG_OMAPL1X7_LCD) += omapl1x7_lcd.o

omapl1x7_lcd-objs := lcd_drv.o  lidd_hal.o  hd44780_hal.o

omapl1x7_lcd.o:      $(omapl1x7_lcd-objs)
	$(LD) -r -o $@ $(omapl1x7_lcd-objs)
