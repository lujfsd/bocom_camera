#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>
#include <linux/smp_lock.h>
#include <linux/delay.h>
#include <linux/slab.h> 
#include <linux/irq.h>
#include <linux/gpio_keys.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>


#define ICOOL210

#define	PIXCIR_DEBUG		1

#ifdef  ICOOL210


#define I2C_BUS_NUM			0
#define GPIO_INT			S5PV210_GPH0(1)		
#define GPIO_RESET			S5PV210_GPH1(2)		
#define IRQ				IRQ_EINT(1)

#else

#define I2C_BUS_NUM			2
#define GPIO_INT			S5PV210_GPH0(3)		
#define GPIO_RESET			S5PV210_GPF3(4)		
#define IRQ				IRQ_EINT(3)	

#endif

#define	SLAVE_ADDR			0x38

#define SPECOP				0x38


	
#define	MAXX				48
#define	MAXY				48

#define TOUCHSCREEN_MINX 	0
#define TOUCHSCREEN_MAXX 	800//14000//13823
#define TOUCHSCREEN_MINY 	0
#define TOUCHSCREEN_MAXY 	480//8200//7669


static struct workqueue_struct *ts_wq;

struct ts_i2c_data
{
	struct i2c_client *client;
	struct input_dev *input;
	struct delayed_work work;
	int irq;
};

int calibrate_cmd = 0;

static int __init ts_calibrate(char *str)
{
	calibrate_cmd = 1;
	return 1;
}

__setup("calibrate", ts_calibrate);

static void calibrate(struct i2c_client *client)
{
	int rc;
	unsigned char Wrbuf[2];
	printk("\n");
	dev_err(&client->dev, "calibrate start!\n");
	
	msleep(100);
	
	Wrbuf[0] = 0x00;
	Wrbuf[1] = 0x0;
	
	rc = i2c_master_send(client, Wrbuf, 2);
	if(rc != 2) {
		printk("master send error, ret = %d\n",rc);
	}

/*
	Wrbuf[0] = 0x80;
	Wrbuf[1] = 0x10;
	
	rc = i2c_master_send(client, Wrbuf, 2);
	if(rc != 2) {
		printk("master send error, ret = %d\n",rc);
	}

	Wrbuf[0] = 0x88;
	Wrbuf[1] = 0x9;
	
	rc = i2c_master_send(client, Wrbuf, 2);
	if(rc != 2) {
		printk("master send error, ret = %d\n",rc);
	}


*/		

	Wrbuf[0] = 0x04;
	Wrbuf[1] = 0x0;
	
	rc = i2c_master_send(client, Wrbuf, 2);
	if(rc != 2) {
		printk("master send error, ret = %d\n",rc);
	}


	Wrbuf[0] = 0x07;
	Wrbuf[1] = 0x0;
	
	rc = i2c_master_send(client, Wrbuf, 2);
	if(rc != 2) {
		printk("master send error, ret = %d\n",rc);
	}


/*	
	dev_err(&client->dev, "calibrate 5\n");			
	msleep(1000);
	dev_err(&client->dev, "calibrate 4\n");			
	msleep(1000);		
	dev_err(&client->dev, "calibrate 3\n");			
	msleep(1000);
	dev_err(&client->dev, "calibrate 2\n");			
	msleep(1000);
	dev_err(&client->dev, "calibrate 1\n");			
	msleep(1000);
	dev_err(&client->dev, "calibrate 0\n");	
*/		

	dev_err(&client->dev, "calibrate ok!\n\n");	
}
/*******************************class*************************************/
static struct class *ts_class;

struct ts_device 
{
	struct i2c_client *client;	
	struct device dev;
};

#define to_ts_device(obj) container_of(obj, struct ts_device, dev)

static ssize_t ts_class_show_calibration(struct device *dev,
		struct device_attribute *attr,char *buf)
{
	return sprintf(buf, "%d\n", 0);
}

static ssize_t ts_class_store_calibration(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct ts_device *bd = to_ts_device(dev);
	
	//printk("ts: set calibration %d %d %d %d \n",count,buf[0],buf[1],buf[2]);
	
	if(strcasecmp(buf, "calibrate\n"))
	{
		printk("ts calibrate error: input command error,should be calibrate\n");	
		return count;
	}
	else
	{
		calibrate(bd->client);
	}

	return count;
}

static int ts_class_suspend(struct device *dev, pm_message_t state)
{
	return 0;
}

static int ts_class_resume(struct device *dev)
{
	return 0;
}

static void ts_class_device_release(struct device *dev)
{
	struct ts_device *bd = to_ts_device(dev);
	kfree(bd);
}

static struct device_attribute ts_class_device_attributes[] = {
	__ATTR(ts_calibration, 0666, ts_class_show_calibration, ts_class_store_calibration),
	__ATTR_NULL,
};

void ts_class_device_register(struct i2c_client *client)
{
	struct ts_device *new_bd;
	int rc;

//	printk("ts_device_register: name=%s\n", "ts_calibration");

	new_bd = kzalloc(sizeof(struct ts_device), GFP_KERNEL);
	if (!new_bd)
		return ;

	new_bd->client = client;
	new_bd->dev.class = ts_class;
	new_bd->dev.parent = NULL;
	new_bd->dev.release = ts_class_device_release;
	dev_set_name(&new_bd->dev, "ts_class_calibration");
	dev_set_drvdata(&new_bd->dev, NULL);

	rc = device_register(&new_bd->dev);
	if (rc) {
		kfree(new_bd);
		return ;
	}
}


void ts_class_device_unregister(struct ts_device *bd)
{
	if (!bd)
		return;
		
	device_unregister(&bd->dev);
}
/*******************************class end*************************************/


static int ts_open(struct input_dev *dev)
{
	return 0;
}

static void ts_close(struct input_dev *dev)
{
}

static int posx[5], posy[5];
static unsigned char buf[31];
//typedef unsigned short s16;

int pendown=0;
unsigned long x,y;

static void ts_poscheck(struct work_struct *work)
{
	struct ts_i2c_data *tsdata = container_of(work,
			struct ts_i2c_data,
			work.work);
	int ret;
	int i;

	int num;
	
	
	/*
	printk("\n");
	for (i = 0;i< ret;i++)
	{
		printk(" 0x%x ",Rdbuf[i]);
	}
	printk("\n");


         y = (s16)(Rdbuf[5] & 0x0F)<<8 | (s16)Rdbuf[6]; 
	 x = (s16)(Rdbuf[3] & 0x0F)<<8 | (s16)Rdbuf[4];  
	
	printk("x is %d, y is %d\n",x,y);

	*/
	if(gpio_get_value(GPIO_INT))
	{
		msleep(5);
		if(gpio_get_value(GPIO_INT))
		{
			pendown = 0;
			input_report_key(tsdata->input, BTN_TOUCH, 0);
	 		input_report_abs(tsdata->input,ABS_PRESSURE, 0);
			input_sync(tsdata->input);	
			enable_irq(tsdata->irq);
		}else
			goto TS_READ;
	}
	else
	{    
TS_READ:
	     pendown ++;
	     if (pendown < 2)
		goto dont_read;

	      ret = i2c_smbus_read_i2c_block_data(tsdata->client, 0, sizeof(buf), buf);

	      num =  buf[2] & 0x07;
	      if (num == 1)
	      {
		
			  // printk("touch screen down!\n");
			    posx[0] = (s16)(buf[3] & 0x0F)<<8 | (s16)buf[4];  
			    posy[0] = (s16)(buf[5] & 0x0F)<<8 | (s16)buf[6]; 

			    input_report_abs(tsdata->input, ABS_X, (unsigned long)posx[0]);
			    input_report_abs(tsdata->input, ABS_Y, (unsigned long)posy[0]);

			    input_report_abs(tsdata->input, ABS_Z, 0);
			    input_report_abs(tsdata->input,ABS_PRESSURE, 0x1);
				
			    
			    input_report_key(tsdata->input, BTN_TOUCH, 1);
			   	
			   
	  		   input_sync(tsdata->input);

	       }	
	dont_read:	
	       msleep(5);
	      queue_work(ts_wq, &tsdata->work.work);
	
}

}

static irqreturn_t ts_ts_isr(int irq, void *dev_id)
{
	//printk("*******************ts_ts_isr************\n");
	struct ts_i2c_data *tsdata = (struct ts_i2c_data *)dev_id;
//	if ((status_reg == 0) || (status_reg == NORMAL_MODE)) {
		
		disable_irq_nosync(irq);
		//msleep(3);
		queue_work(ts_wq, &tsdata->work.work);
//	}

	return IRQ_HANDLED;
}



static int ts_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{

	struct ts_i2c_data *tsdata;
	struct input_dev *input;
	struct device *dev;
	struct i2c_dev *i2c_dev;
	int error;

	printk("ts_i2c_probe\n");
	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_READ_BYTE)) {
		printk(KERN_INFO "i2c_check_functionality error\n");
		return -1;
	}

	tsdata = kzalloc(sizeof(*tsdata), GFP_KERNEL);
	if (!tsdata) {
		dev_err(&client->dev, "failed to allocate driver data!\n");
		error = -ENOMEM;
		dev_set_drvdata(&client->dev, NULL);
		return error;
	}
	
	input = input_allocate_device();
	if (!input) {
		dev_err(&client->dev, "failed to allocate input device!\n");
		error = -ENOMEM;
		input_free_device(input);
		kfree(tsdata);
	}

	input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ;
	input->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);

	input_set_abs_params(input, ABS_X, 0, 0x3FF, 0, 0);
	input_set_abs_params(input, ABS_Y, 0, 0x3FF, 0, 0);
	input_set_abs_params(input, ABS_PRESSURE, 0, 255, 0, 0);


	input->name = client->name;
	input->id.bustype = BUS_I2C;
	input->dev.parent = &client->dev;
	input->open = ts_open;
	input->close = ts_close;

	input_set_drvdata(input, tsdata);
	
//	i2c_set_clientdata(client, &tsdata);
	
	dev_set_drvdata(&client->dev, tsdata);

	tsdata->irq = client->irq;
	tsdata->input = input;
	tsdata->client = client;
	
	INIT_WORK(&tsdata->work.work, ts_poscheck);

	if (input_register_device(input)) {
		input_free_device(input);
		kfree(tsdata);
	}

	error = gpio_request(GPIO_RESET, "touch_reset");

	if (error < 0) {
		pr_err("gpio-irq: failed to request GPIO %d,"
			" error %d\n", S5PV210_GPH2(5), error);
	}
	
	s3c_gpio_cfgpin(GPIO_RESET,S3C_GPIO_OUTPUT);
	
	gpio_direction_output(GPIO_RESET,0);
	mdelay(20);
	gpio_direction_output(GPIO_RESET,1);	
	mdelay(100);

	calibrate_cmd = 1;

	if(calibrate_cmd)
	{
		calibrate(tsdata->client);
	}
	
	error = gpio_request(GPIO_INT, "touch_irq");

	if (error < 0) {
		pr_err("gpio-irq: failed to request GPIO %d,"
			" error %d\n", GPIO_INT, error);
	}
	
	error = gpio_direction_input(GPIO_INT);
	if (error < 0) {
		pr_err("gpio-keys: failed to configure input"
			" direction for GPIO %d, error %d\n",
			GPIO_INT, error);
		gpio_free(GPIO_INT);
	}

	s3c_gpio_cfgpin(GPIO_INT,0x0f);
	s3c_gpio_setpull(GPIO_INT,S3C_GPIO_PULL_UP);

	if (request_irq(tsdata->irq, ts_ts_isr, IRQF_SHARED|IRQF_TRIGGER_FALLING,client->name, tsdata)) {
	//if (request_irq(tsdata->irq, ts_ts_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,client->name, tsdata)) {
		dev_err(&client->dev, "Unable to request touchscreen IRQ.\n");
		input_unregister_device(input);
		input = NULL;
	}

	dev_err(&tsdata->client->dev, "probe successfully!\n");
	return 0;
}


static int ts_i2c_suspend(struct i2c_client *client, pm_message_t mesg)
{
	return 0;
}

static int ts_i2c_resume(struct i2c_client *client)
{
	return 0;
}

static int ts_i2c_remove(struct i2c_client *client)
{
	struct ts_i2c_data *tsdata = dev_get_drvdata(&client->dev);
	free_irq(tsdata->irq, tsdata);
	
	input_unregister_device(tsdata->input);
	kfree(tsdata);
	dev_set_drvdata(&client->dev, NULL);

	return 0;
}

static const struct i2c_device_id ts_i2c_id[] =
{
	{ "gzsd_ts", 0 },
	{ }
};
MODULE_DEVICE_TABLE( i2c, ts_i2c_id);

static struct i2c_driver ts_i2c_driver =
{ 	.driver =
		{
			.owner = THIS_MODULE,
			.name = "gzsd_multi_touch_driver",
		}, 
	.probe = ts_i2c_probe, 
	.remove = ts_i2c_remove,
	.suspend = ts_i2c_suspend, 
	.resume = ts_i2c_resume,
	.id_table = ts_i2c_id, 
};


int ts_attach_adapter(void)
{
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct i2c_client *client;

	printk("ts_attach_adapter for icool210.\n");
	
	adapter = i2c_get_adapter(I2C_BUS_NUM);

	memset(&info, 0, sizeof(struct i2c_board_info));

	strlcpy(info.type, "gzsd_ts", I2C_NAME_SIZE);
		
	info.addr = SLAVE_ADDR;
	client = i2c_new_device(adapter, &info);
	if (!client)
	{
		printk("failed to add i2c driver\n");
		return -ENODEV;
	}
	
	client->irq = IRQ;

	/*******************************class*************************************/
	ts_class = class_create(THIS_MODULE, "gzsd_multi_touch_class");
	if (IS_ERR(ts_class)) {
		printk(KERN_WARNING "Unable to create ts class; errno = %ld\n",
				PTR_ERR(ts_class));
		return PTR_ERR(ts_class);
	}

	ts_class->dev_attrs = ts_class_device_attributes;
	ts_class->suspend = ts_class_suspend;
	ts_class->resume = ts_class_resume;
	
	ts_class_device_register(client);
	/*******************************class*************************************/

	return 0;
}


static int __init i2c_ts_init(void)
{
	int ret;
	
	printk("gzsd multi touch init\n");
	
	ts_wq = create_singlethread_workqueue("ts_wq");
	if(!ts_wq)
		return -ENOMEM;	
	
	ret = ts_attach_adapter();
	if (ret != 0)
		pr_err("Failed to ts_attach_adapter: %d\n", ret);
		
	return i2c_add_driver(&ts_i2c_driver);
}

static void __exit i2c_ts_exit(void)
{
	i2c_del_driver(&ts_i2c_driver);
	
	if(ts_wq)
		destroy_workqueue(ts_wq);
		
	/*******************************class*************************************/		
	class_destroy(ts_class);
	/*******************************class*************************************/
}

module_init( i2c_ts_init);
module_exit( i2c_ts_exit);



MODULE_AUTHOR(" gzsdit@gmail.com ");
MODULE_DESCRIPTION("gzsd multi-touch driver");
MODULE_LICENSE("GPL");
