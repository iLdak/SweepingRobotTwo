/**************************************************************
深圳银星智能电器有限公司
文件名：power.c
功能：power驱动程序文件
版本：1.0
作者：肖双良
时间：2008.12.13
修改信息：
		   
修改时间：2009.7.2
修改内容：
1、将大电流充电的电流改为800毫安，方便兼容使用1A的开关电源

修改时间：2009.5.6
修改内容：
1、将涓流充电的电流改为150毫安，方便兼容使用1500毫安时的电池

修改时间：2009.4.10
修改内容：增加全局变量 battery_chargecurrent，为5秒内的平均充电的电流 

**************************************************************/		 
//=============================头文件=========================
#include "power.h"
#include "soft.h"	
#include "hardware.h"
#include "ad.h"
#include "pwm.h"
#include "lcdkey.h"
#include "time1.h"
#include "temp.h"	
#include "key.h"		
#include "function.h"
#include "err.h"
#include "rtc.h"
//======================私有定义=============================
//======================全局变量=============================	 
POWER power ;
bool POWER_ready;
bool VOL_TEMP_ready;
uint16  jt_chargecurrent;   //静态时主板所耗电流
uint16  battery_chargecurrent;//电池5秒内的平均电流
uint16  battery_voltage;    //电池1秒的电压
uint16  battery_temp;       //电池1秒的温度
uint16  l_current;          //左轮电流
uint16  r_current;          //右轮电流
uint16  dust_current;       //灰尘风机电流
uint16  m_current;          //中扫电流	

uint16  battery_chargecurrent_1s;//电池1秒内的平均电流
uint16  battery_voltage_1s;    //电池1秒的电压
uint16  battery_temp_1s;       //电池1秒的温度
uint16  l_current_1s;          //左轮电流
uint16  r_current_1s;          //右轮电流
uint16  dust_current_1s;       //灰尘风机电流
uint16  m_current_1s;          //中扫电流		
//=====================私有变量=================================
//=====================全局函数=================================	
//=====================私有函数=================================
//=====================函数实体=================================
/**************************************************************
程序功能：初始化电池电源
输入：无
输出：无
说明：初始化与电源有关的io，并且判断电池是否有NTC。
**************************************************************/
void init_power (void)
{
  battery_temp = account_current(BATTERY_TEMP);
  power.voltage = account_current(BATTERY_VOLTAGE);
  battery_voltage = power.voltage;
  
  jt_chargecurrent = account_current(CHARGE_CURRENT);
  battery_chargecurrent = jt_chargecurrent;//电池5秒内的平均电流
  power.current = 0;
  if( battery_temp < 355)
  {
    giv_sys_err = 25;
  }
  if(battery_temp > 3902)
  {
    giv_sys_err = 26;
  }
  power.charge_dc = 0;
  power.charge_seat = 0;
  power.pwm = 0;
}
/*************************************************************
功能： 充电控制
输入：无
输出：无
*************************************************************/
void chargeing(void)
{
	if((power.charge_dc == 0)&&(power.charge_seat == 0))
	{
	   power.pwm = 0;
	   power.step = 0;
	   disable_pwm(CHARGE_PWM);
	   return;
	}
	if( POWER_ready != TRUE )
	{
	   return;
	}
	POWER_ready = FALSE;
	ChargeControl();	
}
/************************************************
功能：充电控制
************************************************/
void ChargeControl(void)
{
 uint16 piv_current;
	//=====================充电控制==============
	
	piv_current = account_current(CHARGE_CURRENT);   //采样当时的电流
	
	switch(power.step)
	{
	 //================开始充电前的准备=======
	    case 0:
		    power.time = giv_sys_time;
			power.step = 1;
	        disable_pwm(CHARGE_PWM);
		    return;
		//======================接上电极10秒钟，察看电池是否接上
	    case 1:
		    if((giv_sys_time - power.time) > 50000)//接触上电10秒钟
			{
                if( battery_temp < 355)	  //电池温度过低，电池NTC短路
                {
                    giv_sys_err = 25;
					Init_Err();
					return ;
                }
                if(battery_temp > 3902)	  //电池温度过高故障
                {
                    giv_sys_err = 26;
					Init_Err();
					return ;
                }
				//====================判断电池是否需要激活流程 
				if((giv_BatteryActivation != 0)|| ((giv_BatteryChargeTime + 7776000) < Rtc_time))
				{
				    giv_BatteryActivation = 1;
					power.step = 5;
				}
				else
				{			 
				    power.step = 2;			  
				    //如果电池充电标志为1，则直接进入第4步。
			        if(giv_BatteryChargeForbid != 0)
				    {
				        power.step = 4;
					    Dis_PowerGlint = FALSE;
				    }
				}
				giv_BatteryChargeTime = Rtc_time;				 //保存本次充电时间
				WriteBakBatteryChargeTime();					 //
				WriteBakBatteryActivation();					 //保存本次充电激活的标志

				jt_chargecurrent = battery_chargecurrent;
			    power.temp = battery_temp;//Get_BatteryTemp();
				power.time = giv_sys_time;
			}
		    return;	
		//使用180ma充电5分钟并且电池温度低于40度，看电池是否允许大电流充电，不允许则进行涓流充电
	    case 2:
			//充电电流大于150MA则降低PWM值
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 190))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //电流大于设定电流
	            }
			}
			else if((piv_current < jt_chargecurrent)||((piv_current - jt_chargecurrent) <170))
			{
			    
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //电流小于设定电流
	            }
			}
		    if(((giv_sys_time - power.time) > 50000)&&(battery_temp > temp[70])) //充电时间为5分钟并且电池温度低于40度
			{
				power.step = 3;
				power.time = giv_sys_time;
			}
			if((giv_sys_time - power.time) > 72000000)
			{
			    power.step = 3;
				power.time = giv_sys_time;
			}
		    break;	
			
		//电池限流限压的过程，判断电池转为涓流的条件是：
		//1、电池充电电流小于360ma；
		//2、电池的绝对温度大于50度
		//3、在电池温度大于40度的情况下，滇池温度每分钟上升速度大于1.5度
	    case 3:
			//充电电流大于设定电流则降低PWM值
			if(((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 1986)))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //电流大于设定电流
	            }
			}
			else  if(((piv_current < jt_chargecurrent)||((piv_current - jt_chargecurrent) <1970)))
			{
			    
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //电流小于设定电流
	            }
			}
			//判断电池的绝对温度大于50度或者电池电压大于21伏,电池转为涓流充电
			if(battery_temp < temp[80])
			{
			    power.step = 4;
				power.time = giv_sys_time;
			}
			//电池的充电电流小于360ma认为电池已经充满
			if(((battery_chargecurrent < jt_chargecurrent) 
			|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //电流小于360ma
			&&((giv_sys_time - power.time) > 6000000))
			{
			    power.step = 4;
				power.time = giv_sys_time;
			}
			//在大电流充电5小时以上，强制性将电池置为充饱状态。
			if(((giv_sys_time - power.time) > 180000000)|| (battery_voltage > 2369))
			{
			    power.step = 4;
				power.time = giv_sys_time;	  
				giv_BatteryFDCap = 0;
				Dis_PowerGlint = FALSE;
				giv_BatteryChargeForbid = 1;
			}
			////////每分钟判断一次////////////////////////
		    if(gbv_minute != FALSE)
			{
			    gbv_minute = FALSE;
			    if(battery_temp < temp[70])
				{
					if((battery_temp < power.temp)&&((power.temp - battery_temp) > 50))
					{
					    power.step = 4;
						power.time = giv_sys_time;
					}
				}
				power.temp = battery_temp;
			}
		    break;	
	    case 4:
			//充电电流大于180MA则降低PWM值
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 190))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //电流大于设定电流
	            }
			}
			else  if((piv_current < jt_chargecurrent)||((piv_current - jt_chargecurrent) < 170))
			{
			    
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //电流小于设定电流
	            }
			}
			////////每分钟判断一次////////////////////////
		    if(gbv_minute != FALSE&&(giv_BatteryChargeForbid == 0))
			{
			    gbv_minute = FALSE;
			    if((battery_temp > temp[70])//电池的绝对温度小于40度，充电电流大于100ma
				&& ((battery_chargecurrent > jt_chargecurrent)&&((battery_chargecurrent - jt_chargecurrent) > 130)))//电池电压小于15v
				{
				    power.step = 5;
					power.time = giv_sys_time; 
					Dis_PowerGlint = TRUE;
				}
			    if(((giv_sys_time - power.time) > 12000000)
				  &&(Dis_PowerGlint == TRUE)
				  && (battery_temp < temp[70]))
				{
				    giv_BatteryFDCap = 0;
				    Dis_PowerGlint = FALSE;
					giv_BatteryChargeForbid = 1;
				}
				power.temp = battery_temp;
			}
		    break;
		/*********        下面的几个过程为电池激活的过程    ***************/
		case 5:	  //电池以0.05C充电即80ma充电5分钟 
		case 9:		 
			//充电电流大于150MA则降低PWM值
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 190))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //电流大于设定电流
	            }
			}
			else if((piv_current <jt_chargecurrent)||((piv_current - jt_chargecurrent) < 170))
			{
			    
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //电流小于设定电流
	            }
			}
			if((giv_sys_time - power.time) > 3000000)
			{
			    power.step++;
				power.time = giv_sys_time;
				power.pwm = 0;
			}
		    break;	

		case 6:	  //电池休息5分钟		
		case 8:								  
		case 10:		   
		case 12:
			if((giv_sys_time - power.time) > 3000000)
			{
			    power.step++;
				power.time = giv_sys_time;
				if(power.step == 13)
				{
				    giv_BatteryActivation = 0;
					WriteBakBatteryActivation();
				}
			}						  
			power.pwm = 0;
			break;
		case 7:	  //电池以0.1C即150MA充电10分钟
		case 11:  
			//充电电流大于150MA则降低PWM值
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 372))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //电流大于设定电流
	            }
			}
			else if((piv_current < jt_chargecurrent)||((piv_current - jt_chargecurrent) < 360))
			{
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //电流小于设定电流
	            }
			}
			if((giv_sys_time - power.time) > 6000000)
			{
			    power.step++;
				power.time = giv_sys_time; 
				power.pwm = 0;
			}
		    break;
		case 13:   //电池以0.1C充电11小时作为充电充满	  
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 372))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //电流大于设定电流
	            }
			}
			else  if((piv_current <jt_chargecurrent)||((piv_current - jt_chargecurrent) < 360))
			{
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //电流小于设定电流
	            }
			}
			// 在充电11个小时后，电池显示充电已饱
			if(((giv_sys_time - power.time) > 396000000)&& ((Dis_PowerGlint == TRUE)))
			{
				    giv_BatteryFDCap = 0;
				    Dis_PowerGlint = FALSE;
					giv_BatteryChargeForbid = 1;
			}
		    break;
		default :
		    power.step = 0;		
	}
	if((charge_dc.key == 0)&&(charge_seat.key == 0))
	{
	    disable_pwm(CHARGE_PWM);
	    power.pwm = 0;
	}
	else
	{
	    enable_pwm(CHARGE_PWM,power.pwm);   //执行pwm动作
	}	  	
}
/************************************************
功能：判断是否有外部电源插入。
输入：无
输出：无
************************************************/
void judge_charge(void)
{
static uint32 time0,time1;
////////////////////判断dc座充电输入/////////////////
   if(power.charge_seat == 0)
   {
       if(power.charge_dc == 1)//原来确定DC座有电源插入
       {
           if(charge_dc.key == 0)
	       {
	           if((giv_sys_time - time0) > 2000)   //没有插入电源1秒以上
		       {
		           time0 = giv_sys_time;
			       power.charge_dc = 0;    //确定没有插入电源
		       }
	       }
	       else
	       {
	           time0 = giv_sys_time;  
	       }
       }
       else
       {
           if(charge_dc.key != 0)
	       {
	           if((giv_sys_time - time0) > 1000)    //插入电源0.2秒以上
		       {
		           time0 = giv_sys_time;
			       power.charge_dc = 1;     //  确定有插入电源
			       power.step = 0;         //开始进入充电
		       }
	       }
	       else
	       {
	           time0 = giv_sys_time;
	       }
       }
   }
////////////////////判断回充座充电输入/////////////////
   if(power.charge_dc == 0)
   {
       if(power.charge_seat == 1)//原来确定回充座有电源插入
       {
           if(charge_seat.key == 0)
	       {
	           if((giv_sys_time - time1) > 10000)   //没有插入电源1秒以上
		       {
		           time1 = giv_sys_time;
			       power.charge_seat = 0;    //确定没有插入电源
		       }
	       }
	       else
	       {
	           time1 = giv_sys_time;  
	       }
       }
       else
       {
           if(charge_seat.key != 0)
	       {
	           if((giv_sys_time - time1) > 2000)    //插入电源0.1秒以上
		       {
		           time1 = giv_sys_time;
			       power.charge_seat = 1;     //  确定有插入电源
				   power.step = 0;         //开始进入充电
		       }
	       }
	       else
	       {
	           time1 = giv_sys_time;
	       }
       }
   }
}


/*************************************************************
功能：温度采样，1秒钟采样一次，采样10次计算平均值保存，
*************************************************************/
void  sampling_temp_voltage(void)
{
static uint32 l_linshi_current = 0,               //左轮的临时电流
              r_linshi_current = 0,               //右轮的临时电流
		      m_linshi_current = 0,               //中扫的临时电流
		      dust_linshi_current = 0,            //灰尘风机的临时电流
              battery_linshi_voltage = 0,         //电池的临时电压          
		      battery_linshi_temp = 0,            //电池的临时温度
              battery_linshi_chargecurrent = 0;//电池5秒内的平均电流
			  
static uint16 sampling_number = 0;                //采样次数 

static uint32 l_linshi_current_1s = 0,               //左轮的临时电流
              r_linshi_current_1s = 0,               //右轮的临时电流
		      m_linshi_current_1s = 0,               //中扫的临时电流
		      dust_linshi_current_1s = 0,            //灰尘风机的临时电流
              battery_linshi_voltage_1s = 0,         //电池的临时电压          
		      battery_linshi_temp_1s = 0,            //电池的临时温度
              battery_linshi_chargecurrent_1s = 0;//电池1秒内的平均电流
			  
static uint16 sampling_number_1s = 0;                //采样次数

uint32 t;
   if(VOL_TEMP_ready == TRUE)
   {
      VOL_TEMP_ready = FALSE;
   }
   else
   {
     return;
   }
   sampling_number ++;
   t =	account_current(L_CURRENT);
   l_linshi_current += t;
   l_linshi_current_1s += t ;
   t = 	account_current(R_CURRENT);
   r_linshi_current += t;
   r_linshi_current_1s += t;
   t = 	account_current(M_CURRENT) ;
   m_linshi_current += t;
   m_linshi_current_1s += t;
   t =	account_current(DUST_CURRENT);
   dust_linshi_current += t;
   dust_linshi_current_1s += t;
   t =	account_current(BATTERY_VOLTAGE);
   battery_linshi_voltage += t;
   battery_linshi_voltage_1s += t;
   t =	account_current(BATTERY_TEMP);
   battery_linshi_temp += t;
   battery_linshi_temp_1s += t;		
   t =	 account_current(CHARGE_CURRENT);
   battery_linshi_chargecurrent += t;
   battery_linshi_chargecurrent_1s += t;
   if(sampling_number == 25000)
   {
      battery_voltage = (uint16) (battery_linshi_voltage/25000);    //电池1秒的电压
      battery_temp = (uint16) (battery_linshi_temp/25000);       //电池1秒的温度
      l_current = (uint16) (l_linshi_current/25000);          //左轮电流
      r_current = (uint16) (r_linshi_current/25000);          //右轮电流
      dust_current = (uint16) (dust_linshi_current/25000);       //灰尘风机电流
      m_current = (uint16) (m_linshi_current/25000);          //中扫电流
	  battery_chargecurrent = (uint16)(battery_linshi_chargecurrent/25000);//充电平均电流
	  sampling_number = 0;
	  battery_linshi_voltage = 0;
	  battery_linshi_temp = 0;
	  l_linshi_current = 0;
	  r_linshi_current = 0;
	  dust_linshi_current = 0;
	  m_linshi_current = 0;
	  battery_linshi_chargecurrent = 0;
   }		 
    			
   sampling_number_1s ++;
   if(sampling_number_1s == 5000)
   {
      battery_voltage_1s = (uint16) (battery_linshi_voltage_1s/5000);    //电池1秒的电压
      battery_temp_1s = (uint16) (battery_linshi_temp_1s/5000);       //电池1秒的温度
      l_current_1s = (uint16) (l_linshi_current_1s/5000);          //左轮电流
      r_current_1s = (uint16) (r_linshi_current_1s/5000);          //右轮电流
      dust_current_1s = (uint16) (dust_linshi_current_1s/5000);       //灰尘风机电流
      m_current_1s = (uint16) (m_linshi_current_1s/5000);          //中扫电流
	  battery_chargecurrent_1s = (uint16)(battery_linshi_chargecurrent_1s/5000);//充电平均电流
	  sampling_number_1s = 0;
	  battery_linshi_voltage_1s = 0;
	  battery_linshi_temp_1s = 0;
	  l_linshi_current_1s = 0;
	  r_linshi_current_1s = 0;
	  dust_linshi_current_1s = 0;
	  m_linshi_current_1s = 0;
	  battery_linshi_chargecurrent_1s = 0;
   }
}
/*************************************************************************
功能: 读取电池温度
返回: 电池的温度值
*************************************************************************/
uint8 Get_BatteryTemp(void)
{
uint8 i;
    if(battery_temp > temp[50])
	{
	    for(i = 50; i > 0; i--)
		{
		    if((battery_temp > temp[i+1])&&(battery_temp < temp[i]))
		    {
			    return i;
		    }
		}	
	}
	else
	{
	    for(i = 50;i < 118;i++)
		{
		    if((battery_temp > temp[i+1])&&(battery_temp < temp[i]))
		    {
			    return i ;
		    }
		}
	}
	return 0;
}
