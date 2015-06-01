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


uint8 do_action_border()
{	
	   uint32 m;  
	   //uint16 s;
	   	 	/*
		if(mode.action != dm)
		{
			stop_rap();
			mode.action = dm  ;
		}	 */

		if(action.sign==0)//在当前运行模式下，继续运动；对象是轮子
		{
			action.sign = 1;
			action.step=0;
			
		}

  m = read_bump();
  switch(mode.step)
  {
  	  case 0:
		
      case 1:
	      if(do_action(3,METE_THREE))//METE_H5
		  {
		      mode.step = 0;
		  } 
		  if( m != 0)
		  {
		      stop_rap();
			  mode.angle_wall = angle720; //angle720
			  mode.step = 2;
			  mode.step_bp=0;
		  }	
		  Enable_wall();
		  return 0; 
	  case 2:
	       if(do_action(1,angle5))
		  {
		      mode.step = 0;
		  }	
		  if( m != 0)
		  {
		      stop_rap();
			  mode.step = 2;	 
			  mode.angle_wall = angle720;//angle720
			  mode.step_bp=0;
		  }	
		  Enable_wall();
		  return 0;
	  case 3:
	      if(ybs() == 0)
		  {
		      mode.step = 4;
		  }	 
		  return 0;
	  case 4:
	      if( FindSeat() == 0)
		  {
		      mode.step = 0;
		  }		
	 	 Enable_wall(); // Disable_wall();
	      return 0;
	  case 5:
	      if(do_action(1,angle120))//angle90
		  {
		      mode.step = 0;
		  }
		  Enable_wall();
		   return 0;
	  default :
	      mode.step = 0;
		  	  	       
  }

	return 0;
}



int Init_Border_child()
{		
	if(ybs() == 1)//此处就是执行沿边扫
		return 1;

	Enable_wall();
	return 0;
}


/******************************************************************/
void Init_Border(void)
{					 
    /******初始化显示***********/
    clr_display();
    Dis_On = TRUE;            //显示标志。
    Dis_PrOn = TRUE; 			//预约天数亮
    Dis_HourOn = TRUE; 		//小时亮
    Dis_MinuteOn = TRUE; 		//分钟亮
    Dis_ColOn = TRUE; 		//时间点亮
    Dis_WeekOn = TRUE; 		//星期亮
    Dis_SpeedOn = TRUE; 		//速度亮
    Dis_PowerOn = TRUE; 		//电池图标亮
    Dis_ChargeOn = FALSE ;		//充电图标亮
	Dis_ColGlint = TRUE ;	    //显示时间点为闪烁
	Dis_AuGlint = TRUE;
	
	Dis_SpeedGo = TRUE; 
	/******初始化设置的值********************/
	gsv_work_mode.work_mode = BORDER;	 //工作模式为清扫
	mode.mode = BORDER;
	clr_ram();
    Enable_Sweep();
	Enable_earth();
	Enable_wall();
	enable_hwincept();//允许红外接收电源
	Reset_Speed_Send();  //允许红外发送  
	Display_Real();
	Init_Action();
	mode.step = 0xfe;
	WriteWorkState();
}

/*****************************************************************
功能：沿墙清扫程序
*****************************************************************/
void Do_Border(void)
{
//static uint8 y = 0;//有碰撞信号时 y = 1;否则为0
//static uint8 z = 0;//在执行来回扫时 z = 1,否则为0

   //NVIC_GenerateSystemReset();
  //-----------电池电压低时进入自动回充
  if(battery_voltage < LOWPOWER)/*************************************************/
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
  
  //-----------在最开始启动时判断左右轮是否离地，否则退出扫地模式
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
  }
  //----------遥控器按键管理		     
  if(Sweep_YaoKong_Manage() == 1)
  {
      return;
  }

  //----------在面板上有三个按键中的任意一个按下了
  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
  {
      key1.sign = 1;
	  key2.sign = 1;
	  key3.sign = 1;
	  Init_Cease();	 
	  Enable_Beep();
	  return;
  }
  //-----------显示显示屏的内容
  Display_Real();
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&m_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw); 	
  clr_hw_effect(&dummy_wall); 
  //----------下面是一些需要驱动的行动-----------------
  Io_Driver();	 
  Dis_Power = Get_Dispower();	

	if((l_rap.rap < Speed)&&(r_rap.rap < Speed)&&(mode.bump == 0))
	{
	    l_rap.rap = (l_rap.rap*3)/2;
	    r_rap.rap = (r_rap.rap*3)/2;
	}

     //-----------在回充座附近将墙检的灵敏度设置为高
  if((up_hw.sign != 0) && ((up_hw.data&0x8f)==0))
  {
      Wall_SensitivityHigh();
	  mode.angle_wall = 0;
  }
  else
  {
      Wall_SensitivityLow();
  }

  //-------------有遥控器时机器停止按照遥控方向运动
  if(YaoKongFangXiang())
  {
      return;
  }
  //-------------是否有需要保护的内容
  if(Action_Protect())
  {
     return;
  }

	if(mode.fangxiang>2)mode.fangxiang = 0;
  if((mode.fangxiang != 0)&&(mode.fangxiang != 1)&&(mode.fangxiang != 2))
  {
      mode.fangxiang = 0;
  }


	if(Init_Border_child())
	{
      //y=1;
      return;
	} 
		mode.ybs_sign = 1;
	  mode.angle_wall=angle720;	  
	  mode.long_wall = 0;
  if((18 == zhulujing[mode.step]) || (19 == zhulujing[mode.step])||(mode.sum_bump > 10))
  {	//mode.step=1,5,9,11,17,21
	  mode.ybs_sign = 1;
	  mode.angle_wall=angle720;	  
	  mode.long_wall = 0;
	 
  }	 
   
	if(do_action_border())
	{
	/*
		mode.step++;
		if(mode.step > 4)
		{
		mode.step=0;
		}*/
	}
	
}


