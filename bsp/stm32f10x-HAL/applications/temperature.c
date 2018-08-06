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

//#include "drv_mlx90614_1.h"
//#include "drv_mlx90614_2.h"
//#include "drv_mlx90614_3.h"
//#include "drv_mlx90614_4.h"

#include "type_def.h"
#include "string.h"

#define TEM_DEG	0
#define TEM_RES	0

#define MLX90614_GET_DATA     0x02    //读数据

rt_device_t device[4];

//MLX90614设备名字
//设备名字的长度有要求，如果太长的话后面的就无法识别。
//在注册设备时会出现错误。
//多个设备相同时最好在前面几个字母加以区别
static const char* MLX90614_name[] = 
{
	"TEM1_DEVICE",
	"TEM2_DEVICE",
	"TEM3_DEVICE",
	"TEM4_DEVICE"
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
	rt_err_t err;
	//初始化IIC总线
	err = IIC1_init();
	if(err != RT_EOK)
	rt_kprintf("IIC1 init failed !\r\n");
	
	err = IIC2_init();
	if(err != RT_EOK)
	rt_kprintf("IIC1 init failed !\r\n");
	
	err = IIC3_init();
	if(err != RT_EOK)
	rt_kprintf("IIC1 init failed !\r\n");
	
	err = IIC4_init();
	if(err != RT_EOK)
	rt_kprintf("IIC1 init failed !\r\n");
	
	
	//结合IIC总线，初始化设备
	for(int i = 0;i<4;i++)
	{
		err = mlx90614_Init(MLX90614_name[i],IIC_name[i]);
		if(err != RT_EOK)
		rt_kprintf("IIC_device[%s] IIC_buf[%s] init failed !\r\n",MLX90614_name[i],IIC_name[i]);
	}
}
static void find_mlx90614()
{
	for(int i=0;i<4;i++)
	{
		device[i] = rt_device_find(MLX90614_name[i]);
		if(device[i] == RT_NULL)
		{
			LOG(TEM_DEG,("[%s]==>not find!\r\n",MLX90614_name[i]));
			return;
		}	
	}
}

static void tem_thread_entry(void *parameter)
{ 
	float tem[4];
	char data[20];
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
			hole_regist_write_float(tem[i],eTem1_h + i * 2);	
			sprintf(data,"TEM %d => %0.2f",i+1,tem[i]);			
			LOG(TEM_DEG,("%s\r\n",data));			
			if(i == 3)
			LOG(TEM_DEG,("********************\r\n"));
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

