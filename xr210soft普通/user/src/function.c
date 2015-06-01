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

//=========================私有定义=========================
const uint16 M_PROTECTCURRENT =   466;//292;//中扫保护电流 500MA,计算公式  1A*0.47R*4096/3.3  
const uint16 M_STOPCURRENT =   788;//中扫停止电流 1350MA,计算公式  1A*0.47R*4096/3.3
const uint16 DUST_PROTECTCURRENT =   292; //灰尘风机电流 500MA						   
const uint16 WHEEL_PROTECTCURRENT =  292;//234; //左右轮电流 400MA

const uint16 BATTERY_PREOTECTTEMP = 815;  //电池保护温度  60度
//==========================全局变量=========================

MODE mode;
WORK_MODE gsv_work_mode;	

uint8 read_sideearth(void);	
uint8 giv_sys_err;         //系统的错误代码	  
uint8 giv_sys_err_bak;         //系统的错误代码
//动作   0:停止  1:原地左转  2原地右转 3前进   4后退   5旋转1  6：旋转2  。。。18旋转14	，19走螺旋线
/***********************************************************
在正常工作时的主路径：
前行1米，扫6圈，前行1米，左转90度，前行1.2米，右转90度，
前行1米，扫6圈，前行1米，左转90度，前行1.2米，右转90度，
前行1米，扫6圈，左转90度，前行1.2米，前行2米， 左转90度，
扫6圈，前行1米，左转90度，前行1.2米，右转90度，前行1米，
扫6圈，前行1米，左转90度，前行1.2米，右转90度，前行1米，
扫6圈，右转135度 结束，从头再开始
************************************************************/
uint8 zhulujing[]={
              3,         19,        3,            2,             3,            18,
			  
              3,         1,         3,            19,            21,           19,
			  
			  3,         1,         3,            2,             3,            19,
			  
			  3,         1,         3,            18,            21,           0}; //主路径动作
uint32 zhulujinglong[]={
              METE,      15,        METE,         angle60,       METE*4,      15,
			  
              METE_H7,   angle30 ,  METE_5,       15,            2,            15,
			  
              METE,      angle90,   METE_H7,      angle30,       METE_H5,      15,
			  
              METE,      angle45,   METE*4,      15,            2};//主路径的距离
/******************************************************
机器碰撞后的旋转的角度与碰撞次数的关系
*******************************************************/
uint32 angle[52]={angle15,angle15,
			    angle45,  angle15,  angle105, angle15, angle15,
			    angle105, angle75,  angle75,  angle30, angle45,
				angle30,  angle15,  angle15,  angle45, angle15,
				angle15,  angle15,  angle105, angle15, angle15,
				angle105, angle15,	angle30,  angle15, angle60,
				angle15,  angle15,	angle105, angle15, angle15,
				angle45,  angle60,	angle45,  angle15, angle90,
				angle45,  angle75,  angle75,  angle60, angle45,
				angle45,  angle105,	angle45,  angle30, angle30,
				angle15,  angle75,	angle15,  angle60, angle135};//碰撞后机器旋转的角度
/*uint32 qj[52] = { METE,METE,
                METE_TWO, METE_1,   angle360, METE_5,  METE_5,
				METE_0,   angle240, METE_2,   METE_3,  METE_8,
				METE_0,   angle240,	angle240, angle360,METE,
				METE,     METE_8, 	angle300, METE_0,  METE_7,
				angle330, METE_1,   METE_6,   angle270,METE_2,
				METE_5,   angle270, METE_3,   METE_4,  angle360,
				METE,     METE,     METE,     METE_TWO,angle360,
				angle360, angle360, METE_2,   METE_2,  METE_2,
				METE_TWO, METE_TWO, METE_3,	  METE_3,  METE_3,
				angle300, angle300, angle300, METE_1,  METE_1};*/

///////////////////////私有变量////////////////////////////////////	
//uint8 piv_display;         //系统显示电源，范围0--3； 
///////////////////////私有函数////////////////////////////////////
///////////////////////全局函数////////////////////////////////////
/*****************************************************************
功能：初始化路径
*****************************************************************/
void Init_Mode(void)
{
    mode.mode = CEASE;//开始系统处于静止待机状态
	gsv_work_mode.work_mode = SWEEP;
	gsv_work_mode.uv = ReadBKPUV();				//从备份域中读UV状态
	gsv_work_mode.speed =  ReadBKPSpeed();		//从备份域中读速度
	if(gsv_work_mode.speed == 1)
	{
		Speed = HIGHSPEED;
	}
	else
	{
		Speed = LOWSPEED;
	}
	if((RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)||
	  (RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET))
    {
	    RCC_ClearFlag();
		mode.mode =  ReadBKPMode();
		Dis_On = TRUE;            //显示标志。
		switch(mode.mode)
		{
			case SWEEP:
			    Init_Sweep();
				break;
			case SPOT:
			    Init_Spot();
				break;
			case TELE:
			    Init_Tele();
				break;
			case DOCKING:
			    Init_Docking();
				break;
			case CHARGEING:
			    Init_Chargeing();
				break;
			case SLEEP:
			    Init_Sleep();
				break;
			case BORDER:
				Init_Border();
				break;
			default :
			    Init_Cease();
		}

    }
	else
	{
	    Do_Test();
        Init_Cease();
	}




}
/*****************************************************************
功能: 运行工作模式
*****************************************************************/
void Action_Mode(void)
{ 
  switch (mode.mode)
  {    
      /*********************开机待机状态*******************/
      case CEASE: 
	      Do_Cease();   
	      break;
	  
	  /*********************正常清扫状态*******************/
	  case SWEEP:
	      Do_Sweep();
	      break;
	  
	  /*********************局部清扫状态*******************/
	  case SPOT:
	      Do_Spot();    
	      break;
	  
	  /*********************遥控器控制状态*****************/
	  case TELE:
	      Do_Tele();
	      break;
	  
	  /*********************找回充座状态*******************/
	  case DOCKING:
	      Do_Docking();
	      break;
	  
	  /*********************找回充座状态*******************/
	  case CHARGEING:
	      Do_Chargeing();
	      break;
	  
	  /*********************找回充座状态*******************/
	  case SLEEP:
	      Do_Sleep();
	      break;
	  
	  /*********************找回充座状态*******************/
	  case ERR:
	      Do_Err();
	      break;
	  /*********************沿边扫***********************/
	  case BORDER:
		  Do_Border();
		  break;
	  
	  /*********************默认状态***********************/
	  default :
	      Init_Cease();	  
  }
}
/**************************************************************************
功能：取反uv灯
**************************************************************************/
void Turn_UV(void)
{
    if(gsv_work_mode.uv == FALSE)
	{
	    gsv_work_mode.uv = TRUE;  
	}
	else
	{
	    gsv_work_mode.uv = FALSE;
	}
	WriteWorkState();
}
 
/**************************************************** 
功能：改变机器行走速度
输入：无
输出：Wheel_State = 1;
返回：无
****************************************************/
void Change_Speed(void)
{
    if( gsv_work_mode.speed == 1)
	{
	    gsv_work_mode.speed = 2;
		Speed = LOWSPEED;//750;
		l_rap.rap /= (HIGHSPEED/LOWSPEED);//2;	   
		r_rap.rap /= (HIGHSPEED/LOWSPEED);//2;
	}
	else
	{
	    gsv_work_mode.speed = 1; 
		Speed = HIGHSPEED;//1500;
		l_rap.rap *= (HIGHSPEED/LOWSPEED);//2;	   
		r_rap.rap *= (HIGHSPEED/LOWSPEED);//2;
	}
	WriteWorkState();
}
/*****************************************************************
显示实时状态
*****************************************************************/
void Display_Real(void)
{
uint32 t;
    t = Rtc_time % 604800;		
    Dis_Preengage = (uint8)PreengageDate;	//预约内容	
    Dis_Week = (uint8)(t / 86400);
    Dis_Minute = (uint8)((t % 3600) / 60);
    Dis_Hour = (uint8)((t % 86400) / 3600) ;
	Display_Work_Mode();
}
/*****************************************************************
显示工作的状态
*****************************************************************/
void Display_Work_Mode(void)
{					
	Dis_Speed = gsv_work_mode.speed;
	Dis_UvOn = 	gsv_work_mode.uv;
	if(mode.mode == SPOT)
	{
	    Dis_SpOn = TRUE;
		Dis_AuOn = FALSE;
	}
	else if(mode.mode == SWEEP)
	{	
	    Dis_SpOn = FALSE;
		Dis_AuOn = TRUE;
	}
	else
	{		   
	    Dis_SpOn = FALSE;
		Dis_AuOn = FALSE;
	}
}
/*****************************************************************
使能机器清扫工作
*****************************************************************/
void  Enable_Sweep(void)
{
    gbv_action_uv = gsv_work_mode.uv;
    gbv_action_s_motor = TRUE;
    gbv_action_m_motor = TRUE;
    gbv_action_dust_motor = TRUE;
}
/*****************************************************************
使能机器清扫工作
*****************************************************************/
void  Disable_Sweep(void)
{
    gbv_action_uv = FALSE;
    gbv_action_s_motor = FALSE;
    gbv_action_m_motor = FALSE;
    gbv_action_dust_motor = FALSE;
}

/******************************************
读取碰撞信息
*******************************************/
uint32  read_bump(void)
{
		if((e_r.sign==FARN))
		{
			if((mode.bump > 4) || (mode.bump == 0))	   
			{	
				  mode.bump=4;//E_R;
				  mode.step_bp=0;
				  stop_rap();
			}
			return 4;//E_R;
		}
		if((e_l.sign == FARN))
		{
			if((mode.bump > 4) || (mode.bump == 0))	    //左地检悬空
			{ 
				  mode.bump = 1;// E_L;
				  mode.step_bp=0;
				  stop_rap();
			}
			return 1;//E_L;
		}   
		if((e_lm.sign==FARN))
		{
			if((mode.bump > 4) || (mode.bump == 0))	    
			{
				  mode.bump= 2;//E_LM;
				  mode.step_bp=0;
				  stop_rap();
			}
			return 2;//E_LM;
		}
		if((e_rm.sign==FARN))
		{
			if((mode.bump > 4) || (mode.bump == 0))	    
			{ 
				  mode.bump=3;//E_RM;
				  mode.step_bp=0;
				  stop_rap();
			}
			return 3;//E_RM;
			}	 				
		if (1 == l_bump.key)
		{
			if((mode.bump == 0))	 //左前撞
			{
				  mode.bump=5;//L_BUMP;
				  mode.step_bp=0;
				  stop_rap();
			}
		    return 5;//L_BUMP;
		}
		if (1 == r_bump.key)
		{
			if((mode.bump == 0))	 //右前撞
			{
				  mode.bump=6;//R_BUMP;
				  mode.step_bp=0;
				  stop_rap();
			}
			return 6;//R_BUMP;
		}  
		
		if((dummy_wall.effect))
		{
		
			       if((mode.bump == 0))
			       {
				           mode.bump=10;
				           mode.step_bp=0;
				           stop_rap();
			       }
			       return 10;
			
		}     
		if((w_l.sign==NEAR)&&(w_l.on==1))
		{
			if((mode.bump == 0))	    //左墙检靠近墙
			{
			      mode.bump=7;//W_L;
				  mode.step_bp=0;
				  stop_rap();
			}	  
			return 7;//W_L;
		}
		if((w_lm.sign==NEAR)&&(w_lm.on==1))
		{
			if((mode.bump == 0))		 //左中墙检靠近墙
			{
			      mode.bump=8;//W_LM;
				  mode.step_bp=0;
				  stop_rap();
			}
			
			return 8;//W_LM;
		}
		if((w_m.sign==NEAR)&&(w_m.on==1))
		{
			if((mode.bump == 0))	   //中墙检靠近墙
			{
			      mode.bump=9;//W_M;
				  mode.step_bp=0;
				  stop_rap();
			}
			return 9;//W_M;
		}
		
		if((w_rm.sign==NEAR)&&(w_rm.on==1))
		{
			if((mode.bump == 0))		//右中墙检靠近墙
			{
			      mode.bump=15;//W_RM;
				  mode.step_bp=0;
				  stop_rap();
			}
			return 15;//W_RM;
		}
		
		if((w_r.sign==NEAR)&&(w_r.on==1))
		{
			if((mode.bump == 0))		//右墙检靠近墙		
			{
			      mode.bump=14;//W_R;
				  mode.step_bp=0;
				  stop_rap();
			}	
			return 14;//W_R;
		}				
		return 0;
}

/****************************************************************
是否有保护的内容，有则处理并返回1，否则返回0
****************************************************************/
uint8 Read_Protect(void)
{
static uint32 s_current_time;
    if(dust_current > DUST_PROTECTCURRENT)		//灰尘风机电流过大
    {   
        giv_sys_err = 17;
		Init_Err();
		return 1;	    
    }	
    if(s_current.sign == 0)
	{
	    gbv_action_s_motor = FALSE;
		s_current_time = giv_sys_time;
	}
	if(((giv_sys_time - s_current_time) >  50000)&&((mode.mode == SWEEP)||(mode.mode == DOCKING)))
	{
		gbv_action_s_motor = TRUE;
	}
		   
//=====================各种异常信号====================

    if(m_current > M_PROTECTCURRENT)			//中扫电流过大
    {   		  
		if(mode.abnormity != 16)
	    {
		    mode.abnormity = 16;
		    mode.step_abn = 0;		 
		    stop_rap();
		}
		return mode.abnormity;   
    }	
		 					 
	if(r_current > WHEEL_PROTECTCURRENT)
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 31;
		    mode.step_abn = 0; 
		    stop_rap();
		}
		return mode.abnormity;   
	}
	if(l_current > WHEEL_PROTECTCURRENT)
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 32;
		    mode.step_abn = 0; 
		    stop_rap();
		}
		return mode.abnormity;  	  
	}

	if((l_ring.state == BAD))	   //左轮不转的情况
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 19;
		    mode.step_abn = 0;
		    stop_rap();
		}
		return mode.abnormity;
	}

	if((r_ring.state))	   //右轮不转的情况
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 20;
		    mode.step_abn = 0;
		    stop_rap();
		}					
		return mode.abnormity;
	}
 
	if (l_lidi.key == 0) 				  //左轮离地
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 22;
		    mode.step_abn = 0;
		    stop_rap();	
//		    Disable_Sweep();	    //关闭风机马达等
		}			   
		return mode.abnormity;
	}

	if (r_lidi.key == 0)				  //右轮离地
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 23;
		    mode.step_abn = 0;
		    stop_rap();	  
//		    Disable_Sweep();	    //关闭风机马达等
		}		
		return mode.abnormity;
    }	  
	return 0;
}
/*****************************************************************
功能：对保护信号进行处理
*****************************************************************/    
uint8 Action_Protect(void)
{
uint8 m;  
uint32 n;
static uint32 piv_abntime;
    m = Read_Protect();
	if(m == 1)
	{
	    return 1;
	}
	if(mode.abnormity != 0)
	{
	    n = read_bump();
	    if((((e_l.sign == FARN)&&(e_lm.sign == FARN))
		 &&((e_rm.sign == FARN)&&(e_r.sign == FARN)))
		 &&((l_lidi.key == 0)&&(r_lidi.key == 0)))  //在轮子离地并且地检离地时机器停机
		{
		    Init_Cease();
			return 1;    
		}
		if(( m == 0) && (mode.step_abn != 1))
		{
		    stop_rap();
		    mode.step_abn = 1;
		} 
		if((n != 0)&&(mode.step_abn != 1)&&(mode.step_abn < 7))
		{ 
		    stop_rap();
			mode.step_abn = 7;
		}
		if(mode.abnormity == 16)
		{
	 	    switch (mode.step_abn)
		    {
		        case  0:		   
			        piv_abntime = giv_sys_time;
				    mode.step_abn = 3;
			        return 1;
			    case  1:
				    if(do_action(1, angle15 * (giv_sys_time % 10)))
			        {
				        mode.abnormity = 0;	   
						mode.step = 0;
					    return 0;					
				    }
				    if((giv_sys_time - action.time) > 100000)
				    {  				   
				        stop_rap();
				        mode.step_abn = 3;
				    }
				    return 1;
			    case 3:	   
				    if(do_action(1,angle45))
					{
				        mode.step_abn = 4; 
					} 
				    if((giv_sys_time - action.time) > 30000)
				    {  				   
				        stop_rap();
				        mode.step_abn = 4;	  
				    }
				    break;

			    case 4:	  
				    if(do_action(3,angle360))
					{
				        mode.step_abn = 3; 
					} 
				    if((giv_sys_time - action.time) > 100000)
				    {  				     
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err(); 
				    }
				    break; 
				case 7:	
				    if(do_action(4,angle90))		  
				    {
					    mode.step_abn++;
			            break;
				    }
					if( n == 0)
					{
					    stop_rap();
						mode.bump = 0;
						mode.step_abn = 3;
					}
				    if((giv_sys_time - action.time) > 100000)
				    {  				  
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err();
				    }
				    break;  
				case 8:	
				    if(do_action(1,angle90))		  
				    {
					    mode.step_abn++;
			            break;
				    } 
					if( n == 0)
					{
					    stop_rap();
						mode.bump = 0;
						mode.step_abn = 4;
					}
				    if((giv_sys_time - action.time) > 100000)
				    {  				  
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err();
				    }
				    break;  
				case 9:	
				    if(do_action(4,angle90))		  
				    {
				        giv_sys_err = mode.abnormity;
					    Init_Err();
			            return 1;
				    }  
					if( n == 0)
					{
					    stop_rap();
						mode.bump = 0;
						mode.step_abn = 3;
					}
				    if((giv_sys_time - action.time) > 100000)
				    {  				  
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err();
				    }
				    break;
				default :
				    mode.step_abn = 0;
					return 0;				   
		    }
			if((giv_sys_time - piv_abntime) > 150000)
			{
			    stop_rap();
			    giv_sys_err = mode.abnormity;
				Init_Err();
			}
			return 1;
		}
		else
		{	
		    if(mode.abnormity == 21)
			{	
	 	        switch (mode.step_abn)
		    	{
		        	case  0:	
			        	piv_abntime = giv_sys_time;
				    	mode.step_abn = 2;
			        	return 1;
			    	case  1:
				    	if(do_action(1, angle15 * (giv_sys_time % 10)))
			        	{
				        	mode.abnormity = 0;
							mode.step = 0;
					    	return 0;					
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				   
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
				    	return 1;
					case 2:
			        	if(do_action(4,angle360))
		            	{
				        	mode.step_abn = 3;
			            	return 1;
			        	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
						break;
			    	case 3:
				    	if(do_action(1,angle45))
				    	{
					    	mode.step_abn = 4;
				    	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					
				        	stop_rap();
				        	mode.step_abn = 4;
				    	}
				    	break;

			    	case 4:	 		
			        	if(do_action(3,angle360))
				    	{
					    	mode.step_abn = 5;
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 5;
				    	}
				    	break;
			    	case 5:
				    	if(do_action(2,angle90))
						{
					   		mode.step_abn = 6;
						} 	 
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 6;
				    	}
						break; 
					case 6:	
				    	if(do_action(4,angle180))		  
				    	{
							mode.abnormity = 0;
							mode.step = 0;
			            	return 0;
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
							mode.abnormity = 0;
			            	return 0;
				    	}
				    	break;	 
					case 7:	
				    	if(do_action(4,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	}
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	break;  
					case 8:	
				    	if(do_action(1,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	} 
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 4;
						}
				    	break;  
					case 9:	
				    	if(do_action(4,angle90))		  
				    	{
							mode.abnormity = 0;
							mode.step = 0;
			            	return 0;
				    	}  
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	break;
		    	}  
				if((giv_sys_time - piv_abntime) > 250000)
				{
			    	stop_rap();	
					mode.abnormity = 0;
					mode.step = 0;
			        return 0;
				}
				return 1;
			}
			else
			{  
	 	        switch (mode.step_abn)
		    	{
		        	case  0:	
			        	piv_abntime = giv_sys_time;
				    	mode.step_abn = 2;
			        	return 1;
			    	case  1:
				    	if(do_action(1, angle15 * (giv_sys_time % 10)))
			        	{
				        	mode.abnormity = 0;
							mode.step = 0;
					    	return 0;					
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				   
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
				    	return 1;
					case 2:
			        	if(do_action(4,angle60))
		            	{
				        	mode.step_abn = 3;
			            	return 1;
			        	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
						break;
			    	case 3:
				    	if(do_action(1,angle45))
				    	{
					    	mode.step_abn = 4;
				    	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					
				        	stop_rap();
				        	mode.step_abn = 4;
				    	}
				    	break;

			    	case 4:	 		
			        	if(do_action(3,angle60))
				    	{
					    	mode.step_abn = 5;
				    	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 5;
				    	}
				    	break;
			    	case 5:
				    	if(do_action(2,angle90))
						{
					   	mode.step_abn = 6;
						} 	 
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 6;
				    	}
						break; 
					case 6:	
				    	if(do_action(4,angle90))		  
				    	{
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
			            	return 1;
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;	 
					case 7:	
				    	if(do_action(4,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	}
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;  
					case 8:	
				    	if(do_action(1,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	} 
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 4;
						}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;  
					case 9:	
				    	if(do_action(4,angle90))		  
				    	{
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
			            	return 1;
				    	}  
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;
		    	}  
				if((giv_sys_time - piv_abntime) > 150000)
				{
			    	stop_rap();
			    	giv_sys_err = mode.abnormity;
					Init_Err();
				}
				return 1;
			}
		}

	}
    mode.abnormity=0;
    return 0;
}

/**************************************
左转
输入参数    longth    所转的脉冲数
输出  无
***************************************/
void  left(uint32 longth)
{
 enable_rap(BACK, longth,FRONT,longth);

}
/**************************************
左转
输入参数    longth    所转的脉冲数
输出  无
***************************************/
void  right(uint32 longth)
{
enable_rap(FRONT, longth,BACK,longth);
}

 /**************************************
左转
输入参数    longth    所转的脉冲数
输出  无
***************************************/

void  back(uint32 longth)
{
  enable_rap(BACK, longth,BACK,longth);  	        
}
/**************************************
左转
输入参数    longth    所转的脉冲数
输出  无
***************************************/
void  forward(uint32 longth)
{
    enable_rap(FRONT, longth,FRONT,longth);
}
/*************************************************
清除路径有关的内存
*************************************************/
void clr_ram(void)
{
 mode.bump=0;
 mode.step=0;
 mode.step_bp=0;
 mode.abnormity=0;
 mode.step_abn=0;
 mode.times_bp=0;
 mode.angle_wall=0;
 mode.long_wall=0;
 mode.start_wall=0;
 mode.muck=0;
 mode.step_mk=0;
 mode.times_mk=0;
 mode.ybs_sign =0;

 mode.time = giv_sys_time;
 action.sign=0;
 stop_rap();
 Init_Io_Driver();  //关闭中扫边扫以及风机紫外并
 disable_pwm(CHARGE_PWM);
 power.pwm = 0;
}
/*****************************************
根据动作代码执行指定动作
*****************************************/
uint8 do_action(uint8 dm,uint32 length)
{
uint32 m;  
		if(mode.action != dm)
		{
			stop_rap();
			mode.action = dm  ;
		}
		if(action.sign==0)//在当前运行模式下，继续运动；对象是轮子
		{
			if(length == 0)		//形参
			{
			   return 1;
			}
			action.sign = 1;
			action.step=0;
			action.time = giv_sys_time;	//记下动作的开始时间
			action.length = length;	 //入口的预行动距离
		}
		m = (giv_sys_time-action.time);//计算预定的时间，
		if((m>1200000)&&(dm<18))//如果是正常的状态，而且时间超出，将停止当前运行模式
		{
			if(dm != 3)
			{
				stop_rap();
				
				return 1;
			}
		}	   
		switch (dm)
		{
			case  0:
				return 1;
			case  1:					   //左转
				if(action.step==0)
				{
					left(action.length);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign)&&(!r_rap.sign))//两轮的状态是都停下了
				{
					action.sign=0;
					return 1;
				}
				return 0;
			case  2:				   //右转
				if(!action.step)
				{
					right(action.length);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign)&&(!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}
				return 0;
			case  3:				 //前进
				if(!action.step)
				{
					forward(action.length);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign)&&(!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}
				return 0;
				
			case  4:				//后退
				if(!action.step)
				{
					back(action.length);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign)&&(!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}
				return 0;
			case  5:				//左轮不动向左转
				if(!action.step)
				{
					enable_rap(FRONT,0,FRONT,action.length);
					action.step=1;
					return 0;
				}
				if((!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}  
				return 0;
			case  6:			   //右轮不动向右转
				if(!action.step)
				{
				 	enable_rap(FRONT,action.length,FRONT,0);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign))
				{
					action.sign=0;
					return 1;
				}  
				return 0;	 
			case  7:				//右轮不动向左转
				if(!action.step)
				{									 
					enable_rap(BACK,action.length,BACK,0);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign))
				{
					action.sign=0;
					return 1;
				}  
				return 0;
			case  8:			   //左轮不动向右转
				if(!action.step)
				{								 
					enable_rap(BACK,0,BACK,action.length);
					action.step=1;
					return 0;
				}
				if((!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}  
				return 0;
			case  18:
				if(louxuan( 0 ))	//顺时针走螺旋线	 
				{
					action.sign=0;       
					return 1;
				}
				else
				return 0;
			case  19:
				if(louxuan( 1 ))	//逆时针走螺旋线	 
				{
					action.sign=0;       
					return 1;
				}
				else
				return 0;
			case  20:
				if(!action.step)
				{
					forward(action.length);
					action.step=1;
					return 0;
				}
				if(action.step == 1)
				{  
					if((!l_rap.sign)&&(!r_rap.sign))
					{
						back(angle30);
						action.step++;
					}
					return 0;  
				}
				if(action.step == 2)
				{  
					if((!l_rap.sign)&&(!r_rap.sign))
					{
						forward(angle30);
						action.step++;
					}
					return 0;  
				}
				if((!l_rap.sign)&&(!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}
				return 0;	          
			case  21:
				if(z_road())	//走螺旋线	 
				{
					action.sign=0;       
					return 1;
				}
				else
				return 0;
			default :
				stop_rap();
		}
		return 1;
}
/***************************************************
走Z型线，来回数由前面给出， 此程序由   do_action()调用。
输入：无
返回  如果完成了则返回1，否则返回0；
***************************************************/
uint8 z_road(void)
{
  switch(action.step)
  {
    case 0:
    case 6:
    case 12:
	case 18:
	  if((!l_rap.sign)&&(!r_rap.sign))
	  {
	     forward(METE_5);		//向前1.5米
	     action.step++;
	  }
	  return 0;
	case 1:
    case 7:
	case 13:
	case 19:
	  if((l_rap.sign)||(r_rap.sign))
	  {
		return 0;
	  }
	  action.step++;
	case 2:
    case 8:
	case 14:
	case 20:
	  if((!l_rap.sign)&&(!r_rap.sign))
	  {
		action.step++; 
		enable_rap(BACK, angle165, FRONT, angle165);
	  }
	  return 0;
	case 3 :
    case 9:
	case 15:
	case 21:
	  if((!l_rap.sign)&&(!r_rap.sign))
	  {
         if( action.step == 21)
           {
			mode.bump = 0;
        	return 1;
            }
	    forward(METE_5);
		action.step++;
	  }  
	  return 0;
	case 4:
    case 10:
	case 16:
	  if((l_rap.sign)||(r_rap.sign))
	  {
		return 0;
	  }
	  action.step++;
	case 5:
    case 11:
	case 17:
	  if((!l_rap.sign)&&(!r_rap.sign))
	  {
		action.step++; 
		enable_rap(FRONT, angle165, BACK, angle165);
	  }
	  return 0;  
	default :
	return 1; 
  }
}
/***************************************************
走螺旋线，圈数由前面给出， 此程序由   do_action()调用。
螺旋线不能大于20圈  
输入：f = 1,向左转圈；f = 0，向右转圈。
返回  如果完成了则返回1，否则返回0；
***************************************************/
uint8 louxuan(  uint8  f)
{
  switch (action.step)
  {
   case  0:
          if( f != 0)
		  {
	       left(angle360);				 //原地转圈1圈
		  }
		  else
		  {
		   right(angle360);				 //原地转圈1圈
		  }
		  action.step++;
		  return 0;
   case   1:
   case   2:
   case   3:
   case   4:
         if((!l_rap.sign)&&(!r_rap.sign))
		  {
		    if( f != 0)
		     {
              enable_rap(BACK,(angle180-action.step*angle45), FRONT,(angle180+action.step*angle45));
			 }
			 else
			 {
			  enable_rap(FRONT, (angle180+action.step*angle45),BACK,(angle180-action.step*angle45)); 
			 }
			action.step++;
		  }
		  return 0;
   case   5:
   case   6:
   case   7:
   case   8:
   case   9:
   case   10:
   case   11:
   case   12:
   case   13:
   case   14:
   case   15:
   case   16:
   case   17:
   case   18:
   case   19:
   case   20:
   case   21:
   case   22:
   case   23:
   case   24:
   case   25:
   case   26:
   case   27:
   case   28:
   case   29:
   case   30:
   case   31:
   case   32:
   case   33:
   case   34:
   case   35:
   case   36:
   case   37:
   case   38:
   case   39:
   case   40:
          if((!l_rap.sign)&&(!r_rap.sign))
		  { 

			if( (action.step==(action.length*2-1))|| (action.step==40))
			  {
			    action.sign=0;
				return 1;
			  }	
		    if( f != 0)
		     {
		    	enable_rap(FRONT,(action.step-4)*angle45,FRONT,(action.step-4)*angle45+angle360);
			 }
			 else
			 {
			    enable_rap(FRONT,(action.step-4)*angle45+angle360,FRONT,(action.step-4)*angle45);
			 } 					 
			action.step++;
		  }
		  return 0;
  }
  return 1;
}
/***********************************************
程序名：read_earth
程序说明：读取地检信号
返回：在有地检信号时停止动作，将 mode.bump置为其
      地检的值。
***********************************************/
uint8 read_earth(void)
{						   
			if(read_sideearth())
			{
				return 1;
			} 
			if(e_lm.sign == FARN)
			{
				stop_rap();
				mode.step_bp = 0;
				mode.bump = 2;
				return 1;
			}   
			if(e_rm.sign == FARN)
			{
				stop_rap();
				mode.step_bp = 0;
				mode.bump = 3;
				return 1;
			} 
			return 0;		  
}
/***********************************************
程序名：read_earth
程序说明：读取地检信号
返回：在有地检信号时停止动作，将 mode.bump置为其
      地检的值。
***********************************************/
uint8 read_sideearth(void)
{						   
			if(e_r.sign == FARN)
			{
				stop_rap();
				mode.step_bp = 0;
				mode.bump = 4;
				return 1;
			} 
			if(e_l.sign == FARN)
			{
				stop_rap();
				mode.step_bp = 0;
				mode.bump = 1;
				return 1;
			} 
			return 0;		  
}

/*****************************
 沿边扫
 软件中 mode.long_wall 指示沿墙走的距离
*****************************/
/*
uint8 ybs(void)
{
uint32 y;
static uint8 outybs = 0;
    y=read_bump();
    if(mode.bump)
    {		 
	    mode.sum_bump = 0;	
		mode.fangxiang = 0;
	    if((l_rap.rap > 1000)||(r_rap.rap > 1000))
		{
		    l_rap.rap = (l_rap.rap*2)/3;
		    r_rap.rap = (r_rap.rap*2)/3;
		}
        if((mode.angle_wall<10))
        {	
            if(outybs == 0)
			{
			    stop_rap();
				outybs = 1;
			}
            if(!do_action(1,angle45))	   //已经绕障碍一周
			{		   
	            return 1;
			}
			outybs = 0;
	        mode.bump=0;
	        return 0;
        }  //结束 if((mode.angle_wall<10))
        if(mode.angle_wall > angle360 * 5)
        {
		    if(action_wall != FALSE)
			{
		        Disable_wall();
			}
			if(mode.angle_wall > angle360 * 8)
			{			
			    stop_rap();
//			    if(gsv_yaokong.test == 0)	   //取消在沿墙扫时的停止动作
				{			
//				    Init_Cease();
				}
//				else
				{
				    mode.angle_wall = 0;
				}
				return 1;
			}
        }	 
//===================================================		
		 //碰撞信息
        switch (mode.bump)	 
        {					 
	        case  10: //up_hw 
	        case  11: //l_hw
	        case  12: //m_hw
	        case  13: //r_hw	  
	            switch(mode.step_bp)
	            {	
	                case 0:		  
			            if(do_action(4,angle360))
			            {
						    mode.step_bp = 8;
				            return 1;
				        }
						if((y == 0)||(y == 7)||(y == 14))
						{
						    stop_rap();
							mode.step_bp ++;
						}
		                return 1;
		            case 1:	   
		                if(do_action(4,angle30))
			            { 
				            mode.step_bp++;  	 
				            return 1;
			            }
			            if((y != 0)&&(y != 14))
			            {		   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
				            return  1; 
			            }
		                break;
		            case 2 :
					    if(do_action(1,angle90))
						{
						    mode.angle_wall += l_ring.length+r_ring.length;
							mode.step_bp++;
							return 1;
						}
			            if((y != 0)&&(y != 14))
			            {		   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
						    mode.angle_wall += l_ring.length+r_ring.length;
				            return  1; 
			            }
						break;
					case 3:
					case 12:
					    if(do_action(3,angle180))
						{
			                mode.long_wall +=  l_ring.length;
							mode.step_bp = 4;
							return 1;
						}	
						if(y != 0)
						{	   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
						    mode.long_wall +=  l_ring.length;
						}
						break;		 
		            case 4:
					case 11:
			            if(do_action(2,angle90))
			            {			
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
			                mode.step_bp++;	 
				        }
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
						}
					    return 1;
		            case 5:
					case 10:
		                if(do_action(3,angle90))
			            {
				            mode.step_bp++;	 
			                mode.long_wall +=  l_ring.length;
				            return 1;
			        	}
			            if(y)
			            {
			                mode.bump=y;
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 6:			 
		                if(do_action(2,angle30))
			            {
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
			                mode.step_bp++;	 
				        }
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
						}
						break;	
					case 7:		 
		                if(do_action(3,METE))
			            { 
				            mode.step_bp = 0;
							mode.bump = 0;
							mode.angle_wall = 0;  	 
				            return 0;
			            }	 
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						}
		                break; 
					case 8:		
				        if(do_action(6,angle360+angle180))
					    {
					        mode.step_bp = 9;
							mode.angle_wall += angle720 - l_ring.length;
					    }
					    if(y == 0)
					    {
					        stop_rap();
						    mode.step_bp = 10;
						    return 1;
					    }
					    if((y <10)||(y > 13))
					    {
					        stop_rap();
						    mode.step_bp = 0;
						    mode.bump = y;
					    }
					    return 1; 
					case 9:
					    do_action(3,METE);  
					    if(y == 0)
					    {
					        stop_rap();
						    mode.step_bp = 5;
						    return 1;
					    }
					    if((y <10)||(y > 13))
					    {
					        stop_rap();
						    mode.step_bp = 0;
						    mode.bump = y;
				 	    }
						break;
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }//结束switch(mode.step_bp)
			    return 1;
		//==============================================

	        case  7:    //W_L:
	        case  9://W_M:
	        case  8://W_LM:		   			  
        	case  15://W_RM:
	            switch(mode.step_bp)
	            {	
	                case 0:		  
			            if(!do_action(4,angle15))
			            {
				            return 1;
				        }
			            mode.step_bp++;
		                return 1;
		            case 1:	   
		                if(do_action(1,angle180))
			            { 
				            mode.step_bp = 5;  	 
				            return 1;
			            }
			            if((y == mode.bump)||(y == 14))
			            {
				            return  1; 
			            }
			            stop_rap();
			            mode.angle_wall+=l_ring.length*2;
				        if( y == 0)
				        {		
			                mode.step_bp = 2;	
				        }
				        else
				        {
						    mode.step_bp = 0;
					        mode.bump = y;
				        }	
		                break;
		            case 2 :
			            if(!do_action(1,angle30))
			            {
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length*2;  
			            mode.step_bp++;
		            case 3:
		                if(do_action(3,angle90))
			            {
				            mode.step_bp++;	 
			                mode.long_wall +=  l_ring.length;
				            return 1;
			        	}
			            if(y)
			            {
			                mode.bump=y;
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
		            case 4:
		                if(do_action(6,angle360))
			            {						  
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             if(mode.angle_wall>l_ring.length)
				        	 {								 
				                 mode.angle_wall-=l_ring.length;
					         }
				             else 
				        	 {					
				                 mode.angle_wall=0;
				        	 }
				             mode.step_bp=0;
			            }			    
		                break;
	                case 5:
		                if(do_action(3,angle90)) //修改时间2009.12.11
			            { 
						    if(mode.bump == 7)
							{
							    w_l.on = 0;
							}
							if(mode.bump == 8)
							{
							   w_lm.on = 0;
							} 
							if(mode.bump == 9)
							{
							   w_m.on = 0;
							}	
							if(mode.bump == 15)
							{
							   w_rm.on = 0;
							}
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
			            }
			            if((y == mode.bump))
			            {
				            return  1; 
			            }
			            stop_rap();
				        if( y == 0)
				        {		
			                mode.step_bp = 1;	
				        }
				        else
				        {
				            mode.step_bp = 0;
					        mode.bump = y;
				        }	
		                return 1;
					case 6:		
		                if(do_action(3,angle90))
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }//结束switch(mode.step_bp)
	            return 1;

            case  1://E_L
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(7,angle360))
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}
			            else if((y==1))
			        	     {
			        	        return 1; 
			        	     }
			            stop_rap();
			            mode.step_bp++;	 
			            mode.angle_wall+=l_ring.length;
		                break;
		            case 1:
			            if(!do_action(7,angle30))
			            {			
			        	    return 1;
			        	}
			            mode.step_bp++;		 
			            mode.angle_wall+=l_ring.length;
		                break;	 
		            case 2:
			            if(!do_action(8,angle15))
			            {			 
						    read_earth();
			        	    return 1;
			        	}			   
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }
			            mode.step_bp++;
		                break;
		            case 3 :
			            if(!do_action(1,angle60))
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length*2;
			            mode.step_bp++;
		            case 4:
		                if(do_action(3,angle90))
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 5:
		                if(do_action(6,angle360))
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 6:		
		                if(do_action(3,angle90))
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//结束switch(mode.step_bp)
		        return 1; 	  
        	case  4://E_R:	 
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(8,angle360))
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}
			            else if((y==4))
			        	     {
			        	        return 1; 
			        	     }
			            stop_rap();
			            mode.step_bp++;	 
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }	 
		                break;
		            case 1:
			            if(!do_action(8,angle30))
			            {			
			        	    return 1;
			        	}
			            mode.step_bp++;	
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }	 
		                break;	 
		            case 2:
			            if(!do_action(7,angle15))
			            {			 
						    read_earth();
			        	    return 1;
			        	}			   
			            mode.step_bp++;			
			            mode.angle_wall+=l_ring.length;
		                break;
		            case 3 :
			            if(!do_action(1,angle60))
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length*2;
			            mode.step_bp++;
		            case 4:
		                if(do_action(3,angle90))
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 5:
		                if(do_action(6,angle360))
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 6:		
		                if(do_action(3,angle90))
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//结束switch(mode.step_bp)
		        return 1; 	 
	        case  2://E_LM
	        case  3://E_RM:	
        	case  5://L_BUMP:
        	case  6://R_BUMP:
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(4,angle360))
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}				
						read_sideearth() ;
			            if((y == mode.bump))
			        	{
			        	     return 1; 
			        	}
			            stop_rap();
			            mode.step_bp++;	
		                break;
		            case 1:
			            if(!do_action(4,angle15))
			            {
			        	    return 1;
			        	}		   
						read_sideearth() ;
			            mode.step_bp++;
		                break;
		            case 2 :
			            if(!do_action(1,angle60))
			            {
						    read_earth();
					        return 1;
				        }
						
			            mode.angle_wall+=l_ring.length*2;
			            mode.step_bp++;
		            case 3:
		                if(do_action(3,angle90))
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 4:
		                if(do_action(6,angle360))
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 5 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 5:		
		                if(do_action(3,angle90))
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//结束switch(mode.step_bp)
		        return 1; 
        	case  14://W_R:		
                switch (mode.step_bp)
				{
	                case 0:
					    if(do_action(1,angle180))
						{
						    mode.step_bp = 6;
						}
			            if((y != 14))
			            { 			   
							stop_rap();	
							mode.angle_wall += r_ring.length*2;
				            if(y == 0)
				            {
								mode.step_bp = 1;
				            }
				            else
				            {
				                mode.bump = y;
				            }
			            }
			            return 1;
		            case 1:			  
	                case 4:	 
		                if(do_action(6,angle360))
			            {
			                mode.step_bp += 2;
			                mode.long_wall +=  l_ring.length/2;	 
							if( mode.angle_wall > l_ring.length)
							{
							     mode.angle_wall -= l_ring.length;
							}
							else
							{
							     mode.angle_wall = 0;
							}
			                return 1; 
			            }
			            if(y != 0)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length/2;
							if( mode.angle_wall > l_ring.length)
							{
							     mode.angle_wall -= l_ring.length;
							}
							else
							{
							     mode.angle_wall = 0;
							}
							if(y == 14)
							{
							    mode.step_bp ++;
							}
							else
							{	  
				                mode.bump = y;
				                mode.step_bp = 0;
							}
			            }
			            return 1;
	                case 2:
		                if(do_action(3,angle60))
			            {
			                mode.step_bp = 0;
			                mode.long_wall +=  l_ring.length;
			                return 1; 
			            }
			            if(y != 14)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length;
							if(y == 0)
							{
							    mode.step_bp = 1;
							}
							else
							{	
				                mode.bump = y;
				                mode.step_bp = 0;
							}
			            }
			            return 1;
					case 3:	  
		                if(do_action(3,angle90))
			            {
			                mode.step_bp = 4;
			                mode.long_wall +=  l_ring.length;
			                return 1; 
			            }
			            if(y != 0)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length;
				            mode.bump = y;
				            mode.step_bp = 0;
			            }
			            return 1;
	                case 6:	 
					    //修改时间2009.12.11	 
						w_r.on = 0;
				        mode.bump = 0;
						mode.step_bp = 0;
				        mode.angle_wall = 0 ;
			            return 1;				 
		            default :
					     stop_rap();
		                 mode.step_bp=0;
	            }//结束	switch (mode.step_bp)
                return  1;   
            default :
	            mode.bump = 0;	   
        }//结束swicth(bump)			   
    } //结束if(mode.bump)
    return 0;
}  */
 //***************************************************
 //---dengpo modify about port_code----
 //---2010/11/29-----------------------
 //add to times for bump---------------
 //****************************************************
 /*
uint8 action_bump(void)
{
uint32 m;
static uint8 s=0;
    if(mode.ybs_sign == 1)	//需要沿着墙壁扫的过程
    {
		    if( ybs() == 1)
		    {
			        s = s%10;
			        if( 9 == s)
			        {
			            m = 50000;
			        }
			        else
			        {
			            if(s == 6)
				        {
				            m =  30000;
				        }
				        else
				        {
				            m =  10000;
				        }
			        }
			        if(mode.long_wall > m)
			        {
			            s++;
			            stop_rap();
			            mode.long_wall =0;
				        mode.ybs_sign = 0;
				        mode.angle_wall = 0;
						Enable_wall();
			        }
			        return 1;
		    }
			Enable_wall();
		    return 0;
    }//结束if(mode.ybs_sign == 1)
	if(mode.bump != 0)
	{
		    if((l_rap.rap > 1000)||(r_rap.rap > 1000))
			{
			    l_rap.rap = (l_rap.rap*2)/3;
			    r_rap.rap = (r_rap.rap*2)/3;
			}
	}	 
    m=read_bump();
    switch(mode.bump)
    {
	    case 1://E_L		   
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(7,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==1))
				    {
				        return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(7,angle30))
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(8,angle15))
				    {
				        mode.step_bp++;
				    }	  
				    read_earth();
				    return 1;
		        case  3:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  4:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
	    case 2://E_LM 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m!=0)&&(m < 7))
				    {  
						read_sideearth() ;
				        return 1;
				    }
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;	
				    }	   
					read_sideearth() ;
				    return 1;
		        case  2:   
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				         mode.times_bp=0;
					     mode.bump=0;
				         return 0;
				    } 
				    read_earth();
					break;
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return 1;	  
	    case 5://L_BUMP:  
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle120))
		            {  	  
					    mode.step_bp = 4;
					    return 1; 
		            }
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;	
				    }	   
				    read_earth();
				    return 1;
		        case  2:   
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				         mode.times_bp=0;
					     mode.bump=0;
				         return 0;
				    } 
				    read_earth();
					break;
				case  4:
				    if(do_action(2,angle120))
					{
					    mode.step_bp = 5;
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;	
				case  5:
				    if(do_action(3,angle120))
					{			  
					    giv_sys_err = mode.bump;
						Init_Err();
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return 1;
	    case 4://E_R: 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(8,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==4))
				    {
				     return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(8,angle30))
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(7,angle15))
				    {
				        mode.step_bp++;
				    }	
				    read_earth();
				    return 1;
		        case  3:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  4:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
	    case 3://E_RM:
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m!=0)&&(m < 7))
				    { 
						read_sideearth() ;
				        return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;
				    }			   
					read_sideearth() ;
				    return 1;
		        case  2:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;	  
	    case 6://R_BUMP:		
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle120))
		            {  
					    mode.step_bp = 4;
					    return 1; 
		            }	 
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
				    if((m!=0)&&(m < 7))
				    { 
						read_earth() ;
				        return 1;
				    }		   
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;
				    }			   
					read_sideearth() ;
				    return 1;
		        case  2:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;		
				case  4:
				    if(do_action(1,angle120))
					{
					    mode.step_bp = 5;
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;	
				case  5:
				    if(do_action(3,angle120))
					{			  
					    giv_sys_err = mode.bump;
						Init_Err();
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
	    case 7://W_L:
	    case 8://W_LM:	 		          
		    switch (mode.step_bp)
		    {
		        case 0:
				    if(!do_action(4,angle15))
					{
					    return  1;
					}
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}	 
					mode.sum_bump++;
					mode.step_bp = 1;
					return 1;
				case 1:
			        if(do_action(mode.fangxiang,angle180))
				    {
					    mode.step_bp = 3;
					    return 1;
				    }
				    if((m == 7)||(m == 8))
				    {
				        return 1;
				    }		   
					stop_rap();	  
				    mode.step_bp ++;
				    if(m != 0)
				    {
					    mode.step_bp = 0;
					    mode.bump = m;
					    return 1;
				    }
			    case  2:
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    }
			        return  1; 
		        case 3:
			        if(do_action(3,angle90))//修改时间2009.12.11
			         { 
						    if(mode.bump == 7)
							{
							    w_l.on = 0;
							}
							if(mode.bump == 8)
							{
							   w_lm.on = 0;
							} 
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
				    }
				    if((m == 7)||(m == 8))
				    {
				        return 1;
				    }		 
				    stop_rap();
				    mode.step_bp=2;
				    if(m != 0)
				    {
					    mode.step_bp = 0;
					    mode.bump = m;
				    }
			        return  1; 
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    } 
			return 1;	 	
	    case 10://up_hw			 	
	    case 11://l_hw	 	
	    case 12://m_hw
	    case 13://r_hw		
		    switch(mode.step_bp)
			{
			    case 0:
				    if(do_action(4,angle360))
					{
					    mode.step_bp = 3; 
						mode.sum_bump++;  
					    if(mode.fangxiang == 0)
					    {
					        mode.fangxiang = 1;
					    }	
					} 
					if((m == 0)||(m == 7)||(m == 14))
					{
					    stop_rap();
						mode.step_bp = 1;  
					    if(mode.fangxiang == 0)
					    {
					        mode.fangxiang = 1;
					    }
						return 1;
					}
					return 1;  
			    case 1:
				    if(do_action(4,angle90))
					{
					    mode.step_bp = 2; 
					} 
					if(m == 0)
					{
						return 1;
					}
					stop_rap();
					mode.step_bp = 0;
					mode.bump = m;
					return 1;
			    case 2:
				    if(do_action(1,angle180))
					{
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
					}
					if((m != 0))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
			    case 3:
				    if(do_action(6,angle360+angle180))
					{
					    mode.step_bp = 4;
					}
					if(m == 0)
					{
					    stop_rap();
						mode.step_bp = 5;
						return 1;
					}
					if((m <10)||(m > 13))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
				case 4:
				    do_action(3,METE);	
					if(m == 0)
					{
					    stop_rap();
						mode.step_bp = 5;
						return 1;
					}
					if((m <10)||(m > 13))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}			
					return 1;
			    case 5:
				    if(do_action(3,angle90))
					{
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
					}
					if(m != 0)
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
				default :
				    stop_rap();
				    mode.step_bp = 0;
			}
			return 1;
	    case 9://W_M:									  
	    case 14://W_R:	
	    case 15://W_RM:
	        switch(mode.step_bp)
		    {
		        case 0:			   
			        if(!do_action(4,angle15))
				    {
					    return 1;  
				    }				
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
					mode.step_bp = 1;
			    case  1:
			        if(do_action(mode.fangxiang,angle180))
			        {
					    mode.step_bp = 3;
					    return 1;
				    }
			        if((m == 9)||(m==14)||(m==15))
			        {
				        return  1; 
				    }		 
				    stop_rap();
				    mode.step_bp++;
				    if(m != 0)
				    {
					    stop_rap();
					    mode.step_bp=0;
					    mode.bump=m;
					    return 1;
				    }	 
			    case  2:
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    } 
			        return 1;
		        case 3:
			        if(do_action(3,angle90))//修改时间2009.12.11
			        { 
							if(mode.bump == 9)
							{
							   w_m.on = 0;
							}			   
							if(mode.bump == 14)
							{
							   w_r.on = 0;
							} 
							if(mode.bump == 15)
							{
							   w_rm.on = 0;
							}
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
				    }
				    if((m == 9)||(m==14)||(m==15))
				    {
				        return 1;
				    }		
				    stop_rap();
				    mode.step_bp=2;
				    if(m != 0)
				    {
					    mode.step_bp=0;
					    mode.bump=m;
				    }
			        return  1; 
			    default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
			return 1;
	    default :
	        mode.bump = 0;
	        break;
	} //结束  switch(mode.bump)
    return 0;
}
*/
/************************************************************
核对当前时间是否与预约时间一致，如果是则返回1；否则返回0
************************************************************/
uint8 Check_PreengageTime(void)
{
uint16 piv_t;
uint8  piv_hour;
uint8  piv_minute;
static uint8 piv_preengage;
uint32 t;
    t = Rtc_time % 604800;
    piv_minute = (uint8)((t % 3600) / 60);
    piv_hour = (uint8)((t % 86400) / 3600) ;  
    piv_t = (piv_hour << 8)	+ piv_minute;
	if(piv_t == PreengageTime)
	{
	    if(piv_preengage == 1)
		{
		    piv_preengage = 0;
	        if((((1<<(t/86400))& PreengageDate) != 0))
	        {
		        return 1;
		    }
		}
	}
	else
	{
	    piv_preengage = 1;
	} 
	return 0;  
}
/************************************************************
初始化碰撞以及保护
************************************************************/
void Init_Action(void)
{
//#define     NEAR            0XFE                              //近
//#define     FARN            0XFD                              //远
   e_l.sign = NEAR;
   e_lm.sign =NEAR;
   e_rm.sign = NEAR;	
   e_r.sign = NEAR;
   l_bump.key = 0;
   r_bump.key = 0;
   w_l.sign = FARN;
   w_lm.sign = FARN;
   w_m.sign = FARN;
   w_r.sign = FARN;
   w_rm.sign = FARN;
   l_ring.state = GOOD;
   r_ring.state = GOOD;
}
/****************************************************
功能：根据电池电压判断显示电压
输入：无
输出：
返回：无
****************************************************/
uint8 Get_Dispower(void)
{
uint32  t;
    t = (giv_BatteryFDCap *10)/giv_BatteryCapability;
    if(t > 7)
	{
	    return 1;
	}
	if(t > 4)
	{
	    return 2;
	}
	return 3 ;
}
/***************************************************
功能：每相隔1秒计算电池剩余容量
***************************************************/
void AccountCapability(void)
{
    if(giv_UsePowerTime == Rtc_time)
	{
	    return ;
	}
	AccountCapabilityReal();
}

/****************************************************
功能：计算机器使用的电池电量
输入：无
输出：
返回：无
说明：通过计算在各种模式下工作时间来判断使用的电量
      改变计数值
	  giv_UsePowerTime    
	  giv_BatteryFDCap
	  
	  在电池电压低于13.2v时，电池认为已经放完电，并且将放电容量存入备份寄存器
****************************************************/
void  AccountCapabilityReal(void)
{
static uint8 entercapability = 0;
uint32 t;//耗电时间
uint32 curr;//耗电电流
	t = 0;
	curr = 0;
	if(Rtc_time > giv_UsePowerTime)
	{
	    t = Rtc_time  - giv_UsePowerTime;
	}
	giv_UsePowerTime = Rtc_time;
    switch(mode.mode)
	{
        case CEASE: 
	    case ERR:
		    curr = 71;
		    break;
        case SLEEP:
		    curr = 20;
	        break;
	    case SWEEP:
	    case SPOT:
	    case TELE:
		    curr = 1200;
		    break ;
	    case DOCKING:
		    curr = 400;
	        break;
	    case CHARGEING:
	        switch(power.step)
			{
				case 2:
				case 4:	  
				case 7:
				case 11:
				case 13:
				    curr = 150;
				    break; 
		        case 5:	  //电池以0.05C充电即80ma充电5分钟 
		        case 9:	
				    curr = 80;
				    break; 
				case 3:
				    curr = 800;
					break;
			}
	        break;
	}
	t = t*curr;
	if(t > 2600)
	{
	    return;
	}
	if(mode.mode == CHARGEING)
	{
	    if(giv_BatteryFDCap > t)
		{
		    giv_BatteryFDCap -= t;
		}
		else
		{
		    giv_BatteryFDCap = 0;
		}
	}
	else 
	{
	    if(giv_BatteryFDCap < 0x83d600)
	    giv_BatteryFDCap += t;
	}
    if((((giv_BatteryFDCap *10)/giv_BatteryCapability) > 2)&&(giv_BatteryChargeForbid == 1))
	{
	    giv_BatteryChargeForbid = 0;
	}
	WriteFDCap();
	if(battery_voltage < LOWPOWER)
	{
	    if(entercapability == 0)
		{
		    entercapability = 1;
			giv_BatteryChargeForbid = 0;
			if((giv_BatteryFDCap > 0x36ee80)&&(giv_BatteryFDCap < 0x83d600))
			{
		        giv_BatteryCapability = giv_BatteryFDCap;
			}
			else
			{
				giv_BatteryFDCap = 	 giv_BatteryCapability;
			}					   
			WriteBatteryCapability();
		}
	}
	else
	{
	    if(battery_voltage > TWOPOWER)
		{
		    entercapability = 0;
		}
	}
    return;
}
/*******************************************************
功能：检查中扫和风机的是否在
返回：在返回0，否则返回1
*******************************************************/
uint8 CheckMDustMotor(void)
{
uint16 piv_dust,piv_m;
uint16 v;
    piv_dust = account_current(DUST_CURRENT);
	piv_m = account_current(M_CURRENT);
	Set_Dust_Motor();
    pbv_state_dust_motor = TRUE;
	DelayMs(500);
	v = account_current(DUST_CURRENT) ;
	if((v < piv_dust)||(( v - piv_dust) < 10))
	{
	     DelayMs(500);
	     v = account_current(DUST_CURRENT) ;
		 if((v < piv_dust)||(( v - piv_dust) < 10))
		 {
	         giv_sys_err = 33;
		     Init_Err();
		     return 1;
		 }
	}
    Set_M_Motor();
	//Reset_M_ZNMOS();
	//Set_M_ZPMOS();
	pbv_state_m_motor = TRUE;
	DelayMs(500);
	v = account_current(M_CURRENT) ;
	if((v < piv_m)||(( v - piv_m) < 10))
	{
	     DelayMs(500);
	     v = account_current(M_CURRENT) ;
		 if((v < piv_m)||(( v - piv_m) < 10))
		 {
	         giv_sys_err = 34;
		     Init_Err();
		     return 1;
		 }
	}
	return 0;
}
//===================================================================

uint8 action_bump(void)
{
uint32 m;
static uint8 s=0;

    if(mode.ybs_sign == 1)	//需要沿着墙壁扫的过程
    {
		    if( ybs() == 1)		   //此处就是执行沿边扫
		    {
			        s = s%10;
			     /*   if( 9 == s)
			        {
			            m = 50000;
			        }
			        else
			        {
			            if(s == 6)
				        {
				            m =  30000;
				        }
				        else
				        {
				            m =  10000;
				        }
			        } */
					if(s==9)m = 30000;
					else if(s==8||s==7||s==6)m = 20000;
					else if(s<6)m = 5000;
				
			        if(mode.long_wall > m)	 //超出墙边扫的范围，退出沿边扫
			        {
			            s++;
			            stop_rap();
			            mode.long_wall =0;
				        mode.ybs_sign = 0;
				        mode.angle_wall = 0;
						Enable_wall();
			        }
			        return 1;
		    }
			Enable_wall();
		    return 0;
    }//结束if(mode.ybs_sign == 1)
	if(mode.bump != 0)	 //前期有无碰撞
	{
	        if((l_rap.rap > 1000)||(r_rap.rap > 1000))
			{
			    l_rap.rap = (l_rap.rap*2)/3;
			    r_rap.rap = (r_rap.rap*2)/3;
			}
	}
	 
    m=read_bump();//变量的再度利用 ，//M变成碰撞信息 M=mode.bump
	
    switch(mode.bump) //状态的解析以及处理方式
    {//-------------------------
	    case 1://E_L		   
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(7,angle360)) //右轮不动向左转	,左半身后退向左传
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==1))
				    {
				        return 1;
				    }		   
					if(mode.fangxiang == 0)	//如果开始是不动的，需调整为右转方向
					{
					    mode.fangxiang = 2;
					}	 
					mode.sum_bump++;        //碰撞次数的临时计算，以便后面的使用 
				 	stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(7,angle30))  //左轮不动，左转一定角度 ，右半身前进向左转
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(8,angle15))  //左轮不动，右转一定的角度
				    {
				        mode.step_bp++;
				    }	  
				    read_earth();
				    return 1;
		        case  3:		
			        if(do_action(mode.fangxiang,angle45))  //全身向右旋转一定的角度
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  4:	//mode.bump=1；mode.step_bp=4；	  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))//两轮同时转动，右轮旋转一定的角度
			        { //以防死角，以不同的角度旋转，而逃脱
				        mode.times_bp++;
				        if(mode.times_bp>51)mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth(); //修改/mode.bump，优先级别的提高，运动中的监测
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
	   //-----------------------------------
	    case 2://E_LM 
	        switch  (mode.step_bp)
		    {
			    case  0:												  
	                if(do_action(4,angle360))	 //	后退
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m!=0)&&(m < 7))	//除左边地检的地检和碰撞检测状况下的，对地检测
				    {  
						read_sideearth() ;
				        return 1;
				    }
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))   //后退
				    {
				        mode.step_bp++;	
				    }	   
					read_sideearth() ;
				    return 1;
		        case  2:   
			        if(do_action(mode.fangxiang,angle45))	//右转
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))//右转时间调整
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return 1;
		//--------------------------------------		  
	    case 5://L_BUMP:  	//左右碰撞的处理动作相对多两步
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle120))	 //后退
		            {  	  
					    mode.step_bp = 4;
					    return 1;                //表示有过碰撞
		            }
			//	if((ADC_ConvertedValue[7] & 0XFFF)>(ADC_ConvertedValue[14] & 0XFFF)||
			//	   (ADC_ConvertedValue[14] & 0XFFF)>(ADC_ConvertedValue[15] & 0XFFF))mode.fangxiang=2;
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}	
				    if((m!=0)&&(m < 7))//M=READ_BUMP（）；
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle30))	//后退angle15
				    {
				        mode.step_bp++;	        //直到后退的完成,除非地检发生意外
				    }	   
				    read_earth();
				    return 1;
		        case  2:   
			        if(do_action(2,angle30));//mode.fangxiang,angle30))	 //右转或者左传	angle45
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(2,angle[mode.times_bp]))	 //时间调整
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				case  4:
				    if(do_action(2,angle75))	 //右转	 angle120
					{
					    mode.step_bp = 5;
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
					mode.ybs_sign=1;
				    stop_rap();
				    mode.step_bp=1;
					break;	
				case  5:
				    if (mode.ybs_sign==0)
					{
					    if(do_action(3,angle60))	 //向前行走	angle120 1293大约20CM
						{			  
						    giv_sys_err = mode.bump;
							Init_Err();
							return 1;
						}	
					    if((m!=0)&&(m < 7))
					    {  
							read_earth() ;
					        return 1;
					    }
					}					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return 1;
		//-----------------------------------------
	    case 4://E_R: 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(8,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==4))
				    {
				     return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(8,angle30))
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(7,angle15))
				    {
				        mode.step_bp++;
				    }	
				    read_earth();
				    return 1;
		        case  3:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  4:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
		//---------------------------------------------
	    case 3://E_RM:
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m!=0)&&(m < 7))
				    { 
						read_sideearth() ;
				        return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;
				    }			   
					read_sideearth() ;
				    return 1;
		        case  2:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
		//--------------------------------------------------		  
	    case 6://R_BUMP:		
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle120))	 //后退
		            {  
					    mode.step_bp = 4;
					    return 1; 
		            }	 
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
				    if((m!=0)&&(m < 7))
				    { 
						read_earth() ;
				        return 1;
				    }		   
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle30))	 //后退angle15
				    {
				        mode.step_bp++;
				    }			   
					read_sideearth() ;
				    return 1;
		        case  2:		
			        if(do_action(1,angle30)) //最初不是向右，那么现在 向左；是，继续向右angle45
			        {
				        mode.step_bp++;											 
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(1,angle[mode.times_bp])) //调整时间
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;		
				case  4:
				    if(do_action(1,angle60))	//左传angle120
					{
					    mode.step_bp = 5;
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
					mode.ybs_sign=1;
				    stop_rap();
				    mode.step_bp=1;
					break;	
				case  5:
				    if(mode.ybs_sign==0)
				    {
					    if(do_action(3,angle60))  //向前行走 angle120
						{			  
						    giv_sys_err = mode.bump;
							Init_Err();
							return 1;
						}	
					    if((m!=0)&&(m < 7))
					    {  
							read_earth() ;
					        return 1;
					    }
					}
					mode.sum_bump++;
	          	    stop_rap();
				    mode.step_bp=1;
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;//以防程序在入口的时候，没有步骤
		    }
		    return  1;
		//--左边墙检处理-------------------------------
	    case 7://W_L:
	    case 8://W_LM:	 		          
		    switch (mode.step_bp)
		    {
		        case 0:
				    if(!do_action(4,angle15))
					{
					    return  1;
					}
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}	 
					mode.sum_bump++;
			       	mode.step_bp = 1;
					return 1;
				case 1:
			        if(do_action(mode.fangxiang,angle75)) //angle180
				    {
					    mode.step_bp = 3;
					    return 1;
				    }
				    if((m == 7)||(m == 8))
				    {
				        return 1;
				    }		   
					stop_rap();	  
				    mode.step_bp ++;
				    if(m != 0)
				    {
					    mode.step_bp = 0;
					    mode.bump = m;
					    return 1;
				    }
			    case  2:
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    }
			        return  1; 
		        case 3:
			        if(do_action(3,angle60))//修改时间2009.12.11 angle90
			         { 
						    if(mode.bump == 7)
							{
							    w_l.on = 0;
							}
							if(mode.bump == 8)
							{
							   w_lm.on = 0;
							} 
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
				    }
				    if((m == 7)||(m == 8))
				    {
				        return 1;
				    }		 
				    stop_rap();
				    mode.step_bp=2;
				    if(m != 0)
				    {
					    mode.step_bp = 0;
					    mode.bump = m;
				    }
			        return  1; 
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    } 
			return 1;	
		//--------------------------------------------	 	
	    case 10://up_hw			 	
	    case 11://l_hw	 	
	    case 12://m_hw
	    case 13://r_hw		
		    switch(mode.step_bp)
			{
			    case 0:
				    if(do_action(4,angle360))
					{
					    mode.step_bp = 3; 
						mode.sum_bump++; 
						mode.ybs_sign=1;
						if(mode.fangxiang == 0)
					    {
					        mode.fangxiang = 1;
					    }	
					} 
					if((m == 0)||(m == 7)||(m == 14))
					{
					    stop_rap();
						mode.step_bp = 1;  
					    if(mode.fangxiang == 0)
					    {
					        mode.fangxiang = 1;
					    }
						return 1;
					}
					return 1;  
			    case 1:
				    if(do_action(4,angle60))//angle90
					{
					    mode.step_bp = 2; 
					} 
					if(m == 0)
					{
						return 1;
					}
					stop_rap();
					mode.step_bp = 0;
					mode.bump = m;
					return 1;
			    case 2:
				    if(do_action(1,angle180))
					{
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
					}
					if((m != 0))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
			    case 3:
				    if(do_action(6,angle180))//angle360+angle180
					{
					    mode.step_bp = 4;
					}
					if(m == 0)
					{
					    stop_rap();
						mode.step_bp = 5;
						return 1;
					}
					if((m <10)||(m > 13))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
				case 4:
				   if(mode.ybs_sign==0)
				   do_action(3,METE);//1米前进脱离	
					if(m == 0)
					{
					    stop_rap();
						mode.step_bp = 5;
						return 1;
					}
					if((m <10)||(m > 13))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}			
					return 1;
			    case 5:
				    if(do_action(3,angle30))//angle90
					{
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
					}
					if(m != 0)
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
				default :
				    stop_rap();
				    mode.step_bp = 0;
			}
			return 1;
		//----------------------------------------------
	    case 9://W_M:									  
	    case 14://W_R:	
	    case 15://W_RM:
	        switch(mode.step_bp)
		    {
		        case 0:			   
			        if(!do_action(4,angle15))
				    {
					    return 1;  
				    }				
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
					mode.step_bp = 1;
			    case  1:
			        if(do_action(mode.fangxiang,angle75))//angle180
			        {
					    mode.step_bp = 3;
					    return 1;
				    }
			        if((m == 9)||(m==14)||(m==15))
			        {
				        return  1; 
				    }		 
				    stop_rap();
				    mode.step_bp++;
				    if(m != 0)
				    {
					    stop_rap();
					    mode.step_bp=0;
					    mode.bump=m;
					    return 1;
				    }	 
			    case  2:
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    } 
			        return 1;
		        case 3:
			        if(do_action(3,angle60))//修改时间2009.12.11 angle90
			        { 
							if(mode.bump == 9)
							{
							   w_m.on = 0;
							}			   
							if(mode.bump == 14)
							{
							   w_r.on = 0;
							} 
							if(mode.bump == 15)
							{
							   w_rm.on = 0;
							}
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
				    }
				    if((m == 9)||(m==14)||(m==15))
				    {
				        return 1;
				    }		
				    stop_rap();
				    mode.step_bp=2;
				    if(m != 0)
				    {
					    mode.step_bp=0;
					    mode.bump=m;
				    }
			        return  1; 
			    default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
			return 1;
	    default :
	        mode.bump = 0;
	        break;
	} //结束  switch(mode.bump)
    return 0;
}
 //---------------------------------------------------------------
uint8 ybs(void)
{
uint32 y;
static uint8 outybs = 0;
    y=read_bump();
    if(mode.bump)
    {		 
	    mode.sum_bump = 0;	
		mode.fangxiang = 0;
	    if((l_rap.rap > 1000)||(r_rap.rap > 1000))
		{
		    l_rap.rap = (l_rap.rap*2)/3;
		    r_rap.rap = (r_rap.rap*2)/3;
		}
        if((mode.angle_wall<10))
        {	
            if(outybs == 0)
			{
			    stop_rap();
				outybs = 1;
			}
            if(!do_action(1,angle45))	   //已经绕障碍一周
			{		   
	            return 1;
			}
			outybs = 0;
	        mode.bump=0;
	        return 0;
        }  //结束 if((mode.angle_wall<10))
       
    if(mode.angle_wall > angle360 * 5) //3880*5
        {
		    if(action_wall != FALSE)
			{
		        Disable_wall();
			}
			if(mode.angle_wall > angle360 * 8) //超出一定的试行运动，依然没有脱离时，停止
			{			
			    stop_rap();
				mode.angle_wall = 0;
				return 1;
			}
        }	 
//===================================================		
		 //碰撞信息
        switch (mode.bump)	 
        {
//--------------------------------------
//--第一情况,红外碰撞,dummy wall							 
	        case  10: //up_hw 
	       // case  11: //l_hw
	      //  case  12: //m_hw
	      //  case  13: //r_hw	  
	            switch(mode.step_bp)	  //碰撞时的步骤标志
	            {	
	                case 0:		  
			            if(do_action(4,angle360))  //后退
			            {
						    mode.step_bp = 8;
				            return 1;
				        }
						if((y == 0)||(y == 7)||(y == 14))  //如果有左或者右墙检，继续后退
						{
						    stop_rap();
							mode.step_bp ++;
						}
		                return 1;
		            case 1:	   
		                if(do_action(4,angle30))	 //后退
			            { 
				            mode.step_bp++;  	 
				            return 1;
			            }
			            if((y != 0)&&(y != 14))	   //直到右墙检无信号
			            {		   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
				            return  1; 
			            }
		                break;
		            case 2 :
					    if(do_action(1,angle90))   //左传
						{
						    mode.angle_wall += l_ring.length+r_ring.length;	 //旋转角度的计算
							mode.step_bp++;
							return 1;
						}
			            if((y != 0)&&(y != 14))	 //没有左边和右边墙检
			            {		   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	      //新的碰撞信息
						    mode.angle_wall += l_ring.length+r_ring.length;	 //旋转角度的计算
				            return  1; 
			            }
						break;
					case 3:
					case 12:
					    if(do_action(3,angle180))	  //前进
						{
			                mode.long_wall +=  l_ring.length;	 //前进距离的计算，都是以左轮行走的脉冲数
							mode.step_bp = 4;
							return 1;
						}	
						if(y != 0)	  //按照预期y应该为0, 不是,则有新的碰转
						{	   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
						    mode.long_wall +=  l_ring.length;
						}
						break;		 
		            case 4:
					case 11:
			            if(do_action(2,angle90))		//右转
			            {			
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  //左转做加，右转做减法运算
							}
							else
							{
							    mode.angle_wall  = 0;
							}
			                mode.step_bp++;	 
				        }
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
						}
					    return 1;
		            case 5:
					case 10:
		                if(do_action(3,angle90))	//前进
			            {
				            mode.step_bp++;	 
			                mode.long_wall +=  l_ring.length;
				            return 1;
			        	}
			            if(y)
			            {
			                mode.bump=y;
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 6:			 
		                if(do_action(2,angle30))	//右转
			            {
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
			                mode.step_bp++;	 
				        }
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
						}
						break;	
					case 7:		 
		                if(do_action(3,METE))	 //前进，非沿墙走，1米
			            { 
				            mode.step_bp = 0;
							mode.bump = 0;
							mode.angle_wall = 0;  	//将墙检角度计算，重新归零，以备下一次的使用 
				            return 0;
			            }	 
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						}
		                break; 
					case 8:		
				        if(do_action(6,angle360+angle180))	 //	  右轮不动向右转
					    {
					        mode.step_bp = 9;
							mode.angle_wall += angle720 - l_ring.length;  //只有一个轮子旋转时的，角度计算
					    }
					    if(y == 0)
					    {
					        stop_rap();
						    mode.step_bp = 10;
						    return 1;
					    }
					    else //if((y <10)||(y > 13))
					    {
					        stop_rap();
						    mode.step_bp = 0;
						    mode.bump = y;
					    }
					    return 1; 
					case 9:
					    do_action(3,METE);  	//离开墙边，前进1米
					    if(y == 0)
					    {
					        stop_rap();
						    mode.step_bp = 5;
						    return 1;
					    }
					    else //if((y <10)||(y > 13))
					    {
					        stop_rap();
						    mode.step_bp = 0;
						    mode.bump = y;
				 	    }
						break;
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }//结束switch(mode.step_bp)
			    return 1;
//--------------------------------------------------
//--第二情况碰撞,墙检（左，左中，中，右中）

	        case  7: //W_L:
	        case  9://W_M:
	        case  8://W_LM:		   			  
        	case  15://W_RM:
	            switch(mode.step_bp)
	            {	
	                case 0:		  
			            if(!do_action(4,angle15)) //houtui
			            {
				            return 1;
				        }
			            mode.step_bp++;
		                return 1;
		            case 1:	   
		                if(do_action(1,angle180))  //zuozhuan
			            { 
				            mode.step_bp = 5;  	 
				            return 1;
			            }
			            if((y == mode.bump)||(y == 14))
			            {
				            return  1; 
			            }
			            stop_rap();
			            mode.angle_wall+=l_ring.length+r_ring.length;
				        if( y == 0)
				        {		
			                mode.step_bp = 2;	
				        }
				        else
				        {
						    mode.step_bp = 0;
					        mode.bump = y;
				        }	
		                break;
		            case 2 :
			            if(!do_action(1,angle30))
			            {
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length+r_ring.length;  
			            mode.step_bp++;
		            case 3:
		                if(do_action(3,angle90))  //沿着墙边行走一定的距离
			            {
				            mode.step_bp++;	 
			                mode.long_wall +=  l_ring.length;
				            return 1;
			        	}
			            if(y)
			            {
			                mode.bump=y;
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
		            case 4:
		                if(do_action(6,angle360))	 //单轮右转角度的计算 ，兼顾右墙检，俗话[回头看看，调整自己]
			            {						  
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			            }
			            if(y) //为获得的新信息，做准备
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall += (int) l_ring.length/2;
				             if(mode.angle_wall>l_ring.length) //是否处在中性线
				        	 {								 
				                 mode.angle_wall-=l_ring.length;
					         }
				             else 
				        	 {					
				                 mode.angle_wall=0;
				        	 }
				             mode.step_bp=0;
			            }			    
		                break;
	                case 5:
		                if(do_action(3,angle90)) //修改时间2009.12.11
			            { 
						    if(mode.bump == 7)
							{
							    w_l.on = 0;
							}
							if(mode.bump == 8)
							{
							   w_lm.on = 0;
							} 
							if(mode.bump == 9)
							{
							   w_m.on = 0;
							}	
							if(mode.bump == 15)
							{
							   w_rm.on = 0;
							}
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
			            }
			            if((y == mode.bump))
			            {
				            return  1; 
			            }
			            stop_rap();
				        if( y == 0)
				        {		
			                mode.step_bp = 1;	
				        }
				        else
				        {
				            mode.step_bp = 0;
					        mode.bump = y;
				        }	
		                return 1;
					case 6:		
		                if(do_action(3,angle90)) //双腿向前行走
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2; //行走的距离
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }//结束switch(mode.step_bp)
	            return 1;
//---------------------------------------------
//第三情况碰撞,左地碰撞
            case  1://E_L
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(7,angle360)) //右轮不动向左转，半后退
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}
			            else if((y==1))
			        	     {
			        	        return 1; 
			        	     }
			            stop_rap();
			            mode.step_bp++;	 
			            mode.angle_wall+=l_ring.length;	//独轮行走角度计算
		                break;
		            case 1:
			            if(!do_action(7,angle30))
			            {			
			        	    return 1;
			        	}
			            mode.step_bp++;		 
			            mode.angle_wall+=l_ring.length;
		                break;	 
		            case 2:
			            if(!do_action(8,angle15))	//左轮不动向右转，右身后退
			            {			 
						    read_earth();
			        	    return 1;
			        	}			   
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;	 	//独轮行走角度计算
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }
			            mode.step_bp++;
		                break;
		            case 3 :
			            if(!do_action(1,angle60))	  //双轮 左转
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length+r_ring.length;
			            mode.step_bp++;
		            case 4:
		                if(do_action(3,angle90)) //沿边前进
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;	 //沿边前进的距离
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 5:
		                if(do_action(6,angle360))	  //左半身前进向右转动，目的靠近墙壁，查看右边墙检信号
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;	   //中性线
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 6:		
		                if(do_action(3,angle90))	//双腿向前行走
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2; //行走的距离
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//结束switch(mode.step_bp)
		        return 1; 
//----------------------------------------------------------------
//--第四情况碰撞,右地					  
        	case  4://E_R:	 
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(8,angle360))	//右半边抽身后退
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}
			            else if((y==4))
			        	     {
			        	        return 1; 
			        	     }
			            stop_rap();
			            mode.step_bp++;	 
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;	   //转动的角度
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }	 
		                break;
		            case 1:
			            if(!do_action(8,angle30))
			            {			
			        	    return 1;
			        	}
			            mode.step_bp++;	
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }	 
		                break;	 
		            case 2:
			            if(!do_action(7,angle15))  //左半边抽身后退
			            {			 
						    read_earth();
			        	    return 1;
			        	}			   
			            mode.step_bp++;			
			            mode.angle_wall+=l_ring.length;	  //角度
		                break;
		            case 3 :
			            if(!do_action(1,angle60))	 //全身左转
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length+r_ring.length;
			            mode.step_bp++;
		            case 4:
		                if(do_action(3,angle90))   //向前行走
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 5:
		                if(do_action(6,angle360))	 //左侧身向右旋转，探右边墙检
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 6:		
		                if(do_action(3,angle90))   //全身前进
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//结束switch(mode.step_bp)
		        return 1; 
//----------------------------------------------------------
//--第五情况碰撞					 
	        case  2://E_LM
	        case  3://E_RM:	
        	case  5://L_BUMP:
        	case  6://R_BUMP:
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(4,angle360))	  //全身后退
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}				
						read_sideearth() ;
			            if((y == mode.bump))   //no changing
			        	{
			        	     return 1; 
			        	}
			            stop_rap();
			            mode.step_bp++;	
		                break;
		            case 1:
			            if(!do_action(4,angle15))	  //全身后退
			            {
			        	    return 1;
			        	}		   
						read_sideearth() ;
			            mode.step_bp++;
		                break;
		            case 2 :
			            if(!do_action(1,angle60))	   //全身左转
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length+r_ring.length;
			            mode.step_bp++;
		            case 3:
		                if(do_action(3,angle90))	  //全身前进
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length; //以左轮行走为基准
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 4:
		                if(do_action(6,angle360)) //左半身前进向右转
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 5 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;	 //中心线定位
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 5:		
		                if(do_action(3,angle90))//全身前进
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;  //中心线定位
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//结束switch(mode.step_bp)
		        return 1; 
//----------------------------------------------------------------
//--第六情况碰撞,右墙碰撞
        	case  14://W_R:		
                switch (mode.step_bp)
				{
	                case 0:
					    if(do_action(1,angle180))  //全身左传
						{
						    mode.step_bp = 6;
						}
			            if((y != 14))
			            { 			   
							stop_rap();	
							mode.angle_wall += r_ring.length+l_ring.length;
				            if(y == 0)
				            {
								mode.step_bp = 1;
				            }
				            else
				            {
				                mode.bump = y;
				            }
			            }
			            return 1;
		            case 1:			  
	                case 4:	 
		                if(do_action(6,angle360))	//左半身前进向右转
			            {
			                mode.step_bp += 2;
			                mode.long_wall +=  l_ring.length/2;	 
							if( mode.angle_wall > l_ring.length)
							{
							     mode.angle_wall -= l_ring.length;
							}
							else
							{
							     mode.angle_wall = 0;
							}
			                return 1; 
			            }
			            if(y != 0)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length/2;
							if( mode.angle_wall > l_ring.length)   //中心线定位
							{
							     mode.angle_wall -= l_ring.length;
							}
							else
							{
							     mode.angle_wall = 0;
							}
							if(y == 14)
							{
							    mode.step_bp ++;
							}
							else
							{	  
				                mode.bump = y;
				                mode.step_bp = 0;
							}
			            }
			            return 1;
	                case 2:
		                if(do_action(3,angle60))   //全身前进
			            {
			                mode.step_bp = 0;
			                mode.long_wall +=  l_ring.length;
			                return 1; 
			            }
			            if(y != 14)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length;
							if(y == 0)
							{
							    mode.step_bp = 1;
							}
							else
							{	
				                mode.bump = y;
				                mode.step_bp = 0;
							}
			            }
			            return 1;
					case 3:	  
		                if(do_action(3,angle90))   //全身前进
			            {
			                mode.step_bp = 4;
			                mode.long_wall +=  l_ring.length;
			                return 1; 
			            }
			            if(y != 0)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length;
				            mode.bump = y;
				            mode.step_bp = 0;
			            }
			            return 1;
	                case 6:	 
					    //修改时间2009.12.11	 
						w_r.on = 0;
				        mode.bump = 0;
						mode.step_bp = 0;
				        mode.angle_wall = 0 ;
			            return 1;				 
		            default :
					     stop_rap();
		                 mode.step_bp=0;
	            }//结束	switch (mode.step_bp)
                return  1;   
            default :
	            mode.bump = 0;	   
        }//结束swicth(bump)			   
    } //结束if(mode.bump)
    return 0;
} 


