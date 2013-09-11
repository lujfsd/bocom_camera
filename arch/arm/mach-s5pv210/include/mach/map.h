/* linux/arch/arm/mach-s5pv210/include/mach/map.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * S5PV210 - Memory map definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_MAP_H
#define __ASM_ARCH_MAP_H __FILE__

#include <plat/map-base.h>
#include <plat/map-s5p.h>

#define S5PV210_PA_DM9000       (0xA8000000)
#define S5P_PA_DM9000           S5PV210_PA_DM9000

#define S5PC110_PA_ONENAND	(0xB0000000)
#define S5PC110_PA_ONENAND_DMA	(0xB0600000)

/* NAND */
#define S5PV210_PA_NAND		(0xB0E00000)
#define S5P_PA_NAND		S5PV210_PA_NAND

#define S5PV210_SZ_NAND		SZ_1M
#define S5P_SZ_NAND		S5PV210_SZ_NAND

#define S5PV210_PA_CHIPID	(0xE0000000)
#define S5P_PA_CHIPID		S5PV210_PA_CHIPID

#define S5PV210_PA_SYSCON	(0xE0100000)
#define S5P_PA_SYSCON		S5PV210_PA_SYSCON

#define S5PV210_PA_GPIO		(0xE0200000)
#define S5P_PA_GPIO		S5PV210_PA_GPIO

/* SPI */
#define S5PV210_PA_SPI0		0xE1300000
#define S5PV210_PA_SPI1		0xE1400000

#define S5PV210_PA_IIC0		(0xE1800000)
#define S5PV210_PA_IIC1		(0xFAB00000)
#define S5PV210_PA_IIC2		(0xE1A00000)

#define S5PV210_PA_TIMER	(0xE2500000)
#define S5P_PA_TIMER		S5PV210_PA_TIMER

#define S5PV210_PA_SYSTIMER	(0xE2600000)

#define S5PV210_PA_WDT		(0xE2700000)
#define S5P_PA_WDT		S5PV210_PA_WDT

#define S5PV210_PA_RTC          (0xE2800000)
#define S5P_PA_RTC		S5PV210_PA_RTC

#define S5PV210_VA_RTC          S3C_ADDR(0x00c00000)
#define S5P_VA_RTC		S5PV210_VA_RTC

#define S5PV210_PA_WATCHDOG	(0xE2700000)

#define S5PV210_PA_UART		(0xE2900000)

#define S5P_PA_UART0		(S5PV210_PA_UART + 0x0)
#define S5P_PA_UART1		(S5PV210_PA_UART + 0x400)
#define S5P_PA_UART2		(S5PV210_PA_UART + 0x800)
#define S5P_PA_UART3		(S5PV210_PA_UART + 0xC00)

#define S5P_SZ_UART		SZ_256

#define S5P_SZ_UART_FULL	SZ_1K
#define S3C_VA_UARTx(uart) (S3C_VA_UART + ((uart * S5P_SZ_UART_FULL)))

#define S5PV210_PA_SROMC	(0xE8000000)

/* usb */
#define S5PV210_PA_OTG          (0xEC000000)
#define S5PV210_SZ_OTG          SZ_1M

#define S5PV210_PA_OTGSFR       (0xEC100000)
#define S5PV210_SZ_OTGSFR       SZ_1M

#define S5PV210_PA_USB_EHCI	(0xEC200000)
#define S5P_PA_USB_EHCI		S5PV210_PA_USB_EHCI
#define S5P_SZ_USB_EHCI     	SZ_1M

#define S5PV210_PA_USB_OHCI	(0xEC300000)
#define S5P_PA_USB_OHCI		S5PV210_PA_USB_OHCI
#define S5P_SZ_USB_OHCI     	SZ_1M
/* end usb */

#define S5PV210_PA_MDMA		0xFA200000
#define S5PV210_PA_PDMA0	0xE0900000
#define S5PV210_PA_PDMA1	0xE0A00000

#define S5PV210_PA_FB		(0xF8000000)

#define S5PV210_PA_HSMMC(x)	(0xEB000000 + ((x) * 0x100000))

#define S5PV210_PA_VIC0		(0xF2000000)
#define S5P_PA_VIC0		S5PV210_PA_VIC0

#define S5PV210_PA_VIC1		(0xF2100000)
#define S5P_PA_VIC1		S5PV210_PA_VIC1

#define S5PV210_PA_VIC2		(0xF2200000)
#define S5P_PA_VIC2		S5PV210_PA_VIC2

#define S5PV210_PA_VIC3		(0xF2300000)
#define S5P_PA_VIC3		S5PV210_PA_VIC3

#define S5P_VA_VIC0             (S3C_VA_IRQ + 0x0)
#define S5P_VA_VIC1             (S3C_VA_IRQ + 0x10000)
#define S5P_VA_VIC2             (S3C_VA_IRQ + 0x20000)
#define S5P_VA_VIC3             (S3C_VA_IRQ + 0x30000)

#define S5PV210_PA_LCD	   	(0xF8000000)
#define S5P_PA_LCD		S5PV210_PA_LCD
#define S5PV210_SZ_LCD		SZ_1M
#define S5P_SZ_LCD		S5PV210_SZ_LCD

#define S5PV210_PA_CSIS		(0xFA600000)
#define S5P_PA_CSIS		S5PV210_PA_CSIS
#define S5PV210_SZ_CSIS		SZ_1M
#define S5P_SZ_CSIS		S5PV210_SZ_CSIS

#define S5PV210_PA_MFC          (0xF1700000)
#define S5P_PA_MFC		S5PV210_PA_MFC
#define S5PV210_SZ_MFC          SZ_1M
#define S5P_SZ_MFC		S5PV210_SZ_MFC

#define S5PV210_PA_JPEG		(0xFB600000)
#define S5PV210_SZ_JPEG		SZ_1M

#define S5PV210_PA_FIMC0	(0xFB200000)
#define S5P_PA_FIMC0		S5PV210_PA_FIMC0
#define S5PV210_PA_FIMC1	(0xFB300000)
#define S5P_PA_FIMC1		S5PV210_PA_FIMC1
#define S5PV210_PA_FIMC2	(0xFB400000)
#define S5P_PA_FIMC2		S5PV210_PA_FIMC2
#define S5PV210_SZ_FIMC0	SZ_1M
#define S5P_SZ_FIMC0		S5PV210_SZ_FIMC0
#define S5PV210_SZ_FIMC1	SZ_1M
#define S5P_SZ_FIMC1		S5PV210_SZ_FIMC1
#define S5PV210_SZ_FIMC2	SZ_1M
#define S5P_SZ_FIMC2		S5PV210_SZ_FIMC2

#define S5PV210_PA_IPC		(0xFB700000)
#define S5P_PA_IPC		S5PV210_PA_IPC
#define S5PV210_SZ_IPC		SZ_1M
#define S5P_SZ_IPC		S5PV210_SZ_IPC

#if defined(CONFIG_MACH_SMDKV210) || defined(CONFIG_MACH_GZSD210)
#define S5PV210_PA_SDRAM	(0x20000000)
#else
#define S5PV210_PA_SDRAM	(0x30000000)
#endif
#define S5P_PA_SDRAM		S5PV210_PA_SDRAM

/* KEYPAD IF */
#define S5PV2XX_SZ_KEYPAD       SZ_4K

#define S5PV2XX_PA_ADC          (0xE1700000)
#define S3C_PA_ADC              S5PV2XX_PA_ADC

#define S5PV2XX_PA_KEYPAD       (0xE1600000)
#define S3C_PA_KEYPAD           S5PV2XX_PA_KEYPAD
#define S3C_SZ_KEYPAD           S5PV2XX_SZ_KEYPAD

#define S5PV210_PA_AUDSS	(0xEEE10000)

/* I2S */
#define S5PV210_PA_IIS0		0xEEE30000
#define S5PV210_PA_IIS1		0xE2100000
#define S5PV210_PA_IIS2		0xE2A00000

/* PCM */
#define S5PV210_PA_PCM0		0xE2300000
#define S5PV210_PA_PCM1		0xE1200000
#define S5PV210_PA_PCM2		0xE2B00000

/* AC97 */
#define S5PV210_PA_AC97		0xE2200000

/* SPDIF */
#define S5PV210_PA_SPDIF	0xE1100000

#define S5PV210_PA_ADC		(0xE1700000)

/* mfc */
#define S5PV210_PA_MFC		(0xF1700000)
#define S5PV210_SZ_MFC		SZ_1M
#define S5P_PA_MFC		S5PV210_PA_MFC
#define S5P_SZ_MFC		S5PV210_SZ_MFC


/* jpeg */
#define S5PV210_PA_JPEG		(0xFB600000)
#define S5P_PA_JPEG		S5PV210_PA_JPEG
#define S5P_SZ_JPEG		SZ_1M

/* rotator */
#define S5PV210_PA_ROTATOR	(0xFA300000)
#define S5P_PA_ROTATOR		S5PV210_PA_ROTATOR
#define S5P_SZ_ROTATOR		SZ_1M

/* fimg2d */
#define S5PV210_PA_FIMG2D	(0xFA000000)
#define S5P_PA_FIMG2D		S5PV210_PA_FIMG2D
#define S5P_SZ_FIMG2D		SZ_1M

/* compatibiltiy defines. */
#define S3C_PA_UART		S5PV210_PA_UART
#define S3C_PA_HSMMC0		S5PV210_PA_HSMMC(0)
#define S3C_PA_HSMMC1		S5PV210_PA_HSMMC(1)
#define S3C_PA_HSMMC2		S5PV210_PA_HSMMC(2)
#define S3C_PA_HSMMC3		S5PV210_PA_HSMMC(3)
#define S3C_PA_IIC		S5PV210_PA_IIC0
#define S3C_PA_IIC1		S5PV210_PA_IIC1
#define S3C_PA_IIC2		S5PV210_PA_IIC2
#define S3C_PA_FB		S5PV210_PA_FB
#define S3C_PA_WDT		S5PV210_PA_WATCHDOG

#define SAMSUNG_PA_ADC		S5PV210_PA_ADC

/* CEC */
#define S5PV210_PA_CEC		(0xE1B00000)
#define S5P_PA_CEC		S5PV210_PA_CEC
#define S5P_SZ_CEC		SZ_4K

/* TVOUT */
#define S5PV210_PA_TVENC	(0xF9000000)
#define S5P_PA_TVENC		S5PV210_PA_TVENC
#define S5P_SZ_TVENC		SZ_1M

#define S5PV210_PA_VP		(0xF9100000)
#define S5P_PA_VP		S5PV210_PA_VP
#define S5P_SZ_VP		SZ_1M

#define S5PV210_PA_MIXER	(0xF9200000)
#define S5P_PA_MIXER		S5PV210_PA_MIXER
#define S5P_SZ_MIXER		SZ_1M

#define S5PV210_PA_HDMI		(0xFA100000)
#define S5P_PA_HDMI		S5PV210_PA_HDMI
#define S5P_SZ_HDMI		SZ_1M

#define S5PV210_I2C_HDMI_PHY	(0xFA900000)
#define S5P_I2C_HDMI_PHY	S5PV210_I2C_HDMI_PHY
#define S5P_I2C_HDMI_SZ_PHY	SZ_1K

#define S5P_PA_DMC0		(0xF0000000)
#define S5P_VA_DMC0		S3C_ADDR(0x00800000)

#define S5P_PA_DMC1		(0xF1400000)
#define S5P_VA_DMC1		S3C_ADDR(0x00900000)

#define S5P_VA_BUS_AXI_DSYS	S3C_ADDR_CPU(0x6000)
#define S5PV210_PA_BUS_AXI_DSYS	(0xFA700000)

/* usb */
#define S3C_PA_OTG              S5PV210_PA_OTG
#define S3C_SZ_OTG              S5PV210_SZ_OTG

#define S3C_PA_OTGSFR           S5PV210_PA_OTGSFR
#define S3C_SZ_OTGSFR           S5PV210_SZ_OTGSFR

/* end usb */
#endif /* __ASM_ARCH_MAP_H */
