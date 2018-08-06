/*
 * �����嵥����ʱ��
 * ����������ͨ����ʱ��������gpio��������Ӷ����Ʋ������
 */
#include "timer4.h"
#include "common.h"
#include "log.h"
#include "rtdevice.h"
#include "user_mb_app.h"

#define TIM4_DEG	0
#define TIM4_RES	0

static rt_uint8_t step_f = 1;
static rt_uint8_t step_r = 4;

#define MOTOR_SETP1 1
#define MOTOR_SETP2 2
#define MOTOR_SETP3 3
#define MOTOR_SETP4 4


#if 1
//���1
//#define M1_EN_H		rt_pin_write(eM1_en,1)		//���1 ʹ�ܸ�
//#define M1_EN_L		rt_pin_write(eM1_en,0)		//���1 ʹ�ܵ�
//#define	A1_P_H		rt_pin_write(eA1_P, 1)		//���1 A+ ��
//#define	A1_P_L		rt_pin_write(eA1_P, 0)		//���1 A+ ��
//#define	A1_N_H		rt_pin_write(eA1_N, 1)		//���1 A- ��
//#define	A1_N_L		rt_pin_write(eA1_N, 0)		//���1 A- ��
//#define	B1_P_H		rt_pin_write(eB1_P, 1)		//���1 B+ ��
//#define	B1_P_L		rt_pin_write(eB1_P, 0)		//���1 B+ ��
//#define	B1_N_H		rt_pin_write(eB1_N, 1)		//���1 B- ��
//#define	B1_N_L		rt_pin_write(eB1_N, 0)		//���1 B- ��
//���2
#define M2_EN_H		rt_pin_write(eM2_en,1)		//���1 ʹ�ܸ�
#define M2_EN_L		rt_pin_write(eM2_en,0)		//���1 ʹ�ܵ�
#define	A2_P_H		rt_pin_write(eA2_P, 1)		//���1 A+ ��
#define	A2_P_L		rt_pin_write(eA2_P, 0)		//���1 A+ ��
#define	A2_N_H		rt_pin_write(eA2_N, 1)		//���1 A- ��
#define	A2_N_L		rt_pin_write(eA2_N, 0)		//���1 A- ��
#define	B2_P_H		rt_pin_write(eB2_P, 1)		//���1 B+ ��
#define	B2_P_L		rt_pin_write(eB2_P, 0)		//���1 B+ ��
#define	B2_N_H		rt_pin_write(eB2_N, 1)		//���1 B- ��
#define	B2_N_L		rt_pin_write(eB2_N, 0)		//���1 B- ��

//���࣬���ģ�(+A)(+B)--(-A)(+B)--(-A)(-B)--(+A)(-B)-- 
//���࣬���ģ�(+A)--(+B)--(-A)--(-B)-- 


//��������
//���1	
//#define MOTOR1_STEP1	A1_P_H;	A1_N_L;  B1_P_H; B1_N_L
//#define MOTOR1_STEP2	A1_P_L;	A1_N_H;  B1_P_H; B1_N_L
//#define MOTOR1_STEP3	A1_P_L;	A1_N_H;  B1_P_L; B1_N_H
//#define MOTOR1_STEP4	A1_P_H;	A1_N_L;  B1_P_L; B1_N_H
//���2
#define MOTOR2_STEP1	A2_P_H;	A2_N_L;  B2_P_H; B2_N_L
#define MOTOR2_STEP2	A2_P_L;	A2_N_H;  B2_P_H; B2_N_L
#define MOTOR2_STEP3	A2_P_L;	A2_N_H;  B2_P_L; B2_N_H
#define MOTOR2_STEP4	A2_P_H;	A2_N_L;  B2_P_L; B2_N_H
//��������
//���1	
//#define MOTOR1_STEP1_S	A1_P_H;	A1_N_L 
//#define MOTOR1_STEP2_S	B1_P_H; B1_N_L
//#define MOTOR1_STEP3_S	A1_P_L;	A1_N_H
//#define MOTOR1_STEP4_S	B1_P_L; B1_N_H
//���2
#define MOTOR2_STEP1_S	A2_P_H;	A2_N_L
#define MOTOR2_STEP2_S	B2_P_H; B2_N_L 
#define MOTOR2_STEP3_S	A2_P_L;	A2_N_H
#define MOTOR2_STEP4_S	B2_P_L; B2_N_H

#endif


TIM_HandleTypeDef TIM4_Handler; 									//��ʱ�����
//������ʱ��
void TIM4_Start(void)
{
	HAL_TIM_Base_Start_IT(&TIM4_Handler); 							//ʹ�ܶ�ʱ�� 3 �Ͷ�ʱ�� 3 �����ж�
}
static void motor_stop(void);
//ֹͣ��ʱ��
void TIM4_Stop(void)
{
	//motor_stop before stop TIM4.
	motor_stop();
	HAL_TIM_Base_Stop_IT(&TIM4_Handler);
}
//ͨ�ö�ʱ�� 3 �жϳ�ʼ��
//arr���Զ���װֵ�� psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ�� 3!(��ʱ�� 3 ���� APB1 �ϣ�ʱ��Ϊ HCLK/2)

void TIM4_Init(rt_uint16_t arr,rt_uint16_t psc)
{
	TIM4_Handler.Instance			=	TIM4; 						//ͨ�ö�ʱ�� 3
	TIM4_Handler.Init.Prescaler		=	psc;						//��Ƶϵ��
	TIM4_Handler.Init.CounterMode	=	TIM_COUNTERMODE_UP; 		//���ϼ�����
	TIM4_Handler.Init.Period		=	arr; 						//�Զ�װ��ֵ
	TIM4_Handler.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV1; 	//ʱ�ӷ�Ƶ����
	
	HAL_TIM_Base_Init(&TIM4_Handler); 								//��ʼ����ʱ�� 3
//	HAL_TIM_Base_Start_IT(&TIM4_Handler); 							//ʹ�ܶ�ʱ�� 3 �Ͷ�ʱ�� 3 �����ж�
}

//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻ HAL_TIM_Base_Init()��������
static void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	__HAL_RCC_TIM4_CLK_ENABLE(); 									//ʹ�� TIM4 ʱ��
	HAL_NVIC_SetPriority(TIM4_IRQn,1,3); 							//�����ж����ȼ�����ռ 1�������ȼ� 3
	HAL_NVIC_EnableIRQ(TIM4_IRQn); 									//���� ITM3 �ж�
}

//��ʱ�� 3 �жϷ�����
static void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM4_Handler);
}

//�����ת
static void motor_run_forward()
{
	rt_pin_write(eM1_en,PIN_HIGH);
	rt_pin_write(eM2_en,PIN_HIGH);
	rt_uint16_t DOUBLE;
	//���usSRegHoldBuf[eDouble_ctr]��ֵΪ0�������������˶�������Ϊ���������˹���
	DOUBLE = !usSRegHoldBuf[eDouble_ctr2];
	switch(step_f)
	{
		case MOTOR_SETP1:
			if(DOUBLE)
			{
				MOTOR2_STEP1;
			}
			else
			{
				MOTOR2_STEP1_S;	
			}
			step_f++;
		break;
		
		case MOTOR_SETP2:
			if(DOUBLE)
			{
				MOTOR2_STEP2;
			}
			else
			{
				MOTOR2_STEP2_S;	
			}
			step_f++;
		break;
		
		case MOTOR_SETP3:
			if(DOUBLE)
			{
				MOTOR2_STEP3;
			}
			else
			{
				MOTOR2_STEP3_S;	
			}
			step_f++;
		break;
		
		case MOTOR_SETP4:
			if(DOUBLE)
			{
				MOTOR2_STEP4;
			}
			else
			{
				MOTOR2_STEP4_S;	
			}
			if(step_f >= 4)
			step_f = 1;
		break;				
	}
}

//�����ת
static void motor_run_reversal()
{
	rt_pin_write(eM1_en,PIN_HIGH);
	rt_pin_write(eM2_en,PIN_HIGH);
	rt_uint16_t DOUBLE;
	//���usSRegHoldBuf[eDouble_ctr]��ֵΪ0�������������˶�������Ϊ���������˹���
	DOUBLE = ! usSRegHoldBuf[eDouble_ctr2];
	switch(step_r)
	{
		case MOTOR_SETP1:
			if(DOUBLE)
			{
				MOTOR2_STEP1;
			}
			else
			{
				MOTOR2_STEP1_S;	
			}
			if(step_r <= 1)
			step_r = 4;
		break;
		
		case MOTOR_SETP2:
			if(DOUBLE)
			{
				MOTOR2_STEP2;
			}
			else
			{
				MOTOR2_STEP2_S;	
			}
			step_r--;
		break;
		
		case MOTOR_SETP3:
			if(DOUBLE)
			{
				MOTOR2_STEP3;
			}
			else
			{
				MOTOR2_STEP3_S;	
			}
			step_r--;
		break;
		
		case MOTOR_SETP4:
			if(DOUBLE)
			{
				MOTOR2_STEP4;
			}
			else
			{
				MOTOR2_STEP4_S;	
			}
			step_r--;
		break;				
	}
}

//���ֹͣ
static void motor_stop(void)
{
	rt_pin_write(eM2_en,PIN_LOW);	
	rt_pin_write(eA2_P,PIN_LOW);
	rt_pin_write(eA2_N,PIN_LOW);	
	rt_pin_write(eB2_P,PIN_LOW);
	rt_pin_write(eB2_N,PIN_LOW);
}


//��ʱ�� 3 �жϷ���������
static void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&TIM4_Handler))
	{
		if(key_f == 0)
		{
			motor_run_forward();
		}
		else if(key_r == 0)
		{
			motor_run_reversal();	
		}
		LOG(TIM4_DEG,("TIM4 running ......\r\n"));
	}
}

#include "finsh.h"
MSH_CMD_EXPORT(TIM4_Stop, stop TIM4);
