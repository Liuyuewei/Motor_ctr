#ifndef COMMON_H__
#define COMMON_H__
#include "rtconfig.h"
#include "rtthread.h"


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
	
	/********限位GPIO********/
	eChannel1_F,						//通道1前限位	PB9
	eChannel1_B,						//通道1后限位	PC0
	
	eChannel2_F,						//通道2前限位	PB5
	eChannel2_B,						//通道2后限位	PB6
	
	eChannel3_F,						//通道3前限位	PC12
	eChannel3_B,						//通道3后限位	PD2
	
	eChannel4_F,						//通道4前限位	PA12
	eChannel4_B,						//通道4后限位	PA15
	
	
}pinName_t;


//线程优先级	RT_THREAD_PRIORITY_MAX	32

#define RT_THREAD_PRIORITY_LED		10			//LED	
#define RT_THREAD_PRIORITY_MOTOR	11			//motor	
#define RT_THREAD_PRIORITY_MODBUS	12			//modbus	

/* 信号量控制块 */
extern struct rt_semaphore timer_sem;


#endif

