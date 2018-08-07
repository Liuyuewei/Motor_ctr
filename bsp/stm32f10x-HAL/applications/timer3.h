#ifndef TIMER3_H__
#define TIMER3_H__
#include <rtthread.h>
#include "stm32f1xx_hal.h"


//extern TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 
//初始化定时器
void TIM3_Init(rt_uint16_t arr,rt_uint16_t psc);
void TIM4_Init(rt_uint16_t arr,rt_uint16_t psc);
//启动定时器
void TIM3_Start(void);
void TIM4_Start(void);
//停止定时器
void TIM3_Stop(void);
void TIM4_Stop(void);
#endif

