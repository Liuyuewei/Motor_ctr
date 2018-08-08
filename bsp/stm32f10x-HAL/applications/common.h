#ifndef COMMON_H__
#define COMMON_H__
#include "rtconfig.h"
#include "rtthread.h"
#include "mb.h"



//���尴����ʼֵ
extern rt_uint8_t key_f ;	//��ת
extern rt_uint8_t key_r;	//��ת
extern rt_uint8_t key_s;	//ֹͣ

//GPIO enum
typedef enum pinName
{
	eLed_run = 0,						//LED_RUN		PA2
	eBeep_ctrl,                         //������			PB1
	
	/********ж�����GPIO********/
	eM1_en,								//ж�����ʹ��	PC8
	eA1_P,								//ж�����A1+	PC7	
	eA1_N,								//ж�����A1-	PB15
	eB1_P,								//ж�����B1+	PB14
	eB1_N,								//ж�����B1-	PC6
	
	/********������GPIO********/
	eM2_en,								//������ʹ��	PA11
	eA2_P,								//������A2+	PA8
	eA2_N,								//������A2-	PB13
	eB2_P,								//������B2+	PB12
	eB2_N,								//������B2-	PC9
	
	/********����GPIO********/
	eKEY_F,								//�����ת		PA7
	eKEY_R,								//�����ת		PC5
	eKEY_S,								//���ֹͣ		PB0
	
	/********��λGPIO********/
	eChannel1_B,						//ͨ��1ǰ��λ	PB9
	eChannel1_F,						//ͨ��1����λ	PC0
	
	eChannel2_B,						//ͨ��2ǰ��λ	PB5
	eChannel2_F,						//ͨ��2����λ	PB6
	
	eChannel3_B,						//ͨ��3ǰ��λ	PC12
	eChannel3_F,						//ͨ��3����λ	PD2
	
	eChannel4_B,						//ͨ��4ǰ��λ	PA12
	eChannel4_F,						//ͨ��4����λ	PA15
	
	/********�¶ȴ�����GPIO********/
	eTem1_SCL,							//ͨ��1�¶ȴ�����SCL		PC1
	eTem1_SDA,							//ͨ��1�¶ȴ�����SDA		PC2
	
	eTem2_SCL,							//ͨ��2�¶ȴ�����SCL		PB7
	eTem2_SDA,							//ͨ��2�¶ȴ�����SDA		PB8	
	
	eTem3_SCL,							//ͨ��3�¶ȴ�����SCL		PB3
	eTem3_SDA,							//ͨ��3�¶ȴ�����SDA		PB4
	
	eTem4_SCL,							//ͨ��4�¶ȴ�����SCL		PC10
	eTem4_SDA,							//ͨ��4�¶ȴ�����SDA		PC11
			
}pinName_t;
//GPIO enum
typedef enum hold_reginst
{
	//�ĸ�ͨ���¶�
	eTem1_h = 0,
	eTem1_l,
	eTem2_h,
	eTem2_l,
	eTem3_h,
	eTem3_l,
	eTem4_h,
	eTem4_l,
	eDouble_ctr1,
	eDouble_ctr2,
	eStep1,
	eStep2
			
}hold_reginst_t;

//�߳����ȼ�	RT_THREAD_PRIORITY_MAX	32

#define RT_THREAD_PRIORITY_LED		10			//LED	
#define RT_THREAD_PRIORITY_MOTOR	11			//motor	
#define RT_THREAD_PRIORITY_MODBUS	9			//modbus	

/* �ź������ƿ� */
extern struct rt_semaphore timer_sem;

/******��������д�뱣�ּĴ���******/
void hole_regist_write_float(float data,int location);


#endif

