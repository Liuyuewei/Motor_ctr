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
#include "type_def.h"
#include "string.h"

#define MLX90614_DEG	1
#define MLX90614_RES	0

rt_device_t device[4];

//MLX90614设备名字
static const char* MLX90614_name[] = 
{
	"mlx90614_1",
	"mlx90614_2",
	"mlx90614_3",
	"mlx90614_4"
};
//IIC总线名字
static const char* IIC_name[] = 
{
	"iic1",
	"iic2",
	"iic3",
	"iic4"
};

ALIGN(RT_ALIGN_SIZE)

static rt_uint8_t 	tem_stack[ 512 ];

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
	for(int i = 0;i<4;i++)
	{
		mlx90614_Init(MLX90614_name[i],IIC_name[i]);
	}
}
static void find_mlx90614()
{
	for(int i=0;i<4;i++)
	{
		device[i] = rt_device_find(MLX90614_name[i]);
		if(device[i] == RT_NULL)
		{
			LOG(MLX90614_DEG,("[%s]==>not find!\r\n",MLX90614_name[i]));
			return;
		}	
	}
}

static void tem_thread_entry(void *parameter)
{ 
	float tem[4];
	
	typedef struct
	{
		u8_t date_l;
		u8_t date_h;
		u8_t pec;
	}buf_t;
	
	buf_t buf[4];
	
	//初始化IIC总线及设备
    rt_tem_init();
	//查找设备
	find_mlx90614();
	

    while (1)
    {
		for(u8_t i=0;i<4;i++)
		{
			rt_device_control(device[i],MLX90614_GET_DATA,&buf[i]);  
			tem[i] = (buf[i].date_h<<8 | buf[i].date_l) * 0.02 -273.15;
			hole_regist_write_float(tem[i],eTem1 + i);			
			rt_thread_delay(RT_TICK_PER_SECOND / 2);	//延时500ms
		}
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

