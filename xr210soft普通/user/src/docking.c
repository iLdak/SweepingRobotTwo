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
uint8 Docking_YaoKong_Manage(void) ;
//======================全局函数==================================
//======================私有函数==================================	
uint8 FindSeat(void);
uint16 ReadHwSign(void);
/******************************************************************
回充程序的初始化
******************************************************************/
void Init_Docking(void)
{
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
	Dis_Power = 3;	
	Dis_PowerGlint = TRUE;
	Dis_SpeedGo = TRUE; 
	/******初始化设置的值********************/
	mode.mode = DOCKING;
	clr_ram();
	Enable_earth();
	Enable_wall();
	enable_hwincept();           //允许红外接收电源
	Reset_Speed_Send();          //允许红外发送  
	gbv_action_s_motor = TRUE;
	Display_Real();
	Init_Action();
	WriteWorkState();
}

/******************************************************************
在清扫时的遥控器程序
******************************************************************/
uint8 Docking_YaoKong_Manage(void) 
{
    if(gsv_yaokong.sign == 1)     //有遥控器按键按下
	{
	    gsv_yaokong.sign = 0;
		switch(	gsv_yaokong.key)
		{
		    case YAOKONG_ESC:	   
			case YAOKONG_DOCKING_SUN:
			    Init_Cease();
				Enable_Beep();
			    return 1;
			case YAOKONG_SPOT_FRI:	
			    Init_Spot();
				Enable_Beep();
			    return 1; 		
			case YAOKONG_AUTO_THU:
			    Init_Sweep();	
				Enable_Beep();
			    return 1;
	        case YAOKONG_V_SAT:
		        Change_Speed();
				Enable_Beep();
		        return 0;
		        //按下uv键将uv指示取反
	        case YAOKONG_UV_WED:
				Init_Border();
		    	//Turn_UV();
				//Enable_Sweep();
				Enable_Beep();
				return 1;
		}
	}
	return 0;
}
/******************************************************************
回充程序
******************************************************************/
void Do_Docking(void)
{
uint32 m;	
uint16 s;		  
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

   //有充电的电源插入
  if((power.charge_dc == 1) || (power.charge_seat == 1))
  {
     if((mode.step != 3) || ((mode.step_mk != 4)&&(mode.step_mk != 5)))
	 {	
	     stop_rap();
		 DelayMs(200);
	     mode.step = 3;
		 mode.step_mk = 5;	//充电
		 mode.step_bp = 0;
	 }
  }

  //遥控器按键管理		     
  if(Docking_YaoKong_Manage() == 1)
  {
      return;
  }
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&m_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw);
  clr_hw_effect(&dummy_wall);  
  
  Io_Driver();
  //电池彻底没电，机器右没有找到回冲座，机器停止找，进入睡眠模式
  if((battery_voltage < NOPOWER)&&(mode.step != 3))
  {
     Init_Sleep() ;
	 return;
  }
  
  //增加如果机器有电时如果预约时间到，则前去扫地
  if(Check_PreengageTime())
  {
	   Enable_Beep();
	   Init_Sweep();
	   return;
  }
  //有遥控器时机器停止按照遥控方向运动
  if(YaoKongFangXiang())
  {
      return;
  }
  //是否有需要保护的内容
  if((mode.step != 3)||((mode.step_mk != 3)&&(mode.step_mk != 5)))
  {
      if(Action_Protect())
      {
          return;
      }
  }

  //在直走和沿边走时收到回充座信号
  if( mode.step != 3)
  {		
      s = ReadHwSign();		
      if((s&0x08) != 0)//不仅是顶上红外
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 0;//顶上区域
	  }		  	
      if((s&0x5555) != 0)  //0X4444+0X1111 不仅仅是中间和左边
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 1;//左边区域
	  }	
      if((s&0x2222) != 0)  //不仅仅是右边
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 2;//右边区域
	  }	
  }
  m = read_bump();
  switch(mode.step)
  {
      case 0:
	      if(do_action(3,METE_5))//METE_H5
		  {
		      mode.step = 1;
		  } 
		  if( m != 0)
		  {
		      stop_rap();
			  mode.angle_wall = angle720; //angle720
			  mode.step = 2;
			  mode.step_bp=0;
		  }	
		  Enable_wall();
		  return; 
	  case 1:
	       if(do_action(1,angle360))
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
		  return;
	  case 2:
	      if(ybs() == 0)
		  {
		      mode.step = 4;
		  }	 
		  return;
	  case 3:
	      if( FindSeat() == 0)
		  {
		      mode.step = 0;
		  }		
	 	 Enable_wall(); // Disable_wall();
	      return;
	  case 4:
	      if(do_action(1,angle120))//angle90
		  {
		      mode.step = 0;
		  }
		  Enable_wall();
		  return;
	  default :
	      mode.step = 0;	       
  }
}
/**********************************************
读取红外信号
**********************************************/
uint16 ReadHwSign(void)
{  //将接收到的红外数据进行解析
   //对象分为左、顶上、中间、右边，而后作运算，得出一个具体的处理对象
    uint16 s;
	    s = 0; 
	//-------------1
	    if((l_hw.effect == 1)&&((l_hw.data&0x0f) == 0))
		{
		    //红外信号是顶上信号
		    if(l_hw.data == 0x70)
			{		 
		        s |= 0x8000;
			}
				 //红外信号是左边信号
			else if((l_hw.data == 0xe0) || (l_hw.data == 0xa0)
			     || (l_hw.data == 0x60) || (l_hw.data == 0x20))
				 {
				     s |= 0x1000;
				 }
				 //红外信号是中间区域信号
				 else if((l_hw.data == 0xb0)|| (l_hw.data == 0x30)
			          || (l_hw.data == 0x80)|| (l_hw.data == 0x00)
					  || (l_hw.data == 0xc0)|| (l_hw.data == 0x40))
					  {
					      s |= 0x4000;
					  }
					  //红外信号是右边信号
					  else if((l_hw.data == 0xd0)|| (l_hw.data == 0x90)
					       || (l_hw.data == 0x50)|| (l_hw.data == 0x10))
					       {
					           s |= 0x2000;
					       }
		}
		//---------2
	    if((m_hw.effect == 1)&&((m_hw.data&0x0f) == 0))
		{
		    if(m_hw.data == 0x70)
			{		 
		        s |= 0x800	 ;
			}
			else if((m_hw.data == 0xe0) || (m_hw.data == 0xa0)
			     || (m_hw.data == 0x60) || (m_hw.data == 0x20))
				 {
				     s |= 0x100;
				 }
				 //红外信号是中间区域信号
				 else if((m_hw.data == 0xb0)|| (m_hw.data == 0x30)
			          || (m_hw.data == 0x80)|| (m_hw.data == 0x00)
					  || (m_hw.data == 0xc0)|| (m_hw.data == 0x40))
					  {
					      s |= 0x400;
					  }
					  //红外信号是右边信号
					  else if((m_hw.data == 0xd0)|| (m_hw.data == 0x90)
					       || (m_hw.data == 0x50)|| (m_hw.data == 0x10))
					       {
					           s |= 0x200;
					       }
		}
		//-------3
	    if((r_hw.effect == 1)&&((r_hw.data&0x0f) == 0))
		{
		    if(r_hw.data == 0x70)
			{		 
		        s |= 0x80;
			}
			else if((r_hw.data == 0xe0) || (r_hw.data == 0xa0)
			     || (r_hw.data == 0x60) || (r_hw.data == 0x20))
				 {
				     s |= 0x10;
				 }
				 //红外信号是中间区域信号
				 else if((r_hw.data == 0xb0)|| (r_hw.data == 0x30)
			          || (r_hw.data == 0x80)|| (r_hw.data == 0x00)
					  || (r_hw.data == 0xc0)|| (r_hw.data == 0x40))
					  {
					      s |= 0x40;
					  }
					  //红外信号是右边信号
					  else if((r_hw.data == 0xd0)|| (r_hw.data == 0x90)
					       || (r_hw.data == 0x50)|| (r_hw.data == 0x10))
					       {
					           s |= 0x20;
					       }
		}
	    //------4
	    if((up_hw.effect == 1)&&((up_hw.data&0x0f) == 0))
		{
		    if(up_hw.data == 0x70)
			{		 
		        s |= 0x08;
			}
			else if((up_hw.data == 0xe0) || (up_hw.data == 0xa0)
			     || (up_hw.data == 0x60) || (up_hw.data == 0x20))
				 {
				     s |= 0x01;
				 }
				 //红外信号是中间区域信号
				 else if((up_hw.data == 0xb0)|| (up_hw.data == 0x30)
			          || (up_hw.data == 0x80)|| (up_hw.data == 0x00)
					  || (up_hw.data == 0xc0)|| (up_hw.data == 0x40))
					  {
					      s |= 0x04;
					  }
					  //红外信号是右边信号
					  else if((up_hw.data == 0xd0)|| (up_hw.data == 0x90)
					       || (up_hw.data == 0x50)|| (up_hw.data == 0x10))
					       {
					           s |= 0x02;
					       }
		}
	return s;
}
/**********************************************
发现回充座信号后找回充座
**********************************************/
uint8 FindSeat(void)
 {
    uint32 m;  //碰撞信号
	uint16 s;//  红外信号
    static uint32 piv_stime = 0;
	static uint32 piv_length = 0;
	    if(mode.bump == 0)
		{
		    mode.bump = 9;
		}
	    m = read_bump();
		s = ReadHwSign();
	    if(( l_rap.rap > 750) || (r_rap.rap > 750) )
		{
	        l_rap.rap = l_rap.rap/2;
	        r_rap.rap = r_rap.rap/2;
	    }
		switch (mode.step_mk )
		{
//---------------------找到顶上的信号
		    case 0:
			    if(((s & 0x7777) != 0)&&(mode.step_bp < 7))
				{
				    stop_rap();
					if((s & 0x1111) != 0)    //找到左边信号
					{
					    mode.step_bp = 7;
					}	 
					if((s & 0x6666) != 0)	//找到右边信号
					{
					    mode.step_bp = 8;
					}
			
					return 1;
				}
			    switch(mode.step_bp)
				{
				    case 0:
					    do_action(3,angle720);
						if((m != 0)&&(m < 7))     //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;	  //碰撞信号的摄取
							break ;
						} 
					    if(((s & 0x0f) == 0)&&(l_ring.length > angle60)) //不是顶上红外
						{
						    stop_rap();
							mode.step_bp = 1;
						}
						break;
					case 1:
					    if(do_action(1,angle360+angle30))
						{
						    mode.step_bp = 2;
						}
						if((m != 0)&&(m < 7))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
						if((s & 0x0f) != 0) 
						{
						    stop_rap();
							mode.step_bp = 5;
						}
						break;
					case 2:
					    if(do_action(3,angle90))	//开始时旋转360度，希望用左边或者右边接收头收到信号
						{
							mode.step_bp = 1;
						}
						if((m != 0)&&(m < 7))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
					    if(((s & 0x0f) != 0))
						{
						    stop_rap();
							mode.step_bp = 5;
						}
						break;
					case 3:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							return 1;
						}
						if((m == 0)||(m > 6))
						{
						    stop_rap();
							mode.step_bp = 4;
						}
						break;
				    case 4:
					    if(do_action(1,angle30))
						{
						    mode.step_bp = 2;
						}
						break;
						
					//向右转动希望找到信号
					case 5:					  
					    if(do_action(3,angle150))
						{
						    mode.step_bp = 6;
						}
						if((m != 0)&&(m < 7))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
					    if(((s & 0x0f) == 0))
						{
						    stop_rap();
							mode.step_bp = 1;
						}
						break;
						
					//向左转动360度，希望中间接收头收到信号
					case 6:		   			   //向左转动
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 5;
						}	 
						break;
					case 7:
					    if(do_action(3,angle75))
						{
					        mode.step_bp = 13;
						    mode.step_mk = 1;
						}
						break;
					case 8:
					    if(do_action(1,angle90))
						{
					        mode.step_bp = 9;
						}
						break;
					case 9:
					    if(do_action(3,angle90))
						{
					        mode.step_bp = 12;
						}
						if((m != 0)&&(m < 7))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 10;
						} 
						break;
					case 10:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							return 1;
						}
						if((m == 0)||(m > 6))
						{
						    stop_rap();
							mode.step_bp = 11;
						}
						break;
					case 11:
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 9;
						}
						return 1;
					case 12:
					    if(do_action(1,angle330))
						{
						    mode.step_bp = 9;
						}
						if((s & 0x0600) != 0)
						{
						    stop_rap();
							mode.step_bp = 12;
							mode.step_mk = 2;
						}
						break;
					default :
					    mode.step_bp = 0;
				}
				return 1;	
		    //找到左边的信号				    
			case 1:
			    if( ((up_hw.effect != 0) && ((up_hw.data & 0x80) == 0 ))
				   && (mode.step_bp < 12))	 //中间接收头收到顶上的信号
				{
				    stop_rap();
					mode.step_bp = 12;
				}
				if(((s&0x0200) != 0) ) 
				{
				    if((mode.step_bp < 12))  //收到右边或者中间信号
					{			 
					    mode.step_mk = 2;	
					}
					else
					{
					    if(mode.step_bp <17)
						{
						    mode.step_bp = 17;
						}
					}	
				}
			    switch(mode.step_bp)
			    {
			        //向左旋转希望中间的接收头能收到左边信号
			        case 0:
				        if(do_action(2,angle360))
					    {
					        mode.step_bp = 1; //转完一圈后，中间没有收到信号
					    }
					    if(m_hw.effect != 0) //中间接收头收到信号，机器下一步向前进
					    {                     
							DelayMs(100);
					        stop_rap();
							DelayMs(200);
						    mode.step_bp = 9;
					    }
						if((m != 0)&&((m <7)))  //有碰撞信号、第一次
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    return 1;

				    //转完一圈后前接收头不能收到信号，继续向前进1米
				    case 1:
				        if(do_action(3,METE))
						{		 
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
						if((m_hw.effect == 0)&&(up_hw.effect == 0)&&(l_ring.length > angle60)) //接收头收不到信号
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break;
				   	//没有收到信号掉头
					case 2:
					    if(do_action(6,angle720+angle120))
						{
							mode.step = 0;
							mode.step_mk = 0;
						}
					    if((m_hw.effect != 0)||(up_hw.effect != 0)) //接收头收到信号
					    {
					        stop_rap();
						    mode.step_bp = 0;
					    }
						if((m != 0)&&((m <7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step = 0;
							mode.step_mk = 0;
						}
						break;
					//机器碰到障碍后退
					case 3:	
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 4;
					    }
						break;
                    //机器向右转90度
				    case 4:
					    if(do_action(2,angle90))
						{
						    mode.step_bp = 5;
						}
						break;
					//机器前进15厘米
					case 5:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //有碰撞信号，可能是右边的障碍物。
						{
						    stop_rap();
							mode.step_bp = 6;
						}
						if((m_hw.effect == 0)&&(up_hw.effect == 0)) //接收头收不到信号
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break ;
					//机器后退到摆脱障碍
					case 6:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 7;
					    }
						break;
					//机器向左转动30度
					case 7:
					    if(do_action(1,angle30))
						{
						    mode.step_bp = 5;
						} 
						break;
					//机器向前进两米
					case 9:        
				        do_action(3,METE_TWO);	 
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if((m_hw.effect == 0)&&(l_ring.length > angle90))
						{
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 10;
						}
				   	    break;
						
					//前面的接收头没有收到信号，机器向左转动希望找到信号
				    case 10:
					    if(do_action(6,angle60))
						{
						    mode.step_bp = 11;
						}
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(m_hw.effect != 0)
						{   
							DelayMs(200);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//前面的接收头没有收到信号，机器向右转动希望找到信号
					case 11:
					    if(do_action(1,angle360))
						{
						    mode.step_bp = 1;
						}
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(m_hw.effect != 0)
						{
							DelayMs(100);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//机器收到顶上信号，然后向右转120度，直到左边接收头收到信号
					case 12:	
				        if(do_action(2,angle120))
						{
							mode.step_bp = 13;
						}
						if(( s&0xf000) != 0)  
						{
						    stop_rap();
							mode.step_bp = 13;
						}
						if((s&0x0600) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
				   	    break;
					
					//机器前进希望找到中间或者右边信号
					case 13:
					    if(do_action(3,angle75))
						{
						    if((s & 0x0001) == 0)
						    {
						        stop_rap();
							    mode.step_bp = 16;
								break;
						    }
						}
					    if((m != 0)&&(m < 7))
						{
						    stop_rap();
						    mode.step_bp = 14;
						}
						if((s & 0x0606) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
						break;	
					
					//机器找到了中间或者右边信号，机器再向前走15厘米
					case 14:   
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 15;
					    }
						break;
					case 15	:
					    if(do_action(1,angle30))
						{
						    mode.step_bp = 13;
						}
						break;
					case 16:
					    if(do_action(1,angle360))
						{
						    mode.step_bp = 0;
							mode.step = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0100) != 0)
						{
						    stop_rap();
							mode.step_bp = 12;
						}
						if((s & 0x0606) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
						break;
					case 17:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 19;
						}
						if((m != 0)&&(m<7))
						{
						    stop_rap();
							mode.step_bp = 18;
						}
						break;
					case 18:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 19;
					    }
						break;	
					//机器找到中间区域，向左转动，希望前面接收头能收到中间信号
					case 19:
					    if(do_action(1,angle360))
						{
						    mode.step = 0;
							mode.step_bp = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0500) != 0)
						{
						    stop_rap();
							mode.step_bp = 0;
							mode.step_mk = 3;
						}
						break;
					default :
					    mode.step_bp = 0;
			    }
			    return 1; 
		    //找到右边的信号				    
			case 2:
			    if(((up_hw.effect != 0) && ((up_hw.data & 0x80) == 0 ))
				   && (mode.step_bp < 12))	 //中间接收头收到顶上的信号
				{
				    stop_rap();
					mode.step_bp = 12;
				}
				if(((s&0x0100) != 0) )
				{
				    if((mode.step_bp < 12))  //收到右边或者中间信号
					{	   
					    mode.step_mk = 1;	
					}
					else
					{
					    if(mode.step_bp <17)
						{
						    mode.step_bp = 17;
						}
					}	
				}
			    switch(mode.step_bp)
			    {
			        //向左旋转希望中间的接收头能收到左边信号
			        case 0:
				        if(do_action(1,angle360))
					    {
					        mode.step_bp = 1; //转完一圈后，中间没有收到信号
					    }
					    if(m_hw.effect != 0) //中间接收头收到信号，机器下一步向前进
					    {                     
							DelayMs(100);
					        stop_rap();
							DelayMs(200);
						    mode.step_bp = 9;
					    }
						if((m != 0)&&((m <7)))  //有碰撞信号、第一次
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    return 1;

				    //转完一圈后前接收头不能收到信号，继续向前进1米
				    case 1:
				        if(do_action(3,METE))
						{		 
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
						if((m_hw.effect == 0)&&(up_hw.effect == 0)&&(l_ring.length > angle60)) //接收头收不到信号
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break;
				   	//没有收到信号掉头
					case 2:
					    if(do_action(5,angle720+angle120))
						{
							mode.step = 0;
							mode.step_mk = 0;  //信号区域
						}
					    if((m_hw.effect != 0)||(up_hw.effect != 0)) //接收头收到信号
					    {
					        stop_rap();
						    mode.step_bp = 0;
					    }
						if((m != 0)&&((m <7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step = 0;
							mode.step_mk = 0;
						}
						break;
					//机器碰到障碍后退
					case 3:	
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 4;
					    }
						break;
                    //机器向右转90度
				    case 4:
					    if(do_action(1,angle90))
						{
						    mode.step_bp = 5;
						}
						break;
					//机器前进15厘米
					case 5:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //有碰撞信号，可能是右边的障碍物。
						{
						    stop_rap();
							mode.step_bp = 6;
						}
						if((m_hw.effect == 0)&&(up_hw.effect == 0)) //接收头收不到信号
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break ;
					//机器后退到摆脱障碍
					case 6:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 7;
					    }
						break;
					//机器向左转动30度
					case 7:
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 5;
						} 
						break;
					//机器向前进两米
					case 9:        
				        do_action(3,METE_TWO);	 
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if((m_hw.effect == 0)&&(l_ring.length > angle90))
						{
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 10;
						}
				   	    break;
						
					//前面的接收头没有收到信号，机器向左转动希望找到信号
				    case 10:
					    if(do_action(5,angle60))
						{
						    mode.step_bp = 11;
						}
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(m_hw.effect != 0)
						{   
							DelayMs(200);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//前面的接收头没有收到信号，机器向右转动希望找到信号
					case 11:
					    if(do_action(2,angle360))
						{
						    mode.step_bp = 1;
						}
						if((m != 0)&&((m < 7)))  //有碰撞信号
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(m_hw.effect != 0)
						{
							DelayMs(100);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//机器收到顶上信号，然后向右转120度，直到左边接收头收到信号
					case 12:	
				        if(do_action(1,angle120))
						{
							mode.step_bp = 13;
						}
						if(( s&0x00f0) != 0)  
						{
						    stop_rap();
							mode.step_bp = 13;
						}			
						if((s&0x0500) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
				   	    break;
					
					//机器前进希望找到中间或者右边信号
					case 13:
					    if(do_action(3,angle75))
						{	
						    if((s & 0x0002) == 0)
						    {
						        stop_rap();
							    mode.step_bp = 16;
						    }
						}
					    if((m != 0)&&(m < 7))
						{
						    stop_rap();
						    mode.step_bp = 14;
						}
						if((s & 0x0505) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
						break;	
					
					//机器找到了中间或者右边信号，机器再向前走15厘米
					case 14:   
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 15;
					    }
						break;
					case 15	:
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 13;
						}
						break;
					case 16:
					    if(do_action(2,angle360))
						{
						    mode.step_bp = 0;
							mode.step = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0200) != 0)
						{
						    stop_rap();
							mode.step_bp = 12;
						}
						if((s & 0x0505) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
						break;
					case 17:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 19;
						}
						if((m != 0)&&(m<7))
						{
						    stop_rap();
							mode.step_bp = 18;
						}
						break;
					case 18:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //没有碰撞信号
					    {
					        stop_rap();
						    mode.step_bp = 19;
					    }
						break;	
					//机器找到中间区域，向左转动，希望前面接收头能收到中间信号
					case 19:
					    if(do_action(2,angle360))
						{
						    mode.step = 0;
							mode.step_bp = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0600) != 0)
						{
						    stop_rap();
							mode.step_bp = 0;
							mode.step_mk = 3;
						}
						break;
					default :
					    mode.step_bp = 0;
			    }
			    return 1; 

			//找到了中间的信号
			case 3:	
				if((l_bump.key != 0)&&(r_bump.key != 10)&&(mode.step_bp != 10))  //有碰撞信号
				{
					stop_rap();
					piv_stime = giv_sys_time;
					mode.step_bp = 10;
				}
	            if(( l_rap.rap > 500) || (r_rap.rap > 500) )
		        {
	                l_rap.rap = l_rap.rap/2;
	                r_rap.rap = r_rap.rap/2;
		        }
			   	switch (mode.step_bp)
				{
				    //顶上接收头收到右边信号，向左转360度希望能找到信号
				    case 0:
					    do_action(3,METE);
						if((s & 0x0400) != 0)  //中间接收头收到中间信号
						{
						     stop_rap();
							 DelayMs(200);
							 mode.step_bp = 6;
						}
						else
						{
						    stop_rap();
							DelayMs(200);
						    if((s & 0x0200) != 0) //中间接收头收到左边信号
						    {
							    mode.step_bp = 4;
						    }
							else
							{
							    mode.step_bp = 1;
							}
						}	 
					    break;
				    //顶上接收头找到左边信号向右转
					case 1:
					    if(do_action(2,angle90))
						{
						   mode.step_bp = 11;
						}
						else
						{
						    if((s & 0x0200) != 0) //收到左边信号
						    {
						        stop_rap();
							    DelayMs(200);
								if(	l_ring.length > angle30)
								{
								    piv_length = l_ring.length;
							        mode.step_bp = 2;
								}
								else
								{
								    enable_rap(FRONT, angle20,FRONT,angle60);
								    mode.step_bp = 7;
								}
						    }
						}
						break;	
					//中间接收头找到了中间信号，机器向前前进	
				    case 2:
					    if(do_action(1,(piv_length/2)))
						{
						   mode.step_bp = 3;
						}
						break;	
					//中间接收头收到右边信号，机器向左转，希望找到中间信号
				    case 3:		   
					    if(do_action(3,angle30))
						{
						    mode.step_bp = 0;
						}
						if((s & 0x0100) != 0)
						{
						    stop_rap();
							enable_rap(FRONT, angle60,FRONT,angle20);
						    mode.step_bp = 8;
						} 
						if((s & 0x0200) != 0)
						{
						    stop_rap();
							enable_rap(FRONT, angle20,FRONT,angle60);
						    mode.step_bp = 7;
						}
					    break;
				    case 4:
					    if(do_action(1,angle90))
						{
						   mode.step_bp = 12;
						}
						else
						{
						    if((s & 0x0100) != 0) //收到左边信号
						    {
						        stop_rap();
							    DelayMs(200);		
								if(	l_ring.length > angle30)
								{
								    piv_length = l_ring.length;
							        mode.step_bp = 5;
								}
								else
								{						  
								    enable_rap(FRONT, angle60,FRONT,angle20);
								    mode.step_bp = 8;
								}
						    }
						}
						break;
					//中间接收头收到左边信号，机器向右转，希望找到中间信号	  
					case 5:		
					    if(do_action(2,(piv_length/2)))
						{
						   mode.step_bp = 3;
						}
						break;	
					case 6:
					    if(do_action(2,angle90))
						{
						   mode.step_bp = 11;
						}
						if((s & 0x0200) != 0) //收到左边信号
						{
						    stop_rap();
						    DelayMs(200);
							mode.step_bp = 4;
						}
						break;
					
					case 7:
					    if((l_rap.sign == 0)&&(l_rap.sign == 0))
						{			  
						   mode.step_bp = 0;
						}	 
						if((s & 0x0400) != 0)  //收到中间信号
						{			 
						     stop_rap();
							 mode.step_bp = 9;
							
						}
						else
						{		
						    if((s & 0x0100) != 0) //收到左边信号
						    {					   
						        stop_rap();
							    enable_rap(FRONT, angle60,FRONT,angle20);
							    mode.step_bp = 8;
						    }
						}
						break;
					case 8:	
					    if((l_rap.sign == 0)&&(l_rap.sign == 0))
						{			  
						   mode.step_bp = 0;
						}	 
						if((s & 0x0400) != 0)  //收到中间信号
						{			 
						     stop_rap();
							 mode.step_bp = 9;
							
						}
						else
						{		
						    if((s & 0x0200) != 0) //收到右边信号
						    {				   
						        stop_rap();
							    enable_rap(FRONT, angle20,FRONT,angle60);
							    mode.step_bp = 7;
						    }
						}
						break;				  
					case 9:
					    if((l_rap.sign == 0)&&(l_rap.sign == 0))
						{
						    enable_rap(FRONT, angle270,FRONT,angle270);
						}
						if((s & 0x0200) != 0) //收到右边信号
						{				   
						    stop_rap();
							enable_rap(FRONT, angle20,FRONT,angle60);
							mode.step_bp = 7;
						}  	
						if((s & 0x0100) != 0) //收到左边信号
						{					   
						    stop_rap();
							enable_rap(FRONT, angle60,FRONT,angle20);
							mode.step_bp = 8;
						}
					    break;
					case 10:
					//在发现碰撞信号时停下等待2秒	
					    if((giv_sys_time - piv_stime) > 10000)
						{
						    mode.step_mk = 4;	 //等待
							mode.step_bp = 0;
						}
						break;
					case 11:
					    if(do_action(1,angle90))
						{
						    mode.step_bp = 13;
						}
						if((s & 0x0400) != 0) //中间接收头收到中间信号
						{
						    stop_rap();
							mode.step_bp = 9;
						}
						break; 
					case 12:
					    if(do_action(2,angle90))
						{
						    mode.step_bp = 13;
						}
						if((s & 0x0400) != 0) //中间接收头收到中间信号
						{
						    stop_rap();
							mode.step_bp = 9;
						}
						break;
					case 13:
					    if(do_action(3,angle75))
						{
						    if((s & 0x0100) != 0)
							{
							    mode.step_mk = 1;
								mode.step_bp = 12;
							}	
						    if((s & 0x0200) != 0)
							{
							    mode.step_mk = 2;
								mode.step_bp = 12;
							}
							return 1;
						}
						if((s & 0x0400) != 0) //中间接收头收到中间信号
						{
						    stop_rap();
							mode.step_bp = 9;
						}
						break;
					default :
					    mode.step_bp = 0;
						stop_rap();
				}
				return 1;   //结束收到中间区域的程序
			//机器在中间区域没有对准电极时掉头往回走
			case 4:
			    switch(mode.step_bp)
				{
				    case 0:	
					    if(do_action(4,angle45))
						{
						    mode.step_bp = 1;
						}
						break;
					case 1:
					    if(do_action(1,angle180))
						{
						   mode.step_bp = 2;
						}
						break;
					case 2:
					    if(do_action(3,METE_H5))
						{
						   mode.step_bp = 4;
						}
						if((m != 0)&&(m < 7))
						{
						    stop_rap();
							mode.step_bp = 3;
						}
						break;
					case 3:
					    if(do_action(4,angle45))
						{
						    mode.step_bp = 4;
						}
						if((m == 0)||(m > 6))
						{
						    stop_rap();
							mode.step_bp = 4;
						}
						break;
					case 4:
					    if(do_action(2,angle180))
						{
						   mode.step_bp = 0;
						   mode.step = 0;
						   mode.step_mk = 0;
						}
						break;
					default :
					    mode.step_bp = 0;
						stop_rap();

				}
				return 1;
			case 5:
			    if(do_action(3,120))
				{
	                    if((power.charge_dc == 1) || (power.charge_seat == 1))
	                    {
	                        Init_Chargeing();
	                        return 1;
	                    } 
	                    else
	                    {
		                    mode.step_mk = 4;
	                    }
				}
	            if(( l_rap.rap > 200) || (r_rap.rap > 200) )
		        {
	                l_rap.rap = 200;
	                r_rap.rap = 200;
		        } 
//					  
				{
				    if((l_ring.length > 30)||(r_ring.length > 30)||((giv_sys_time-action.time) > 500))
					{
				        stop_rap();
	                    if((power.charge_dc == 1) || (power.charge_seat == 1))
	                    {
	                        Init_Chargeing();
	                        return 1;
	                    } 
	                    else
	                    {
		                    mode.step_mk = 4;
	                    }
					}
				}
				return 1;
		}
 return 1;
}
