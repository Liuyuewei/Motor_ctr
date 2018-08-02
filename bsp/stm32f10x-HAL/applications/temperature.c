/*
 * 程序清单：传感器温度采集
 *
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "finsh.h"
#include "common.h"
#include "temperature.h"
#include "log.h"
#include "drv_iic1.h"
#include "drv_iic2.h"
#include "drv_iic3.h"
#include "drv_iic4.h"
#include "drv_mlx90614.h"


#define MLX90614_DEG	1
#define MLX90614_RES	0

rt_device_t MLX90614_device1;
rt_device_t MLX90614_device2;
rt_device_t MLX90614_device3;
rt_device_t MLX90614_device4;

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t tem_stack[ 512 ];
/* 线程的TCB控制块 */
static struct rt_thread tem_thread;

void rt_tem_init(void)
{
	//初始化IIC总线
	IIC1_init();
	IIC2_init();
	IIC3_init();
	IIC4_init();
	
	//结合IIC总线，初始化设备
	mlx90614_Init("mlx90614_1","iic1");
	mlx90614_Init("mlx90614_2","iic2");
	mlx90614_Init("mlx90614_3","iic3");
	mlx90614_Init("mlx90614_4","iic4");
	
}
static void find_mlx90614()
{
	
	MLX90614_device1 = rt_device_find("mlx90614_1");
	if(MLX90614_device1 ==RT_NULL)
	{
		LOG(MLX90614_DEG,("[%s]==>not find!\r\n","mlx90614_1"));
		return;
	}
	
	MLX90614_device2 = rt_device_find("mlx90614_2");
	if(MLX90614_device2 ==RT_NULL)
	{
		LOG(MLX90614_DEG,("[%s]==>not find!\r\n","mlx90614_2"));
		return;
	}
	
	MLX90614_device3 = rt_device_find("mlx90614_3");
	if(MLX90614_device3 ==RT_NULL)
	{
		LOG(MLX90614_DEG,("[%s]==>not find!\r\n","mlx90614_3"));
		return;
	}
	
	MLX90614_device4 = rt_device_find("mlx90614_4");
	if(MLX90614_device4 ==RT_NULL)
	{
		LOG(MLX90614_DEG,("[%s]==>not find!\r\n","mlx90614_4"));
		return;
	}	
}

static void tem_thread_entry(void *parameter)
{ 
	u8_t buf1[2];
	u8_t buf2[2];
	u8_t buf3[2];
	u8_t buf4[2];
	
    rt_tem_init();
	find_mlx90614();
	

    while (1)
    {
		rt_device_control(MLX90614_device1,MLX90614_GET_DATA,buf1);  
		
		rt_thread_delay(RT_TICK_PER_SECOND / 2);
		
		rt_device_control(MLX90614_device2,MLX90614_GET_DATA,buf2);  
		
		rt_thread_delay(RT_TICK_PER_SECOND / 2);
		
		rt_device_control(MLX90614_device3,MLX90614_GET_DATA,buf3);  
		
		rt_thread_delay(RT_TICK_PER_SECOND / 2);
		
		rt_device_control(MLX90614_device4,MLX90614_GET_DATA,buf4);  
		
		rt_thread_delay(RT_TICK_PER_SECOND / 2);
	
    }
}

int thread_init_temperature(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&tem_thread,
                            "tem",
                            tem_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&tem_stack[0],
                            sizeof(tem_stack),
                            RT_THREAD_PRIORITY_LED,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&tem_thread);
    }
    return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(thread_init_temperature,thread_init_temperature);

