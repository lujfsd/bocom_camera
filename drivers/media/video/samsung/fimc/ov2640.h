/* linux/drivers/media/video/ov2640.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *	         http://www.samsung.com/
 *
 * Driver for OV2640 (UXGA camera) from Samsung Electronics
 * 1/4" 2.0Mp CMOS Image Sensor SoC with an Embedded Image Processor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __OV2640_H__
#define __OV2640_H__

struct ov2640_reg {
	unsigned char addr;
	unsigned char val;
};

struct ov2640_regset_type {
	unsigned char *regset;
	int len;
};

/*
 * Macro
 */
#define REGSET_LENGTH(x)	(sizeof(x)/sizeof(ov2640_reg))

/*
 * User defined commands
 */
/* S/W defined features for tune */
#define REG_DELAY	0xFF00	/* in ms */
#define REG_CMD		0xFFFF	/* Followed by command */

/* Following order should not be changed */
enum image_size_ov2640 {
	/* This SoC supports upto UXGA (1600*1200) */
	QQVGA,	/* 160*120 */
	QCIF,	/* 176*144 */
	QVGA,	/* 320*240 */
	CIF,	/* 352*288 */
	VGA,	/* 640*480 */
	SVGA,	/* 800*600 */
	HD720P,	/* 1280*720 */
	SXGA,	/* 1280*1024 */
	UXGA,	/* 1600*1200 */
};

/*
 * Following values describe controls of camera
 * in user aspect and must be match with index of ov2640_regset[]
 * These values indicates each controls and should be used
 * to control each control
 */
enum ov2640_control {
	OV2640_INIT,
	OV2640_EV,
	OV2640_AWB,
	OV2640_MWB,
	OV2640_EFFECT,
	OV2640_CONTRAST,
	OV2640_SATURATION,
	OV2640_SHARPNESS,
};

#define OV2640_REGSET(x)	{	\
	.regset = x,			\
	.len = sizeof(x)/sizeof(ov2640_reg),}


/*
 * User tuned register setting values
 */
u8 ov2640_init_reg[][2] = {
    {0x12, 0x80},
    {0xff, 0x00},
    {0x2c, 0xff},
    {0x2e, 0xdf},
    {0xff, 0x01},
    {0x3c, 0x32},
    {0x11, 0x00},
    {0x09, 0x02},
    {0x04, 0x28},
    {0x13, 0xe5},
    {0x14, 0x48},
    {0x2c, 0x0c},
    {0x33, 0x78},
    {0x3a, 0x33},
    {0x3b, 0xfB},
    {0x3e, 0x00},
    {0x43, 0x11},
    {0x16, 0x10},
    {0x39, 0x02},
    {0x35, 0x88},
    {0x22, 0x0a},
    {0x37, 0x40},
    {0x23, 0x00},
    {0x34, 0xa0},
    {0x36, 0x1a},
    {0x06, 0x02},
    {0x07, 0xc0},
    {0x0d, 0xb7},
    {0x0e, 0x01},
    {0x4c, 0x00},
    {0x4a, 0x81},
    {0x21, 0x99},
    {0x24, 0x40},
    {0x25, 0x38},
    {0x26, 0x82},
    {0x5c, 0x00},
    {0x63, 0x00},
    {0x46, 0x3f},
    {0x0c, 0x3c},
    {0x61, 0x70},
    {0x62, 0x80},
    {0x7c, 0x05},
    {0x20, 0x80},
    {0x28, 0x30},
    {0x6c, 0x00},
    {0x6d, 0x80},
    {0x6e, 0x00},
    {0x70, 0x02},
    {0x71, 0x94},
    {0x73, 0xc1},
    {0x3d, 0x34},
    {0x5a, 0x57},
    {0x4f, 0xbb},
    {0x50, 0x9c},
    {0xff, 0x00},
    {0xe5, 0x7f},
    {0xf9, 0xc0},
    {0x41, 0x24},
    {0xe0, 0x14},
    {0x76, 0xff},
    {0x33, 0xa0},
    {0x42, 0x20},
    {0x43, 0x18},
    {0x4c, 0x00},
    {0x87, 0xd0},
    {0x88, 0x3f},
    {0xd7, 0x03},
    {0xd9, 0x10},
    {0xd3, 0x82},
    {0xc8, 0x08},
    {0xc9, 0x80},
    {0x7c, 0x00},
    {0x7d, 0x00},
    {0x7c, 0x03},
    {0x7d, 0x48},
    {0x7d, 0x48},
    {0x7c, 0x08},
    {0x7d, 0x20},
    {0x7d, 0x10},
    {0x7d, 0x0e},
    {0x90, 0x00},
    {0x91, 0x0e},
    {0x91, 0x1a},
    {0x91, 0x31},
    {0x91, 0x5a},
    {0x91, 0x69},
    {0x91, 0x75},
    {0x91, 0x7e},
    {0x91, 0x88},
    {0x91, 0x8f},
    {0x91, 0x96},
    {0x91, 0xa3},
    {0x91, 0xaf},
    {0x91, 0xc4},
    {0x91, 0xd7},
    {0x91, 0xe8},
    {0x91, 0x20},
    {0x92, 0x00},
    {0x93, 0x06},
    {0x93, 0xe3},
    {0x93, 0x05},
    {0x93, 0x05},
    {0x93, 0x00},
    {0x93, 0x04},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x96, 0x00},
    {0x97, 0x08},
    {0x97, 0x19},
    {0x97, 0x02},
    {0x97, 0x0c},
    {0x97, 0x24},
    {0x97, 0x30},
    {0x97, 0x28},
    {0x97, 0x26},
    {0x97, 0x02},
    {0x97, 0x98},
    {0x97, 0x80},
    {0x97, 0x00},
    {0x97, 0x00},
    {0xc3, 0xed},
    {0xa4, 0x00},
    {0xa8, 0x00},
    {0xc5, 0x11},
    {0xc6, 0x51},
    {0xbf, 0x80},
    {0xc7, 0x10},
    {0xb6, 0x66},
    {0xb8, 0xA5},
    {0xb7, 0x64},
    {0xb9, 0x7C},
    {0xb3, 0xaf},
    {0xb4, 0x97},
    {0xb5, 0xFF},
    {0xb0, 0xC5},
    {0xb1, 0x94},
    {0xb2, 0x0f},
    {0xc4, 0x5c},
    {0xc0, 0xc8},
    {0xc1, 0x96},
    {0x86, 0x3d},
    {0x50, 0x00},
    {0x51, 0x90},
    {0x52, 0x2c},
    {0x53, 0x00},
    {0x54, 0x00},
    {0x55, 0x88},
    {0x57, 0x00},
    {0x5a, 0x90},
    {0x5b, 0x2c},
    {0x5c, 0x05},
    {0xc3, 0xed},
    {0x7f, 0x00},
    {0xda, 0x00},
    {0xe5, 0x1f},
    {0xe1, 0x67},
    {0xe0, 0x00},
    {0xdd, 0xdf},//0x7f},
    {0x05, 0x00},

};


#define OV2640_INIT_REGS	((sizeof(ov2640_init_reg)/sizeof(ov2640_init_reg[0])))

/*
 * EV bias
 */

static const struct ov2640_reg ov2640_ev_m6[] = {
};

static const struct ov2640_reg ov2640_ev_m5[] = {
};

static const struct ov2640_reg ov2640_ev_m4[] = {
};

static const struct ov2640_reg ov2640_ev_m3[] = {
};

static const struct ov2640_reg ov2640_ev_m2[] = {
};

static const struct ov2640_reg ov2640_ev_m1[] = {
};

static const struct ov2640_reg ov2640_ev_default[] = {
};

static const struct ov2640_reg ov2640_ev_p1[] = {
};

static const struct ov2640_reg ov2640_ev_p2[] = {
};

static const struct ov2640_reg ov2640_ev_p3[] = {
};

static const struct ov2640_reg ov2640_ev_p4[] = {
};

static const struct ov2640_reg ov2640_ev_p5[] = {
};

static const struct ov2640_reg ov2640_ev_p6[] = {
};

/* Order of this array should be following the querymenu data */
static const unsigned char *ov2640_regs_ev_bias[] = {
	(unsigned char *)ov2640_ev_m6, (unsigned char *)ov2640_ev_m5,
	(unsigned char *)ov2640_ev_m4, (unsigned char *)ov2640_ev_m3,
	(unsigned char *)ov2640_ev_m2, (unsigned char *)ov2640_ev_m1,
	(unsigned char *)ov2640_ev_default, (unsigned char *)ov2640_ev_p1,
	(unsigned char *)ov2640_ev_p2, (unsigned char *)ov2640_ev_p3,
	(unsigned char *)ov2640_ev_p4, (unsigned char *)ov2640_ev_p5,
	(unsigned char *)ov2640_ev_p6,
};

/*
 * Auto White Balance configure
 */
static const struct ov2640_reg ov2640_awb_off[] = {
};

static const struct ov2640_reg ov2640_awb_on[] = {
};

static const unsigned char *ov2640_regs_awb_enable[] = {
	(unsigned char *)ov2640_awb_off,
	(unsigned char *)ov2640_awb_on,
};

/*
 * Manual White Balance (presets)
 */
static const struct ov2640_reg ov2640_wb_tungsten[] = {

};

static const struct ov2640_reg ov2640_wb_fluorescent[] = {

};

static const struct ov2640_reg ov2640_wb_sunny[] = {

};

static const struct ov2640_reg ov2640_wb_cloudy[] = {

};

/* Order of this array should be following the querymenu data */
static const unsigned char *ov2640_regs_wb_preset[] = {
	(unsigned char *)ov2640_wb_sunny,
	(unsigned char *)ov2640_wb_cloudy,
	(unsigned char *)ov2640_wb_tungsten,
	(unsigned char *)ov2640_wb_fluorescent,
};

/*
 * Color Effect (COLORFX)
 */
static const struct ov2640_reg ov2640_color_normal[] = {
};

static const struct ov2640_reg ov2640_color_monochrome[] = {
};

static const struct ov2640_reg ov2640_color_sepia[] = {
};

static const struct ov2640_reg ov2640_color_aqua[] = {
};

static const struct ov2640_reg ov2640_color_negative[] = {
};

static const struct ov2640_reg ov2640_color_sketch[] = {
};

/* Order of this array should be following the querymenu data */
static const unsigned char *ov2640_regs_color_effect[] = {
	(unsigned char *)ov2640_color_normal,
	(unsigned char *)ov2640_color_monochrome,
	(unsigned char *)ov2640_color_sepia,
	(unsigned char *)ov2640_color_aqua,
	(unsigned char *)ov2640_color_sketch,
	(unsigned char *)ov2640_color_negative,
};

/*
 * Contrast bias
 */
static const struct ov2640_reg ov2640_contrast_m2[] = {
};

static const struct ov2640_reg ov2640_contrast_m1[] = {
};

static const struct ov2640_reg ov2640_contrast_default[] = {
};

static const struct ov2640_reg ov2640_contrast_p1[] = {
};

static const struct ov2640_reg ov2640_contrast_p2[] = {
};

static const unsigned char *ov2640_regs_contrast_bias[] = {
	(unsigned char *)ov2640_contrast_m2,
	(unsigned char *)ov2640_contrast_m1,
	(unsigned char *)ov2640_contrast_default,
	(unsigned char *)ov2640_contrast_p1,
	(unsigned char *)ov2640_contrast_p2,
};

/*
 * Saturation bias
 */
static const struct ov2640_reg ov2640_saturation_m2[] = {
};

static const struct ov2640_reg ov2640_saturation_m1[] = {
};

static const struct ov2640_reg ov2640_saturation_default[] = {
};

static const struct ov2640_reg ov2640_saturation_p1[] = {
};

static const struct ov2640_reg ov2640_saturation_p2[] = {
};

static const unsigned char *ov2640_regs_saturation_bias[] = {
	(unsigned char *)ov2640_saturation_m2,
	(unsigned char *)ov2640_saturation_m1,
	(unsigned char *)ov2640_saturation_default,
	(unsigned char *)ov2640_saturation_p1,
	(unsigned char *)ov2640_saturation_p2,
};

/*
 * Sharpness bias
 */
static const struct ov2640_reg ov2640_sharpness_m2[] = {
};

static const struct ov2640_reg ov2640_sharpness_m1[] = {
};

static const struct ov2640_reg ov2640_sharpness_default[] = {
};

static const struct ov2640_reg ov2640_sharpness_p1[] = {
};

static const struct ov2640_reg ov2640_sharpness_p2[] = {
};

static const unsigned char *ov2640_regs_sharpness_bias[] = {
	(unsigned char *)ov2640_sharpness_m2,
	(unsigned char *)ov2640_sharpness_m1,
	(unsigned char *)ov2640_sharpness_default,
	(unsigned char *)ov2640_sharpness_p1,
	(unsigned char *)ov2640_sharpness_p2,
};

#endif
