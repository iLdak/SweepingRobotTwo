#include   "time1.h"
volatile  uint32  giv_sys_time;      //����Ϊ����ϵͳ��ϵͳʱ��
volatile bool Sec;				     //���־
volatile bool Sec_Two;		         //����ı�־
bool gbv_minute;                     //���ӱ�־�����ڶ�ʱ������صĵ�ѹ��

vu16 CCR1_Val = 1200;                //��ʱ���ļ������塣
/*************************************************
��������void init_time(void)
���ܣ���ʼ����ʱ��2��ʹ����Ϊϵͳʱ�ӡ�
���룺�ޣ�
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
���ܣ���ʱ����
���룺������
�������
****************************************************/
void DelayMs(uint32 Ms)
{
uint32 stime;  //��ʼʱ��
    stime = giv_sys_time;
	while (((giv_sys_time - stime) < Ms*10))
	{
	    IWDG_ReloadCounter();
	}
}
