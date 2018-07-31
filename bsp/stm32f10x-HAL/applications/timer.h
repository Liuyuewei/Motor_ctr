#ifndef TIMER_H__
#define TIMER_H__
#include <rtthread.h>
#include "stm32f1xx_hal.h"


extern TIM_HandleTypeDef TIM3_Handler;      //¶¨Ê±Æ÷¾ä±ú 
void TIM3_Init(rt_uint16_t arr,rt_uint16_t psc);

#endif

