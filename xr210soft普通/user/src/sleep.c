#include "function.h"
#include "hardware.h"
#include "io_driver.h"
#include "key.h"
#include "lcdkey.h"
#include "power.h"
#include "ring.h"
#include "test.h"
#include "A7105.h"
#include "wall_earth.h"
#include "hwincept.h"
#include "ad.h"
#include "pwm.h"
#include "time1.h"
#include "rtc.h"
#include "flash.h"
#include "cease.h" 
#include "sweep.h" 
#include "docking.h"
#include "spot.h"
#include "tele.h"
#include "chargeing.h"
#include "sleep.h"
#include "err.h"

//=====================˽�ж���==============================
  #ifdef  DEBUG   
  #define   DBGMCU_CR ((uint32 *)0xe0042004)
  #endif   
//=====================ȫ�ֱ���==============================
bool ExitInput;     //�а������ⲿ��Դ�ⲿ����
//=====================˽�б���==============================
//=====================ȫ�ֺ���==============================	  
void Init_Sleep(void); 
void Do_Sleep(void);
//=====================˽�к���==============================	
void RCC_EXITSTOP(void);
void Enable_KeyIni(void);
void Disable_KeyIni(void);
void Enable_YaoKongIni(void);
void Disable_YaoKongIni(void);
//======================����ʵ��==============================
/******************************************************************
���ܣ���ʼ��������
******************************************************************/
void Init_Sleep(void)
{
		      /******��ʼ����ʾ***********/
    clr_display();	
	/******��ʼ�����õ�ֵ********************/
	mode.mode = SLEEP;
	clr_ram();
	
	Disable_earth();
	Disable_wall();
	disable_hwincept();//��ֹ������յ�Դ
	Set_Speed_Send();  //��ֹ���ⷢ��  
	WriteWorkState(); 
}
/******************************************************************
���ܣ�ִ�г��
******************************************************************/
void Do_Sleep(void)
{
uint32 t;
uint8 s;
  //�ڵ���ʱ������ͣ��״̬�µ���
  #ifdef  DEBUG   
  *((uint32 *)0xe0042004) |= 2;
  #endif   
   Set_LCD_Power();
   Set_Wall_Send();              //�رշ���
   Set_Earth_Send();             //�رշ���						  
   Reset_Beep();
   ADC_Cmd(ADC1, DISABLE);		 //�ص�AD����
   	  
      IWDG_ReloadCounter();
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_SetPrescaler(IWDG_Prescaler_256);//ʹ��
      IWDG_SetReload(1000);
      IWDG_ReloadCounter();
   for(;;)
   {
       Enable_KeyIni();
       //================���������ж�ʱ��================
       t = (RTC_GetCounter() + 2);
	   PWR_BackupAccessCmd(ENABLE);
       RTC_WaitForLastTask();
	   RTC_SetAlarm(t);
       RTC_WaitForLastTask();
	   PWR_BackupAccessCmd(DISABLE);
	   
       PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); 
	   RCC_EXITSTOP();
	   IWDG_ReloadCounter();
       Disable_KeyIni();
	   //��ȡʵʱʱ��ֵ
	   Rtc_time = RTC_GetCounter();
	   AccountCapabilityReal(); //����ĵ���
	   
	   //===============������ԤԼʱ�䵽����������ɨ��ģʽ
	   if(Check_PreengageTime())
	   {
		    Init_Lcd();
		    Init_Sweep();
		    break;
	   }
	   //��ң������Դ��������
	   if(gsv_yaokong.sign == 1)
	   {
		    gsv_yaokong.sign = 0;
			if(gsv_yaokong.key == YAOKONG_ESC)
			{
				Init_Lcd();
				Init_Cease();
			    break;
			}
	   }
	   gbv_a7105_reset = TRUE;
	   EXTI_GenerateSWInterrupt(EXTI_Line0);
	   if(ExitInput != FALSE)
	   {
	       ExitInput = FALSE;
		   t = giv_sys_time;
		   s = 0;
		   while((giv_sys_time - t) < 5000)
		   {
		       AutoReadKey();
			   judge_charge();
			   //������������������е�����һ��������
			   if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
			   {
			       s = 1;
				   key1.sign = 1;
				   key2.sign = 1;
				   key3.sign = 1;
				   Init_Lcd();
				   Init_Cease();
				   break;
			   }
			   //�г��ĵ�Դ����
			   if((power.charge_dc == 1) || (power.charge_seat == 1))
			   {
			       Init_Lcd(); 
				   Init_Chargeing();
				   s = 1;
				   break;
			   }
		   }
		   if(s == 1)
		   {
		       break;
		   }
	   }
   }   
   Dis_On = TRUE;            //��ʾ��־��
   Enable_Beep();
 }
/*************************************************************
���ܣ���ֹͣģʽ�˳�ʱ��ʱ�����á�
���룺��
�������
*************************************************************/
void RCC_EXITSTOP(void)
{
	 RCC_PLLCmd(ENABLE);
	 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
	/* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
}
/************************************************************
���ܣ��������������ж�
���룺��
�������
************************************************************/
void Enable_KeyIni(void)
{
	EXTI->IMR |= 0x180e;
}
/************************************************************
���ܣ���ֹ���������ж�
���룺��
�������
************************************************************/
void Disable_KeyIni(void)
{
	EXTI->IMR &= 0xffffE7f1;
}
/************************************************************
���ܣ��������������ж�
���룺��
�������
************************************************************/
void Enable_YaoKongIni(void)
{
	EXTI->IMR |= 0x1;
}
/************************************************************
���ܣ���ֹ���������ж�
���룺��
�������
************************************************************/
void Disable_YaoKongIni(void)
{
	EXTI->IMR &= 0xfffffffe;
}


