/*
 * 程序清单：定时器
 * 程序描述：通过定时器来控制gpio的输出，从而控制步进电机
 */
#include "timer3.h"
#include "common.h"
#include "log.h"
#include "rtdevice.h"
#include "user_mb_app.h"

#define TIM3_DEG	0
#define TIM3_RES	0

rt_uint8_t step_f = 1;
rt_uint8_t step_r = 4;

#define MOTOR_SETP1 1
#define MOTOR_SETP2 2
#define MOTOR_SETP3 3
#define MOTOR_SETP4 4


#if 1
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
//#define M2_EN_H		rt_pin_write(eM2_en,1)		//电机1 使能高
//#define M2_EN_L		rt_pin_write(eM2_en,0)		//电机1 使能低
//#define	A2_P_H		rt_pin_write(eA2_P, 1)		//电机1 A+ 高
//#define	A2_P_L		rt_pin_write(eA2_P, 0)		//电机1 A+ 低
//#define	A2_N_H		rt_pin_write(eA2_N, 1)		//电机1 A- 高
//#define	A2_N_L		rt_pin_write(eA2_N, 0)		//电机1 A- 低
//#define	B2_P_H		rt_pin_write(eB2_P, 1)		//电机1 B+ 高
//#define	B2_P_L		rt_pin_write(eB2_P, 0)		//电机1 B+ 低
//#define	B2_N_H		rt_pin_write(eB2_N, 1)		//电机1 B- 高
//#define	B2_N_L		rt_pin_write(eB2_N, 0)		//电机1 B- 低

//两相，四拍：(+A)(+B)--(-A)(+B)--(-A)(-B)--(+A)(-B)-- 
//单相，四拍：(+A)--(+B)--(-A)--(-B)-- 


//两相四排
//电机1	
#define MOTOR1_STEP1	A1_P_H;	A1_N_L;  B1_P_H; B1_N_L
#define MOTOR1_STEP2	A1_P_L;	A1_N_H;  B1_P_H; B1_N_L
#define MOTOR1_STEP3	A1_P_L;	A1_N_H;  B1_P_L; B1_N_H
#define MOTOR1_STEP4	A1_P_H;	A1_N_L;  B1_P_L; B1_N_H
//电机2
//#define MOTOR2_STEP1	A2_P_H;	A2_N_L;  B2_P_H; B2_N_L
//#define MOTOR2_STEP2	A2_P_L;	A2_N_H;  B2_P_H; B2_N_L
//#define MOTOR2_STEP3	A2_P_L;	A2_N_H;  B2_P_L; B2_N_H
//#define MOTOR2_STEP4	A2_P_H;	A2_N_L;  B2_P_L; B2_N_H
//单相四排
//电机1	
#define MOTOR1_STEP1_S	A1_P_H;	A1_N_L 
#define MOTOR1_STEP2_S	B1_P_H; B1_N_L
#define MOTOR1_STEP3_S	A1_P_L;	A1_N_H
#define MOTOR1_STEP4_S	B1_P_L; B1_N_H
//电机2
//#define MOTOR2_STEP1_S	A2_P_H;	A2_N_L
//#define MOTOR2_STEP2_S	B2_P_H; B2_N_L 
//#define MOTOR2_STEP3_S	A2_P_L;	A2_N_H
//#define MOTOR2_STEP4_S	B2_P_L; B2_N_H

#endif


TIM_HandleTypeDef TIM3_Handler; 									//定时器句柄
//启动定时器
void TIM3_Start(void)
{
	HAL_TIM_Base_Start_IT(&TIM3_Handler); 							//使能定时器 3 和定时器 3 更新中断
}
static void motor_stop(void);
//停止定时器
void TIM3_Stop(void)
{
	//motor_stop before stop tim3.
	motor_stop();
	HAL_TIM_Base_Stop_IT(&TIM3_Handler);
}
//通用定时器 3 中断初始化
//arr：自动重装值。 psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器 3!(定时器 3 挂在 APB1 上，时钟为 HCLK/2)

void TIM3_Init(rt_uint16_t arr,rt_uint16_t psc)
{
	TIM3_Handler.Instance			=	TIM3; 						//通用定时器 3
	TIM3_Handler.Init.Prescaler		=	psc;						//分频系数
	TIM3_Handler.Init.CounterMode	=	TIM_COUNTERMODE_UP; 		//向上计数器
	TIM3_Handler.Init.Period		=	arr; 						//自动装载值
	TIM3_Handler.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV1; 	//时钟分频因子
	
	HAL_TIM_Base_Init(&TIM3_Handler); 								//初始化定时器 3
//	HAL_TIM_Base_Start_IT(&TIM3_Handler); 							//使能定时器 3 和定时器 3 更新中断
}

//定时器底层驱动，开启时钟，设置中断优先级
//此函数会被 HAL_TIM_Base_Init()函数调用		该函数不能用static定时
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	__HAL_RCC_TIM3_CLK_ENABLE(); 									//使能 TIM3 时钟
	HAL_NVIC_SetPriority(TIM3_IRQn,1,3); 							//设置中断优先级，抢占 1，子优先级 3
	HAL_NVIC_EnableIRQ(TIM3_IRQn); 									//开启 ITM3 中断
}

//定时器 3 中断服务函数
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);
}

//电机正转
static void motor_run_forward()
{
	rt_pin_write(eM1_en,PIN_HIGH);
	rt_pin_write(eM2_en,PIN_HIGH);
	rt_uint16_t DOUBLE;
	//如果usSRegHoldBuf[eDouble_ctr]的值为0，则按两相四排运动。否则为单相四排运功。
	DOUBLE = !usSRegHoldBuf[eDouble_ctr1];
	switch(step_f)
	{
		case MOTOR_SETP1:
			if(DOUBLE)
			{
				MOTOR1_STEP1;
			}
			else
			{
				MOTOR1_STEP1_S;
			}
			step_f++;
		break;
		
		case MOTOR_SETP2:
			if(DOUBLE)
			{
				MOTOR1_STEP2;
			}
			else
			{
				MOTOR1_STEP2_S;
			}
			step_f++;
		break;
		
		case MOTOR_SETP3:
			if(DOUBLE)
			{
				MOTOR1_STEP3;
			}
			else
			{
				MOTOR1_STEP3_S;
			}
			step_f++;
		break;
		
		case MOTOR_SETP4:
			if(DOUBLE)
			{
				MOTOR1_STEP4;
			}
			else
			{
				MOTOR1_STEP4_S;
			}
			if(step_f >= 4)
			step_f = 1;
		break;				
	}
}

//电机反转
static void motor_run_reversal()
{
	
	rt_pin_write(eM1_en,PIN_HIGH);
	rt_pin_write(eM2_en,PIN_HIGH);
	rt_uint16_t DOUBLE;
	//如果usSRegHoldBuf[eDouble_ctr]的值为0，则按两相四排运动。否则为单相四排运功。
	DOUBLE = ! usSRegHoldBuf[eDouble_ctr1];
	switch(step_r)
	{
		case MOTOR_SETP1:
			if(DOUBLE)
			{
				MOTOR1_STEP1;
			}
			else
			{
				MOTOR1_STEP1_S;
			}
			if(step_r <= 1)
			step_r = 4;
		break;
		
		case MOTOR_SETP2:
			if(DOUBLE)
			{
				MOTOR1_STEP2;
			}
			else
			{
				MOTOR1_STEP2_S;
			}
			step_r--;
		break;
		
		case MOTOR_SETP3:
			if(DOUBLE)
			{
				MOTOR1_STEP3;
			}
			else
			{
				MOTOR1_STEP3_S;
			}
			step_r--;
		break;
		
		case MOTOR_SETP4:
			if(DOUBLE)
			{
				MOTOR1_STEP4;
			}
			else
			{
				MOTOR1_STEP4_S;
			}
			step_r--;
		break;				
	}
}

//电机停止
static void motor_stop(void)
{
	//使能脚：高使能 初始化
	rt_pin_write(eM1_en,PIN_LOW);	
	rt_pin_write(eA1_P,PIN_LOW);
	rt_pin_write(eA1_N,PIN_LOW);	
	rt_pin_write(eB1_P,PIN_LOW);
	rt_pin_write(eB1_N,PIN_LOW);	
//	//使能脚：高使能		初始化
//	rt_pin_write(eM2_en,PIN_LOW);	
//	rt_pin_write(eA2_P,PIN_LOW);
//	rt_pin_write(eA2_N,PIN_LOW);	
//	rt_pin_write(eB2_P,PIN_LOW);
//	rt_pin_write(eB2_N,PIN_LOW);
}


//定时器 3 中断服务函数调用   该函数不能用static定时
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&TIM3_Handler))
	{
		if(key_f == 0)
		{
			motor_run_forward();
		}
		else if(key_r == 0)
		{
			motor_run_reversal();	
		}
		LOG(TIM3_DEG,("TIM3 running ......\r\n"));
	}
}

#include "finsh.h"
MSH_CMD_EXPORT(TIM3_Stop, stop tim3);

