/*
* File:  vz89te.c
* Brief: This file provide driver for iic_bus
* Change Logs:
* Data      Author Note
* 2018-8-2 刘越伟 第一版本
*/
#include "drv_mlx90614.h"
#include "type_def.h"

#define I2C_ADDR        0X00 >> 1

#define MLX90614_READ_STATUS  0x01    //读状态命令
#define MLX90614_GET_DATA     0x02    //读数据


typedef struct Mlx90614Device
{
    struct rt_device device;
    struct rt_i2c_bus_device *iicBus;
}Mlx90614Device_t;

/*
* brief: write cmd
* param: None
* retval None
*/
static int writeMlx90614(struct rt_i2c_bus_device * i2c_device,unsigned char cmd)
{
    struct rt_i2c_msg msg[2];
    u8_t buf[5] = {0x00,0x00,0x00,0x00,0xFF};
    int err;
    msg[0].addr = I2C_ADDR;
	msg[0].flags = RT_I2C_WR;
	msg[0].len = 1;
	msg[0].buf = &cmd;
    
    msg[1].addr = I2C_ADDR;
	msg[1].flags = RT_I2C_WR | RT_I2C_NO_START;
	msg[1].len = 5;
	msg[1].buf = buf;
    
	err =  rt_i2c_transfer(i2c_device, msg, 2);
	if(err != 1)
		return -1;
	else
		return 0;
}
/*
* brief: read cmd.
* param: None
* retval None
*/
static int readMlx90614(struct rt_i2c_bus_device * i2c_device,unsigned char *data)
{
    struct rt_i2c_msg msg[2];
    int err;
	u8_t buf[1] = {0x07};
	//将从机地址及要读寄存器地址写入设备
    msg[0].addr  = I2C_ADDR;
	msg[0].flags = RT_I2C_WR;
	msg[0].len = 1;
	msg[0].buf = buf;
	
	//重启之后直接去读取3字节的数据
	msg[1].addr  = I2C_ADDR;
	msg[1].flags = RT_I2C_RD;
	msg[1].len = 3;
	msg[1].buf = data;
	
	err =  rt_i2c_transfer(i2c_device, msg, 2);
	if(err != 1)
		return -1;
	else
		return 0;
}

/*
* brief: vz89te_control.
* param: None
* retval None
*/
static rt_err_t rt_mlx90614_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    Mlx90614Device_t * Mlx90614Device;
    int err;

    RT_ASSERT(dev != RT_NULL);
    Mlx90614Device = (Mlx90614Device_t *)dev;
    
    switch(cmd)
    {
        case MLX90614_GET_DATA:
        { 
            err = readMlx90614(Mlx90614Device->iicBus,args);
			if(err != 0)
			{
				return RT_ERROR;
			}
           
			return RT_EOK;
        }          
    }        
    return RT_EOK;
}
/*
* brief: Initialize .
* param: None
* retval None
*/
rt_err_t mlx90614_Init(const char *name,const char * bus)
{
	rt_err_t err;
    Mlx90614Device_t * device;
    device = rt_malloc(sizeof(Mlx90614Device_t));
    if(device == RT_NULL)
        return RT_ERROR;
    rt_memset(device,0,sizeof(Mlx90614Device_t));

    device->device.type	= RT_Device_Class_Char;

    /* register sht21 device */
    device->device.init 	= RT_NULL;
    device->device.open 	= RT_NULL;
    device->device.close	= RT_NULL;
    device->device.read 	= RT_NULL;
    device->device.write	= RT_NULL;
    device->device.control 	= rt_mlx90614_control;

    /* no private */
    device->device.user_data = RT_NULL;
    device->iicBus = rt_i2c_bus_device_find(bus);   //查找总线
    if(device->iicBus == RT_NULL)
    {
        rt_kprintf("not find i2c device\n");
        return RT_ERROR;
    }	
    err = rt_device_register(&device->device, name, RT_DEVICE_FLAG_RDWR);
    return err;
}


