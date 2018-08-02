
/*
 * 程序清单：modbus线程
 *
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "modbus_app.h"
#include "common.h"
#include "mb.h"


//从机地址
#define SLAVE_ADD	0X66
//从机串口	3
#define SLAVE_UART	0X03
//波特率
#define SLAVE_BaudRate	115200

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t modbus_stack[ 512 ];
/* 线程的TCB控制块 */
static struct rt_thread modbus_thread;

void rt_modbus_init(void)
{
	//初始化 RTU模式 从机地址为0x66 USART3 115200 无校验
	eMBInit(MB_RTU, SLAVE_ADD, SLAVE_UART, SLAVE_BaudRate, MB_PAR_NONE);
	
	//启动FreeModbus 
	eMBEnable(); 
}

static void modbus_thread_entry(void *parameter)
{
	rt_modbus_init();
	
	unsigned char buff[] = {0x00,0x11,0x00,0x22,0x00,0x33};
	
	hold_regist_write(buff,0,3,MB_REG_WRITE);
	
	
	
    while (1)
    {
		//FreeMODBUS不断查询
		eMBPoll(); 
    }
}

int thread_init_modbus(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&modbus_thread,
                            "modbus",
                            modbus_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&modbus_stack[0],
                            sizeof(modbus_stack),
                            RT_THREAD_PRIORITY_MODBUS,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&modbus_thread);
    }
    return 0;
}
#include "finsh.h"
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(thread_init_modbus, modbus_thread_init);
