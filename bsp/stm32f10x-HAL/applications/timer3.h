#ifndef TIMER_H__
#define TIMER_H__
#include <rtthread.h>
#include "stm32f1xx_hal.h"


//extern TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 
//初始化定时器
void TIM3_Init(rt_uint16_t arr,rt_uint16_t psc);
//启动定时器
void TIM3_Start(void);
//停止定时器
void TIM3_Stop(void);

#endif

