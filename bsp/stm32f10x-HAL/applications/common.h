#ifndef COMMON_H__
#define COMMON_H__
#include "rtconfig.h"

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
	
}pinName_t;


//�߳����ȼ�	RT_THREAD_PRIORITY_MAX	32

#define RT_THREAD_PRIORITY_LED		RT_THREAD_PRIORITY_MAX			//LED	
#define RT_THREAD_PRIORITY_MOTOR	RT_THREAD_PRIORITY_MAX - 1		//motor	




#endif

