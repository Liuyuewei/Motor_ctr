#ifndef TIMER4_H__
#define TIMER4_H__
#include <rtthread.h>
#include "stm32f1xx_hal.h"


//extern TIM_HandleTypeDef TIM4_Handler;      //��ʱ����� 
//��ʼ����ʱ��
void TIM4_Init(rt_uint16_t arr,rt_uint16_t psc);
//������ʱ��
void TIM4_Start(void);
//ֹͣ��ʱ��
void TIM4_Stop(void);

#endif

