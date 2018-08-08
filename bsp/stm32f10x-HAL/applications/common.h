#ifndef COMMON_H__
#define COMMON_H__
#include "rtconfig.h"
#include "rtthread.h"
#include "mb.h"



//定义按键初始值
extern rt_uint8_t key_f ;	//正转
extern rt_uint8_t key_r;	//反转
extern rt_uint8_t key_s;	//停止

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
	eChannel1_B,						//通道1前限位	PB9
	eChannel1_F,						//通道1后限位	PC0
	
	eChannel2_B,						//通道2前限位	PB5
	eChannel2_F,						//通道2后限位	PB6
	
	eChannel3_B,						//通道3前限位	PC12
	eChannel3_F,						//通道3后限位	PD2
	
	eChannel4_B,						//通道4前限位	PA12
	eChannel4_F,						//通道4后限位	PA15
	
	/********温度传感器GPIO********/
	eTem1_SCL,							//通道1温度传感器SCL		PC1
	eTem1_SDA,							//通道1温度传感器SDA		PC2
	
	eTem2_SCL,							//通道2温度传感器SCL		PB7
	eTem2_SDA,							//通道2温度传感器SDA		PB8	
	
	eTem3_SCL,							//通道3温度传感器SCL		PB3
	eTem3_SDA,							//通道3温度传感器SDA		PB4
	
	eTem4_SCL,							//通道4温度传感器SCL		PC10
	eTem4_SDA,							//通道4温度传感器SDA		PC11
			
}pinName_t;
//GPIO enum
typedef enum hold_reginst
{
	//四个通道温度
	eTem1_h = 0,
	eTem1_l,
	eTem2_h,
	eTem2_l,
	eTem3_h,
	eTem3_l,
	eTem4_h,
	eTem4_l,
	eDouble_ctr1,
	eDouble_ctr2,
	eStep1,
	eStep2
			
}hold_reginst_t;

//线程优先级	RT_THREAD_PRIORITY_MAX	32

#define RT_THREAD_PRIORITY_LED		10			//LED	
#define RT_THREAD_PRIORITY_MOTOR	11			//motor	
#define RT_THREAD_PRIORITY_MODBUS	9			//modbus	

/* 信号量控制块 */
extern struct rt_semaphore timer_sem;

/******将浮点数写入保持寄存器******/
void hole_regist_write_float(float data,int location);


#endif

