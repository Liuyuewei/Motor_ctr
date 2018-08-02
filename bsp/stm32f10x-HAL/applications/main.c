/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include "led.h"
#include "motor.h"
#include "modbus_app.h"
int main(void)
{
	/* user app entry */
	thread_init_led();			//��������ָʾ���߳�
	thread_init_motor();		//������������߳�
	thread_init_modbus();		//����modbus�߳�
	return 0;
}
