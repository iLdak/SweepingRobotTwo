#include   "time1.h"
volatile  uint32  giv_sys_time;      //定义为整个系统的系统时钟
volatile bool Sec;				     //秒标志
volatile bool Sec_Two;		         //两秒的标志
bool gbv_minute;                     //分钟标志，用于定时采样电池的电压。

vu16 CCR1_Val = 1200;                //定时器的计数脉冲。
/*************************************************
函数名：void init_time(void)
功能：初始化定时器2，使其作为系统时钟。
输入：无，
*************************************************/
void init_time(void)
{
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

 NVIC_InitTypeDef NVIC_InitStructure;
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = CCR1_Val;  //1200        
  TIM_TimeBaseStructure.TIM_Prescaler = 2;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_Cmd(TIM2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_Update , ENABLE);
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/****************************************************
功能：延时毫秒
输入：毫秒数
输出：无
****************************************************/
void DelayMs(uint32 Ms)
{
uint32 stime;  //开始时间
    stime = giv_sys_time;
	while (((giv_sys_time - stime) < Ms*10))
	{
	    IWDG_ReloadCounter();
	}
}
