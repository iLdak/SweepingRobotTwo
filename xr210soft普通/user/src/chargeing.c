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

void Init_Chargeing(void); 
void Do_Chargeing(void);
void Init_Chargeing(void)
{
   			 
    /******��ʼ����ʾ***********/
    clr_display();
    Dis_On = TRUE;            //��ʾ��־��
    Dis_HourOn = TRUE; 		//Сʱ��
    Dis_MinuteOn = TRUE; 		//������
    Dis_ColOn = TRUE; 		//ʱ�����
    Dis_WeekOn = TRUE; 		//������
    Dis_PowerOn = TRUE; 		//���ͼ����
    Dis_ChargeOn = TRUE ;		//���ͼ����
	Dis_ColGlint = TRUE ;	    //��ʾʱ���Ϊ��˸	
	Dis_PowerGlint = TRUE;
	/******��ʼ�����õ�ֵ********************/
	mode.mode = CHARGEING;
	clr_ram();
	
	Disable_earth();
	Disable_wall();
    
	disable_hwincept();//���������յ�Դ
	Set_Speed_Send();  //������ⷢ��  
	power.pwm = 0;
	Enable_Beep();
	disable_pwm(CHARGE_PWM);
	power.step = 0;
	WriteWorkState();
}
/******************************************************************
���ܣ�ִ�г��
******************************************************************/
void Do_Chargeing(void)
{
 static uint8 piv_mode = 0;
 static uint8 piv_seat = 0;
	if( POWER_ready == FALSE )
	{
	   return;
	}
	POWER_ready = FALSE;
	if(power.charge_dc != 0)
	{			   	
	    piv_seat = 0;
	}
    if(power.charge_seat != 0)
	{					
	    piv_seat = 1;
	    if((gsv_yaokong.sign == 1))
		{		
			Enable_Beep();			 
		    gsv_yaokong.sign = 0;
		    if(((gsv_yaokong.key == YAOKONG_AUTO_THU	)||(gsv_yaokong.key == YAOKONG_SPOT_FRI	) || (gsv_yaokong.key == YAOKONG_UV_WED	)
		    || (gsv_yaokong.key == YAOKONG_LEFT	)||(gsv_yaokong.key == YAOKONG_RIGHT	)
		    || (gsv_yaokong.key == YAOKONG_FRONT)||(gsv_yaokong.key == YAOKONG_BACK	)))
		    {				
			    if(mode.step == 0)
				{
			        mode.step = 1;
					if(gsv_yaokong.key == YAOKONG_AUTO_THU)
					{
					    piv_mode = SWEEP;
					}
					else
					{
					    if(gsv_yaokong.key == YAOKONG_SPOT_FRI)
					    {
					        piv_mode = SPOT;
					    }
					    else
					    {
				            piv_mode = CEASE;
					    }
					}
				    Enable_earth();
				    enable_hwincept();       //���������յ�Դ
				    Reset_Speed_Send();      //������ⷢ��
				}
		    }
		}
	    //������ԤԼʱ�䵽����������ɨ��ģʽ  //��ʱ������Ч
	    if(Check_PreengageTime() ||
		  (((PreengageDate&0x80) != 0)&&(giv_BatteryChargeForbid != 0)))
	    {   
		    if(mode.step == 0)
			{
			    mode.step = 1;
				piv_mode = SWEEP;
				Enable_earth();
				enable_hwincept();            //���������յ�Դ
				Reset_Speed_Send();           //������ⷢ��
			}
	    }
	}
	//��ʾʵʱʱ�䡣
	Dis_Week = (uint8)((Rtc_time%604800) / 86400);
    Dis_Minute = (uint8)((Rtc_time % 3600) / 60);
    Dis_Hour = (uint8)((Rtc_time % 86400) / 3600) ;
	
	comm_rap();
	switch(mode.step)
	{
	    case 0:
		    break;
		case 1:
		    if(do_action(4,angle60))
			{
			    mode.step = 2;
			}
			return;
		case 2:
		    if(do_action(1,angle180))
			{
			    mode.step = 3;
			}
			return ;
		case 3:
		    if(do_action(3,angle90))
			{
			    if(piv_mode == CEASE)
				{
				    Init_Cease();
				}
				if(	piv_mode == SWEEP)
				{
				    Init_Sweep();
				}
				if(	piv_mode == SPOT)
				{
					Init_Spot();
				}
			}
			return;
		default :
		    mode.step = 0;
	}
    if((power.charge_dc == 0)&&(power.charge_seat == 0))
	{
	    disable_pwm(CHARGE_PWM);
		enable_hwincept();
		DelayMs(1000);
		if((piv_seat == 1)&&((m_hw.sign != 0)||(up_hw.sign != 0)))
	    {
		    Init_Docking();
		}
		else
		{
	        Init_Cease();
		}
		return ;
	}		   
	
	//====================������================================
	ChargeControl();	
    Dis_Power = Get_Dispower();
}
