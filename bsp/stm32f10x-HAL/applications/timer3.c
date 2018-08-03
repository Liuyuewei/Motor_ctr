/*
 * �����嵥����ʱ��
 * ����������ͨ����ʱ��������gpio��������Ӷ����Ʋ������
 */
#include "timer3.h"
#include "common.h"
#include "log.h"

#define TIM3_DEG	1
#define TIM3_RES	1
struct rt_semaphore timer_sem;

void timer_semaphore_init()
{
	rt_err_t result;	
	result = rt_sem_init(&timer_sem,"tim3_sem",0,RT_IPC_FLAG_PRIO);
	if(result != RT_EOK)
	rt_kprintf("Timer sem init failed!\r\n");
}
TIM_HandleTypeDef TIM3_Handler; 									//��ʱ�����
//������ʱ��
void TIM3_Start(void)
{
	HAL_TIM_Base_Start_IT(&TIM3_Handler); 							//ʹ�ܶ�ʱ�� 3 �Ͷ�ʱ�� 3 �����ж�
}
//ֹͣ��ʱ��
void TIM3_Stop(void)
{
	HAL_TIM_Base_Stop_IT(&TIM3_Handler);
}
//ͨ�ö�ʱ�� 3 �жϳ�ʼ��
//arr���Զ���װֵ�� psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ�� 3!(��ʱ�� 3 ���� APB1 �ϣ�ʱ��Ϊ HCLK/2)

void TIM3_Init(rt_uint16_t arr,rt_uint16_t psc)
{
	timer_semaphore_init();											//��ʼ���ź���
	TIM3_Handler.Instance			=	TIM3; 						//ͨ�ö�ʱ�� 3
	TIM3_Handler.Init.Prescaler		=	psc;						//��Ƶϵ��
	TIM3_Handler.Init.CounterMode	=	TIM_COUNTERMODE_UP; 		//���ϼ�����
	TIM3_Handler.Init.Period		=	arr; 						//�Զ�װ��ֵ
	TIM3_Handler.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV1; 	//ʱ�ӷ�Ƶ����
	
	HAL_TIM_Base_Init(&TIM3_Handler); 								//��ʼ����ʱ�� 3
//	HAL_TIM_Base_Start_IT(&TIM3_Handler); 							//ʹ�ܶ�ʱ�� 3 �Ͷ�ʱ�� 3 �����ж�
}

//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻ HAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	__HAL_RCC_TIM3_CLK_ENABLE(); 									//ʹ�� TIM3 ʱ��
	HAL_NVIC_SetPriority(TIM3_IRQn,1,3); 							//�����ж����ȼ�����ռ 1�������ȼ� 3
	HAL_NVIC_EnableIRQ(TIM3_IRQn); 									//���� ITM3 �ж�
}

//��ʱ�� 3 �жϷ�����
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);
}



//��ʱ�� 3 �жϷ���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&TIM3_Handler))
	{
		rt_enter_critical();
		//��ʱ����ʱ���ͷ�һ���ź���
		rt_sem_release(&timer_sem);
		rt_exit_critical();
		LOG(TIM3_DEG,("sem number = %d\r\n",timer_sem.value));
	}
}
#include "finsh.h"
MSH_CMD_EXPORT(TIM3_Stop, stop tim3);
