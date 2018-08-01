/*
 * 程序清单：电机控制程序
 * 程序描述：通过定时器来控制gpio的输出，从而控制步进电机
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
//电机1
#define M1_EN_H		rt_pin_write(eM1_en,1)		//电机1 使能高
#define M1_EN_L		rt_pin_write(eM1_en,0)		//电机1 使能低
#define	A1_P_H		rt_pin_write(eA1_P, 1)		//电机1 A+ 高
#define	A1_P_L		rt_pin_write(eA1_P, 0)		//电机1 A+ 低
#define	A1_N_H		rt_pin_write(eA1_N, 1)		//电机1 A- 高
#define	A1_N_L		rt_pin_write(eA1_N, 0)		//电机1 A- 低
#define	B1_P_H		rt_pin_write(eB1_P, 1)		//电机1 B+ 高
#define	B1_P_L		rt_pin_write(eB1_P, 0)		//电机1 B+ 低
#define	B1_N_H		rt_pin_write(eB1_N, 1)		//电机1 B- 高
#define	B1_N_L		rt_pin_write(eB1_N, 0)		//电机1 B- 低
//电机2
#define M2_EN_H		rt_pin_write(eM2_en,1)		//电机1 使能高
#define M2_EN_L		rt_pin_write(eM2_en,0)		//电机1 使能低
#define	A2_P_H		rt_pin_write(eA2_P, 1)		//电机1 A+ 高
#define	A2_P_L		rt_pin_write(eA2_P, 0)		//电机1 A+ 低
#define	A2_N_H		rt_pin_write(eA2_N, 1)		//电机1 A- 高
#define	A2_N_L		rt_pin_write(eA2_N, 0)		//电机1 A- 低
#define	B2_P_H		rt_pin_write(eB2_P, 1)		//电机1 B+ 高
#define	B2_P_L		rt_pin_write(eB2_P, 0)		//电机1 B+ 低
#define	B2_N_H		rt_pin_write(eB2_N, 1)		//电机1 B- 高
#define	B2_N_L		rt_pin_write(eB2_N, 0)		//电机1 B- 低

//两相，四拍：(+A)(+B)--(-A)(+B)--(-A)(-B)--(+A)(-B)-- 
//单相，四拍：(+A)--(+B)--(-A)--(-B)-- 

#if 1
//两相四排
//电机1	
#define MOTOR1_STEP1	A1_P_H;	A1_N_L;  B1_P_H; B1_N_L
#define MOTOR1_STEP2	A1_P_L;	A1_N_H;  B1_P_H; B1_N_L
#define MOTOR1_STEP3	A1_P_L;	A1_N_H;  B1_P_L; B1_N_H
#define MOTOR1_STEP4	A1_P_H;	A1_N_L;  B1_P_L; B1_N_H
//电机2
#define MOTOR2_STEP1	A2_P_H;	A2_N_L;  B2_P_H; B2_N_L
#define MOTOR2_STEP2	A2_P_L;	A2_N_H;  B2_P_H; B2_N_L
#define MOTOR2_STEP3	A2_P_L;	A2_N_H;  B2_P_L; B2_N_H
#define MOTOR2_STEP4	A2_P_H;	A2_N_L;  B2_P_L; B2_N_H

#else	
//单相四排
//电机1	
#define MOTOR1_STEP1_S	A1_P_H;	A1_N_L 
#define MOTOR1_STEP2_S	B1_P_H; B1_N_L
#define MOTOR1_STEP3_S	A1_P_L;	A1_N_H
#define MOTOR1_STEP4_S	B1_P_L; B1_N_H
//电机2
#define MOTOR2_STEP1_S	A2_P_H;	A2_N_L
#define MOTOR2_STEP2_S	B2_P_H; B2_N_L 
#define MOTOR2_STEP3_S	A2_P_L;	A2_N_H
#define MOTOR2_STEP4_S	B2_P_L; B2_N_H
#endif


ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t motor_stack[ 512 ];

/* 线程的TCB控制块 */
static struct rt_thread motor_thread;


//	eLed_run = 0,						//LED_RUN		PA2
//	eBeep_ctrl,                         //蜂鸣器			PB1
//	
//	/********卸杯电机GPIO********/
//	eM1_en,								//卸杯电机使能	PC8
//	eA1_P,								//卸杯电机A1+	PC7	
//	eA1_N,								//卸杯电机A1-	PB15
//	eB1_P,								//卸杯电机B1+	PB14
//	eB1_N,								//卸杯电机B1-	PC6
//	
//	/********锁轴电机GPIO********/
//	eM2_en,								//锁轴电机使能	PA11
//	eA2_P,								//锁轴电机A2+	PA8
//	eA2_N,								//锁轴电机A2-	PB13
//	eB2_P,								//锁轴电机B2+	PB12
//	eB2_N,								//锁轴电机B2-	PC9
//	
//	/********按键GPIO********/
//	eKEY_F,								//电机正转		PA7
//	eKEY_R,								//电机反转		PC5
//	eKEY_S,								//电机停止		PB0

//电机控制引脚模式配置
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
	TIM3_Init(5000-1,9000-1);	//用来设置定时器频率

    while (1)
    {
		//永久等待信号量，获取信号量，则信号量的数量减1
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

    /* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(motor_ctr_init);
#endif

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(motor_ctr_init, motor contrl);

