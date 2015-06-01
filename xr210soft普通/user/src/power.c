/**************************************************************
�����������ܵ������޹�˾
�ļ�����power.c
���ܣ�power���������ļ�
�汾��1.0
���ߣ�Ф˫��
ʱ�䣺2008.12.13
�޸���Ϣ��
		   
�޸�ʱ�䣺2009.7.2
�޸����ݣ�
1������������ĵ�����Ϊ800�������������ʹ��1A�Ŀ��ص�Դ

�޸�ʱ�䣺2009.5.6
�޸����ݣ�
1����������ĵ�����Ϊ150�������������ʹ��1500����ʱ�ĵ��

�޸�ʱ�䣺2009.4.10
�޸����ݣ�����ȫ�ֱ��� battery_chargecurrent��Ϊ5���ڵ�ƽ�����ĵ��� 

**************************************************************/		 
//=============================ͷ�ļ�=========================
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
//======================˽�ж���=============================
//======================ȫ�ֱ���=============================	 
POWER power ;
bool POWER_ready;
bool VOL_TEMP_ready;
uint16  jt_chargecurrent;   //��̬ʱ�������ĵ���
uint16  battery_chargecurrent;//���5���ڵ�ƽ������
uint16  battery_voltage;    //���1��ĵ�ѹ
uint16  battery_temp;       //���1����¶�
uint16  l_current;          //���ֵ���
uint16  r_current;          //���ֵ���
uint16  dust_current;       //�ҳ��������
uint16  m_current;          //��ɨ����	

uint16  battery_chargecurrent_1s;//���1���ڵ�ƽ������
uint16  battery_voltage_1s;    //���1��ĵ�ѹ
uint16  battery_temp_1s;       //���1����¶�
uint16  l_current_1s;          //���ֵ���
uint16  r_current_1s;          //���ֵ���
uint16  dust_current_1s;       //�ҳ��������
uint16  m_current_1s;          //��ɨ����		
//=====================˽�б���=================================
//=====================ȫ�ֺ���=================================	
//=====================˽�к���=================================
//=====================����ʵ��=================================
/**************************************************************
�����ܣ���ʼ����ص�Դ
���룺��
�������
˵������ʼ�����Դ�йص�io�������жϵ���Ƿ���NTC��
**************************************************************/
void init_power (void)
{
  battery_temp = account_current(BATTERY_TEMP);
  power.voltage = account_current(BATTERY_VOLTAGE);
  battery_voltage = power.voltage;
  
  jt_chargecurrent = account_current(CHARGE_CURRENT);
  battery_chargecurrent = jt_chargecurrent;//���5���ڵ�ƽ������
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
���ܣ� ������
���룺��
�������
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
���ܣ�������
************************************************/
void ChargeControl(void)
{
 uint16 piv_current;
	//=====================������==============
	
	piv_current = account_current(CHARGE_CURRENT);   //������ʱ�ĵ���
	
	switch(power.step)
	{
	 //================��ʼ���ǰ��׼��=======
	    case 0:
		    power.time = giv_sys_time;
			power.step = 1;
	        disable_pwm(CHARGE_PWM);
		    return;
		//======================���ϵ缫10���ӣ��쿴����Ƿ����
	    case 1:
		    if((giv_sys_time - power.time) > 50000)//�Ӵ��ϵ�10����
			{
                if( battery_temp < 355)	  //����¶ȹ��ͣ����NTC��·
                {
                    giv_sys_err = 25;
					Init_Err();
					return ;
                }
                if(battery_temp > 3902)	  //����¶ȹ��߹���
                {
                    giv_sys_err = 26;
					Init_Err();
					return ;
                }
				//====================�жϵ���Ƿ���Ҫ�������� 
				if((giv_BatteryActivation != 0)|| ((giv_BatteryChargeTime + 7776000) < Rtc_time))
				{
				    giv_BatteryActivation = 1;
					power.step = 5;
				}
				else
				{			 
				    power.step = 2;			  
				    //�����س���־Ϊ1����ֱ�ӽ����4����
			        if(giv_BatteryChargeForbid != 0)
				    {
				        power.step = 4;
					    Dis_PowerGlint = FALSE;
				    }
				}
				giv_BatteryChargeTime = Rtc_time;				 //���汾�γ��ʱ��
				WriteBakBatteryChargeTime();					 //
				WriteBakBatteryActivation();					 //���汾�γ�缤��ı�־

				jt_chargecurrent = battery_chargecurrent;
			    power.temp = battery_temp;//Get_BatteryTemp();
				power.time = giv_sys_time;
			}
		    return;	
		//ʹ��180ma���5���Ӳ��ҵ���¶ȵ���40�ȣ�������Ƿ�����������磬�����������������
	    case 2:
			//����������150MA�򽵵�PWMֵ
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 190))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //���������趨����
	            }
			}
			else if((piv_current < jt_chargecurrent)||((piv_current - jt_chargecurrent) <170))
			{
			    
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //����С���趨����
	            }
			}
		    if(((giv_sys_time - power.time) > 50000)&&(battery_temp > temp[70])) //���ʱ��Ϊ5���Ӳ��ҵ���¶ȵ���40��
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
			
		//���������ѹ�Ĺ��̣��жϵ��תΪ����������ǣ�
		//1����س�����С��360ma��
		//2����صľ����¶ȴ���50��
		//3���ڵ���¶ȴ���40�ȵ�����£�����¶�ÿ���������ٶȴ���1.5��
	    case 3:
			//�����������趨�����򽵵�PWMֵ
			if(((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 1986)))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //���������趨����
	            }
			}
			else  if(((piv_current < jt_chargecurrent)||((piv_current - jt_chargecurrent) <1970)))
			{
			    
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //����С���趨����
	            }
			}
			//�жϵ�صľ����¶ȴ���50�Ȼ��ߵ�ص�ѹ����21��,���תΪ������
			if(battery_temp < temp[80])
			{
			    power.step = 4;
				power.time = giv_sys_time;
			}
			//��صĳ�����С��360ma��Ϊ����Ѿ�����
			if(((battery_chargecurrent < jt_chargecurrent) 
			|| ((battery_chargecurrent - jt_chargecurrent) < 894)) //����С��360ma
			&&((giv_sys_time - power.time) > 6000000))
			{
			    power.step = 4;
				power.time = giv_sys_time;
			}
			//�ڴ�������5Сʱ���ϣ�ǿ���Խ������Ϊ�䱥״̬��
			if(((giv_sys_time - power.time) > 180000000)|| (battery_voltage > 2369))
			{
			    power.step = 4;
				power.time = giv_sys_time;	  
				giv_BatteryFDCap = 0;
				Dis_PowerGlint = FALSE;
				giv_BatteryChargeForbid = 1;
			}
			////////ÿ�����ж�һ��////////////////////////
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
			//����������180MA�򽵵�PWMֵ
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 190))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //���������趨����
	            }
			}
			else  if((piv_current < jt_chargecurrent)||((piv_current - jt_chargecurrent) < 170))
			{
			    
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //����С���趨����
	            }
			}
			////////ÿ�����ж�һ��////////////////////////
		    if(gbv_minute != FALSE&&(giv_BatteryChargeForbid == 0))
			{
			    gbv_minute = FALSE;
			    if((battery_temp > temp[70])//��صľ����¶�С��40�ȣ�����������100ma
				&& ((battery_chargecurrent > jt_chargecurrent)&&((battery_chargecurrent - jt_chargecurrent) > 130)))//��ص�ѹС��15v
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
		/*********        ����ļ�������Ϊ��ؼ���Ĺ���    ***************/
		case 5:	  //�����0.05C��缴80ma���5���� 
		case 9:		 
			//����������150MA�򽵵�PWMֵ
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 190))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //���������趨����
	            }
			}
			else if((piv_current <jt_chargecurrent)||((piv_current - jt_chargecurrent) < 170))
			{
			    
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //����С���趨����
	            }
			}
			if((giv_sys_time - power.time) > 3000000)
			{
			    power.step++;
				power.time = giv_sys_time;
				power.pwm = 0;
			}
		    break;	

		case 6:	  //�����Ϣ5����		
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
		case 7:	  //�����0.1C��150MA���10����
		case 11:  
			//����������150MA�򽵵�PWMֵ
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 372))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //���������趨����
	            }
			}
			else if((piv_current < jt_chargecurrent)||((piv_current - jt_chargecurrent) < 360))
			{
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //����С���趨����
	            }
			}
			if((giv_sys_time - power.time) > 6000000)
			{
			    power.step++;
				power.time = giv_sys_time; 
				power.pwm = 0;
			}
		    break;
		case 13:   //�����0.1C���11Сʱ��Ϊ������	  
			if((piv_current > jt_chargecurrent)&&((piv_current - jt_chargecurrent) > 372))
			{
	            if(power.pwm > 0)
	            {
	                power.pwm --;                       //���������趨����
	            }
			}
			else  if((piv_current <jt_chargecurrent)||((piv_current - jt_chargecurrent) < 360))
			{
	            if(power.pwm < 1000)
	            {
	                power.pwm ++;                       //����С���趨����
	            }
			}
			// �ڳ��11��Сʱ�󣬵����ʾ����ѱ�
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
	    enable_pwm(CHARGE_PWM,power.pwm);   //ִ��pwm����
	}	  	
}
/************************************************
���ܣ��ж��Ƿ����ⲿ��Դ���롣
���룺��
�������
************************************************/
void judge_charge(void)
{
static uint32 time0,time1;
////////////////////�ж�dc���������/////////////////
   if(power.charge_seat == 0)
   {
       if(power.charge_dc == 1)//ԭ��ȷ��DC���е�Դ����
       {
           if(charge_dc.key == 0)
	       {
	           if((giv_sys_time - time0) > 2000)   //û�в����Դ1������
		       {
		           time0 = giv_sys_time;
			       power.charge_dc = 0;    //ȷ��û�в����Դ
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
	           if((giv_sys_time - time0) > 1000)    //�����Դ0.2������
		       {
		           time0 = giv_sys_time;
			       power.charge_dc = 1;     //  ȷ���в����Դ
			       power.step = 0;         //��ʼ������
		       }
	       }
	       else
	       {
	           time0 = giv_sys_time;
	       }
       }
   }
////////////////////�жϻس����������/////////////////
   if(power.charge_dc == 0)
   {
       if(power.charge_seat == 1)//ԭ��ȷ���س����е�Դ����
       {
           if(charge_seat.key == 0)
	       {
	           if((giv_sys_time - time1) > 10000)   //û�в����Դ1������
		       {
		           time1 = giv_sys_time;
			       power.charge_seat = 0;    //ȷ��û�в����Դ
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
	           if((giv_sys_time - time1) > 2000)    //�����Դ0.1������
		       {
		           time1 = giv_sys_time;
			       power.charge_seat = 1;     //  ȷ���в����Դ
				   power.step = 0;         //��ʼ������
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
���ܣ��¶Ȳ�����1���Ӳ���һ�Σ�����10�μ���ƽ��ֵ���棬
*************************************************************/
void  sampling_temp_voltage(void)
{
static uint32 l_linshi_current = 0,               //���ֵ���ʱ����
              r_linshi_current = 0,               //���ֵ���ʱ����
		      m_linshi_current = 0,               //��ɨ����ʱ����
		      dust_linshi_current = 0,            //�ҳ��������ʱ����
              battery_linshi_voltage = 0,         //��ص���ʱ��ѹ          
		      battery_linshi_temp = 0,            //��ص���ʱ�¶�
              battery_linshi_chargecurrent = 0;//���5���ڵ�ƽ������
			  
static uint16 sampling_number = 0;                //�������� 

static uint32 l_linshi_current_1s = 0,               //���ֵ���ʱ����
              r_linshi_current_1s = 0,               //���ֵ���ʱ����
		      m_linshi_current_1s = 0,               //��ɨ����ʱ����
		      dust_linshi_current_1s = 0,            //�ҳ��������ʱ����
              battery_linshi_voltage_1s = 0,         //��ص���ʱ��ѹ          
		      battery_linshi_temp_1s = 0,            //��ص���ʱ�¶�
              battery_linshi_chargecurrent_1s = 0;//���1���ڵ�ƽ������
			  
static uint16 sampling_number_1s = 0;                //��������

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
      battery_voltage = (uint16) (battery_linshi_voltage/25000);    //���1��ĵ�ѹ
      battery_temp = (uint16) (battery_linshi_temp/25000);       //���1����¶�
      l_current = (uint16) (l_linshi_current/25000);          //���ֵ���
      r_current = (uint16) (r_linshi_current/25000);          //���ֵ���
      dust_current = (uint16) (dust_linshi_current/25000);       //�ҳ��������
      m_current = (uint16) (m_linshi_current/25000);          //��ɨ����
	  battery_chargecurrent = (uint16)(battery_linshi_chargecurrent/25000);//���ƽ������
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
      battery_voltage_1s = (uint16) (battery_linshi_voltage_1s/5000);    //���1��ĵ�ѹ
      battery_temp_1s = (uint16) (battery_linshi_temp_1s/5000);       //���1����¶�
      l_current_1s = (uint16) (l_linshi_current_1s/5000);          //���ֵ���
      r_current_1s = (uint16) (r_linshi_current_1s/5000);          //���ֵ���
      dust_current_1s = (uint16) (dust_linshi_current_1s/5000);       //�ҳ��������
      m_current_1s = (uint16) (m_linshi_current_1s/5000);          //��ɨ����
	  battery_chargecurrent_1s = (uint16)(battery_linshi_chargecurrent_1s/5000);//���ƽ������
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
����: ��ȡ����¶�
����: ��ص��¶�ֵ
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
