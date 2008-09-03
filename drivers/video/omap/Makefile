#
# Makefile for the new OMAP framebuffer device driver
#

obj-$(CONFIG_FB_OMAP) += omapfb.o

objs-yy := omapfb_main.o

objs-y$(CONFIG_ARCH_OMAP1) += lcdc.o
objs-y$(CONFIG_ARCH_OMAP2) += dispc.o
objs-y$(CONFIG_ARCH_OMAP3) += dispc.o

objs-$(CONFIG_ARCH_OMAP1)$(CONFIG_FB_OMAP_LCDC_EXTERNAL) += sossi.o
objs-$(CONFIG_ARCH_OMAP2)$(CONFIG_FB_OMAP_LCDC_EXTERNAL) += rfbi.o

objs-y$(CONFIG_FB_OMAP_LCDC_HWA742) += hwa742.o
objs-y$(CONFIG_FB_OMAP_LCDC_BLIZZARD) += blizzard.o

obj-$(CONFIG_OMAP_FB_LCD_AMS_DELTA) += lcd_ams_delta.o
obj-$(CONFIG_OMAP_FB_LCD_H4) += lcd_h4.o
obj-$(CONFIG_OMAP_FB_LCD_H3) += lcd_h3.o
obj-$(CONFIG_OMAP_FB_LCD_H2) += lcd_h2.o
obj-$(CONFIG_OMAP_FB_LCD_PALMTE) += lcd_palmte.o
obj-$(CONFIG_OMAP_FB_LCD_PALMTT) += lcd_palmtt.o
obj-$(CONFIG_OMAP_FB_LCD_PALMZ71) += lcd_palmz71.o
obj-$(CONFIG_OMAP_FB_LCD_INN1610) += lcd_inn1610.o
obj-$(CONFIG_OMAP_FB_LCD_INN1510) += lcd_inn1510.o
obj-$(CONFIG_OMAP_FB_LCD_OSK) += lcd_osk.o
obj-$(CONFIG_OMAP_FB_LCD_SX1) += lcd_sx1.o

obj-$(CONFIG_OMAP_FB_LCD_APOLLON) += lcd_apollon.o
obj-$(CONFIG_OMAP_FB_LCD_2430SDP) += lcd_2430sdp.o
obj-$(CONFIG_OMAP_FB_LCD_2430SDP) += lcd_2430sdp.o
obj-$(CONFIG_OMAP_FB_LCD_OMAP2EVM) += lcd_omap2evm.o
obj-$(CONFIG_OMAP_FB_LCD_OMAP3EVM) += lcd_omap3evm.o
obj-$(CONFIG_OMAP_FB_LCD_OMAP3BEAGLE) += lcd_omap3beagle.o
obj-$(CONFIG_OMAP_FB_LCD_MIPID) += lcd_mipid.o

omapfb-objs := $(objs-yy)

