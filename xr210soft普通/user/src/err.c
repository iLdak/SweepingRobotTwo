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

//======================私有定义=================================
//======================全局变量=================================
//======================私有变量=================================
//======================全局函数=================================	  
void Init_Err(void); 
void Do_Err(void);
//======================私有函数=================================	
//======================函数实体=================================
/******************************************************************
功能：初始化充电程序
******************************************************************/
void Init_Err(void)
{
uint16 m = 0;
    WriteBakErr(giv_sys_err);
	switch(giv_sys_err)
	{
	    case 1:
		case 2:
		case 3:
		case 4:
		    m = 1;
			break;
		case 16:   
		    m = 2;
			break;
		case 18:   
		    m = 3;
			break;
		case 31:   
		case 32:   
		    m = 4;
			break;	
		case 33:   
		    m = 5;
			break;
		default :
		    Init_Cease();
			return ;	  

	}
	giv_sys_err_bak = 	giv_sys_err;
	giv_sys_err = 0;
    /******初始化显示***********/
    clr_display();	
	/******初始化设置的值********************/
	clr_ram();
	
	Disable_earth();
	Disable_wall();
	disable_hwincept();       //禁止红外接收电源
	Set_Speed_Send();         //禁止红外发送  
	mode.mode = ERR;  
    Dis_On = TRUE;            //显示标志。
	
    Dis_HourOn = TRUE; 		  //小时亮
    Dis_MinuteOn = TRUE; 	  //分钟亮 
	Dis_ErrOn = TRUE;
	Dis_ErrGlint = TRUE;
	Dis_Hour = 140;
	Dis_Minute = m;	 			
	WriteWorkState();
}
/******************************************************************
功能：执行充电
******************************************************************/
void Do_Err(void)
{
 ///遥控器按键管理		     
  if(Sweep_YaoKong_Manage() == 1)
  {
	  giv_sys_err = 0;
      return;
  }
  ///在面板上有三个按键中的任意一个按下了
  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
  {
      key1.sign = 1;
	  key2.sign = 1;
	  key3.sign = 1;
	  Init_Cease();	 
	  Enable_Beep();
	  giv_sys_err = 0;
	  return;
  }
  if((giv_sys_time % 100000) < 1000)
  {
	  Enable_Beep();
  }
}
