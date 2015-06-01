#include "pwm.h"
#include "soft.h"
#include "lcdkey.h"
/**************************************************************
程序名称：void  Init_PWM(void)
描述：初始化pwm驱动程序，将定时器3、定时器4 配置为pwm1模式，使用
      边沿对齐模式向上计数
输入：无
输出：无
**************************************************************/
void  Init_PWM(void)
{
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC 
                     | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO , ENABLE);
   /////////////////////使能定时器3、4的时钟////////////
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
////////////////下面定义定时器3，驱动充电pwm、pwm频率36kHz///////////////	
	  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;   //PWM重装值 ，就是ARR参数      
  TIM_TimeBaseStructure.TIM_Prescaler = 0;   //时钟分频系数 ,就是PSC参数   
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;   //时钟分频因子，在CR1寄存器中
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数方式，在CR1寄存器中 
  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
   /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //通道的工作模式  
  TIM_OCInitStructure.TIM_Channel = TIM_Channel_1;         //通道数 
  TIM_OCInitStructure.TIM_Pulse = 0;                       //脉冲有效宽度
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //有效电平
    
  TIM_OCInit(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
  ///////////////////////下面驱动四个左右轮的控制信号//////////////
	  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;   //PWM重装值 ，就是ARR参数      
  TIM_TimeBaseStructure.TIM_Prescaler = 5;   //时钟分频系数 ,就是PSC参数   
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;   //时钟分频因子，在CR1寄存器中
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数方式，在CR1寄存器中 
  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
   /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //通道的工作模式  
  TIM_OCInitStructure.TIM_Channel = TIM_Channel_1;         //通道数 
  TIM_OCInitStructure.TIM_Pulse = 0;                       //脉冲有效宽度
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //有效电平
    
  TIM_OCInit(TIM4, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_Channel = TIM_Channel_2;         
  
  TIM_OCInit(TIM4, &TIM_OCInitStructure); 

  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_Channel = TIM_Channel_3;    
  
  TIM_OCInit(TIM4, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_Channel = TIM_Channel_4;   
  
  TIM_OCInit(TIM4, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(TIM4, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM4, ENABLE);
  
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
  
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
  
  
  GPIO_InitStructure.GPIO_Pin = L_FRONTONOFF | L_BACKONOFF | 
                                R_FRONTONOFF | R_BACKONOFF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  
  GPIO_SetBits(GPIOE, L_FRONTONOFF);
  GPIO_SetBits(GPIOE, R_FRONTONOFF);
  GPIO_SetBits(GPIOE, L_BACKONOFF);
  GPIO_SetBits(GPIOE, R_BACKONOFF);
  
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
}
/*********************************************
功能：使能PWM
输入：channel 通道名称
              ：L_FRONT     左轮向前
			  ：L_BACK      左轮向后
			  ：R_FRONT     右轮向前
			  ：R_BACK      右轮向后
			  ：CHARGE_PWM  充电pwm
	  pwm     占空比，其值在0——1000之间	
输出：无
	  
*********************************************/
void  enable_pwm(uint16 channel,uint16  pwm)
{
   if(pwm > 1000)
     pwm = 1000;
   switch (channel)
   {
      case   L_FRONT:
	     if(GPIO_ReadInputDataBit(GPIOE,  L_BACKONOFF) == Bit_RESET)
		 {
		     GPIO_SetBits(GPIOE, L_BACKONOFF);
			 delay(1000);
		 }
		 GPIO_ResetBits(GPIOE, L_FRONTONOFF);
	     TIM_SetCompare3(TIM4,pwm);
	     TIM_SetCompare4(TIM4,0);
	  break  ;
	  
      case   L_BACK:
	     if(GPIO_ReadInputDataBit(GPIOE,  L_FRONTONOFF) == Bit_RESET)
		 {
		     GPIO_SetBits(GPIOE,L_FRONTONOFF );
			 delay(1000);
		 }
		 GPIO_ResetBits(GPIOE, L_BACKONOFF);
	     TIM_SetCompare4(TIM4,pwm);
	     TIM_SetCompare3(TIM4,0);
	  break  ;
	  
      case   R_BACK:
	     if(GPIO_ReadInputDataBit(GPIOE,  R_BACKONOFF) == Bit_RESET)
		 {
		     GPIO_SetBits(GPIOE, R_BACKONOFF);
			 delay(1000);
		 }
		 GPIO_ResetBits(GPIOE, R_FRONTONOFF);
	     TIM_SetCompare2(TIM4,pwm);
	     TIM_SetCompare1(TIM4,0);
		 
	  break  ;
	  
      case   R_FRONT:
	     if(GPIO_ReadInputDataBit(GPIOE,  R_FRONTONOFF) == Bit_RESET)
		 {
		     GPIO_SetBits(GPIOE,R_FRONTONOFF );
			 delay(1000);
		 }
		 GPIO_ResetBits(GPIOE, R_BACKONOFF);
	     TIM_SetCompare1(TIM4,pwm);
	     TIM_SetCompare2(TIM4,0);
	  break  ;
	  
      case   CHARGE_PWM:
	     TIM_SetCompare1(TIM3,pwm);
	  break  ;
   }	 
}
/*********************************************
功能：关闭PWM
输入：channel 通道名称
              ：L_FRONT     左轮向前
			  ：L_BACK      左轮向后
			  ：R_FRONT     右轮向前
			  ：R_BACK      右轮向后
输出：无
	  
*********************************************/
void  disable_pwm(uint16 channel)
{
   switch (channel)
   {
      case   L_FRONT:
	     TIM_SetCompare3(TIM4,0);
	  break  ;
	  
      case   L_BACK:
	     TIM_SetCompare4(TIM4,0);
	  break  ;
	  
      case   R_BACK:
	     TIM_SetCompare2(TIM4,0);
	  break  ;
	  
      case   R_FRONT:
	     TIM_SetCompare1(TIM4,0);
	  break  ;
	  
      case   CHARGE_PWM:
	     TIM_SetCompare1(TIM3,0);
	  break  ;
   }	 
}
/*********************************************
功能：使能PWM
输入：channel 通道名称
              ：L_FRONT     左轮向前
			  ：L_BACK      左轮向后
			  ：R_FRONT     右轮向前
			  ：R_BACK      右轮向后
			  ：CHARGE_PWM  充电pwm
	  pwm     占空比，其值在0——1000之间	
输出：无
	  
**********************************************/
void  Set_Pwm(uint16 channel,uint16  pwm)
{

   switch (channel)
   {
      case   L_FRONT:
	     TIM_SetCompare3(TIM4,pwm);
	  break  ;
	  
      case   L_BACK:
	     TIM_SetCompare4(TIM4,pwm);
	  break  ;
	  
      case   R_BACK:
	     TIM_SetCompare2(TIM4,pwm);
	  break  ;
	  
      case   R_FRONT:
	     TIM_SetCompare1(TIM4,pwm);
	  break  ;
	  
      case   CHARGE_PWM:
	     TIM_SetCompare1(TIM3,0);
	  break  ;
   }	 
}

