
/*
 * �����嵥��modbus�߳�
 *
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "modbus_app.h"
#include "common.h"
#include "mb.h"
#include "mbutils.h"

//�ӻ���ַ
#define SLAVE_ADD	0X66
//�ӻ�����	3
#define SLAVE_UART	0X03
//������
#define SLAVE_BaudRate	115200

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t modbus_stack[ 512 ];
/* �̵߳�TCB���ƿ� */
static struct rt_thread modbus_thread;

void rt_modbus_init(void)
{
	//��ʼ�� RTUģʽ �ӻ���ַΪ1 USART1 115200 ��У��
	eMBInit(MB_RTU, SLAVE_ADD, SLAVE_UART, SLAVE_BaudRate, MB_PAR_NONE);
	
	//����FreeModbus 
	eMBEnable(); 
}

static void modbus_thread_entry(void *parameter)
{
    while (1)
    {
		//FreeMODBUS���ϲ�ѯ
		eMBPoll(); 
    }
}

int modbus_thread_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&modbus_thread,
                            "modbsu_app",
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
/* ������ msh �����б��� */
MSH_CMD_EXPORT(modbus_thread_init, modbus_thread_init);
