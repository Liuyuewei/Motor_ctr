#ifndef COMMON_H__
#define COMMON_H__
#include "rtconfig.h"
#include "rtthread.h"


//GPIO enum
typedef enum pinName
{
	eLed_run = 0,						//LED_RUN		PA2
	eBeep_ctrl,                         //������			PB1
	
	/********ж�����GPIO********/
	eM1_en,								//ж�����ʹ��	PC8
	eA1_P,								//ж�����A1+	PC7	
	eA1_N,								//ж�����A1-	PB15
	eB1_P,								//ж�����B1+	PB14
	eB1_N,								//ж�����B1-	PC6
	
	/********������GPIO********/
	eM2_en,								//������ʹ��	PA11
	eA2_P,								//������A2+	PA8
	eA2_N,								//������A2-	PB13
	eB2_P,								//������B2+	PB12
	eB2_N,								//������B2-	PC9
	
	/********����GPIO********/
	eKEY_F,								//�����ת		PA7
	eKEY_R,								//�����ת		PC5
	eKEY_S,								//���ֹͣ		PB0
	
	/********��λGPIO********/
	eChannel1_F,						//ͨ��1ǰ��λ	PB9
	eChannel1_B,						//ͨ��1����λ	PC0
	
	eChannel2_F,						//ͨ��2ǰ��λ	PB5
	eChannel2_B,						//ͨ��2����λ	PB6
	
	eChannel3_F,						//ͨ��3ǰ��λ	PC12
	eChannel3_B,						//ͨ��3����λ	PD2
	
	eChannel4_F,						//ͨ��4ǰ��λ	PA12
	eChannel4_B,						//ͨ��4����λ	PA15
	
	
}pinName_t;


//�߳����ȼ�	RT_THREAD_PRIORITY_MAX	32

#define RT_THREAD_PRIORITY_LED		10			//LED	
#define RT_THREAD_PRIORITY_MOTOR	11			//motor	
#define RT_THREAD_PRIORITY_MODBUS	12			//modbus	

/* �ź������ƿ� */
extern struct rt_semaphore timer_sem;


#endif

