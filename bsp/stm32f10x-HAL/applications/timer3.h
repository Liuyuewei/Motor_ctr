#ifndef TIMER3_H__
#define TIMER3_H__
#include <rtthread.h>
#include "stm32f1xx_hal.h"


//extern TIM_HandleTypeDef TIM3_Handler;      //��ʱ����� 
//��ʼ����ʱ��
void TIM3_Init(rt_uint16_t arr,rt_uint16_t psc);
void TIM4_Init(rt_uint16_t arr,rt_uint16_t psc);
//������ʱ��
void TIM3_Start(void);
void TIM4_Start(void);
//ֹͣ��ʱ��
void TIM3_Stop(void);
void TIM4_Stop(void);
#endif

