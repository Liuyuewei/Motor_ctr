/*
 * �����嵥���������¶Ȳɼ�
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

#define MLX90614_GET_DATA     0x02    //������

rt_device_t device[4];

//MLX90614�豸����
//�豸���ֵĳ�����Ҫ�����̫���Ļ�����ľ��޷�ʶ��
//��ע���豸ʱ����ִ���
//����豸��ͬʱ�����ǰ�漸����ĸ��������
static const char* MLX90614_name[] = 
{
	"TEM1_DEVICE",
	"TEM2_DEVICE",
	"TEM3_DEVICE",
	"TEM4_DEVICE"
};
//IIC��������
static const char* IIC_name[] = 
{
	"iic1",
	"iic2",
	"iic3",
	"iic4"
};

ALIGN(RT_ALIGN_SIZE)

static rt_uint8_t 	tem_stack[ 512 ];

/* �̵߳�TCB���ƿ� */
static struct rt_thread tem_thread;

void rt_tem_init(void)
{
	rt_err_t err;
	//��ʼ��IIC����
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
	
	
	//���IIC���ߣ���ʼ���豸
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
	
	//��ʼ��IIC���߼��豸
    rt_tem_init();
	//�����豸
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
			rt_thread_delay(RT_TICK_PER_SECOND / 2);	//��ʱ500ms	
			
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
/* ������ msh �����б��� */
MSH_CMD_EXPORT(thread_init_temperature,thread_init_temperature);

