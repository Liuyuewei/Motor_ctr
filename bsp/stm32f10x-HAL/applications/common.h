#ifndef COMMON_H__
#define COMMON_H__
#include "rtconfig.h"

//GPIO enum
typedef enum pinName
{
	eLed_run = 0,						//LED_RUN		PA2
	eBeep_ctrl,                         //蜂鸣器			PB1
	
	/********卸杯电机GPIO********/
	eM1_en,								//卸杯电机使能	PC8
	eA1_P,								//卸杯电机A1+	PC7	
	eA1_N,								//卸杯电机A1-	PB15
	eB1_P,								//卸杯电机B1+	PB14
	eB1_N,								//卸杯电机B1-	PC6
	
	/********锁轴电机GPIO********/
	eM2_en,								//锁轴电机使能	PA11
	eA2_P,								//锁轴电机A2+	PA8
	eA2_N,								//锁轴电机A2-	PB13
	eB2_P,								//锁轴电机B2+	PB12
	eB2_N,								//锁轴电机B2-	PC9
	
	/********按键GPIO********/
	eKEY_F,								//电机正转		PA7
	eKEY_R,								//电机反转		PC5
	eKEY_S,								//电机停止		PB0
	
}pinName_t;


//线程优先级	RT_THREAD_PRIORITY_MAX	32

#define RT_THREAD_PRIORITY_LED		RT_THREAD_PRIORITY_MAX			//LED	
#define RT_THREAD_PRIORITY_MOTOR	RT_THREAD_PRIORITY_MAX - 1		//motor	




#endif

