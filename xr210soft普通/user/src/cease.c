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
uint32 giv_cease_time ;
//======================私有变量==================================
uint8 piv_yaokongstate = 0;	   //在设置时的设置状态,0没有设置，1设置
                               //当前时间，2设置预约时间
uint8 piv_dis_hour;			   //显示的小时数
uint8 piv_dis_minute;		   //显示的分钟数
uint8 piv_dis_week;			   //显示的小时数
uint8 piv_dis_preengage;	   //显示的预约天数
uint8 piv_set_content;         //设置的内容	0为没有设置，1为设置天数、2为设置小时、3为设置分钟
                               //4为设置星期
//=======================全局函数====================================
void Do_Cease(void);           //执行在待机状态的程序
void Init_Cease(void);
void Cease_No_Key(void);
void Cease_YaoKong_Manage(void);
//=======================私有函数=====================================	
void Cease_display_set(void);
void Cease_SetTime(void);
void Cease_Timing(void);
void Cease_Lode_Time(void);
void Cease_Save_Time(void);
void Cease_Save_Preengage(void);
void Cease_Lode_Preengage(void);
void Cease_Big_Subb_Time(void);
void Cease_Subb_Time(void);
void Cease_Big_Add_Time(void);
void Cease_Add_Time(void);

//=========================函数实体======================================
/*****************************************************************
功能：在待机时的动作
*****************************************************************/
void Do_Cease(void)
{
    /**************如果有遥控器按键*/
	Cease_YaoKong_Manage() ;

	//有面板上的按键 按下
	if(key1.sign == 0)		//按下SPOT按键
	{
	   key1.sign = 1;
	   Enable_Beep();
	   Init_Spot();
	   return ;
	}			 
	if(key2.sign == 0)	    //按下AUTO按键
	{
	   key2.sign = 1; 
	   Enable_Beep();
	   Init_Sweep();
	   return;
	}
	if(key3.sign == 0)	    //按下UV按键
	{
	   key3.sign = 1;  
	   Enable_Beep();
	   //Turn_UV();
	   //Init_Cease();
	   Init_Border();
	}
    if((power.charge_dc == 1) || (power.charge_seat == 1))
	{
	   if(((giv_sys_err_bak != 25)&&(giv_sys_err_bak != 26)))
	   {
	       Init_Chargeing(); 
	       return;
	   }
	}
	/*********根据设置不同显示不同内容******/
    if(piv_yaokongstate == 0)
	{		
	    Display_Real();
	}
	else
	{
	    Cease_display_set();
	}
	Dis_Power = Get_Dispower();	  
	//=========机器的预约时间到，机器进入扫地模式
	if(Check_PreengageTime())
	{
	   Enable_Beep();
	   Init_Sweep();
	   return;
	}
    /**********如果机器在5分钟内没有操作,机器进入睡眠模式**********/
	if((giv_sys_time - giv_cease_time) > 3000000)
	{
	   Init_Sleep();
	   return;
	}
}
/******************************************************************
在待机状态的显示初始化
******************************************************************/
void Init_Cease(void)
{    
    /******初始化显示***********/
    clr_display();
    Dis_On = TRUE;              //显示标志。
    Dis_PrOn = TRUE; 			//预约天数亮
    Dis_HourOn = TRUE; 		    //小时亮
    Dis_MinuteOn = TRUE; 		//分钟亮
    Dis_ColOn = TRUE; 		    //时间点亮
    Dis_WeekOn = TRUE; 		    //星期亮
    Dis_SpeedOn = TRUE; 		//速度亮
    Dis_PowerOn = TRUE; 		//电池图标亮
    Dis_ChargeOn = FALSE ;		//充电图标亮
	Dis_ColGlint = TRUE ;	    //显示时间点为闪烁	
    Dis_SpeedGo = FALSE;        //轮子图标不转动
	   
    Display_Real();	            //显示当前的设置
	
	/******初始化设置的值********************/
	piv_set_content = 0;		//开始设置为没有设置
	piv_yaokongstate = 0;

	/*******初始化输出的控制***************/
	stop_rap();                 //关闭左右轮
	Init_Io_Driver();           //关闭清扫的器件
	Disable_earth();
	Disable_wall();	

	disable_hwincept();         //关闭红外接收电源
	Set_Speed_Send();           //关闭红外发送    
	
	/****设置机器的工作模式******/   
	mode.mode = CEASE; 
	giv_cease_time = giv_sys_time;   
	WriteWorkState();
}
/*****************************************************************
功能：显示设置时的状态
*****************************************************************/
void Cease_display_set(void)
{
	Dis_Preengage = piv_dis_preengage;
    Dis_Week = piv_dis_week;
	Dis_Minute = piv_dis_minute;	
	Dis_Hour = piv_dis_hour;
	Display_Work_Mode();
	Dis_PrGlint = FALSE;
	Dis_HourGlint = FALSE;
	Dis_MinuteGlint = FALSE;
	Dis_WeekGlint =FALSE;
	switch (piv_set_content	)
	{
		case 1:	 
	        Dis_PrGlint = TRUE;	 
		    break;
	    case 2:
	        Dis_HourGlint = TRUE; 	
		    break;
	    case 3:
	        Dis_MinuteGlint = TRUE;	
		    break;
	    case 4:						 
	        Dis_WeekGlint =TRUE; 
		    break;
	}
}
/*****************************************************************
功能：待机遥控器处理程序
******************************************************************/
void Cease_YaoKong_Manage(void)
{
    if(gsv_yaokong.sign == 1) //--------------有遥控器按键按下
	{
	    gsv_yaokong.sign = 0; 
	   Enable_Beep();
	    switch( piv_yaokongstate)
		{
		    //在以前没有按键按下时的情况
		    case 0:
			    Cease_No_Key();
				break;

		    //原来按过设定时间键
			case 1:
			    Cease_SetTime();
			    break;

			//原来按过设定预约键
			case 2:
			    Cease_Timing();
			    break;
			default :
			piv_yaokongstate = 0;
		}
	    giv_cease_time = giv_sys_time;     
	}
}
/*******************************************************
功能：在没有预先按下按键时的处理程序
*******************************************************/
void Cease_No_Key(void)
{
uint32 liv_time;
    switch(	gsv_yaokong.key)
	{
		    //------------按下电源键或者取消键
	    case YAOKONG_ESC:
			liv_time = giv_sys_time;
		    //============等待电源按键弹起===================
		    do
			{
			    A7105_rx();	
				Beep_Driver();	
				IWDG_ReloadCounter();			   
		        if((giv_sys_time - liv_time) > 30000)  //超过5秒
			    {
			        PreengageDate = 0;                 //清除原来的预约设置	  
				    PreengageTime = 0;
				    WritePreengage();	
					Enable_Beep();
					break;			                         
			    }
			}
			while(( gsv_yaokong.key == YAOKONG_ESC) &&(gsv_yaokong.effect != 0)) ;

		    //-------------------判断电源键是否按下时间超过3秒
			if((giv_sys_time - liv_time) < 30000)
			{
			    Init_Sleep();    //进入睡眠模式
			}
		    break;	
		
		    //-------------------按下预约按键进入预约设置	 
	    case YAOKONG_TIMINE_MON:
			Cease_Lode_Preengage();//将预约内容装入临时变量中
			piv_set_content = 1;
		    piv_yaokongstate = 2;
		    break;

		    //按下时间按键进入时间设定
	    case YAOKONG_SETTIME_TUE:	
		    Cease_Lode_Time();
			piv_set_content = 4;
		    piv_yaokongstate = 1;
		    break;

		    //按下uv键将uv指示取反
	    case YAOKONG_UV_WED:
		    Turn_UV();
			Init_Border();
		    //Turn_UV();
		    break;

		    //按下定点清扫进入定点清扫模式
	    case YAOKONG_AUTO_THU:
		    Init_Sweep();;
		    break;

		    //按下自动清扫键
	    case YAOKONG_SPOT_FRI:
		    Init_Spot();
		    break;

	    case YAOKONG_V_SAT:
		    Change_Speed();
		    break;

	    case YAOKONG_DOCKING_SUN:  
			Init_Docking();
		    break;
	    case YAOKONG_LEFT:
	    case YAOKONG_RIGHT:
	    case YAOKONG_FRONT:
	    case YAOKONG_BACK:
		    Init_Tele();
		    break;
	}
}
/*************************************************************
装载时间
*************************************************************/
void Cease_Lode_Time(void)
{	
    piv_dis_preengage = (uint8)PreengageDate;	//预约内容	
    piv_dis_week = (uint8)(((Rtc_time%604800) / 86400)) ;
    piv_dis_minute = (uint8)((Rtc_time % 3600) / 60);
    piv_dis_hour = (uint8)((Rtc_time % 86400) / 3600) ;

}
/*************************************************************
保存时间
*************************************************************/
void  Cease_Save_Time(void)
{
uint32 t;
    t = Rtc_time - (Rtc_time%604800);
    Rtc_time = t + (piv_dis_hour*3600) + (piv_dis_minute*60) + ((piv_dis_week)*86400);
	WriteRtcTime();
}
/*************************************************************
装载时间
*************************************************************/
void Cease_Lode_Preengage(void)
{	
    piv_dis_preengage = (uint8)PreengageDate;	//预约内容	
    piv_dis_week = (uint8)((Rtc_time%604800) / 86400 );
    piv_dis_minute = (uint8)(PreengageTime % 256);
    piv_dis_hour = (uint8)(PreengageTime / 256);

}
/*************************************************************
保存时间
*************************************************************/
void  Cease_Save_Preengage(void)
{
    PreengageDate = piv_dis_preengage ;
	PreengageTime = piv_dis_minute + (piv_dis_hour<<8);
	WritePreengage();
}

/*************************************************************
在设置定时预约操作
*************************************************************/
void Cease_Timing(void)
{	 
uint32 liv_time;	 //按键的开始按下时间
uint8 liv_number;	 
uint8 liv_key_down_time;  //按键的临时按下时间
    switch(	gsv_yaokong.key)
	{
		//////按下电源键或者取消键
	    case YAOKONG_ESC:
			 liv_time = giv_sys_time;
		    ////////等待电源按键弹起//////////////////
		    do
			{
			    A7105_rx();		//	
				Beep_Driver();	 
				IWDG_ReloadCounter(); 
		        if((giv_sys_time - liv_time) > 30000)  //超过5秒
			    {
			        PreengageDate = 0;        //清除原来的预约设置	
				    PreengageTime = 0;
				    WritePreengage();         //
					Enable_Beep();
			    }
			}
			while(( gsv_yaokong.key == YAOKONG_ESC) &&(gsv_yaokong.effect != 0)) ;

		    ////判断电源键是否按下时间超过5秒//////
			Init_Cease();
		    break;	
		
		///////按下预约按键进入预约设置	 
	    case YAOKONG_TIMINE_MON:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x02;   
			}
		    break;

		///////按下时间按键进入时间设定
	    case YAOKONG_SETTIME_TUE:	
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x04;    
			}
		    break;

		//////按下uv键将uv指示取反
	    case YAOKONG_UV_WED:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x08;  
			}
		    break;

		/////按下定点清扫进入定点清扫模式
	    case YAOKONG_AUTO_THU:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x10;   
			}
		    break;

		////按下自动清扫键///////////////
	    case YAOKONG_SPOT_FRI:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x20;   
			}
		    break;

	    case YAOKONG_V_SAT:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x40; 
			}
		    break;

	    case YAOKONG_DOCKING_SUN:  
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x1; 
			}
		    break;

	    case YAOKONG_ED:	   
		    if( piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x80;    
			}
		    break;

	    case YAOKONG_LEFT:		 
		    piv_set_content--; 
		    if(piv_set_content == 0)
			{
			   piv_set_content = 3;
			}
			break;
	    case YAOKONG_RIGHT:	  
		    piv_set_content++; 
		    if(piv_set_content == 4)
			{
			   piv_set_content = 1;
			}
			break;
	    case YAOKONG_FRONT:
		    liv_time = giv_sys_time; //保存开始按下按键的时间
			liv_number = 0;
		    do
			{
			    A7105_rx();		// 读取按键	  
				Beep_Driver();
				liv_key_down_time = (uint8)((giv_sys_time - liv_time)/10000) ; //10000
			    //按键时间大于3秒并且到加秒时间
				if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
				{
				   liv_number =  liv_key_down_time;
				   Cease_Big_Add_Time();
				}			 
				Cease_display_set();
				AutoDisplay();
				IWDG_ReloadCounter();
			}
			while(( gsv_yaokong.key == YAOKONG_FRONT) &&(gsv_yaokong.effect != 0)) ;	
		    Cease_Add_Time();
			break;
	    case YAOKONG_BACK:
		    liv_time = giv_sys_time; //保存开始按下按键的时间
			liv_number = 0;
		    do
			{
			    A7105_rx();		// 读取按键	  
				Beep_Driver();
				liv_key_down_time = (uint8)((giv_sys_time - liv_time)/10000) ;//10000
			    //按键时间大于3秒并且到加秒时间
				if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
				{
				   liv_number =  liv_key_down_time;
				   Cease_Big_Subb_Time();
				}			 
				Cease_display_set();
				AutoDisplay();
				IWDG_ReloadCounter();
			}
			while(( gsv_yaokong.key == YAOKONG_BACK) &&(gsv_yaokong.effect != 0)) ;
		    Cease_Subb_Time();
			break;

	    case YAOKONG_OK:
		    Cease_Save_Preengage();	 
			Init_Cease();
		    break;
	}

}

/*************************************************************
在设置时间操作
*************************************************************/
void Cease_SetTime(void)
{
uint32 liv_time;	 //按键的开始按下时间
uint8 liv_number;	 
uint8 liv_key_down_time;  //按键的临时按下时间
    switch(	gsv_yaokong.key)
	{
		//////按下电源键或者取消键
	    case YAOKONG_ESC:				   
			Init_Cease();
		    break;
		///////按下预约按键进入预约设置	 
	    case YAOKONG_TIMINE_MON:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 1;   
			}
		    break;

		///////按下时间按键进入时间设定
	    case YAOKONG_SETTIME_TUE:	
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 2;  
			}
		    break;

		//////按下uv键将uv指示取反
	    case YAOKONG_UV_WED:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 3;  
			}
		    break;

		/////按下定点清扫进入定点清扫模式
	    case YAOKONG_AUTO_THU:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 4;  
			}
		    break;

		////按下自动清扫键///////////////
	    case YAOKONG_SPOT_FRI:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 5; 
			}
		    break;

	    case YAOKONG_V_SAT:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 6;
			}
		    break;

	    case YAOKONG_DOCKING_SUN:  
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 0;
			}
		    break;
	
	    case YAOKONG_LEFT:	   
		    piv_set_content--;
			if(piv_set_content == 1)
			{
			    piv_set_content = 4;
			}
			break;
	    case YAOKONG_RIGHT:
		    piv_set_content++;
			if(piv_set_content == 5)
			{
			    piv_set_content = 2;
			}	 
			break;
	    case YAOKONG_FRONT:
		    liv_time = giv_sys_time; //保存开始按下按键的时间
			liv_number = 0;
		    do
			{
			    A7105_rx();		// 读取按键	
				Beep_Driver();
				liv_key_down_time = (uint8)((giv_sys_time - liv_time)/10000) ;
			    //按键时间大于3秒并且到加秒时间
				if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
				{
				   liv_number =  liv_key_down_time;
				   Cease_Big_Add_Time();
				}			 
				Cease_display_set();
				AutoDisplay();
				IWDG_ReloadCounter();
			}
			while(( gsv_yaokong.key == YAOKONG_FRONT) &&(gsv_yaokong.effect != 0)) ;	
		    Cease_Add_Time();
			break;
	    case YAOKONG_BACK:
		    liv_time = giv_sys_time; //保存开始按下按键的时间
			liv_number = 0;
		    do
			{
			    A7105_rx();		// 读取按键	 
				Beep_Driver();
				liv_key_down_time = (uint8)((giv_sys_time - liv_time)/10000) ;
			    //按键时间大于3秒并且到加秒时间
				if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
				{
				   liv_number =  liv_key_down_time;
				   Cease_Big_Subb_Time();
				}			 
				Cease_display_set();
				AutoDisplay();
				IWDG_ReloadCounter();
			}
			while(( gsv_yaokong.key == YAOKONG_BACK) &&(gsv_yaokong.effect != 0)) ;
		    Cease_Subb_Time();
			break;

	    case YAOKONG_OK:
		    Cease_Save_Time();	  	   
			Init_Cease();
		    break;
	}
}
/*******************************************************************************
连续增加时间
*******************************************************************************/
void Cease_Big_Add_Time(void)
{
    if(piv_set_content == 3)
    {
	    piv_dis_minute = (piv_dis_minute+3)%60;
	}
	else if(piv_set_content == 2)
	     {
	         piv_dis_hour = (piv_dis_hour + 3) % 24;
	     }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+3)%7;
			  }	  */
}

/*******************************************************************************
单独增加时间
*******************************************************************************/
void Cease_Add_Time(void)
{
    if(piv_set_content == 3)
	{
		piv_dis_minute = (piv_dis_minute+1)%60;
	}
	else if(piv_set_content == 2)
		 {
		     piv_dis_hour = (piv_dis_hour + 1) % 24;
		 }
		 /*else if(piv_set_content == 4)
			 {
				 piv_dis_week = (piv_dis_week+1)%7;
			 }	 */
}

/*******************************************************************************
连续减少时间
*******************************************************************************/
void Cease_Big_Subb_Time(void)
{
    if(piv_set_content == 3)
    {
	    piv_dis_minute = (piv_dis_minute+57)%60;
	}
	else if(piv_set_content == 2)
		 {
			 piv_dis_hour = (piv_dis_hour + 21) % 24;
		 }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+4)%7;
		      }*/
}

/*******************************************************************************
单一减少时间
*******************************************************************************/
void Cease_Subb_Time(void)
{
    if(piv_set_content == 3)
	{
	    piv_dis_minute = (piv_dis_minute+59)%60;
	}
	else if(piv_set_content == 2)
		 {
		     piv_dis_hour = (piv_dis_hour + 23) % 24;
		 }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+6)%7;
			  }	*/
}
