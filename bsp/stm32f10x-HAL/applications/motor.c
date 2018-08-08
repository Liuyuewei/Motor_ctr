/*
 * �����嵥��������Ƴ���
 * �������������߳���Ҫ��ɨ�谴������λ������
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "finsh.h"
#include "motor.h"
#include "common.h"
#include "timer3.h"
#include "log.h"
#include "user_mb_app.h"

#define MOTOR_DEG	0
#define MOTOR_RES	0

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
// ͨ������ ARR��СΪ13����1.3ms Ƶ��Ϊ 1000/1.3 �ӽ�744Hz��
// ���ARR��С�Ļ�����motor�߳��޷���ʱ����ʱ���ͷŵ��ź���
// ARR Խ�� ���Ƶ��Խ��
#define ARR1	26		//�Զ���װ�ؼĴ������ڵ�ֵ
#define ARR2	100		//�Զ���װ�ؼĴ������ڵ�ֵ
#define PSC		7200 / 2	//ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ

//���尴����ʼֵ
rt_uint8_t key_f = 1;	//��ת
rt_uint8_t key_r = 1;	//��ת
rt_uint8_t key_s = 0;	//ֹͣ
//��ʱ��������־
static rt_uint8_t tim3_start_flag = 0;

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



//����ɨ��
static void key_scan(void)
{
	//�������͵�ƽ���£��ߵ�ƽ�ͷ�
	if(rt_pin_read(eKEY_F) == PIN_LOW)
	{
		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//��ʱ250ms
//		if(rt_pin_read(eKEY_F) == PIN_LOW)
//		{
			key_f = 0;
			key_r = 1;
			key_s = 1;
//		}
	}
		
	else if(rt_pin_read(eKEY_R) == PIN_LOW)
	{
		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//��ʱ250ms
//		if(rt_pin_read(eKEY_R) == PIN_LOW)
//		{
			key_f = 1;
			key_r = 0;
			key_s = 1;
//		}
	}
	
	else if(rt_pin_read(eKEY_S) == PIN_LOW)
	{
		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//��ʱ250ms
//		if(rt_pin_read(eKEY_F) == PIN_LOW)
//		{
			key_f = 1;
			key_r = 1;
			key_s = 0;
//		}
	}
}



static void motor_thread_entry(void *parameter)
{
    rt_hw_motor_init();
//	rt_pin_mode(eLed_run, PIN_MODE_OUTPUT);
	TIM3_Init(ARR1,PSC - 1);	//��ʼ����ʱ���������ж�����
	TIM4_Init(ARR2,PSC - 1);
	//����֮�����Զ������˶�
    while (1)
    {
		//�õ��Է����ǣ�����ɨ��õ���ֵһֱΪ�͵�ƽ�������Ƿ����ԭ����Ӳ������������
		key_scan();
//		if((rt_int16_t)usSRegHoldBuf[eStep1] <= 0 || (rt_int16_t)usSRegHoldBuf[eStep2] <= 0)
//		{
//			key_f = 1;
//			key_r = 1;
//			key_s = 0;
//		}
		if(key_f == 0)
		{
			//���
			if(!tim3_start_flag)
			{
				TIM3_Start();
				TIM4_Start();
				tim3_start_flag = 1;
			}
			key_r = 1;
			key_s = 1;
			if(CHANNEL1_F == 1 || CHANNEL2_F == 1 || CHANNEL3_F == 1 || CHANNEL4_F == 1)
			{
				TIM3_Stop();
				TIM4_Stop();
				tim3_start_flag = 0;
				key_f = 1;
				key_r = 0;
				key_s = 1;
			}	
		}
		else if(key_r == 0)
		{	
			if(!tim3_start_flag)
			{
				TIM3_Start();
				TIM4_Start();
				tim3_start_flag = 1;
			}
			key_f = 1;
			key_s = 1;
			if(CHANNEL1_B == 1 || CHANNEL2_B == 1 || CHANNEL3_B == 1 || CHANNEL4_B == 1)
			{
				TIM3_Stop();
				TIM4_Stop();
				tim3_start_flag = 0;
				key_f = 0;
				key_r = 1;
				key_s = 1;
			}		
		}
		else if(key_s == 0)
		{
			tim3_start_flag = 0;
			key_r = 1;
			key_f = 1;
			TIM3_Stop();
			TIM4_Stop();
			LOG(MOTOR_DEG,("motor idle !\r\n"));
		}	
		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//
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


//void TM3_Init_my(rt_uint16_t arr)
//{
//	TIM3_Stop();
//	TIM3_Init(arr,PSC - 1);
//	
//}
    /* ���������RT_SAMPLES_AUTORUN������뵽��ʼ���߳����Զ����� */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(motor_ctr_init);
#endif

/* ������ msh �����б��� */
MSH_CMD_EXPORT(thread_init_motor, motor contrl);
//FINSH_FUNCTION_EXPORT(TM3_Init_my,arr);

