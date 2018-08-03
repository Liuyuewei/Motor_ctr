/*
 * �����嵥��������Ƴ���
 * ����������ͨ����ʱ��������gpio��������Ӷ����Ʋ������
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "finsh.h"
#include "motor.h"
#include "common.h"
#include "timer3.h"
#include "log.h"

#define MOTOR_DEG	0
#define MOTOR_RES	0
//���1
#define M1_EN_H		rt_pin_write(eM1_en,1)		//���1 ʹ�ܸ�
#define M1_EN_L		rt_pin_write(eM1_en,0)		//���1 ʹ�ܵ�
#define	A1_P_H		rt_pin_write(eA1_P, 1)		//���1 A+ ��
#define	A1_P_L		rt_pin_write(eA1_P, 0)		//���1 A+ ��
#define	A1_N_H		rt_pin_write(eA1_N, 1)		//���1 A- ��
#define	A1_N_L		rt_pin_write(eA1_N, 0)		//���1 A- ��
#define	B1_P_H		rt_pin_write(eB1_P, 1)		//���1 B+ ��
#define	B1_P_L		rt_pin_write(eB1_P, 0)		//���1 B+ ��
#define	B1_N_H		rt_pin_write(eB1_N, 1)		//���1 B- ��
#define	B1_N_L		rt_pin_write(eB1_N, 0)		//���1 B- ��
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

#if 1
//��������
//���1	
#define MOTOR1_STEP1	A1_P_H;	A1_N_L;  B1_P_H; B1_N_L
#define MOTOR1_STEP2	A1_P_L;	A1_N_H;  B1_P_H; B1_N_L
#define MOTOR1_STEP3	A1_P_L;	A1_N_H;  B1_P_L; B1_N_H
#define MOTOR1_STEP4	A1_P_H;	A1_N_L;  B1_P_L; B1_N_H
//���2
#define MOTOR2_STEP1	A2_P_H;	A2_N_L;  B2_P_H; B2_N_L
#define MOTOR2_STEP2	A2_P_L;	A2_N_H;  B2_P_H; B2_N_L
#define MOTOR2_STEP3	A2_P_L;	A2_N_H;  B2_P_L; B2_N_H
#define MOTOR2_STEP4	A2_P_H;	A2_N_L;  B2_P_L; B2_N_H

#else	
//��������
//���1	
#define MOTOR1_STEP1_S	A1_P_H;	A1_N_L 
#define MOTOR1_STEP2_S	B1_P_H; B1_N_L
#define MOTOR1_STEP3_S	A1_P_L;	A1_N_H
#define MOTOR1_STEP4_S	B1_P_L; B1_N_H
//���2
#define MOTOR2_STEP1_S	A2_P_H;	A2_N_L
#define MOTOR2_STEP2_S	B2_P_H; B2_N_L 
#define MOTOR2_STEP3_S	A2_P_L;	A2_N_H
#define MOTOR2_STEP4_S	B2_P_L; B2_N_H
#endif

/********************���������ȡ**********************/
#define	KEY_F	rt_pin_read(eKEY_F)		//��ת
#define	KEY_R	rt_pin_read(eKEY_R)		//��ת
#define	KEY_S	rt_pin_read(eKEY_S)		//ֹͣ


/********************��λ�����ȡ**********************/
//�������Ϊ�͵�ƽ�������λ��ʱΪ�ߣ�ָʾ����
#define	CHANNEL1_F	rt_pin_read(eChannel1_F)
#define	CHANNEL1_B	rt_pin_read(eChannel1_B)

#define	CHANNEL2_F	rt_pin_read(eChannel2_F)
#define	CHANNEL2_B	rt_pin_read(eChannel2_B)

#define	CHANNEL3_F	rt_pin_read(eChannel3_F)
#define	CHANNEL3_B	rt_pin_read(eChannel3_B)

#define	CHANNEL4_F	rt_pin_read(eChannel4_F)
#define	CHANNEL4_B	rt_pin_read(eChannel4_B)

/********************��ʱ��**********************/
// Tout = ARR * PSC / 72000000 s
// Tout:��ʱ���ж�����
// 72000000:Ϊ72MHz 
// ͨ������ ARR��СΪ14����1.4ms Ƶ��Ϊ 1000/1.4 �ӽ�1k��
// ���ARR��С�Ļ�����motor�߳��޷���ʱ����ʱ���ͷŵ��ź���
// ARR Խ�� ���Ƶ��Խ��
#define ARR	30		//�Զ���װ�ؼĴ������ڵ�ֵ
#define PSC	7200	//ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ

//���尴����ʼֵ
static rt_uint8_t key_f = 1;	//��ת
static rt_uint8_t key_r = 1;	//��ת
static rt_uint8_t key_s = 1;	//ֹͣ

rt_uint8_t step_f = 1;
rt_uint8_t step_r = 4;

rt_uint8_t tim3_flag = 0;

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t motor_stack[ 512 ];

/* �̵߳�TCB���ƿ� */
static struct rt_thread motor_thread;


//	eLed_run = 0,						//LED_RUN		PA2
//	eBeep_ctrl,                         //������			PB1
//	
//	/********ж�����GPIO********/
//	eM1_en,								//ж�����ʹ��	PC8
//	eA1_P,								//ж�����A1+	PC7	
//	eA1_N,								//ж�����A1-	PB15
//	eB1_P,								//ж�����B1+	PB14
//	eB1_N,								//ж�����B1-	PC6
//	
//	/********������GPIO********/
//	eM2_en,								//������ʹ��	PA11
//	eA2_P,								//������A2+	PA8
//	eA2_N,								//������A2-	PB13
//	eB2_P,								//������B2+	PB12
//	eB2_N,								//������B2-	PC9
//	
//	/********����GPIO********/
//	eKEY_F,								//�����ת		PA7
//	eKEY_R,								//�����ת		PC5
//	eKEY_S,								//���ֹͣ		PB0

//�����������ģʽ����
void rt_hw_motor_init(void)
{
	//���1
	rt_pin_mode(eM1_en, PIN_MODE_OUTPUT);
	rt_pin_mode(eA1_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eA1_N,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB1_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB1_N,  PIN_MODE_OUTPUT);
	//ʹ�ܽţ���ʹ�� ��ʼ��
	rt_pin_write(eM1_en,PIN_LOW);	
	rt_pin_write(eA1_P,PIN_LOW);
	rt_pin_write(eA1_N,PIN_LOW);	
	rt_pin_write(eB1_P,PIN_LOW);
	rt_pin_write(eB1_N,PIN_LOW);	
	//���2
	rt_pin_mode(eM2_en, PIN_MODE_OUTPUT);
	rt_pin_mode(eA2_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eA2_N,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB2_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB2_N,  PIN_MODE_OUTPUT);	
	//ʹ�ܽţ���ʹ��		��ʼ��
	rt_pin_write(eM2_en,PIN_LOW);	
	rt_pin_write(eA2_P,PIN_LOW);
	rt_pin_write(eA2_N,PIN_LOW);	
	rt_pin_write(eB2_P,PIN_LOW);
	rt_pin_write(eB2_N,PIN_LOW);	
	
	//��������
	rt_pin_mode(eKEY_F,PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(eKEY_R,PIN_MODE_INPUT_PULLUP);	
	rt_pin_mode(eKEY_S,PIN_MODE_INPUT_PULLUP);
	
	
	//�ĸ�ͨ����λ	�������Ϊ�͵�ƽ�������λ��ʱΪ�ߣ�ָʾ����
	rt_pin_mode(eChannel1_F,PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(eChannel1_B,PIN_MODE_INPUT_PULLDOWN);
	
	rt_pin_mode(eChannel2_F,PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(eChannel2_B,PIN_MODE_INPUT_PULLDOWN);
	
	rt_pin_mode(eChannel3_F,PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(eChannel3_B,PIN_MODE_INPUT_PULLDOWN);
	
	rt_pin_mode(eChannel4_F,PIN_MODE_INPUT_PULLDOWN);
	rt_pin_mode(eChannel4_B,PIN_MODE_INPUT_PULLDOWN);
	
}

#define MOTOR_SETP1 1
#define MOTOR_SETP2 2
#define MOTOR_SETP3 3
#define MOTOR_SETP4 4

//����ɨ��
static void key_scan(void)
{
	//�������͵�ƽ���£��ߵ�ƽ�ͷ�
	if(rt_pin_read(eKEY_F) == PIN_LOW)
	{
//		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//��ʱ250ms
		key_f = 0;
		key_r = 1;
		key_s = 1;
	}
		
	else if(rt_pin_read(eKEY_R) == PIN_LOW)
	{
//		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//��ʱ250ms
		key_f = 1;
		key_r = 0;
		key_s = 1;
	}
	
	else if(rt_pin_read(eKEY_S) == PIN_LOW)
	{
//		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//��ʱ250ms
		key_f = 1;
		key_r = 1;
		key_s = 0;
	}
}
//�����ת
static void motor_run_forward()
{
	rt_err_t result;	
	if(tim3_flag == 0)
	{
		TIM3_Start();
		tim3_flag = 1;
	}
	
	rt_pin_write(eM1_en,PIN_HIGH);
	
	//���õȴ��ź�������ȡ�ź��������ź�����������1
	result = rt_sem_take(&timer_sem,RT_WAITING_FOREVER);
	
	if(result == RT_EOK )
	{
		switch(step_f)
		{
			case MOTOR_SETP1:
				MOTOR1_STEP1;
				LOG(MOTOR_DEG,("step1\r\n"));
				step_f++;
			break;
			
			case MOTOR_SETP2:
				MOTOR1_STEP2;
				LOG(MOTOR_DEG,("step2\r\n"));
				step_f++;
			break;
			
			case MOTOR_SETP3:
				MOTOR1_STEP3;
				LOG(MOTOR_DEG,("step3\r\n"));
				step_f++;
			break;
			
			case MOTOR_SETP4:
				MOTOR1_STEP4;
				LOG(MOTOR_DEG,("step4\r\n"));
				if(step_f >= 4)
				step_f = 1;
			break;				
		}
	}
}
//�����ת
static void motor_run_reversal()
{
	rt_err_t result;
	if(tim3_flag == 0)
	{
		TIM3_Start();
		tim3_flag = 1;
	}
	rt_pin_write(eM1_en,PIN_HIGH);
	
	//���õȴ��ź�������ȡ�ź��������ź�����������1
	result = rt_sem_take(&timer_sem,RT_WAITING_FOREVER);
	
	if(result == RT_EOK )
	{
		switch(step_r)
		{
			case MOTOR_SETP1:
				MOTOR1_STEP1;
				LOG(MOTOR_DEG,("step1\r\n"));
				if(step_r <= 1)
				step_r = 4;
			break;
			
			case MOTOR_SETP2:
				MOTOR1_STEP2;
				LOG(MOTOR_DEG,("step2\r\n"));
				step_r--;
			break;
			
			case MOTOR_SETP3:
				MOTOR1_STEP3;
				LOG(MOTOR_DEG,("step3\r\n"));
				step_r--;
			break;
			
			case MOTOR_SETP4:
				MOTOR1_STEP4;
				LOG(MOTOR_DEG,("step4\r\n"));
				step_r--;
			break;				
		}
	}
}
//���ֹͣ
static void motor_run_stop()
{
	
	TIM3_Stop();
	tim3_flag = 0;
	//ʹ�ܽţ���ʹ�� ��ʼ��
	rt_pin_write(eM1_en,PIN_LOW);	
	rt_pin_write(eA1_P,PIN_LOW);
	rt_pin_write(eA1_N,PIN_LOW);	
	rt_pin_write(eB1_P,PIN_LOW);
	rt_pin_write(eB1_N,PIN_LOW);	
	//ʹ�ܽţ���ʹ��		��ʼ��
	rt_pin_write(eM2_en,PIN_LOW);	
	rt_pin_write(eA2_P,PIN_LOW);
	rt_pin_write(eA2_N,PIN_LOW);	
	rt_pin_write(eB2_P,PIN_LOW);
	rt_pin_write(eB2_N,PIN_LOW);
}
static void motor_thread_entry(void *parameter)
{
    rt_hw_motor_init();
	TIM3_Init(ARR,PSC - 1);	//��ʼ����ʱ���������ж�����
	//����֮�����Զ������˶�
    while (1)
    {
		//�õ��Է����ǣ�����ɨ��õ���ֵһֱΪ�͵�ƽ�������Ƿ����ԭ�򣬾������Ӳ�������Ƿ�Ϊ����bug��
		key_scan();
		if(key_f == 0)
		{
			key_r = 1;
			key_s = 1;
			motor_run_forward();	
			if(CHANNEL1_F == 1 || CHANNEL2_F == 1 || CHANNEL3_F == 1 || CHANNEL4_F == 1)
			{
				TIM3_Stop();
				key_f = 1;
				key_r = 0;
				key_s = 1;
			}
				
		}
		else if(key_r == 0)
		{	
			key_f = 1;
			key_s = 1;
			motor_run_reversal();	
			if(CHANNEL1_B == 1 || CHANNEL2_B == 1 || CHANNEL3_B == 1 || CHANNEL4_B == 1)
			{
				TIM3_Stop();
				key_f = 0;
				key_r = 1;
				key_s = 1;
			}				
		}
		else if(key_s == 0)
		{
			key_r = 1;
			key_f = 1;
			motor_run_stop();	
		}	
		else
		{
			rt_thread_delay(RT_TICK_PER_SECOND / 4);	//250ms	
			LOG(MOTOR_DEG,("motor idle !\r\n"));
		}	
    }
}

int thread_init_motor(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&motor_thread,
                            "motor",
                            motor_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&motor_stack[0],
                            sizeof(motor_stack),
                            RT_THREAD_PRIORITY_MOTOR,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&motor_thread);
    }
    return 0;
}

    /* ���������RT_SAMPLES_AUTORUN������뵽��ʼ���߳����Զ����� */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(motor_ctr_init);
#endif

/* ������ msh �����б��� */
MSH_CMD_EXPORT(thread_init_motor, motor contrl);


