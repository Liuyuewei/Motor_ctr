/*
 * �����嵥��������Ƴ���
 * ����������ͨ����ʱ��������gpio��������Ӷ����Ʋ������
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "finsh.h"
#include "motor.h"
#include "common.h"
#include "timer.h"
#include "log.h"

#define MOTOR_DEG	1
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
	rt_pin_mode(eM1_en, PIN_MODE_OUTPUT);
	rt_pin_mode(eA1_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eA1_N,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB1_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB1_N,  PIN_MODE_OUTPUT);
	
	rt_pin_mode(eM2_en, PIN_MODE_OUTPUT);
	rt_pin_mode(eA2_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eA2_N,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB2_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB2_N,  PIN_MODE_OUTPUT);	
}

#define MOTOR_SETP1 1
#define MOTOR_SETP2 2
#define MOTOR_SETP3 3
#define MOTOR_SETP4 4
static void motor_thread_entry(void *parameter)
{
    unsigned int count = 0;
	rt_err_t result;
	rt_uint8_t step = 1;
    rt_hw_motor_init();
	TIM3_Init(5000-1,9000-1);	//�������ö�ʱ��Ƶ��

    while (1)
    {
		//���õȴ��ź�������ȡ�ź��������ź�����������1
		result = rt_sem_take(&timer_sem,RT_WAITING_FOREVER);
		
		if(result == RT_EOK )
		{
			switch(step)
			{
				case MOTOR_SETP1:
					MOTOR1_STEP1;
					LOG(MOTOR_DEG,"step1\r\n");
					step++;
				break;
				
				case MOTOR_SETP2:
					MOTOR1_STEP2;
					LOG(MOTOR_DEG,"step2\r\n");
					step++;
				break;
				
				case MOTOR_SETP3:
					MOTOR1_STEP3;
					LOG(MOTOR_DEG,"step3\r\n");
					step++;
				break;
				
				case MOTOR_SETP4:
					MOTOR1_STEP4;
					LOG(MOTOR_DEG,"step4\r\n");
					if(step >= 4)
					step = 1;
				break;				
			}
		}
    }
}

int motor_ctr_init(void)
{
    rt_err_t result;

    /* init led thread */
    result = rt_thread_init(&motor_thread,
                            "motro",
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
MSH_CMD_EXPORT(motor_ctr_init, motor contrl);

