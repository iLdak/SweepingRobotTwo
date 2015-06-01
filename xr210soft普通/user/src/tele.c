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
#include "border.h"

//======================私有定义==================================
//======================全局变量==================================
//======================私有变量==================================
//======================全局函数==================================	  
void Init_Tele(void); 
void Do_Tele(void);
//======================私有函数==================================	
//======================函数实体==================================
/******************************************************************
功能：初始化遥控程序
******************************************************************/
void Init_Tele(void)
{
    /******初始化显示***********/
    clr_display();
    Dis_On = TRUE;               //显示标志。
    Dis_PrOn = TRUE; 			 //预约天数亮
    Dis_HourOn = TRUE; 		     //小时亮
    Dis_MinuteOn = TRUE; 		 //分钟亮
    Dis_ColOn = TRUE; 		     //时间点亮
    Dis_WeekOn = TRUE; 		     //星期亮
    Dis_SpeedOn = TRUE; 		//速度亮
    Dis_PowerOn = TRUE; 		//电池图标亮
    Dis_ChargeOn = FALSE ;		//充电图标亮
	Dis_ColGlint = TRUE ;	    //显示时间点为闪烁
	Dis_SpeedGo = TRUE; 
	
	/******初始化设置的值********************/
	mode.mode = TELE;
	clr_ram();

    Enable_Sweep();

	Enable_earth();
	Enable_wall();
	enable_hwincept();         //允许红外接收电源
	Reset_Speed_Send();        //允许红外发送 
	 
	Display_Real();
	Init_Action();
	WriteWorkState();
}
/*******************************************************************
遥控器遥控工作程序
*******************************************************************/
void Do_Tele(void)
{
  uint32 m;
  
  //电池电压低时进入自动回充
  if(battery_voltage < LOWPOWER)
  {
     if(battery_voltage < NOPOWER)
	 {
	     giv_sys_err = 18;
		 Init_Err();
	 }
	 else
	 {			 
         Init_Docking() ;
		 Enable_Beep_Five();
	 }
	 return;
  }

  //在最开始启动时判断左右轮是否离地，否则退出扫地模式
  if((giv_sys_time - mode.time) < 5000)
  {
      if((l_lidi.key == 0)||(r_lidi.key == 0))
	  {
	      Init_Cease();
	  }
	  return ;
  }
  if((giv_sys_time - mode.time) < 10000)
  {
      if(CheckMDustMotor())
	  {
	      return;
	  }
	  if(gsv_yaokong.key == YAOKONG_BACK)
	  {
		  mode.step = 3;
	  }
  }
  //在面板上有三个按键中的任意一个按下了
  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
  {
      key1.sign = 1;
	  key2.sign = 1;
	  key3.sign = 1;
	  Init_Cease();	 
	  Enable_Beep();
	  return;
  }

  //显示显示屏的内容
  Display_Real();

  //左右轮调速
  comm_rap();
				  
  clr_hw_effect(&l_hw);
  clr_hw_effect(&m_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw); 	
  clr_hw_effect(&dummy_wall); 
  
  Io_Driver();	 
  Dis_Power = Get_Dispower();	

  if(gsv_yaokong.effect == 1) //有遥控器按键按下
  {
	    gsv_yaokong.sign = 0;
		switch(	gsv_yaokong.key)
		{
		    case YAOKONG_ESC:
			    Init_Cease();	
				Enable_Beep();
			    return ;
			case YAOKONG_AUTO_THU:
			    Init_Sweep();
				Enable_Beep();	  
			    return ;
			case YAOKONG_SPOT_FRI:	   
			    Init_Spot();
				Enable_Beep();
			    return ;
			case YAOKONG_DOCKING_SUN:  
			    Init_Docking();
				Enable_Beep();
			    return ;
	        case YAOKONG_V_SAT:	
		        Change_Speed();
				Enable_Beep();
				break;
	        case YAOKONG_UV_WED:	
				Init_Border();
		    	//Turn_UV();
				//Enable_Sweep();
				Enable_Beep();
				return ;
	        case YAOKONG_LEFT:
			    if(mode.step != 1)
				{		   
				    stop_rap();
				    mode.step = 1;
				}
				break;
	        case YAOKONG_RIGHT:	   
			    if(mode.step != 2)
				{		   
				    stop_rap();
				    mode.step = 2;
				}
				break;
	        case YAOKONG_FRONT:		 
			    if(mode.step != 0)
				{		   
				    stop_rap();
				    mode.step = 0;
				}
				break;
	        case YAOKONG_BACK:	   
			    if(mode.step != 3)
				{		   
				    stop_rap();
				    mode.step = 3;
				}
				break;
		}
   }
   else
   {
       if((mode.step == 3))
	   {
	       Init_Cease();
		   return;
	   }
	   else
	   {
	      if(mode.step != 0)
		  {
		      stop_rap();
		      mode.step = 0;
		  }
	   }
   }
  //在遥控的过程中有障碍信号和特殊信号的处理
  m=read_bump();
  if(mode.bump)
  {
      if(do_action(4,angle240))
	  {
	     Init_Cease();	
		 return; 
	  }
	  if(m != 0)
	  {
	      return;
	  }
	  Init_Cease();
	  return;
  }
   //对各个按键的动作
   switch (mode.step)
   {
     case 0:
	    do_action(3,METE_TWO);
	   break;
	case 1:				   
	    do_action(1,angle360);
	   break;
	case 2:					
	    do_action(2,angle360);
	    break;
	case 3:			  
	    do_action(4,METE_TWO);
	    break;
    default:
	    mode.step = 0;	      	   	    
   } 

}
