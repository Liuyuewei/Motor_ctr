/*
 * �����嵥�����������
 *
 * ����ƴ������򵥵����ӣ���������ÿ�ֱ�������г���Ա�Ӵ��ĵ�һ������
 * Hello Worldһ��������������Ӿʹ�����ƿ�ʼ������һ���̣߳�������ʱ�ض�
 * LED���и��£��ػ���
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "finsh.h"
#include "common.h"
#include "led.h"
#include "log.h"

#define LED_DEG	1
#define LED_RES	0

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];
/* �̵߳�TCB���ƿ� */
static struct rt_thread led_thread;

void rt_hw_led_init(void)
{
    rt_pin_mode(eLed_run, PIN_MODE_OUTPUT);
}

static void led_thread_entry(void *parameter)
{
    unsigned int count = 0;

    rt_hw_led_init();

    while (1)
    {
        /* led1 on */
		LOG(LED_DEG,("led on, count : %d\r\n", count));
        count++;
        rt_pin_write(eLed_run, 0);
        rt_thread_delay(RT_TICK_PER_SECOND / 2); /* sleep 0.5 second and switch to other thread */

        /* led1 off */
		LOG(LED_DEG,("led off\r\n"));
        rt_pin_write(eLed_run, 1);
        rt_thread_delay(RT_TICK_PER_SECOND / 2);
    }
}

int thread_init_led(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&led_thread,
                            "led",
                            led_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&led_stack[0],
                            sizeof(led_stack),
                            RT_THREAD_PRIORITY_LED,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&led_thread);
    }
    return 0;
}
    /* ���������RT_SAMPLES_AUTORUN������뵽��ʼ���߳����Զ����� */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(led_run_init);
#endif
/* ������ msh �����б��� */
MSH_CMD_EXPORT(thread_init_led, led run);

