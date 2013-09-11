/*
 * Driver for keys on GPIO lines.
 *
 * Copyright 2009 jiangjianjun <jerryjianjun@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/serial_core.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <plat/map-base.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <mach/regs-irq.h>
#include <linux/gpio.h>
#include <linux/input.h>

#define MAX_BUTTON_CNT 		(6)
//#define DAO	1

static struct timer_list timer;
static struct input_dev * input;

static int s3c_Keycode[MAX_BUTTON_CNT] = {KEY_UP, KEY_DOWN, KEY_ESC, KEY_ENTER, KEY_LEFT, KEY_RIGHT};
static int s3c_button_history[MAX_BUTTON_CNT] = { 0 };

static void s3cbutton_timer_handler(unsigned long data)
{
	int flag;

	/* home */
	flag = gpio_get_value(S5PV210_GPD0(3));
	if(flag != s3c_button_history[0])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[0], 0);
		else
			input_report_key(input, s3c_Keycode[0], 1);
		s3c_button_history[0] = flag;
	}

	/* menu */
	flag = gpio_get_value(S5PV210_GPD0(2));
	if(flag != s3c_button_history[1])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[1], 0);
		else
			input_report_key(input, s3c_Keycode[1], 1);
		s3c_button_history[1] = flag;
	}

	/* back */
	flag = gpio_get_value(S5PV210_GPB(4));
	if(flag != s3c_button_history[2])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[2], 0);
		else
			input_report_key(input, s3c_Keycode[2], 1);
		s3c_button_history[2] = flag;
	}

	/* enter */
	flag = gpio_get_value(S5PV210_GPB(5));
	if(flag != s3c_button_history[3])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[3], 0);
		else
			input_report_key(input, s3c_Keycode[3], 1);
		s3c_button_history[3] = flag;
	}

	/* volumeup */

	flag = gpio_get_value(S5PV210_GPB(6));
	if(flag != s3c_button_history[4])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[4], 0);
		else
			input_report_key(input, s3c_Keycode[4], 1);
		s3c_button_history[4] = flag;
	}

	/* volumedown */
	flag = gpio_get_value(S5PV210_GPB(7));
	if(flag != s3c_button_history[5])
	{
		if(flag)
			input_report_key(input, s3c_Keycode[5], 0);
		else
			input_report_key(input, s3c_Keycode[5], 1);
		s3c_button_history[5] = flag;
	}

	/* Kernel Timer restart */
	mod_timer(&timer,jiffies + HZ/100);
}

static int s3c_button_probe(struct platform_device *pdev)
{
	int i;
	int ret;
	
	/* gpd0_3 (home) */
	ret = gpio_request(S5PV210_GPD0(3), "GPD0");
	if(ret)
		printk("button-smdkv210: request gpio GPD0(3) fail\n");

	/* gpd0_2 (menu) */
	ret = gpio_request(S5PV210_GPD0(2), "GPD0");
	if(ret)
		printk("button-smdkv210: request gpio GPD0(2) fail\n");

	/* gpb4 (back) */
	ret = gpio_request(S5PV210_GPB(4), "GPB");
	if(ret)
		printk("button-smdkv210: request gpio GPB(4) fail\n");

	/* gphb5 (enter) */
	ret = gpio_request(S5PV210_GPB(5), "GPB");
	if(ret)
		printk("button-smdkv210: request gpio GPB(5) fail\n");

	/* gphb6 (volumeup) */

	ret = gpio_request(S5PV210_GPB(6), "GPB");
	if(ret)
		printk("button-smdkv210: request gpio GPB(6) fail\n");

	/* gphb (volumedown) */
	ret = gpio_request(S5PV210_GPB(7), "GPB");
	if(ret)
		printk("button-smdkv210: request gpio GPB(7) fail\n");

	s3c_gpio_setpull(S5PV210_GPD0(3), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPD0(3), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPD0(2), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPD0(2), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPB(4), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPB(4), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPB(5), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPB(5), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPB(6), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPB(6), S3C_GPIO_SFN(0));

	s3c_gpio_setpull(S5PV210_GPB(7), S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S5PV210_GPB(7), S3C_GPIO_SFN(0));

	s3c_button_history[0] = gpio_get_value(S5PV210_GPD0(3));
	s3c_button_history[1] = gpio_get_value(S5PV210_GPD0(2));
	s3c_button_history[2] = gpio_get_value(S5PV210_GPB(4));
	s3c_button_history[3] = gpio_get_value(S5PV210_GPB(5));
	s3c_button_history[4] = gpio_get_value(S5PV210_GPB(6));
	s3c_button_history[5] = gpio_get_value(S5PV210_GPB(7));


	input = input_allocate_device();
	if(!input) 
		return -ENOMEM;

	set_bit(EV_KEY, input->evbit);
	//set_bit(EV_REP, input->evbit);	/* Repeat Key */

	for(i = 0; i < MAX_BUTTON_CNT; i++)
		set_bit(s3c_Keycode[i], input->keybit);

	input->name = "s3c-button";
	input->phys = "s3c-button/input0";

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;

	input->keycode = s3c_Keycode;

	if(input_register_device(input) != 0)
	{
		printk("s3c-button input register device fail!!\n");

		input_free_device(input);
		return -ENODEV;
	}

	/* Scan timer init */
	init_timer(&timer);
	timer.function = s3cbutton_timer_handler;

	timer.expires = jiffies + (HZ/100);
	add_timer(&timer);

	printk("s3c button Initialized!!\n");

	return 0;
}

static int s3c_button_remove(struct platform_device *pdev)
{
	input_unregister_device(input);
	del_timer(&timer);
	
	return  0;
}


#ifdef CONFIG_PM
static int s3c_button_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

static int s3c_button_resume(struct platform_device *pdev)
{
	return 0;
}
#else
#define s3c_button_suspend	NULL
#define s3c_button_resume	NULL
#endif

static struct platform_driver s3c_button_device_driver = {
	.probe		= s3c_button_probe,
	.remove		= s3c_button_remove,
	.suspend	= s3c_button_suspend,
	.resume		= s3c_button_resume,
	.driver		= {
		.name	= "s3c-button",
		.owner	= THIS_MODULE,
	}
};


static struct platform_device s3c_device_button = {
	.name	= "s3c-button",
	.id		= -1,
};

static int __init s3c_button_init(void)
{   
	platform_device_register(&s3c_device_button);       

	return platform_driver_register(&s3c_button_device_driver);
}

static void __exit s3c_button_exit(void)
{
	platform_driver_unregister(&s3c_button_device_driver);
	platform_device_unregister(&s3c_device_button);   
}

module_init(s3c_button_init);
module_exit(s3c_button_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GZSD");
MODULE_DESCRIPTION("Keyboard driver for s3c button.");
MODULE_ALIAS("platform:s3c-button");
