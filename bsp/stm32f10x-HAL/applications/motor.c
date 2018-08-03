/*
 * 程序清单：电机控制程序
 * 程序描述：通过定时器来控制gpio的输出，从而控制步进电机
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

/********************按键输入读取**********************/
#define	KEY_F	rt_pin_read(eKEY_F)		//正转
#define	KEY_R	rt_pin_read(eKEY_R)		//反转
#define	KEY_S	rt_pin_read(eKEY_S)		//停止


/********************限位输入读取**********************/
//正常情况为低电平，到达该位置时为高，指示灯亮
#define	CHANNEL1_F	rt_pin_read(eChannel1_F)
#define	CHANNEL1_B	rt_pin_read(eChannel1_B)

#define	CHANNEL2_F	rt_pin_read(eChannel2_F)
#define	CHANNEL2_B	rt_pin_read(eChannel2_B)

#define	CHANNEL3_F	rt_pin_read(eChannel3_F)
#define	CHANNEL3_B	rt_pin_read(eChannel3_B)

#define	CHANNEL4_F	rt_pin_read(eChannel4_F)
#define	CHANNEL4_B	rt_pin_read(eChannel4_B)

/********************定时器**********************/
// Tout = ARR * PSC / 72000000 s
// Tout:定时器中断周期
// 72000000:为72MHz 
// 通过测试 ARR最小为14，即1.4ms 频率为 1000/1.4 接近1k。
// 如果ARR再小的话，则motor线程无法及时处理定时器释放的信号量
// ARR 越大 电机频率越低
#define ARR	30		//自动重装载寄存器周期的值
#define PSC	7200	//时钟频率除数的预分频值

//定义按键初始值
static rt_uint8_t key_f = 1;	//正转
static rt_uint8_t key_r = 1;	//反转
static rt_uint8_t key_s = 1;	//停止

rt_uint8_t step_f = 1;
rt_uint8_t step_r = 4;

rt_uint8_t tim3_flag = 0;

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
	//电机1
	rt_pin_mode(eM1_en, PIN_MODE_OUTPUT);
	rt_pin_mode(eA1_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eA1_N,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB1_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB1_N,  PIN_MODE_OUTPUT);
	//使能脚：高使能 初始化
	rt_pin_write(eM1_en,PIN_LOW);	
	rt_pin_write(eA1_P,PIN_LOW);
	rt_pin_write(eA1_N,PIN_LOW);	
	rt_pin_write(eB1_P,PIN_LOW);
	rt_pin_write(eB1_N,PIN_LOW);	
	//电机2
	rt_pin_mode(eM2_en, PIN_MODE_OUTPUT);
	rt_pin_mode(eA2_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eA2_N,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB2_P,  PIN_MODE_OUTPUT);
	rt_pin_mode(eB2_N,  PIN_MODE_OUTPUT);	
	//使能脚：高使能		初始化
	rt_pin_write(eM2_en,PIN_LOW);	
	rt_pin_write(eA2_P,PIN_LOW);
	rt_pin_write(eA2_N,PIN_LOW);	
	rt_pin_write(eB2_P,PIN_LOW);
	rt_pin_write(eB2_N,PIN_LOW);	
	
	//三个按键
	rt_pin_mode(eKEY_F,PIN_MODE_INPUT_PULLUP);
	rt_pin_mode(eKEY_R,PIN_MODE_INPUT_PULLUP);	
	rt_pin_mode(eKEY_S,PIN_MODE_INPUT_PULLUP);
	
	
	//四个通道限位	正常情况为低电平，到达该位置时为高，指示灯亮
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

//按键扫描
static void key_scan(void)
{
	//按键：低电平按下，高电平释放
	if(rt_pin_read(eKEY_F) == PIN_LOW)
	{
//		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//延时250ms
		key_f = 0;
		key_r = 1;
		key_s = 1;
	}
		
	else if(rt_pin_read(eKEY_R) == PIN_LOW)
	{
//		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//延时250ms
		key_f = 1;
		key_r = 0;
		key_s = 1;
	}
	
	else if(rt_pin_read(eKEY_S) == PIN_LOW)
	{
//		rt_thread_delay(RT_TICK_PER_SECOND / 4);	//延时250ms
		key_f = 1;
		key_r = 1;
		key_s = 0;
	}
}
//电机正转
static void motor_run_forward()
{
	rt_err_t result;	
	if(tim3_flag == 0)
	{
		TIM3_Start();
		tim3_flag = 1;
	}
	
	rt_pin_write(eM1_en,PIN_HIGH);
	
	//永久等待信号量，获取信号量，则信号量的数量减1
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
//电机反转
static void motor_run_reversal()
{
	rt_err_t result;
	if(tim3_flag == 0)
	{
		TIM3_Start();
		tim3_flag = 1;
	}
	rt_pin_write(eM1_en,PIN_HIGH);
	
	//永久等待信号量，获取信号量，则信号量的数量减1
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
//电机停止
static void motor_run_stop()
{
	
	TIM3_Stop();
	tim3_flag = 0;
	//使能脚：高使能 初始化
	rt_pin_write(eM1_en,PIN_LOW);	
	rt_pin_write(eA1_P,PIN_LOW);
	rt_pin_write(eA1_N,PIN_LOW);	
	rt_pin_write(eB1_P,PIN_LOW);
	rt_pin_write(eB1_N,PIN_LOW);	
	//使能脚：高使能		初始化
	rt_pin_write(eM2_en,PIN_LOW);	
	rt_pin_write(eA2_P,PIN_LOW);
	rt_pin_write(eA2_N,PIN_LOW);	
	rt_pin_write(eB2_P,PIN_LOW);
	rt_pin_write(eB2_N,PIN_LOW);
}
static void motor_thread_entry(void *parameter)
{
    rt_hw_motor_init();
	TIM3_Init(ARR,PSC - 1);	//初始化定时器并设置中断周期
	//启动之后电机自动往反运动
    while (1)
    {
		//用电脑仿真是，按键扫描得到的值一直为低电平。可能是仿真的原因，具体得拿硬件测试是否为代码bug。
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

    /* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(motor_ctr_init);
#endif

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(thread_init_motor, motor contrl);


