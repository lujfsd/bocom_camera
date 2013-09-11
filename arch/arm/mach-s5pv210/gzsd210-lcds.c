/*
 * linux/arch/arm/mach-s5pv210/mini210-lcds.c
 *
 * Copyright (c) 2011 FriendlyARM (www.arm9.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/gpio.h>

#include <plat/fb.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>

#include <../../../drivers/video/samsung/s3cfb.h>


/* s3cfb configs for supported LCD */

static struct s3cfb_lcd wvga_w50 = {
	.width	=  800,
	.height	=  480,
	.bpp = 16,//32,
	.freq = 60,
	
	.timing = {
		.h_fp = 20,//46,
		.h_bp = 30,//38,
		.h_sw = 15,//5,
		.v_fp = 3,//3,
		.v_fpe = 1,
		.v_bp = 20,//20,
		.v_bpe = 1,
		.v_sw = 2,//2,
	},


	.polarity = {
		.rise_vclk	= 0,
		.inv_hsync	= 1,
		.inv_vsync	= 1,
		.inv_vden	= 0,
	},
};

static struct s3cfb_lcd wvga_a70 = {
	.width = 800,
	.height = 480,
	.p_width = 152,
	.p_height = 90,
	.bpp = 32,
	.freq = 85,

	.timing = {
		.h_fp = 40,
		.h_bp = 40,
		.h_sw = 48,
		.v_fp = 17,
		.v_fpe = 1,
		.v_bp = 29,
		.v_bpe = 1,
		.v_sw = 24,
	},
	.polarity = {
		.rise_vclk = 0,
		.inv_hsync = 1,
		.inv_vsync = 1,
		.inv_vden = 0,
	},
};

static struct s3cfb_lcd wvga_s70 = {
	.width = 800,
	.height = 480,
	.p_width = 154,
	.p_height = 86,
	.bpp = 32,
	.freq = 85,

	.timing = {
		.h_fp = 210,
		.h_bp = 44,
		.h_sw = 2,
		.v_fp = 22,
		.v_fpe = 1,
		.v_bp = 21,
		.v_bpe = 1,
		.v_sw = 2,
	},
	.polarity = {
		.rise_vclk = 0,
		.inv_hsync = 1,
		.inv_vsync = 1,
		.inv_vden = 0,
	},
};

static struct s3cfb_lcd gzsd_ch7034 = {
	.width	= 1024,
	.height	= 768,
	.bpp	= 24,
	.freq	= 60,

	.timing = {
		.h_fp	= 10,
		.h_bp	= 20,
		.h_sw	= 10,
		.v_fp	= 10,
		.v_fpe	= 1,
		.v_bp	= 20,
		.v_bpe	= 1,
		.v_sw	= 10,
	},

	.polarity = {
		.rise_vclk	= 0,
		.inv_hsync	= 1,
		.inv_vsync	= 1,
		.inv_vden	= 0,
	},
};
/* Try to guess LCD panel by kernel command line, or
 * using *W50* as default */

static struct {
	char *name;
	struct s3cfb_lcd *lcd;
} gzsd210_lcd_config[] = {
	{ "W50", &wvga_w50 },
	{ "A70", &wvga_a70 },
	{ "S70", &wvga_s70 },
	{ "ch7034", &gzsd_ch7034 },
};

static int lcd_idx = 0;

static int __init gzsd210_setup_lcd(char *str)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(gzsd210_lcd_config); i++) {
		if (!strcasecmp(gzsd210_lcd_config[i].name, str)) {
			lcd_idx = i;
			break;
		}
	}

	return 0;
}
early_param("lcd", gzsd210_setup_lcd);

struct s3cfb_lcd *gzsd210_get_lcd(void)
{
	printk("GZSD210: %s selected\n", gzsd210_lcd_config[lcd_idx].name);
#if defined(CONFIG_FB_S3C_GZSD210)
	lcd_idx = 0;
#elif defined(CONFIG_FB_S3C_CH7034_GZSD210)
	lcd_idx = 3;
#endif
	return gzsd210_lcd_config[lcd_idx].lcd;
}

void gzsd210_get_lcd_res(int *w, int *h)
{
	struct s3cfb_lcd *lcd = gzsd210_lcd_config[lcd_idx].lcd;

	if (w)
		*w = lcd->width;
	if (h)
		*h = lcd->height;

	return;
}
EXPORT_SYMBOL(gzsd210_get_lcd_res);

