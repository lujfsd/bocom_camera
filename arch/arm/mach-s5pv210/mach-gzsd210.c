/* linux/arch/arm/mach-s5pv210/mach-mini210.c
 *
 * Copyright (c) 2011 FriendlyARM (www.arm9.net)
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/gpio.h>
#include <linux/gpio_event.h>
#include <linux/gpio_keys.h>
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/usb/ch9.h>
#include <linux/pwm_backlight.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/console.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/system.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-mem.h>
#include <mach/gpio.h>
#include <mach/gpio-smdkc110.h>
#include <mach/regs-gpio.h>
#include <mach/ts-s3c.h>
#include <mach/adc.h>
#include <mach/param.h>
#include <mach/system.h>

#ifdef CONFIG_S3C64XX_DEV_SPI
#include <plat/s3c64xx-spi.h>
#include <mach/spi-clocks.h>
#endif

#include <linux/usb/gadget.h>

#include <plat/media.h>
#include <mach/media.h>

#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
#endif

#ifdef CONFIG_S5PV210_POWER_DOMAIN
#include <mach/power-domain.h>
#endif


#ifdef CONFIG_CAMERA_OV3640
#include <media/ov3640_platform.h>
#define	CAM_ITU_CH_B
#endif


#ifdef CONFIG_CAMERA_TVP5150_ONBOARD
#include <media/tvp5150.h>
#define	CAM_ITU_CH_A
#endif

#ifdef CONFIG_CAMERA_OV2640_GZSD
#include <media/ov2640.h>
#define	CAM_ITU_CH_B
#endif

#include <plat/regs-serial.h>
#include <plat/s5pv210.h>
#include <plat/devs.h>
#include <plat/cpu.h>
#include <plat/fb.h>
#include <plat/mfc.h>
#include <plat/iic.h>
#include <plat/pm.h>

#include <plat/sdhci.h>
#include <plat/fimc.h>
#include <plat/csis.h>
#include <plat/jpeg.h>
#include <plat/clock.h>
#include <plat/regs-otg.h>
#include <../../../drivers/video/samsung/s3cfb.h>

extern struct s3cfb_lcd *gzsd210_get_lcd(void);

/* Following are default values for UCON, ULCON and UFCON UART registers */
#define S5PV210_UCON_DEFAULT	(S3C2410_UCON_TXILEVEL |	\
				 S3C2410_UCON_RXILEVEL |	\
				 S3C2410_UCON_TXIRQMODE |	\
				 S3C2410_UCON_RXIRQMODE |	\
				 S3C2410_UCON_RXFIFO_TOI |	\
				 S3C2443_UCON_RXERR_IRQEN)

#define S5PV210_ULCON_DEFAULT	S3C2410_LCON_CS8

#define S5PV210_UFCON_DEFAULT	(S3C2410_UFCON_FIFOMODE |	\
				 S5PV210_UFCON_TXTRIG4 |	\
				 S5PV210_UFCON_RXTRIG4)

static struct s3c2410_uartcfg gzsd210_uartcfgs[] __initdata = {
	{
		.hwport		= 0,
		.flags		= 0,
		.ucon		= S5PV210_UCON_DEFAULT,
		.ulcon		= S5PV210_ULCON_DEFAULT,
		.ufcon		= S5PV210_UFCON_DEFAULT,
	},
	{
		.hwport		= 1,
		.flags		= 0,
		.ucon		= S5PV210_UCON_DEFAULT,
		.ulcon		= S5PV210_ULCON_DEFAULT,
		.ufcon		= S5PV210_UFCON_DEFAULT,
	},
#ifndef CONFIG_FIQ_DEBUGGER
	{
		.hwport		= 2,
		.flags		= 0,
		.ucon		= S5PV210_UCON_DEFAULT,
		.ulcon		= S5PV210_ULCON_DEFAULT,
		.ufcon		= S5PV210_UFCON_DEFAULT,
	},
#endif
	{
		.hwport		= 3,
		.flags		= 0,
		.ucon		= S5PV210_UCON_DEFAULT,
		.ulcon		= S5PV210_ULCON_DEFAULT,
		.ufcon		= S5PV210_UFCON_DEFAULT,
	},
};

#define LCD_WIDTH			800
#define LCD_HEIGHT			480
#define BYTES_PER_PIXEL		4
#define PXL2FIMD(pixels)	\
		((pixels) * BYTES_PER_PIXEL * CONFIG_FB_S3C_NR_BUFFERS)

#define S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC0		(24576 * SZ_1K)
#define S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC1		( 9900 * SZ_1K)
#define S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC2		(24576 * SZ_1K)
#define S5PV210_VIDEO_SAMSUNG_MEMSIZE_MFC0		(36864 * SZ_1K)
#define S5PV210_VIDEO_SAMSUNG_MEMSIZE_MFC1		(36864 * SZ_1K)
#define S5PV210_VIDEO_SAMSUNG_MEMSIZE_JPEG		( 8192 * SZ_1K)
#define S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMD	\
		PXL2FIMD(LCD_WIDTH * LCD_HEIGHT)

/* 1920 * 1080 * 4 (RGBA)
 * - framesize == 1080p : 1920 * 1080 * 2(16bpp) * 2(double buffer) = 8MB
 * - framesize <  1080p : 1080 *  720 * 4(32bpp) * 2(double buffer) = under 8MB
 **/
#define S5PV210_VIDEO_SAMSUNG_MEMSIZE_G2D		( 8192 * SZ_1K)
#define S5PV210_VIDEO_SAMSUNG_MEMSIZE_TEXSTREAM	( 4096 * SZ_1K)
#define S5PV210_ANDROID_PMEM_MEMSIZE_PMEM		( 5550 * SZ_1K)
#define S5PV210_ANDROID_PMEM_MEMSIZE_PMEM_GPU1	( 3300 * SZ_1K)
#define S5PV210_ANDROID_PMEM_MEMSIZE_PMEM_ADSP	( 1500 * SZ_1K)

static struct s5p_media_device gzsd210_media_devs[] = {
	[0] = {
		.id = S5P_MDEV_MFC,
		.name = "mfc",
		.bank = 0,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_MFC0,
		.paddr = 0,
	},
	[1] = {
		.id = S5P_MDEV_MFC,
		.name = "mfc",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_MFC1,
		.paddr = 0,
	},
	[2] = {
		.id = S5P_MDEV_FIMC0,
		.name = "fimc0",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC0,
		.paddr = 0,
	},
	[3] = {
		.id = S5P_MDEV_FIMC1,
		.name = "fimc1",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC1,
		.paddr = 0,
	},
	[4] = {
		.id = S5P_MDEV_FIMC2,
		.name = "fimc2",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMC2,
		.paddr = 0,
	},
	[5] = {
		.id = S5P_MDEV_JPEG,
		.name = "jpeg",
		.bank = 0,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_JPEG,
		.paddr = 0,
	},
	[6] = {
		.id = S5P_MDEV_FIMD,
		.name = "fimd",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMD,
		.paddr = 0,
	},
	[7] = {
		.id = S5P_MDEV_TEXSTREAM,
		.name = "texstream",
		.bank = 1,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_TEXSTREAM,
		.paddr = 0,
	},
	[8] = {
		.id = S5P_MDEV_PMEM,
		.name = "pmem",
		.bank = 0,
		.memsize = S5PV210_ANDROID_PMEM_MEMSIZE_PMEM,
		.paddr = 0,
	},
	[9] = {
		.id = S5P_MDEV_PMEM_GPU1,
		.name = "pmem_gpu1",
		.bank = 0, /* OneDRAM */
		.memsize = S5PV210_ANDROID_PMEM_MEMSIZE_PMEM_GPU1,
		.paddr = 0,
	},
	[10] = {
		.id = S5P_MDEV_PMEM_ADSP,
		.name = "pmem_adsp",
		.bank = 0,
		.memsize = S5PV210_ANDROID_PMEM_MEMSIZE_PMEM_ADSP,
		.paddr = 0,
	},
	[11] = {
		.id = S5P_MDEV_G2D,
		.name = "g2d",
		.bank = 0,
		.memsize = S5PV210_VIDEO_SAMSUNG_MEMSIZE_G2D,
		.paddr = 0,
	},
};

static void gzsd210_fixup_bootmem(int id, unsigned int size) {
	int i;

	for (i = 0; i < ARRAY_SIZE(gzsd210_media_devs); i++) {
		if (gzsd210_media_devs[i].id == id) {
			gzsd210_media_devs[i].memsize = size;
		}
	}
}



static void __init gzsd210_setup_clocks(void)
{
	struct clk *pclk;
	struct clk *clk;

#ifdef CONFIG_S3C_DEV_HSMMC
	/* set MMC0 clock */
	clk = clk_get(&s3c_device_hsmmc0.dev, "sclk_mmc");
	pclk = clk_get(NULL, "mout_mpll");
	clk_set_parent(clk, pclk);
	clk_set_rate(clk, 50*MHZ);

	pr_info("%s: %s: source is %s, rate is %ld\n", __func__, clk->name,
			clk->parent->name, clk_get_rate(clk));
#endif

#ifdef CONFIG_S3C_DEV_HSMMC1
	/* set MMC1 clock */
	clk = clk_get(&s3c_device_hsmmc1.dev, "sclk_mmc");
	pclk = clk_get(NULL, "mout_mpll");
	clk_set_parent(clk, pclk);
	clk_set_rate(clk, 50*MHZ);

	pr_info("%s: %s: source is %s, rate is %ld\n", __func__, clk->name,
			clk->parent->name, clk_get_rate(clk));
#endif

#ifdef CONFIG_S3C_DEV_HSMMC2
	/* set MMC2 clock */
	clk = clk_get(&s3c_device_hsmmc2.dev, "sclk_mmc");
	pclk = clk_get(NULL, "mout_mpll");
	clk_set_parent(clk, pclk);
	clk_set_rate(clk, 50*MHZ);

	pr_info("%s: %s: source is %s, rate is %ld\n", __func__, clk->name,
			clk->parent->name, clk_get_rate(clk));
#endif

#ifdef CONFIG_S3C_DEV_HSMMC3
	/* set MMC3 clock */
	clk = clk_get(&s3c_device_hsmmc3.dev, "sclk_mmc");
	pclk = clk_get(NULL, "mout_mpll");
	clk_set_parent(clk, pclk);
	clk_set_rate(clk, 50*MHZ);

	pr_info("%s: %s: source is %s, rate is %ld\n", __func__, clk->name,
			clk->parent->name, clk_get_rate(clk));
#endif
}

#if defined(CONFIG_TOUCHSCREEN_S3C)
static struct s3c_ts_mach_info s3c_ts_platform __initdata = {
	.delay                  = 10000,
	.presc                  = 49,
	.oversampling_shift     = 2,
	.resol_bit              = 12,
	.s3c_adc_con            = ADC_TYPE_2,
};

/* Touch srcreen */
static struct resource s3c_ts_resource[] = {
	[0] = {
		.start = S3C_PA_ADC,
		.end   = S3C_PA_ADC + SZ_4K - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = IRQ_PENDN,
		.end   = IRQ_PENDN,
		.flags = IORESOURCE_IRQ,
	},
	[2] = {
		.start = IRQ_ADC,
		.end   = IRQ_ADC,
		.flags = IORESOURCE_IRQ,
	}
};

struct platform_device s3c_device_ts = {
	.name		  = "s3c-ts",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(s3c_ts_resource),
	.resource	  = s3c_ts_resource,
};

void __init s3c_ts_set_platdata(struct s3c_ts_mach_info *pd)
{
	struct s3c_ts_mach_info *npd;

	npd = kmalloc(sizeof(*npd), GFP_KERNEL);
	if (npd) {
		memcpy(npd, pd, sizeof(*npd));
		s3c_device_ts.dev.platform_data = npd;
	} else {
		pr_err("no memory for Touchscreen platform data\n");
	}
}
#endif

#ifdef CONFIG_KEYBOARD_GPIO
static struct gpio_keys_button gpio_buttons[] = {
	{
		.gpio		= S5PV210_GPD0(3),
		.code		= 103,
		.desc		= "DPAD_UP",
		.active_low	= 1,
		.wakeup		= 0,
	}, {
		.gpio		= S5PV210_GPD0(2),
		.code		= 108,
		.desc		= "DPAD_DOWN",
		.active_low	= 1,
		.wakeup		= 1,
	}, {
		.gpio		= S5PV210_GPB(4),
		.code		= 158,
		.desc		= "DPAD_BACK",
		.active_low	= 1,
		.wakeup		= 0,
	}, {
		.gpio		= S5PV210_GPB(5),
		.code		= 28,
		.desc		= "DPAD_ENTER",
		.active_low	= 1,
		.wakeup		= 0,
	}, {
		.gpio		= S5PV210_GPB(6),
		.code		= 105,
		.desc		= "DPAD_LEFT",
		.active_low	= 1,
		.wakeup		= 0,
	}, {
		.gpio		= S5PV210_GPB(7),
		.code		= 106,
		.desc		= "DPAD_RIGHT",
		.active_low	= 1,
		.wakeup		= 0,
	}
};

static struct gpio_keys_platform_data gpio_button_data = {
	.buttons	= gpio_buttons,
	.nbuttons	= ARRAY_SIZE(gpio_buttons),
};

static struct platform_device s3c_device_gpio_btns = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev		= {
		.platform_data	= &gpio_button_data,
	}
};
#endif



#if defined(CONFIG_KEYPAD_S3C) || defined(CONFIG_KEYPAD_S3C_MODULE)
#if defined(CONFIG_KEYPAD_S3C_MSM)
void s3c_setup_keypad_cfg_gpio(void)
{
	unsigned int gpio;
	unsigned int end;

	/* gpio setting for KP_COL0 */
	s3c_gpio_cfgpin(S5PV210_GPJ1(5), S3C_GPIO_SFN(3));
	s3c_gpio_setpull(S5PV210_GPJ1(5), S3C_GPIO_PULL_NONE);

	/* gpio setting for KP_COL1 ~ KP_COL7 and KP_ROW0 */
	end = S5PV210_GPJ2(8);
	for (gpio = S5PV210_GPJ2(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}

	/* gpio setting for KP_ROW1 ~ KP_ROW8 */
	end = S5PV210_GPJ3(8);
	for (gpio = S5PV210_GPJ3(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}

	/* gpio setting for KP_ROW9 ~ KP_ROW13 */
	end = S5PV210_GPJ4(5);
	for (gpio = S5PV210_GPJ4(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}
}
#else
void s3c_setup_keypad_cfg_gpio(int rows, int columns)
{
	unsigned int gpio;
	unsigned int end;

	end = S5PV210_GPH3(rows);

	/* Set all the necessary GPH2 pins to special-function 0 */
	for (gpio = S5PV210_GPH3(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
	}

	end = S5PV210_GPH2(columns);

	/* Set all the necessary GPK pins to special-function 0 */
	for (gpio = S5PV210_GPH2(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}
}
#endif /* if defined(CONFIG_KEYPAD_S3C_MSM)*/
EXPORT_SYMBOL(s3c_setup_keypad_cfg_gpio);
#endif
#if	0
static void __init gzsd210_wifi_init(void)
{

	
	int err;

	err = gpio_request(S5PV210_GPH1(6), "GPH1");
	if (err) {
		printk(KERN_ERR "failed to request GPH0 for "
				"lcd reset control\n");
		return err;
	}

	gpio_direction_output(S5PV210_GPH1(6), 1);
	mdelay(1);

	gpio_set_value(S5PV210_GPH1(6), 0);
	mdelay(100);

	gpio_set_value(S5PV210_GPH1(6), 1);
	mdelay(10);

	gpio_free(S5PV210_GPH1(6));


       err = gpio_request(S5PV210_GPH1(7), "GPH1");
	if (err) {
		printk(KERN_ERR "failed to request GPH0 for "
				"lcd reset control\n");
		return err;
	}
	gpio_direction_output(S5PV210_GPH1(7), 1);
	gpio_set_value(S5PV210_GPH1(7), 1);
	gpio_free(S5PV210_GPH1(6));
	
}
#endif
#ifdef CONFIG_DM9000
#include <linux/dm9000.h>

/* physical address for dm9000a ...kgene.kim@samsung.com */
#define S5PV210_PA_DM9000_A     (0x88001000)
#define S5PV210_PA_DM9000_F     (S5PV210_PA_DM9000_A + 0x300C)

static struct resource dm9000_resources[] = {
	[0] = {
		.start	= S5PV210_PA_DM9000_A,
		.end	= S5PV210_PA_DM9000_A + SZ_1K*4 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= S5PV210_PA_DM9000_F,
		.end	= S5PV210_PA_DM9000_F + SZ_1K*4 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[2] = {
		.start	= IRQ_EINT(9),
		.end	= IRQ_EINT(9),
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	},
};

static struct dm9000_plat_data dm9000_platdata = {
	.flags		= DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM,
	.dev_addr	= { 0x08, 0x90, 0x00, 0xa0, 0x02, 0x10 },
};

struct platform_device gzsd210_device_dm9000 = {
	.name		= "dm9000",
	.id			= -1,
	.num_resources	= ARRAY_SIZE(dm9000_resources),
	.resource	= dm9000_resources,
	.dev		= {
		.platform_data	= &dm9000_platdata,
	},
};

static int __init dm9000_set_mac(char *str) {
	unsigned char addr[6];
	unsigned int val;
	int idx = 0;
	char *p = str, *end;

	while (*p && idx < 6) {
		val = simple_strtoul(p, &end, 16);
		if (end <= p) {
			/* convert failed */
			break;
		} else {
			addr[idx++] = val;
			p = end;
			if (*p == ':'|| *p == '-') {
				p++;
			} else {
				break;
			}
		}
	}

	if (idx == 6) {
		printk("Setup ethernet address to %pM\n", addr);
		memcpy(dm9000_platdata.param_addr, addr, 6);
	}

	return 1;
}

__setup("ethmac=", dm9000_set_mac);

static void __init gzsd210_dm9000_set(void)
{
	unsigned int tmp;

	tmp = ((0<<28)|(0<<24)|(5<<16)|(0<<12)|(0<<8)|(0<<4)|(0<<0));
	__raw_writel(tmp, (S5P_SROM_BW+0x08));

	tmp = __raw_readl(S5P_SROM_BW);
	tmp &= ~(0xf << 4);
	tmp |= (0x1 << 4); /* dm9000 16bit */
	__raw_writel(tmp, S5P_SROM_BW);

    gpio_request(S5PV210_MP01(1), "nCS1");
    s3c_gpio_cfgpin(S5PV210_MP01(1), S3C_GPIO_SFN(2));
    gpio_free(S5PV210_MP01(1));
}
#endif

//#ifdef CONFIG_FB_S3C_GZSD210
static void lcd_cfg_gpio(struct platform_device *pdev)
{
	int i;

	for (i = 0; i < 8; i++) {
		s3c_gpio_cfgpin(S5PV210_GPF0(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV210_GPF0(i), S3C_GPIO_PULL_NONE);
	}

	for (i = 0; i < 8; i++) {
		s3c_gpio_cfgpin(S5PV210_GPF1(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV210_GPF1(i), S3C_GPIO_PULL_NONE);
	}

	for (i = 0; i < 8; i++) {
		s3c_gpio_cfgpin(S5PV210_GPF2(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV210_GPF2(i), S3C_GPIO_PULL_NONE);
	}

	for (i = 0; i < 4; i++) {
		s3c_gpio_cfgpin(S5PV210_GPF3(i), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5PV210_GPF3(i), S3C_GPIO_PULL_NONE);
	}

	/* mDNIe SEL: why we shall write 0x2 ? */
	writel(0x2, S5P_MDNIE_SEL);

	/* drive strength to max */
	writel(0xaaaaaaaa, S5PV210_GPF0_BASE + 0xc);
	writel(0xaaaaaaaa, S5PV210_GPF1_BASE + 0xc);
	writel(0xaaaaaaaa, S5PV210_GPF2_BASE + 0xc);
	writel(0x000000aa, S5PV210_GPF3_BASE + 0xc);
}

#define S5PV210_GPD_0_0_TOUT_0  (0x2)
#define S5PV210_GPD_0_1_TOUT_1  (0x2 << 4)
#define S5PV210_GPD_0_2_TOUT_2  (0x2 << 8)
#define S5PV210_GPD_0_3_TOUT_3  (0x2 << 12)
static int lcd_backlight_on(struct platform_device *pdev)
{
	int err;
	err = gpio_request(S5PV210_GPD0(0), "GPD0");

	if (err) {
		printk(KERN_ERR "failed to request GPD0 for "
			"lcd backlight control\n");
		return err;
	}

	gpio_direction_output(S5PV210_GPD0(0), 1);

	//s3c_gpio_cfgpin(S5PV210_GPD0(0), S5PV210_GPD_0_3_TOUT_3);

	gpio_free(S5PV210_GPD0(0));
	return 0;
}

static int lcd_backlight_off(struct platform_device *pdev, int onoff)
{
	int err;

	
	err = gpio_request(S5PV210_GPD0(0), "GPD0");

	if (err) {
		printk(KERN_ERR "failed to request GPD0 for "
			"lcd backlight control\n");
		return err;
	}

	gpio_direction_output(S5PV210_GPD0(0), 0);

	//s3c_gpio_cfgpin(S5PV210_GPD0(0), S5PV210_GPD_0_3_TOUT_3);

	gpio_free(S5PV210_GPD0(0));
	return 0;
}

static int lcd_reset_lcd(struct platform_device *pdev)
{
	int err;

	err = gpio_request(S5PV210_GPH0(6), "GPH0");
	if (err) {
		printk(KERN_ERR "failed to request GPH0 for "
				"lcd reset control\n");
		return err;
	}

	gpio_direction_output(S5PV210_GPH0(6), 1);
	mdelay(100);

	gpio_set_value(S5PV210_GPH0(6), 0);
	mdelay(10);

	gpio_set_value(S5PV210_GPH0(6), 1);
	mdelay(10);

	gpio_free(S5PV210_GPH0(6));

	return 0;
}

static struct s3c_platform_fb gzsd210_fb_data __initdata = {
	.hw_ver	= 0x62,
	.nr_wins = 5,
	.default_win = CONFIG_FB_S3C_DEFAULT_WINDOW,
	.swap = FB_SWAP_WORD | FB_SWAP_HWORD,

	.cfg_gpio	= lcd_cfg_gpio,
	.backlight_on	= lcd_backlight_on,
	.backlight_onoff    = lcd_backlight_off,
	.reset_lcd	= lcd_reset_lcd,
};
//#endif

#ifdef CONFIG_S3C64XX_DEV_SPI

#define SMDK_MMCSPI_CS 0
static struct s3c64xx_spi_csinfo smdk_spi0_csi[] = {
	[SMDK_MMCSPI_CS] = {
		.line = S5PV210_GPB(1),
		.set_level = gpio_set_value,
		.fb_delay = 0x0,
	},
};

static struct s3c64xx_spi_csinfo smdk_spi1_csi[] = {
	[SMDK_MMCSPI_CS] = {
		.line = S5PV210_GPB(5),
		.set_level = gpio_set_value,
		.fb_delay = 0x0,
	},
};

static struct spi_board_info s3c_spi_devs[] __initdata = {
	[0] = {
		.modalias        = "spidev",	/* device node name */
		.mode            = SPI_MODE_0,	/* CPOL=0, CPHA=0 */
		.max_speed_hz    = 10000000,
		/* Connected to SPI-0 as 1st Slave */
		.bus_num         = 0,
		.irq             = IRQ_SPI0,
		.chip_select     = 0,
		.controller_data = &smdk_spi0_csi[SMDK_MMCSPI_CS],
	},
	[1] = {
		.modalias        = "spidev",	/* device node name */
		.mode            = SPI_MODE_0,	/* CPOL=0, CPHA=0 */
		.max_speed_hz    = 10000000,
		/* Connected to SPI-1 as 1st Slave */
		.bus_num         = 1,
		.irq             = IRQ_SPI1,
		.chip_select     = 0,
		.controller_data = &smdk_spi1_csi[SMDK_MMCSPI_CS],
	},
};
#endif

#ifdef CONFIG_HAVE_PWM
struct s3c_pwm_data {
	/* PWM output port */
	unsigned int gpio_no;
	const char  *gpio_name;
	unsigned int gpio_set_value;
};

struct s3c_pwm_data pwm_data[] = {

};
#endif

#if defined(CONFIG_BACKLIGHT_PWM)
static struct platform_pwm_backlight_data smdk_backlight_data = {
	.pwm_id  = 3,
	.max_brightness = 255,
	.dft_brightness = 255,
	.pwm_period_ns  = 25000,
};

static struct platform_device smdk_backlight_device = {
	.name      = "pwm-backlight",
	.id        = -1,
	.dev        = {
		.parent = &s3c_device_timer[3].dev,
		.platform_data = &smdk_backlight_data,
	},
};

static void __init smdk_backlight_register(void)
{
	int ret;
#ifdef CONFIG_HAVE_PWM
	int i, ntimer;

	/* Timer GPIO Set */
	ntimer = ARRAY_SIZE(pwm_data);
	for (i = 0; i < ntimer; i++) {
		if (gpio_is_valid(pwm_data[i].gpio_no)) {
			ret = gpio_request(pwm_data[i].gpio_no,
				pwm_data[i].gpio_name);
			if (ret) {
				printk(KERN_ERR "failed to get GPIO for PWM\n");
				return;
			}
			s3c_gpio_cfgpin(pwm_data[i].gpio_no,
				pwm_data[i].gpio_set_value);

			gpio_free(pwm_data[i].gpio_no);
		}
	}
#endif
	ret = platform_device_register(&smdk_backlight_device);
	if (ret)
		printk(KERN_ERR "smdk: failed to register backlight device: %d\n", ret);
}
#endif

#ifdef CONFIG_S5P_ADC
static struct s3c_adc_mach_info s3c_adc_platform __initdata = {
	/* s5pc110 support 12-bit resolution */
	.delay  = 10000,
	.presc  = 49,
	.resolution = 12,
};
#endif

#if defined(CONFIG_CAMERA_TVP5150_ONBOARD) || defined(CONFIG_CAMERA_TVP5150_MODULE)

/*
 * External camera reset
 * Because the most of cameras take i2c bus signal, so that
 * you have to reset at the boot time for other i2c slave devices.
 * This function also called at fimc_init_camera()
 * Do optimization for cameras on your platform.
*/
static int tvp5150_power(int onoff)//low_reset GPE1_4
{
	int err;

	err = gpio_request(S5PV210_GPH0(4), "GPH0_4");
	if (err)
		printk(KERN_ERR "#### failed to request GPH0_4 for RESET\n");	
	if(onoff == 1) {
	printk("reset tvp5150!\n");
	s3c_gpio_setpull(S5PV210_GPH0(4), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV210_GPH0(4), 0);
	mdelay(50);
	gpio_direction_output(S5PV210_GPH0(4), 1);
	}
	gpio_free(S5PV210_GPH0(4));	

	return 0;
}

static struct tvp5150_platform_data tvp5150_plat =
{
	.default_width = 720,
	.default_height = 288,
	.pixelformat = V4L2_PIX_FMT_VYUY,
	.freq = 27000000,
	.is_mipi = 0,
};


static struct i2c_board_info  tvp5150_i2c_info = 
{
	I2C_BOARD_INFO("TVP5150", 0xBa>>1),
	.platform_data = &tvp5150_plat,
};

static struct s3c_platform_camera tvp5150 = {
	.id		= CAMERA_PAR_A,	
	.type		= CAM_TYPE_ITU,

	.fmt		= ITU_656_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.i2c_busnum	= 2,
	.info		= &tvp5150_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_VYUY,
	.srclk_name	= "mout_mpll",
	.clk_name	= "sclk_cam0",
	.clk_rate	= 27000000,
	.line_length	= 720,
	.width		= 720,
	.height		= 288,
	.window		= {
		.left	= 0,
		.top	= 17,
		.width	= 720,
		.height	= 288,
	},

	/* Polarity */
	.inv_pclk	= 1,
	.inv_vsync	= 0,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,

	.cam_power	= tvp5150_power,

};
#endif

#ifdef CONFIG_CAMERA_OV3640

static int smdkv210_OV3640_power(int onoff)//low_reset GPE1_4
{
	int err;
	if(onoff == 1) {
		err = gpio_request(S5PV210_GPF2(5), "GPF2_5"); //pd
		if (err)
			printk(KERN_ERR "#### failed to request GPF2_5 for power\n");	

		gpio_direction_output(S5PV210_GPF2(5), 0);
		mdelay(100);


		err = gpio_request(S5PV210_GPJ1(3), "GPJ1_3"); //reset
		if (err)
			printk(KERN_ERR "#### failed to request GPJ1_3 for RESET\n");	

		gpio_direction_output(S5PV210_GPJ1(3), 0);
		mdelay(30);
		gpio_direction_output(S5PV210_GPJ1(3), 1);
		mdelay(2);

		gpio_free(S5PV210_GPF2(5));
		gpio_free(S5PV210_GPJ1(3));
		printk("ov2640 reset and power up\n");
	}
	return 0;

}

static struct ov3640_platform_data ov3640_plat =
{
	.default_width = 640,
	.default_height = 480,
	.pixelformat = V4L2_PIX_FMT_VYUY,
	.freq = 40000000,
	.is_mipi = 0,
};

static struct i2c_board_info  ov3640_i2c_info = 
{
	I2C_BOARD_INFO("OV3640", 0x78>>1),
	.platform_data = &ov3640_plat,
};

static struct s3c_platform_camera ov3640 = {
	.id		= CAMERA_PAR_B,
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
	.i2c_busnum	= 0,
	.info		= &ov3640_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_VYUY,
	.srclk_name	= "mout_mpll",
	.clk_name	= "sclk_cam0",
	.clk_rate	= 40000000,
	.line_length	= 640,
	.width		= 640,
	.height		= 480,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 640,
		.height	= 480,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 0,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,

	.cam_power	= smdkv210_OV3640_power,

};
#endif
#ifdef CONFIG_CAMERA_OV2640_GZSD
static int smdkv210_OV2640_power(int onoff)//low_reset GPE1_4
{
	int err;
	err = gpio_request(S5PV210_GPF2(6), "GPF2_6");
	if (err)
		printk(KERN_ERR "#### failed to request GPF2_6 for power\n");

	gpio_direction_output(S5PV210_GPF2(6), onoff);
	if(onoff == 1) {
		err = gpio_request(S5PV210_GPF2(5), "GPF2_5"); //pd
		if (err)
			printk(KERN_ERR "#### failed to request GPF2_5 for power\n");	

		gpio_direction_output(S5PV210_GPF2(5), 0);
		mdelay(100);

		err = gpio_request(S5PV210_GPJ1(3), "GPJ1_3"); //reset
		if (err)
			printk(KERN_ERR "#### failed to request GPJ1_3 for RESET\n");	

		gpio_direction_output(S5PV210_GPJ1(3), 0);
		mdelay(30);
		gpio_direction_output(S5PV210_GPJ1(3), 1);
		mdelay(2);

		gpio_free(S5PV210_GPF2(5));
		gpio_free(S5PV210_GPJ1(3));
		printk("ov2640 reset and power up\n");
	}
	//printk("power is %d\n",onoff);
	gpio_free(S5PV210_GPF2(6));
	return 0;

}

static struct ov2640_platform_data ov2640_plat =
{
	.default_width = 1600,
	.default_height = 1200,
	.pixelformat = V4L2_PIX_FMT_YUYV,
	.freq = 27000000,
	.is_mipi = 0,
};

static struct i2c_board_info  ov2640_i2c_info = 
{
	I2C_BOARD_INFO("OV2640", 0x30),
	.platform_data = &ov2640_plat,
};

static struct s3c_platform_camera ov2640 = {
#ifdef CAM_ITU_CH_A
	.id		= CAMERA_PAR_A,
#else
	.id		= CAMERA_PAR_B,
#endif
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
	.i2c_busnum	= 0,
	.info		= &ov2640_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUYV,
	.srclk_name	= "mout_mpll",
	.clk_name	= "sclk_cam0",
	.clk_rate	= 27000000,
	.line_length	= 1600,
	.width		= 1600,
	.height		= 1200,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 1600,
		.height	= 1200,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,

	.cam_power	= smdkv210_OV2640_power,

};
#endif


/* Interface setting */
static struct s3c_platform_fimc fimc_plat_lsi = {
	.srclk_name	= "mout_mpll",
	.clk_name	= "sclk_fimc",
	.lclk_name	= "sclk_fimc_lclk",
	.clk_rate	= 166750000,
#ifdef	CAM_ITU_CH_A
	.default_cam	= CAMERA_PAR_A,
#else
	.default_cam	= CAMERA_PAR_B,
#endif
	.camera		= {

#ifdef CONFIG_CAMERA_OV2640_GZSD
		&ov2640,
#endif

#ifdef CONFIG_CAMERA_OV3640
		&ov3640,
#endif


#if defined(CONFIG_CAMERA_TVP5150_ONBOARD) || defined(CONFIG_CAMERA_TVP5150_MODULE)
		&tvp5150,
#endif

	},
	.hw_ver		= 0x43,
};



#ifdef CONFIG_VIDEO_JPEG_V2
static struct s3c_platform_jpeg jpeg_plat __initdata = {
	.max_main_width		= 800,
	.max_main_height	= 480,
	.max_thumb_width	= 320,
	.max_thumb_height	= 240,
};
#endif

#ifdef CONFIG_SND_SOC_WM8960_GZSD210
#include <sound/wm8960.h>
static struct wm8960_data wm8960_pdata = {
	.capless		= 0,
	.dres			= WM8960_DRES_400R,
};
#endif

/* I2C0 */
static struct i2c_board_info i2c_devs0[] __initdata = {
#ifdef CONFIG_SND_SOC_WM8960
	{
		I2C_BOARD_INFO("wm8960", 0x1a),
		.platform_data  = &wm8960_pdata,
	},
#endif
};

/* I2C1 */
static struct i2c_board_info i2c_devs1[] __initdata = {
#ifdef CONFIG_VIDEO_TV20
	{
		I2C_BOARD_INFO("s5p_ddc", (0x74>>1)),
	},
#endif
};

/* I2C2 */
static struct i2c_board_info i2c_devs2[] __initdata = {
	{
		/* The address is 0xCC used since SRAD = 0 */
		I2C_BOARD_INFO("pcf8563", (0xA2 >> 1)),
	},
};

#define S5PV210_PS_HOLD_CONTROL_REG (S3C_VA_SYS+0xE81C)
static void smdkc110_power_off(void)
{
	/* PS_HOLD output High --> Low */
	writel(readl(S5PV210_PS_HOLD_CONTROL_REG) & 0xFFFFFEFF,
			S5PV210_PS_HOLD_CONTROL_REG);

	while (1);
}

#ifdef CONFIG_BATTERY_S3C
struct platform_device sec_device_battery = {
	.name   = "sec-fake-battery",
	.id = -1,
};
#endif

#ifdef CONFIG_ANDROID_PMEM
static struct android_pmem_platform_data pmem_pdata = {
	.name = "pmem",
	.no_allocator = 1,
	.cached = 1,
	.start = 0,
	.size = 0,
};

static struct android_pmem_platform_data pmem_gpu1_pdata = {
	.name = "pmem_gpu1",
	.no_allocator = 1,
	.cached = 1,
	.buffered = 1,
	.start = 0,
	.size = 0,
};

static struct android_pmem_platform_data pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.no_allocator = 1,
	.cached = 1,
	.buffered = 1,
	.start = 0,
	.size = 0,
};

static struct platform_device pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = { .platform_data = &pmem_pdata },
};

static struct platform_device pmem_gpu1_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &pmem_gpu1_pdata },
};

static struct platform_device pmem_adsp_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &pmem_adsp_pdata },
};

static void __init android_pmem_set_platdata(void)
{
	pmem_pdata.start = (u32)s5p_get_media_memory_bank(S5P_MDEV_PMEM, 0);
	pmem_pdata.size = (u32)s5p_get_media_memsize_bank(S5P_MDEV_PMEM, 0);

	pmem_gpu1_pdata.start =
		(u32)s5p_get_media_memory_bank(S5P_MDEV_PMEM_GPU1, 0);
	pmem_gpu1_pdata.size =
		(u32)s5p_get_media_memsize_bank(S5P_MDEV_PMEM_GPU1, 0);

	pmem_adsp_pdata.start =
		(u32)s5p_get_media_memory_bank(S5P_MDEV_PMEM_ADSP, 0);
	pmem_adsp_pdata.size =
		(u32)s5p_get_media_memsize_bank(S5P_MDEV_PMEM_ADSP, 0);
}
#endif

static struct platform_device *gzsd210_devices[] __initdata = {
#ifdef CONFIG_FIQ_DEBUGGER
	&s5pv210_device_fiqdbg_uart2,
#endif
#ifdef CONFIG_MTD_ONENAND
	&s5pc110_device_onenand,
#endif
#ifdef CONFIG_MTD_NAND
	&s3c_device_nand,
#endif
	&s5p_device_rtc,
#if defined(CONFIG_SND_S3C64XX_SOC_I2S_V4) ||	\
	defined(CONFIG_SND_S5PC1XX_SOC_I2S)
	&s5pv210_device_iis0,
#endif
#ifdef CONFIG_SND_S3C_SOC_AC97
	&s5pv210_device_ac97,
#endif
#ifdef CONFIG_SND_S3C_SOC_PCM
	&s5pv210_device_pcm0,
#endif
#ifdef CONFIG_SND_SOC_SPDIF
	&s5pv210_device_spdif,
#endif
	&s3c_device_wdt,

#ifdef CONFIG_FB_S3C
	&s3c_device_fb,
#endif
#ifdef CONFIG_VIDEO_MFC50
	&s3c_device_mfc,
#endif

#ifdef CONFIG_TOUCHSCREEN_S3C
	&s3c_device_ts,
#endif
	
#ifdef CONFIG_KEYBOARD_GPIO
	&s3c_device_gpio_btns,
#endif
#if defined(CONFIG_KEYPAD_S3C) || defined(CONFIG_KEYPAD_S3C_MODULE)
	&s3c_device_keypad,
#endif
#ifdef CONFIG_S5P_ADC
	&s3c_device_adc,
#endif

#ifdef CONFIG_VIDEO_FIMC
	&s3c_device_fimc0,
	&s3c_device_fimc1,
	&s3c_device_fimc2,
#endif
#ifdef CONFIG_VIDEO_FIMC_MIPI
	&s3c_device_csis,
#endif
#ifdef CONFIG_VIDEO_JPEG_V2
	&s3c_device_jpeg,
#endif
#ifdef CONFIG_VIDEO_G2D
	&s3c_device_g2d,
#endif
#ifdef CONFIG_VIDEO_TV20
	&s5p_device_tvout,
	&s5p_device_cec,
	&s5p_device_hpd,
#endif

	&s3c_device_g3d,
	&s3c_device_lcd,

	&s3c_device_i2c0,
#ifdef CONFIG_S3C_DEV_I2C1
	&s3c_device_i2c1,
#endif
#ifdef CONFIG_S3C_DEV_I2C2
	&s3c_device_i2c2,
#endif

#ifdef CONFIG_USB_OHCI_HCD
	&s3c_device_usb_ohci,
#endif
#ifdef CONFIG_USB_EHCI_HCD
	&s3c_device_usb_ehci,
#endif

#ifdef CONFIG_USB_GADGET
	&s3c_device_usbgadget,
#endif
#ifdef CONFIG_USB_ANDROID
	&s3c_device_android_usb,
#ifdef CONFIG_USB_ANDROID_MASS_STORAGE
	&s3c_device_usb_mass_storage,
#endif
#ifdef CONFIG_USB_ANDROID_RNDIS
	&s3c_device_rndis,
#endif
#endif
#ifdef CONFIG_BATTERY_S3C
	&sec_device_battery,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC
	&s3c_device_hsmmc0,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC1
	&s3c_device_hsmmc1,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC2
	&s3c_device_hsmmc2,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC3
	&s3c_device_hsmmc3,
#endif

#ifdef CONFIG_S3C64XX_DEV_SPI
	&s5pv210_device_spi0,
	&s5pv210_device_spi1,
#endif
#ifdef CONFIG_S5PV210_POWER_DOMAIN
	&s5pv210_pd_audio,
	&s5pv210_pd_cam,
	&s5pv210_pd_tv,
	&s5pv210_pd_lcd,
	&s5pv210_pd_g3d,
	&s5pv210_pd_mfc,
#endif

#ifdef CONFIG_HAVE_PWM
	&s3c_device_timer[0],
	&s3c_device_timer[1],
	&s3c_device_timer[2],
	&s3c_device_timer[3],
#endif

#ifdef CONFIG_DM9000
	&gzsd210_device_dm9000,
#endif
};

static void __init gzsd210_map_io(void)
{
	struct s3cfb_lcd *lcd = gzsd210_get_lcd();
	int fimd_size;

	s5p_init_io(NULL, 0, S5P_VA_CHIPID);
	s3c24xx_init_clocks(24000000);
	s5pv210_gpiolib_init();
	s3c24xx_init_uarts(gzsd210_uartcfgs, ARRAY_SIZE(gzsd210_uartcfgs));

	fimd_size = PXL2FIMD(lcd->width * lcd->height);
	if (fimd_size != S5PV210_VIDEO_SAMSUNG_MEMSIZE_FIMD) {
		gzsd210_fixup_bootmem(S5P_MDEV_FIMD, fimd_size);
	}
	s5p_reserve_bootmem(gzsd210_media_devs, ARRAY_SIZE(gzsd210_media_devs));

#ifdef CONFIG_MTD_ONENAND
	s5pc110_device_onenand.name = "s5pc110-onenand";
#endif
#ifdef CONFIG_MTD_NAND
	s3c_device_nand.name = "s5pv210-nand";
#endif
	s5p_device_rtc.name = "smdkc110-rtc";
}

static void __init gzsd210_fixup(struct machine_desc *desc,
		struct tag *tags, char **cmdline,
		struct meminfo *mi)
{
	mi->bank[0].start = 0x20000000;
	mi->bank[0].size = 256 * SZ_1M;
	mi->bank[0].node = 0;

	mi->bank[1].start = 0x40000000;
	mi->bank[1].size = 256 * SZ_1M;
	mi->bank[1].node = 2;

	mi->nr_banks = 2;
}

unsigned int pm_debug_scratchpad;

static void __init sound_init(void)
{
	u32 reg;

	reg = __raw_readl(S5P_CLK_OUT);
	reg &= ~(0x1f << 12);
	reg &= ~(0xf << 20);
	reg |= 0x12 << 12;
	reg |= 0x1  << 20;
	__raw_writel(reg, S5P_CLK_OUT);

	reg = __raw_readl(S5P_OTHERS);
	reg &= ~(0x3 << 8);
	reg |= 0x0 << 8;
	__raw_writel(reg, S5P_OTHERS);
}

#ifdef CONFIG_VIDEO_TV20
static void s3c_set_qos(void)
{
	/* VP QoS */
	__raw_writel(0x00400001, S5P_VA_DMC0 + 0xC8);
	__raw_writel(0x387F0022, S5P_VA_DMC0 + 0xCC);
	/* MIXER QoS */
	__raw_writel(0x00400001, S5P_VA_DMC0 + 0xD0);
	__raw_writel(0x3FFF0062, S5P_VA_DMC0 + 0xD4);
	/* LCD1 QoS */
	__raw_writel(0x00800001, S5P_VA_DMC1 + 0x90);
	__raw_writel(0x3FFF005B, S5P_VA_DMC1 + 0x94);
	/* LCD2 QoS */
	__raw_writel(0x00800001, S5P_VA_DMC1 + 0x98);
	__raw_writel(0x3FFF015B, S5P_VA_DMC1 + 0x9C);
	/* VP QoS */
	__raw_writel(0x00400001, S5P_VA_DMC1 + 0xC8);
	__raw_writel(0x387F002B, S5P_VA_DMC1 + 0xCC);
	/* DRAM Controller QoS */
	__raw_writel(((__raw_readl(S5P_VA_DMC0)&~(0xFFF<<16))|(0x100<<16)),
			S5P_VA_DMC0 + 0x0);
	__raw_writel(((__raw_readl(S5P_VA_DMC1)&~(0xFFF<<16))|(0x100<<16)),
			S5P_VA_DMC1 + 0x0);
	/* BUS QoS AXI_DSYS Control */
	__raw_writel(0x00000007, S5P_VA_BUS_AXI_DSYS + 0x400);
	__raw_writel(0x00000007, S5P_VA_BUS_AXI_DSYS + 0x420);
	__raw_writel(0x00000030, S5P_VA_BUS_AXI_DSYS + 0x404);
	__raw_writel(0x00000030, S5P_VA_BUS_AXI_DSYS + 0x424);
}
#endif

static bool console_flushed;

static void flush_console(void)
{
	if (console_flushed)
		return;

	console_flushed = true;

	printk("\n");
	pr_emerg("Restarting %s\n", linux_banner);
	if (!try_acquire_console_sem()) {
		release_console_sem();
		return;
	}

	mdelay(50);

	local_irq_disable();
	if (try_acquire_console_sem())
		pr_emerg("flush_console: console was locked! busting!\n");
	else
		pr_emerg("flush_console: console was locked!\n");
	release_console_sem();
}

static void smdkc110_pm_restart(char mode, const char *cmd)
{
	flush_console();
	arm_machine_restart(mode, cmd);
}

static void __init gzsd210_machine_init(void)
{
	arm_pm_restart = smdkc110_pm_restart;

	s3c_usb_set_serial();
	platform_add_devices(gzsd210_devices, ARRAY_SIZE(gzsd210_devices));
#ifdef CONFIG_ANDROID_PMEM
	platform_device_register(&pmem_device);
	platform_device_register(&pmem_gpu1_device);
	platform_device_register(&pmem_adsp_device);
#endif
	pm_power_off = smdkc110_power_off ;

#ifdef CONFIG_ANDROID_PMEM
	android_pmem_set_platdata();
#endif
	/* i2c */
	s3c_i2c0_set_platdata(NULL);
	s3c_i2c1_set_platdata(NULL);
	s3c_i2c2_set_platdata(NULL);

	sound_init();

	i2c_register_board_info(0, i2c_devs0, ARRAY_SIZE(i2c_devs0));
	i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));
	i2c_register_board_info(2, i2c_devs2, ARRAY_SIZE(i2c_devs2));

#ifdef CONFIG_DM9000
	gzsd210_dm9000_set();
#endif

//#ifdef CONFIG_FB_S3C_GZSD210
	gzsd210_fb_data.lcd = gzsd210_get_lcd();
	s3cfb_set_platdata(&gzsd210_fb_data);
	//gzsd210_fb_data.backlight_on();

//#endif

	/* spi */
#ifdef CONFIG_S3C64XX_DEV_SPI
	if (!gpio_request(S5PV210_GPB(1), "SPI_CS0")) {
		gpio_direction_output(S5PV210_GPB(1), 1);
		s3c_gpio_cfgpin(S5PV210_GPB(1), S3C_GPIO_SFN(1));
		s3c_gpio_setpull(S5PV210_GPB(1), S3C_GPIO_PULL_UP);
		s5pv210_spi_set_info(0, S5PV210_SPI_SRCCLK_PCLK,
			ARRAY_SIZE(smdk_spi0_csi));
	}
	if (!gpio_request(S5PV210_GPB(5), "SPI_CS1")) {
		gpio_direction_output(S5PV210_GPB(5), 1);
		s3c_gpio_cfgpin(S5PV210_GPB(5), S3C_GPIO_SFN(1));
		s3c_gpio_setpull(S5PV210_GPB(5), S3C_GPIO_PULL_UP);
		s5pv210_spi_set_info(1, S5PV210_SPI_SRCCLK_PCLK,
			ARRAY_SIZE(smdk_spi1_csi));
	}
	spi_register_board_info(s3c_spi_devs, ARRAY_SIZE(s3c_spi_devs));
#endif

#if defined(CONFIG_TOUCHSCREEN_S3C)
	s3c_ts_set_platdata(&s3c_ts_platform);
#endif

#if defined(CONFIG_S5P_ADC)
	s3c_adc_set_platdata(&s3c_adc_platform);
#endif

#if defined(CONFIG_PM)
	s3c_pm_init();
#endif

#ifdef CONFIG_VIDEO_FIMC
	/* fimc */
	s3c_fimc0_set_platdata(&fimc_plat_lsi);
	s3c_fimc1_set_platdata(&fimc_plat_lsi);
	s3c_fimc2_set_platdata(&fimc_plat_lsi);

#endif

#ifdef CONFIG_VIDEO_FIMC_MIPI
	s3c_csis_set_platdata(NULL);
#endif

#ifdef CONFIG_VIDEO_JPEG_V2
	s3c_jpeg_set_platdata(&jpeg_plat);
#endif

#ifdef CONFIG_VIDEO_MFC50
	/* mfc */
	s3c_mfc_set_platdata(NULL);
#endif

#ifdef CONFIG_VIDEO_TV20
	s3c_set_qos();
#endif

#ifdef CONFIG_S3C_DEV_HSMMC
	s5pv210_default_sdhci0();
#endif
#ifdef CONFIG_S3C_DEV_HSMMC1
	s5pv210_default_sdhci1();
#endif
#ifdef CONFIG_S3C_DEV_HSMMC2
	s5pv210_default_sdhci2();
#endif
#ifdef CONFIG_S3C_DEV_HSMMC3
	s5pv210_default_sdhci3();
#endif
#ifdef CONFIG_S5PV210_SETUP_SDHCI
	s3c_sdhci_set_platdata();
#endif

#ifdef CONFIG_BACKLIGHT_PWM
	smdk_backlight_register();
#endif

	regulator_has_full_constraints();

	gzsd210_setup_clocks();
}

#ifdef CONFIG_USB_SUPPORT
/* Initializes OTG Phy. */
void otg_phy_init(void)
{
	/* USB PHY0 Enable */
	writel(readl(S5P_USB_PHY_CONTROL) | (0x1<<0),
			S5P_USB_PHY_CONTROL);
	writel((readl(S3C_USBOTG_PHYPWR) & ~(0x3<<3) & ~(0x1<<0)) | (0x1<<5),
			S3C_USBOTG_PHYPWR);
	writel((readl(S3C_USBOTG_PHYCLK) & ~(0x5<<2)) | (0x3<<0),
			S3C_USBOTG_PHYCLK);
	writel((readl(S3C_USBOTG_RSTCON) & ~(0x3<<1)) | (0x1<<0),
			S3C_USBOTG_RSTCON);
	msleep(1);
	writel(readl(S3C_USBOTG_RSTCON) & ~(0x7<<0),
			S3C_USBOTG_RSTCON);
	msleep(1);

	/* rising/falling time */
	writel(readl(S3C_USBOTG_PHYTUNE) | (0x1<<20),
			S3C_USBOTG_PHYTUNE);

	/* set DC level as 6 (6%) */
	writel((readl(S3C_USBOTG_PHYTUNE) & ~(0xf)) | (0x1<<2) | (0x1<<1),S3C_USBOTG_PHYTUNE);
}
EXPORT_SYMBOL(otg_phy_init);

/* USB Control request data struct must be located here for DMA transfer */
struct usb_ctrlrequest usb_ctrl __attribute__((aligned(64)));

/* OTG PHY Power Off */
void otg_phy_off(void)
{
	writel(readl(S3C_USBOTG_PHYPWR) | (0x3<<3),
			S3C_USBOTG_PHYPWR);
	writel(readl(S5P_USB_PHY_CONTROL) & ~(1<<0),
			S5P_USB_PHY_CONTROL);
}
EXPORT_SYMBOL(otg_phy_off);

void usb_host_phy_off(void)
{
	__raw_writel(__raw_readl(S3C_USBOTG_PHYPWR) | (0x1<<7)|(0x1<<6),
			S3C_USBOTG_PHYPWR);
	__raw_writel(__raw_readl(S5P_USB_PHY_CONTROL) & ~(1<<1),
			S5P_USB_PHY_CONTROL);
}
EXPORT_SYMBOL(usb_host_phy_off);

void usb_host_phy_init(void)
{
	struct clk *otg_clk;

	otg_clk = clk_get(NULL, "otg");
	clk_enable(otg_clk);

	if (readl(S5P_USB_PHY_CONTROL) & (0x1<<1)) {
		return;
	}

	__raw_writel(__raw_readl(S5P_USB_PHY_CONTROL) | (0x1<<1),
			S5P_USB_PHY_CONTROL);
	__raw_writel((__raw_readl(S3C_USBOTG_PHYPWR)
			& ~(0x1<<7) & ~(0x1<<6)) | (0x1<<8) | (0x1<<5),
			S3C_USBOTG_PHYPWR);
	__raw_writel((__raw_readl(S3C_USBOTG_PHYCLK) & ~(0x1<<7)) | (0x3<<0),
			S3C_USBOTG_PHYCLK);
	__raw_writel((__raw_readl(S3C_USBOTG_RSTCON)) | (0x1<<4) | (0x1<<3),
			S3C_USBOTG_RSTCON);
	udelay(15);
	__raw_writel(__raw_readl(S3C_USBOTG_RSTCON) & ~(0x1<<4) & ~(0x1<<3),
			S3C_USBOTG_RSTCON);
}
EXPORT_SYMBOL(usb_host_phy_init);
#endif

void s3c_setup_uart_cfg_gpio(unsigned char port)
{
}
EXPORT_SYMBOL(s3c_setup_uart_cfg_gpio);

MACHINE_START(GZSD210, "GZSD210")
	/* Maintainer: Kukjin Kim <kgene.kim@samsung.com> */
	.phys_io	= S3C_PA_UART & 0xfff00000,
	.io_pg_offst	= (((u32)S3C_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S5P_PA_SDRAM + 0x100,
	.fixup		= gzsd210_fixup,
	.init_irq	= s5pv210_init_irq,
	.map_io		= gzsd210_map_io,
	.init_machine	= gzsd210_machine_init,
	.timer		= &s5p_systimer,
MACHINE_END
