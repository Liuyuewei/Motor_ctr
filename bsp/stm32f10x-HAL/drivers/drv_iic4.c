/*
* File:  iic_bus.c
* Brief: This file provide driver for iic_bus
* Change Logs:
* Data      Author Note
* 2017-7-14 刘越伟 第一版本
*/
#include "stm32f1xx_hal.h"
#include "rtdevice.h"
#include "rtthread.h"
#include "drv_iic4.h"
#include "common.h"

/*
* brief: Initialize GPIO.
* param: None
* retval None
*/
static void GPIO_Configuration()
{
	rt_pin_mode(eTem4_SCL,PIN_MODE_OUTPUT_OD);
	rt_pin_mode(eTem4_SDA,PIN_MODE_OUTPUT_OD);
}    

/*
* brief: IIC SET SDA.
* param: state  set 0 or 1
* retval None
*/
static void stm32_set_sda(void *data, rt_int32_t state)
{
    if(state == 1)
		rt_pin_write(eTem4_SDA,PIN_HIGH);
    else
		rt_pin_write(eTem4_SDA,PIN_LOW);
}
/*
* brief: IIC SET SCL.
* param: state  set 0 or 1
* retval None
*/
static void stm32_set_scl(void *data,rt_int32_t state)
{
    if(state == 1)
       rt_pin_write(eTem4_SCL,PIN_HIGH);
    else
       rt_pin_write(eTem4_SCL,PIN_LOW);
}
/*
* brief: IIC GET SCL.
* param: None
* retval 0 or 1
*/
static rt_int32_t stm32_get_scl(void *data)
{
	return rt_pin_read(eTem4_SCL);
}
/*
* brief: IIC GET SDA.
* param: None
* retval 0 or 1
*/
static rt_int32_t stm32_get_sda(void *data)
{
	return rt_pin_read(eTem4_SDA);
}
/*
* brief: delay
* param: None
* retval None
*/
static void stm32_udelay(rt_uint32_t us)
{
    rt_int32_t delta;
    /*获得延时经过的tick数*/
    us = us * (SysTick->LOAD / (1000000 / RT_TICK_PER_SECOND));
    /*获取当前时间*/
    delta = SysTick->VAL;
    /*循环获取当前时间，直到达到指定时间后退出*/
    while(delta - SysTick->VAL < us);
}
/*
* brief: config i2c_bit_ops
* param: None
* retval None
*/
static const struct rt_i2c_bit_ops stm32_i2c_bit_ops = 
{
//  .data    = 
    (void*)0xaa,
//  .set_sda = 
    stm32_set_sda,
//  .set_scl = 
    stm32_set_scl,
//  .get_sda = 
    stm32_get_sda,
//  .get_scl = 
    stm32_get_scl,
//  .udelay = 
    stm32_udelay,
//  .delay_us = 
    20,    
//  .timeout = 
    10  
};
/*
* brief: IIC init
* param: None
* retval None
*/
rt_err_t IIC4_init(void)
{
    static struct rt_i2c_bus_device stm32_iic4;
    GPIO_Configuration();
    
    rt_memset((void *)&stm32_iic4,0,sizeof(struct rt_i2c_bus_device));
    stm32_iic4.ops = (void *)&stm32_i2c_bit_ops;
    stm32_iic4.priv = (void *)&stm32_i2c_bit_ops;
    return rt_i2c_bit_add_bus(&stm32_iic4,"iic4");
}








