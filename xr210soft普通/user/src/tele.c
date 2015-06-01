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

//======================˽�ж���==================================
//======================ȫ�ֱ���==================================
//======================˽�б���==================================
//======================ȫ�ֺ���==================================	  
void Init_Tele(void); 
void Do_Tele(void);
//======================˽�к���==================================	
//======================����ʵ��==================================
/******************************************************************
���ܣ���ʼ��ң�س���
******************************************************************/
void Init_Tele(void)
{
    /******��ʼ����ʾ***********/
    clr_display();
    Dis_On = TRUE;               //��ʾ��־��
    Dis_PrOn = TRUE; 			 //ԤԼ������
    Dis_HourOn = TRUE; 		     //Сʱ��
    Dis_MinuteOn = TRUE; 		 //������
    Dis_ColOn = TRUE; 		     //ʱ�����
    Dis_WeekOn = TRUE; 		     //������
    Dis_SpeedOn = TRUE; 		//�ٶ���
    Dis_PowerOn = TRUE; 		//���ͼ����
    Dis_ChargeOn = FALSE ;		//���ͼ����
	Dis_ColGlint = TRUE ;	    //��ʾʱ���Ϊ��˸
	Dis_SpeedGo = TRUE; 
	
	/******��ʼ�����õ�ֵ********************/
	mode.mode = TELE;
	clr_ram();

    Enable_Sweep();

	Enable_earth();
	Enable_wall();
	enable_hwincept();         //���������յ�Դ
	Reset_Speed_Send();        //������ⷢ�� 
	 
	Display_Real();
	Init_Action();
	WriteWorkState();
}
/*******************************************************************
ң����ң�ع�������
*******************************************************************/
void Do_Tele(void)
{
  uint32 m;
  
  //��ص�ѹ��ʱ�����Զ��س�
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

  //���ʼ����ʱ�ж��������Ƿ���أ������˳�ɨ��ģʽ
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
  //������������������е�����һ��������
  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
  {
      key1.sign = 1;
	  key2.sign = 1;
	  key3.sign = 1;
	  Init_Cease();	 
	  Enable_Beep();
	  return;
  }

  //��ʾ��ʾ��������
  Display_Real();

  //�����ֵ���
  comm_rap();
				  
  clr_hw_effect(&l_hw);
  clr_hw_effect(&m_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw); 	
  clr_hw_effect(&dummy_wall); 
  
  Io_Driver();	 
  Dis_Power = Get_Dispower();	

  if(gsv_yaokong.effect == 1) //��ң������������
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
  //��ң�صĹ��������ϰ��źź������źŵĴ���
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
   //�Ը��������Ķ���
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
