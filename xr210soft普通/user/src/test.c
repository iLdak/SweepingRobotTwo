#include "test.h"													  
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
//======================私有定义==================================
#define  VER   350	//109
//======================全局变量==================================
//======================私有变量==================================
//======================全局函数==================================
void frontandback(void) ;
//======================私有函数==================================
void Test_Earth(uint8 state);
void Test_Wall(uint8 state);
void Test_HwIncept(uint8 state);
void Test_Current(uint8 state);
void Test_Action(uint8 state);
void Test_KeyAndYaoKong(uint8 state);
void Test_Display(uint8 state);
void Test_MainBoard(uint8 state);
//=======================函数实体===================================
/******************************************************************
自检整个机器
******************************************************************/
void Do_Test(void)
{
uint32 piv_time;
bool pbv_test;                   //是否进入自检
uint8 piv_test_state;            //自检的内容
uint8 piv_test_item;             //自检的项目 0 检测地检， 1墙检 ，2红外接收 ，3开关量
                                 //4 电压电流， 5动作 ，6 显示、版本系统信息
    piv_time = giv_sys_time;   
    Dis_On = TRUE;              //显示标志。
    Dis_ErrOn = TRUE ;    	    //错误图标亮
    Dis_UvOn = TRUE ;			//uv图标亮
    Dis_PrOn = TRUE ;			//预约天数亮
    Dis_AuOn = TRUE ;			//自动清扫亮
    Dis_SpOn = TRUE ;			//局部清扫亮
    Dis_HourOn = TRUE ;		    //小时亮
    Dis_MinuteOn = TRUE ;		//分钟亮
    Dis_ColOn = TRUE ;		    //时间点亮
    Dis_WeekOn = TRUE ;		    //星期亮
    Dis_SpeedOn = TRUE ;		//速度亮
    Dis_PowerOn = TRUE ;		//电池图标亮
    Dis_ChargeOn = TRUE ;		//充电图标亮
    Dis_ErrGlint = TRUE ;		//错误图标闪烁
    Dis_UvGlint = TRUE ;		//uv图标闪烁
    Dis_PrGlint = TRUE ;		//预约天数闪烁
    Dis_AuGlint = TRUE ;		//自动清扫闪烁
    Dis_SpGlint = TRUE ;		//局部清扫闪烁
    Dis_HourGlint = TRUE ;	    //小时闪烁
    Dis_MinuteGlint = TRUE ;	//分钟闪烁
    Dis_ColGlint = TRUE ;		//时间点闪烁
    Dis_WeekGlint = TRUE ;	    //星期闪烁
    Dis_SpeedGlint = TRUE ;	    //速度闪烁
    Dis_PowerGlint = TRUE ;	    //电池图标闪烁
    Dis_ChargeGlint = TRUE ;	//充电图标闪烁
    Dis_Preengage = 0xff;	    //预约内容
    Dis_Hour = 0; 		        //显示的小时
    Dis_Minute = 0;  		    //显示的分钟
    Dis_Week = 0;			    //显示的星期内容  范围 1——7
    Dis_Speed = 1;		        //显示的速度，范围1——3
    Dis_Power = 3;		        //显示的电源电量 ，范围0——3	
	Enable_earth();
	Enable_wall();
	enable_hwincept();          //允许红外接收电源
	Reset_Speed_Send();         //允许红外发送  
	pbv_test = FALSE;
	piv_test_state = 0;
	piv_test_item = 0;
	Enable_Beep(); 
	do
	{	  
	    Beep_Driver();          //驱动蜂鸣器响
	    AutoReadKey();          //自动读取按键
	    AutoDisplay();          //自动显示
		IWDG_ReloadCounter();
		if((key3.sign == 0)  && (key1.sign == 0))  //修改进入测试按键2011/10/29
        { 
            key1.sign = 1;
	        key2.sign = 1;
	        key3.sign = 1;
	        pbv_test = TRUE;	 
	        Enable_Beep(); 
			clr_display();
			ReadBakBatteryActivation();
			giv_BatteryActivation=0;
            WriteBakBatteryActivation();
			ReadBakBatteryActivation();
			Dis_On = TRUE;        
			piv_test_state = 0;
			giv_sys_err = 0;
		    break;
        }
	}while(giv_sys_time - piv_time < 30000)  ;
	
	while(pbv_test == TRUE)
	{
	    Beep_Driver();            //驱动蜂鸣器响
	    AutoReadKey();            //自动读取按键
	    AutoDisplay();            //自动显示 
        judge_charge();           //自动判断是否有充电信号
        sampling_temp_voltage();  //自动采样马达电流	  
	    A7105_rx();	              //自动接收遥控器信号
	    read_wallearth();		  //自动采样墙检和地检数据
		IWDG_ReloadCounter();
        clr_hw_effect(&l_hw);
        clr_hw_effect(&m_hw);
        clr_hw_effect(&r_hw);	
        clr_hw_effect(&up_hw); 	 
        clr_hw_effect(&dummy_wall); 

        chargeing();
		//按下
		if(key3.sign == 0)
		{			
		    key3.sign = 1;
		    piv_test_state++;
			giv_sys_err = 0;
			if (((piv_test_state == 4)&&(piv_test_item == 0))
			 || ((piv_test_state == 4)&&(piv_test_item == 1))
			 || ((piv_test_state == 5)&&(piv_test_item == 2))
			 || ((piv_test_state == 4)&&(piv_test_item == 3))
			 || ((piv_test_state == 2)&&(piv_test_item == 4))
			 || ((piv_test_state == 8)&&(piv_test_item == 5))
			 || ((piv_test_state == 6)&&(piv_test_item == 6))
			 || ((piv_test_state == 3)&&(piv_test_item == 7)))
			{
			    piv_test_state = 0;
			}  	 
	        Enable_Beep(); 
			clr_display();
			Dis_On = TRUE;       
			clr_ram();
		}
		if((key2.sign == 0))
		{	
	        key2.sign = 1;
		    piv_test_item++;   
			giv_sys_err = 0;
			if(piv_test_item == 8)
			{
			    piv_test_item = 0;
			}
	        Enable_Beep(); 
			clr_display();
			Dis_On = TRUE;  
			piv_test_state = 0;
			clr_ram();	
		}
		if((key1.sign == 0))
		{
	        key1.sign = 1;	  
	        Enable_Beep(); 
		    pbv_test = FALSE;
			return;
		}
		if(giv_sys_err != 0)
		{					
            /******初始化显示***********/
            clr_display();	
	        /******初始化设置的值********************/
	        //clr_ram();
            Dis_On = TRUE;              //显示标志。
	
            Dis_HourOn = TRUE; 		    //小时亮
            Dis_MinuteOn = TRUE; 		//分钟亮 
	        Dis_ErrOn = TRUE;
	        Dis_ErrGlint = TRUE;
	        Dis_Hour = giv_sys_err / 100 + 140;
	        Dis_Minute = giv_sys_err % 100;
		}
		else
		{
		    switch(piv_test_item)
		    {    	
		        //测试mainboard的数据
		        case 0:
				    giv_sys_err = ReadBakErr();
				    if(giv_sys_err)
					break;
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 0;
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE;   
				    Dis_Hour = piv_test_state*10;
				    Dis_Minute = 0;  
				    Test_MainBoard(piv_test_state);
				    break;
		        //测试地检的数据
		        case 1:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 0;
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_Earth(piv_test_state);
				    break;
				
			    //测试墙检的数据
			    case 2:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 1;
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_Wall(piv_test_state);
				    break;
				
			    //检测红外接收数据
		 	    case 3:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 2;
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_HwIncept(piv_test_state);
				    break;
						   
			    //检测开关信号
			    case 4:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 3;	  
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
                    Test_KeyAndYaoKong(piv_test_state);
				    break;
			    //检测各个部分的电流电压
			    case 5:	  
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 4;	
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_Current(piv_test_state);
			        break;
			    //测试各种动作、显示左右轮速度
			    case 6:		
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 5;	
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_Action(piv_test_state);
			        break;
			    case 7:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 6;	
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE;
				    Test_Display(piv_test_state);
			        break;
			    default :
			        piv_test_item = 0;
			
		    }
		}
	} 
}
/******************************************************************
测试遥控器和开关量
******************************************************************/	 
void Test_Display(uint8 state)
{
uint8 i;//循环计数变量
uint8 m;
        switch(state)
		{
		    case 0:	
			    Clr_DisplayData();	  
                lcdkey_bgon();     //打开背光
	            lcdkey_dison();    //打开显示
				for(m=0; m<10;m++)
				{
					Dis_Data[m] = (0xff);
				}			  	
		        lcdkey_disdata(12, Dis_Data, 10); 
				while((key1.sign == 1)&&(key2.sign == 1)&&(key3.sign == 1))
				{					 
					IWDG_ReloadCounter();	
				    AutoReadKey();
				}
			    break;
			case 1:
			    Clr_DisplayData();
				i = 0;
                lcdkey_bgon();     //打开背光
	            lcdkey_dison();    //打开显示
			    while((key1.sign == 1)&&(key2.sign == 1)&&(key3.sign == 1))
				{
			        if(Sec == TRUE)
	                {
	                    Sec = FALSE;
						i++;
						Clr_DisplayData();
						//修改自检的显示
						i = i%24;
						if(i<4)
						{
						    for(m=0; m<10;m++)
							{
							    Dis_Data[m] = (0x11<<(i));
							}
						}
						else
						{
						    if((i%2)==0)
							{	  
						        Dis_Data[(i-4)/2] = 0x0f;
							}
							else
							{						  
						        Dis_Data[(i-4)/2] = 0xf0;
							}
						}
		                lcdkey_disdata(12, Dis_Data, 10); 
					}	
					IWDG_ReloadCounter();
				    AutoReadKey();
				}
			    break;
			case 2:		   
				Dis_Hour =VER/100;// 10+VER/100; 
				Dis_Minute = VER%100;
			    break;
		}
}	   
/******************************************************************
测试遥控器和开关量
******************************************************************/	 
void Test_KeyAndYaoKong(uint8 state)
{
        switch(state)
	    {
		    //测试开关量
	        case 0:		  
				//左轮离地开关	 
				Dis_Hour = 10; 
				Dis_Minute = 0; 
				if(l_lidi.key == 1)
				{
				    Dis_ErrOn = FALSE;
				}
				else 
				{
				    Dis_ErrOn = TRUE;
				}	 
				//右轮离地开关  
				if(r_lidi.key == 1)
				{
				    Dis_SpOn = FALSE;
				}
				else 
				{
				    Dis_SpOn = TRUE;
				}
				//左边碰撞开关
				if(l_bump.key == 0)
				{
				    Dis_UvOn = FALSE;
				}
				else 
				{
				    Dis_UvOn = TRUE;
				}
				//右边碰撞开关
				if(r_bump.key == 0)
				{
				    Dis_AuOn = FALSE;
				}
				else 
				{
				    Dis_AuOn = TRUE;
				}

			    break;
			//测试遥控器
			case 1:		   
				Dis_Hour = 20; 
				if(gsv_yaokong.effect == 1)
				{
				    Dis_Minute = gsv_yaokong.key;
				}
				else
				{
					Dis_MinuteOn = FALSE;
				}
				 
			    break;
	    }
}
/******************************************************************
测试几个动作
******************************************************************/
void Test_Action(uint8 state)
{
static uint32 piv_length = 0;
        switch(state)
		{
		    //向前显示左轮速度
		    case 0:		  
			    comm_rap();
				do_action(3,0xffffff);
			    if(Sec == TRUE)
	            {
	                Sec = FALSE;
					Dis_Hour = (uint8)((l_ring.length - piv_length)/1000 + 10);
					Dis_Minute = (uint8)(((l_ring.length - piv_length)%1000)/10);
					piv_length = l_ring.length ;
				}
			    break;
			//向前显示右轮速度
			case 1:	
			    comm_rap();
				do_action(3,0xffffff);
			    if(Sec == TRUE)
	            {
	                Sec = FALSE;
					Dis_Hour = (uint8)((r_ring.length - piv_length)/1000 + 20);
					Dis_Minute = (uint8)(((r_ring.length - piv_length)%1000)/10);
					piv_length = r_ring.length ;
				}	
			    break;
			//向后显示左轮速度
			case 2:			  
			    comm_rap();
				do_action(4,0xffffff);
			    if(Sec == TRUE)
	            {
	                Sec = FALSE;
					Dis_Hour = (uint8)((l_ring.length - piv_length)/1000 + 30);
					Dis_Minute = (uint8)(((l_ring.length - piv_length)%1000)/10);
					piv_length = l_ring.length ;
				}
			    break;
			//向后显示右轮速度
			case 3:	   	   
			    comm_rap();
				do_action(4,0xffffff);
			    if(Sec == TRUE)
	            {
	                Sec = FALSE;
					Dis_Hour = (uint8)((r_ring.length - piv_length)/1000 + 40);
					Dis_Minute = (uint8)(((r_ring.length - piv_length)%1000)/10);
					piv_length = r_ring.length ;
				}
			    break;
			//交替打开关闭uv灯。
			case 4:
				Io_Driver();
				Dis_Hour = 50;
				Dis_Minute = 0;
				if(Sec == TRUE)
				{			
	                Sec = FALSE;
					if(gbv_action_uv == TRUE)
					{
					   gbv_action_uv = FALSE; 
					   Dis_UvOn = FALSE;
					}
					else
					{
					   gbv_action_uv = TRUE; 
					   Dis_UvOn = TRUE;
					}
				}
			    break; 
		    //老化机器
			case 5:	   
				Io_Driver();  
			    comm_rap();	
				Dis_Hour = 60;
				Dis_Minute = 0;
				gbv_action_uv = TRUE;  
				gbv_action_m_motor = TRUE;	
				gbv_action_s_motor = TRUE;
				gbv_action_dust_motor = TRUE;
				frontandback();
				//----------2011/10/29
			   if(key1.key == 0)//测试增加
				{
				    Dis_SpOn =  TRUE;
				}
				else 
				{
				    Dis_SpOn = FALSE;
				}
			    if(key2.key == 0)//测试增加
				{
				    Dis_PrOn =  TRUE;
				}
				else 
				{
				    Dis_PrOn = FALSE;
				}
			    if(key3.key == 0)//测试增加
				{
				    Dis_UvOn =  TRUE;
				}
				else 
				{
				    Dis_UvOn = FALSE;
				}
				
			    break;
		}
}
/******************************************************************
测试各个部分的电流电压
******************************************************************/
void Test_Current(uint8 state)
{
uint32 piv_current;	 //临时存在的电流
uint8 i;			   //循环计数器
        switch(state)
		{
		    //左轮的电流
		    case 0:
			    do_action(6,0xffffff);
			    comm_rap();
				piv_current = (l_current_1s * 33000)/(4096 * 47);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 10);
				Dis_Minute = (uint8)(piv_current%100);
			    break;  
			//检测右轮的电流
			case 1:		
			    do_action(5,0xffffff); 
			    comm_rap();
				piv_current = (r_current_1s * 33000)/(4096 * 47);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 20);
				Dis_Minute = (uint8)(piv_current%100);
			    break;  
				  		 
			//检测中扫的电流 ---------正转部分电流，关闭反转部分电流
			case 2:			
			    gbv_action_m_motor = TRUE;
				Io_Driver();
				piv_current = (m_current_1s * 33000)/(4096 * 47);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 30);
				Dis_Minute = (uint8)(piv_current%100);
			    break; 

			//检测风机的电流   
			case 3:			
			    gbv_action_dust_motor = TRUE;
				Io_Driver();
				piv_current = (dust_current_1s * 33000)/(4096 * 47);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 40);
				Dis_Minute = (uint8)(piv_current%100);	
			    break;  

			//检测边扫的电流  
			case 4:		   
			    gbv_action_s_motor = TRUE;
				Io_Driver();
			    Dis_Hour = 50;
				if(s_current.key == 0)
				{
				    Dis_Minute = 1;
				}
				else
				{
				    Dis_Minute = 0;
				}
			    break;  
			
			//检测电池电压	  
			case 5:		 
				piv_current = (battery_voltage_1s * 363)/(4096);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 60);
				Dis_Minute = (uint8)(piv_current%100);	
			    break; 
				
			//检测电池温度
			case 6:		
				if((battery_temp < 356)||(battery_temp > 3902))
				{ 
				    Dis_Hour =  79;
				    Dis_Minute = 99;	
				}
				else
				{
				    i = 119;
					for(i =118; i > 0; i--)
					{
					    if((battery_temp > temp[i+1])&&(battery_temp < temp[i]))
						{
						    Dis_Hour =  70 + i/100;
							Dis_Minute = i%100;
							break ;
						}	
					}
				}
			    break;
				
			//检测充电的电流  
			case 7:
			    chargeing();	 
				piv_current = account_current(CHARGE_CURRENT);
				if(piv_current > jt_chargecurrent)
				{
				    piv_current = ((piv_current-jt_chargecurrent)*165)/4096;
				}
				else
				{
				    piv_current = 0;
				}
				Dis_Hour = (uint8)((piv_current%1000)/100 + 80);
				Dis_Minute = (uint8)(piv_current%100);	
				
			    break;
				
		}
}
/******************************************************************
测试地检功能
******************************************************************/
void Test_HwIncept(uint8 state)
{

		if(l_hw.effect == 1)
		{
			Dis_UvOn = TRUE;   
		}
		else 
		{
			Dis_UvOn = FALSE;
		} 		
		if(m_hw.effect == 1)
		{
			Dis_PrOn = TRUE;   
		}
		else 
		{
			Dis_PrOn = FALSE;
		}			
		if(up_hw.effect == 1)
		{
			Dis_AuOn = TRUE;   
		}
		else 
		{
			Dis_AuOn = FALSE;
		}		
		if(r_hw.effect == 1)
		{
			Dis_SpOn = TRUE;   
		}
		else 
		{
			Dis_SpOn = FALSE;
		}
        switch(state)
	    {
		    //检测左边红外接收	
		    case 0:
				Dis_Hour = 10; 
				if(l_hw.effect == 1)
				{
				    Dis_Minute = l_hw.data%100; 
					Dis_Hour += l_hw.data/100;
				}
				else
				{
				    Dis_MinuteOn = FALSE; 
				}
			    break;	 
				
		    //检测中间红外接收	
		    case 1:
				Dis_Hour = 20; 
				if(m_hw.effect == 1)
				{
				    Dis_Minute = m_hw.data%100;   
					Dis_Hour += m_hw.data/100;
				}
				else
				{
				    Dis_MinuteOn = FALSE; 
				}
			    break;
				
		    //检测右边红外接收	
		    case 2:
				Dis_Hour = 30; 
				if(r_hw.effect == 1)
				{
				    Dis_Minute = r_hw.data%100;   
					Dis_Hour += r_hw.data/100;
				}
				else
				{
				    Dis_MinuteOn = FALSE; 
				}
			    break;
				
		    //检测顶上红外接收	
		    case 3:
				Dis_Hour = 40; 
				if(up_hw.effect == 1)
				{
				    Dis_Minute = up_hw.data%100;   
					Dis_Hour += up_hw.data/100;
				}
				else
				{
				    if (dummy_wall.effect == 1)
					{				
				        Dis_Minute = dummy_wall.data%100;   
					    Dis_Hour += dummy_wall.data/100;
					}
					else
					{				  
				        Dis_MinuteOn = FALSE; 
					}				 
				}
			    break;
	    }
}
/******************************************************************
测试地检功能
******************************************************************/
void Test_Wall(uint8 state)
{
uint32 piv_current;		 		
		if(w_l.sign == NEAR)
		{
			Dis_ErrOn = TRUE;   
		}
		else 
		{
			Dis_ErrOn = FALSE;
		}				
		if(w_lm.sign == NEAR)
		{
			Dis_UvOn = TRUE;   
		}
		else 
		{
			Dis_UvOn = FALSE;
		} 		
		if(w_m.sign == NEAR)
		{
			Dis_PrOn = TRUE;   
		}
		else 
		{
			Dis_PrOn = FALSE;
		}			
		if(w_rm.sign == NEAR)
		{
			Dis_AuOn = TRUE;   
		}
		else 
		{
			Dis_AuOn = FALSE;
		}		
		if(w_r.sign == NEAR)
		{
			Dis_SpOn = TRUE;   
		}
		else 
		{
			Dis_SpOn = FALSE;
		}
        switch(state)
		{	//检测左边墙检	
		    case 0:
			    piv_current = ((w_l.difference)*330)/4096;
				Dis_Hour = (uint8)((piv_current%1000)/100+10); 
				Dis_Minute = (uint8)(piv_current%100); 		
			    break;
				
		    //检测左中墙检	
		    case 1:
				piv_current =  (((w_lm.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+ 20); 
				Dis_Minute = (uint8)(piv_current %100); 	
			    break;
				
		    //检测中间墙检	
		    case 2: 
			    piv_current = (((w_m.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+ 30); 
				Dis_Minute = (uint8)(piv_current %100); 		
			    break;
				
		    //检测右中墙检	
		    case 3:
			    piv_current = (((w_rm.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+40); 
				Dis_Minute = (uint8)(piv_current%100); 		
			    break;
				
		    //检测右边墙检	
		    case 4:
			    piv_current = (((w_r.difference)*330)/4096);
				Dis_Hour =  (uint8)((piv_current%1000)/100+50); 
				Dis_Minute = (uint8)(piv_current%100); 		
			    break;
				
		}
}
/******************************************************************
测试地检功能
******************************************************************/
void Test_Earth(uint8 state)
{
uint32 piv_current ;
				  	
		if(e_l.sign == NEAR)
		{
		    Dis_ErrOn = TRUE;   
		}
		else 
		{
			Dis_ErrOn = FALSE;
		}						
		if(e_lm.sign == NEAR)
		{
			Dis_UvOn = TRUE;   
		}
		else 
		{
			Dis_UvOn = FALSE;
		}		 				
		if(e_rm.sign == NEAR)
		{
			Dis_AuOn = TRUE;   
		}
		else 
		{
			Dis_AuOn = FALSE;
		}	  			
		if(e_r.sign == NEAR)
		{
			Dis_SpOn = TRUE;   
		}
		else 
		{
			Dis_SpOn = FALSE;
		}
        switch(state)
		{	//检测左边地检
		    case 0:
			    piv_current = (((e_l.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+10); 
				Dis_Minute = (uint8)(piv_current %100); 			
			    break;
				
		    //检测左中地检			
		    case 1: 
			    piv_current = (((e_lm.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+20); 
				Dis_Minute = (uint8)(piv_current %100); 	
			    break;
				
		    //检测右中地检	
		    case 2: 
			    piv_current = (((e_rm.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+30); 
				Dis_Minute = (uint8)(piv_current %100); 
			    break;
				
		    //检测右边地检	
		    case 3:
			    piv_current = (((e_r.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+40); 
				Dis_Minute = (uint8)(piv_current %100); 	
			    break;
		}				
}
/******************************************************************
功能：机器正反转。向前0.5米再向后0.5米
******************************************************************/
void frontandback(void)
{
 static uint8 direction = 0 ;     //方向
 static uint32 tt;
    switch(direction )
	 {
	    case 0:
		     if ((l_rap.sign == 0) && (r_rap.sign == 0))
			 {
			      direction = 1;
				  tt = giv_sys_time;
			 }
			 break ;
	    case 1:
			 if ((giv_sys_time - tt) > 3000) 
			 {
				 enable_rap(FRONT, 2000,FRONT,2000);
				 direction = 2 ;
			 }
			 break;
		case 2:
		     if ((l_rap.sign == 0) && (r_rap.sign == 0))
			 {
			      direction = 3;
				  tt = giv_sys_time;
			 }
			 break ;
		case 3:
			 if ((giv_sys_time - tt) > 3000) 
			 {
			     direction = 0;
				 enable_rap(BACK, 2000,BACK,2000);
			 }
			 break ;
		default :
			 direction = 0; 
	 }
}
/*************************************************************************
功能：检测主板的程序
输入：无
输出：无
说明：在主板进入自检后即立马对主板进行自检，检验项目包括：左右轮控制与采样电路、
中扫控制与采样电路、边扫控制与采样电路、风机控制与采样电路、UV控制电路、前轮检
测电路、墙检地检红外接收前撞电路的检测。在测试通过后蜂鸣器响一下，没有通过则响
两下。
*************************************************************************/
void Test_MainBoard(uint8 state)
{
static uint8 step = 0;	   
static uint8 step2 = 0;
static uint8 step3 = 0;
static uint32 symbol = 0;
static uint32 stime = 0;
static uint8  test_err = 0;  
uint32 t; 

uint8 i;//循环计数变量
uint8 m;
	Io_Driver();
	comm_rap();
    switch(state)
	{
	    case 0:
		    switch(step)
			{
			    case 0:
				    if(Test_DriveOfWheel())
					{
						step = 6;
						test_err = 1;
						break;
					}
					if(Test_DriveOfBrush())
					{
						step = 6;
						test_err = 2;
						break;
					}
					step++;
					symbol = 0;
					stime = giv_sys_time;
					
                    gbv_action_uv = TRUE;	         /*设置uv灯的状态，由外部模块读取和改变*/
                    gbv_action_s_motor = TRUE;       /*设置边扫马达的状态，由外部模块读取和改变*/
                    gbv_action_m_motor = TRUE;       /*设置中扫马达的状态，由外部模块读取和改变*/ 
                    gbv_action_dust_motor = TRUE;	 /*设置灰尘风机的状态，由外部模块读取和改变*/

				case 1:
				    do_action(3,0xffffff);
				    if((e_l.difference < 1737)&&(e_l.difference > 1414))
					{
					    symbol |= 0x1;
					}	
				    if((e_lm.difference < 1737)&&(e_lm.difference > 1414))
					{
					    symbol |= 0x2;
					}
				    if((e_rm.difference < 1737)&&(e_rm.difference > 1414))
					{
					    symbol |= 0x4;
					}
				    if((e_r.difference < 1737)&&(e_r.difference > 1414))
					{
					    symbol |= 0x8;
					}
				    if((w_l.difference < 1737)&&(w_l.difference > 1414))
					{
					    symbol |= 0x10;
					}
				    if((w_lm.difference < 1737)&&(w_lm.difference > 1414))
					{
					    symbol |= 0x20;
					}
				    if((w_m.difference < 1737)&&(w_m.difference > 1414))
					{
					    symbol |= 0x40;
					}
				    if((w_rm.difference < 1737)&&(w_rm.difference > 1414))
					{
					    symbol |= 0x80;
					}
				    if((w_r.difference < 1737)&&(w_r.difference > 1414))
					{
					    symbol |= 0x100;
					}
					if(l_hw.sign == 1)
					{
					    symbol |= 0x200;
					}	
					if(m_hw.sign == 1)
					{
					    symbol |= 0x400;
					}
					if(up_hw.sign == 1)
					{
					    symbol |= 0x800;
					}
					if(r_hw.sign == 1)
					{
					    symbol |= 0x1000;
					}
					if(l_lidi.key == 1)
					{ 
					    symbol |= 0x2000;
					}
					else
					{
					    symbol |= 0x4000;
					}
					if(r_lidi.key == 1)
					{ 
					    symbol |= 0x8000;
					}
					else
					{
					    symbol |= 0x10000;
					}  
					if(l_bump.key == 1)
					{ 
					    symbol |= 0x20000;
					}
					else
					{
					    symbol |= 0x40000;
					}	
					if(r_bump.key == 1)
					{ 
					    symbol |= 0x80000;
					}
					else
					{
					    symbol |= 0x100000;
					}
						  
					if(l_ring.length > 3)
					{
					    symbol |= 0x400000;
					}
					if(r_ring.length > 3)
					{
					    symbol |= 0x800000;
					}		
					if(s_current.key == 1)
					{ 
					    symbol |= 0x1000000;
					}
					else
					{
					    symbol |= 0x2000000;
					} 
					if((giv_sys_time - stime) > 20000)
					{
					    if(symbol != 0x3dfffff)	//3ffffff
						{
						    step = 6;
						    break;
						}
						step ++;
					}
					break;
				case 2:
				    if((battery_temp_1s > 2253)||(battery_temp_1s < 1843))
					{
						step = 6;
						test_err = 3;
						break;
					}	
				    if((l_current_1s > 291)||(l_current_1s < 175))	//300--500mA
					{											   
						step = 6;
						test_err = 4;
						break;
					} 			  
				    if((r_current_1s > 291)||(r_current_1s < 175))//300--500mA
					{
						step = 6;
						test_err = 5;
						break;
					}
				    if((m_current_1s > 291)||(m_current_1s < 175))	 //300--500mA
					{
						step = 6;
						test_err = 6;
						break;
					}
				    if((dust_current_1s > 291)||(dust_current_1s < 175)) //300--500mA
					{
						step = 6;
						test_err = 7;
						break;
					}
				    ///////将左右轮、中扫的控制方向相反转动
					 
                    gbv_action_uv = TRUE;	/*设置uv灯的状态，由外部模块读取和改变*/
                    gbv_action_s_motor = FALSE;/*设置边扫马达的状态，由外部模块读取和改变*/
                    gbv_action_m_motor = FALSE;//TRUE;/*设置中扫马达的状态，由外部模块读取和改变*/ 
                    gbv_action_dust_motor = FALSE;	/*设置灰尘风机的状态，由外部模块读取和改变*/

					///////初始化时间，准备等待2秒
					stime = giv_sys_time;
					step ++;
					break;
				case 3:
				    if((giv_sys_time - stime) > 20000)
					{
					    step++;
					}  
				    do_action(4,0xffffff);
				    break;
				case 4:	
				    if((l_current_1s > 291)||(l_current_1s < 175))//300--500mA
					{
						step = 6;
						test_err = 8;
						break;
					} 			  
				    if((r_current_1s > 291)||(r_current_1s < 175))//300--500mA
					{
						step = 6;
						test_err = 9;
						break;
					}
				    
					////关闭马达输出	  
				    stop_rap();	
                    gbv_action_uv = FALSE;	/*设置uv灯的状态，由外部模块读取和改变*/
                    gbv_action_s_motor = FALSE;/*设置边扫马达的状态，由外部模块读取和改变*/
                    gbv_action_m_motor = FALSE;/*设置中扫马达的状态，由外部模块读取和改变*/ 
                    gbv_action_dust_motor = FALSE;	/*设置灰尘风机的状态，由外部模块读取和改变*/
					step = 8;	
					stime = giv_sys_time;
					break;
				case 5:
				    Enable_Beep();
					step = 7;
					break;
				case 6:
				    Enable_Beep_number(2);
					step++;
				case 7:
				    stop_rap();	
                    gbv_action_uv = FALSE;	/*设置uv灯的状态，由外部模块读取和改变*/
                    gbv_action_s_motor = FALSE;/*设置边扫马达的状态，由外部模块读取和改变*/
                    gbv_action_m_motor = FALSE;/*设置中扫马达的状态，由外部模块读取和改变*/ 
                    gbv_action_dust_motor = FALSE;	/*设置灰尘风机的状态，由外部模块读取和改变*/
				    Dis_Minute = test_err; 
				    break;
				case 8:		
				    if((giv_sys_time - stime) > 20000)
					{		   
				        if((battery_voltage_1s > 1748)||(battery_voltage_1s < 1500))	//15.5V--13.3V
					    {
						    step = 6;
							test_err = 11;
					    } 
					    if(step!=6)step = 5;
					}
					break;  
								    
			}
		    break;
		case 1:
		    switch(step2)
			{
			    case 0:	  
					stime = giv_sys_time;
					step2 ++;
				    break;
				case 1:
				    if((giv_sys_time - stime) > 20000)
					{
					    if(power.charge_dc == 1)
						{
						    jt_chargecurrent = battery_chargecurrent_1s;
							stime = giv_sys_time;
							enable_pwm(CHARGE_PWM,1000);	
					        step2++;
						}
						else
						{
						    step2 = 4;
							break;
						}
					} 
					break;
				case 2:
				    if((giv_sys_time - stime) > 20000)
					{
					    t = battery_chargecurrent_1s - jt_chargecurrent	;
						if((t<993)||(t>1241))	 //400--500mA
						{
						    step2 = 4;
							break;
						}
						step2++;
					} 
					break;
				case 3:	 
				    Enable_Beep();
					step2 = 5;
					break;
				case 4:
				    Enable_Beep_number(2);	 
					step2++;
					break;
				case 5:
					enable_pwm(CHARGE_PWM,0);
					break;
			}
			break;
		case 2:
		    switch(step3)
			{
			    case 0:	  
					stime = giv_sys_time;
					step3 ++;
				    break;
				case 1:
				    if((giv_sys_time - stime) > 20000)
					{
					    if(power.charge_seat == 1)
						{
						    jt_chargecurrent = battery_chargecurrent_1s;
							stime = giv_sys_time;
							enable_pwm(CHARGE_PWM,1000);	
					        step3++;
						}
						else
						{
						    step3 = 4;
							break;
						}
					} 
					break;
				case 2:
				    if((giv_sys_time - stime) > 20000)
					{
					    t = battery_chargecurrent_1s - jt_chargecurrent	;
						if((t<993)||(t>1241))	 //400--500mA
						{
						    step3 = 4;
							break;
						}
						step3++;
					} 
					break;
				case 3:	 
				    Enable_Beep();
					step3 = 5;
					break;
				case 4:
				    Enable_Beep_number(2);	 
					step3++;
					break;
				case 5:
					enable_pwm(CHARGE_PWM,0);
					break;
			}
			break;	 
		case 3:
			    Clr_DisplayData();
				i = 0;
                lcdkey_bgon();     //打开背光
	            lcdkey_dison();    //打开显示
			    while((key1.sign == 1)&&(key2.sign == 1)&&(key3.sign == 1))
				{
			        if(Sec == TRUE)
	                {
	                    Sec = FALSE;
						i++;
						Clr_DisplayData();
						//修改自检的显示
						i = i%26;
						if(i < 2)
						{	  
				            for(m=0; m<10;m++)
				            {
					            Dis_Data[m] = (0xff);
				            }
						}
						else
						{
						if(i<6)
						{
						    for(m=0; m<10;m++)
							{
							    Dis_Data[m] = (0x11<<(i-2));
							}
						}
						else
						{
						    if((i%2)==0)
							{	  
						        Dis_Data[(i-6)/2] = 0xf;
							}
							else
							{						  
						        Dis_Data[(i-6)/2] = 0xf0;
							}
						}
						}
		                lcdkey_disdata(12, Dis_Data, 10); 
					}	
					IWDG_ReloadCounter();
				    AutoReadKey();
				}
			break;
	}    
}
