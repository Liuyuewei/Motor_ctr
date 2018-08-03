/*
 * 程序清单：定时器
 * 程序描述：通过定时器来控制gpio的输出，从而控制步进电机
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
TIM_HandleTypeDef TIM3_Handler; 									//定时器句柄
//启动定时器
void TIM3_Start(void)
{
	HAL_TIM_Base_Start_IT(&TIM3_Handler); 							//使能定时器 3 和定时器 3 更新中断
}
//停止定时器
void TIM3_Stop(void)
{
	HAL_TIM_Base_Stop_IT(&TIM3_Handler);
}
//通用定时器 3 中断初始化
//arr：自动重装值。 psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器 3!(定时器 3 挂在 APB1 上，时钟为 HCLK/2)

void TIM3_Init(rt_uint16_t arr,rt_uint16_t psc)
{
	timer_semaphore_init();											//初始化信号量
	TIM3_Handler.Instance			=	TIM3; 						//通用定时器 3
	TIM3_Handler.Init.Prescaler		=	psc;						//分频系数
	TIM3_Handler.Init.CounterMode	=	TIM_COUNTERMODE_UP; 		//向上计数器
	TIM3_Handler.Init.Period		=	arr; 						//自动装载值
	TIM3_Handler.Init.ClockDivision	=	TIM_CLOCKDIVISION_DIV1; 	//时钟分频因子
	
	HAL_TIM_Base_Init(&TIM3_Handler); 								//初始化定时器 3
//	HAL_TIM_Base_Start_IT(&TIM3_Handler); 							//使能定时器 3 和定时器 3 更新中断
}

//定时器底层驱动，开启时钟，设置中断优先级
//此函数会被 HAL_TIM_Base_Init()函数调用
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



//定时器 3 中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&TIM3_Handler))
	{
		rt_enter_critical();
		//定时器到时则释放一次信号量
		rt_sem_release(&timer_sem);
		rt_exit_critical();
		LOG(TIM3_DEG,("sem number = %d\r\n",timer_sem.value));
	}
}
#include "finsh.h"
MSH_CMD_EXPORT(TIM3_Stop, stop tim3);
