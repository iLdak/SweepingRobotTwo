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

//=====================私有定义==============================
  #ifdef  DEBUG   
  #define   DBGMCU_CR ((uint32 *)0xe0042004)
  #endif   
//=====================全局变量==============================
bool ExitInput;     //有按键、外部电源外部输入
//=====================私有变量==============================
//=====================全局函数==============================	  
void Init_Sleep(void); 
void Do_Sleep(void);
//=====================私有函数==============================	
void RCC_EXITSTOP(void);
void Enable_KeyIni(void);
void Disable_KeyIni(void);
void Enable_YaoKongIni(void);
void Disable_YaoKongIni(void);
//======================函数实体==============================
/******************************************************************
功能：初始化充电程序
******************************************************************/
void Init_Sleep(void)
{
		      /******初始化显示***********/
    clr_display();	
	/******初始化设置的值********************/
	mode.mode = SLEEP;
	clr_ram();
	
	Disable_earth();
	Disable_wall();
	disable_hwincept();//禁止红外接收电源
	Set_Speed_Send();  //禁止红外发送  
	WriteWorkState(); 
}
/******************************************************************
功能：执行充电
******************************************************************/
void Do_Sleep(void)
{
uint32 t;
uint8 s;
  //在调试时允许在停机状态下调试
  #ifdef  DEBUG   
  *((uint32 *)0xe0042004) |= 2;
  #endif   
   Set_LCD_Power();
   Set_Wall_Send();              //关闭发射
   Set_Earth_Send();             //关闭发射						  
   Reset_Beep();
   ADC_Cmd(ADC1, DISABLE);		 //关掉AD采样
   	  
      IWDG_ReloadCounter();
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_SetPrescaler(IWDG_Prescaler_256);//使用
      IWDG_SetReload(1000);
      IWDG_ReloadCounter();
   for(;;)
   {
       Enable_KeyIni();
       //================设置闹钟中断时间================
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
	   //读取实时时钟值
	   Rtc_time = RTC_GetCounter();
	   AccountCapabilityReal(); //计算耗电量
	   
	   //===============机器的预约时间到，机器进入扫地模式
	   if(Check_PreengageTime())
	   {
		    Init_Lcd();
		    Init_Sweep();
		    break;
	   }
	   //有遥控器电源按键按下
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
			   //在面板上有三个按键中的任意一个按下了
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
			   //有充电的电源插入
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
   Dis_On = TRUE;            //显示标志。
   Enable_Beep();
 }
/*************************************************************
功能：从停止模式退出时的时钟配置。
输入：无
输出：无
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
功能：允许三个按键中断
输入：无
输出：无
************************************************************/
void Enable_KeyIni(void)
{
	EXTI->IMR |= 0x180e;
}
/************************************************************
功能：禁止三个按键中断
输入：无
输出：无
************************************************************/
void Disable_KeyIni(void)
{
	EXTI->IMR &= 0xffffE7f1;
}
/************************************************************
功能：允许三个按键中断
输入：无
输出：无
************************************************************/
void Enable_YaoKongIni(void)
{
	EXTI->IMR |= 0x1;
}
/************************************************************
功能：禁止三个按键中断
输入：无
输出：无
************************************************************/
void Disable_YaoKongIni(void)
{
	EXTI->IMR &= 0xfffffffe;
}


