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

//======================˽�ж���=================================
//======================ȫ�ֱ���=================================
//======================˽�б���=================================
//======================ȫ�ֺ���=================================	  
void Init_Err(void); 
void Do_Err(void);
//======================˽�к���=================================	
//======================����ʵ��=================================
/******************************************************************
���ܣ���ʼ��������
******************************************************************/
void Init_Err(void)
{
uint16 m = 0;
    WriteBakErr(giv_sys_err);
	switch(giv_sys_err)
	{
	    case 1:
		case 2:
		case 3:
		case 4:
		    m = 1;
			break;
		case 16:   
		    m = 2;
			break;
		case 18:   
		    m = 3;
			break;
		case 31:   
		case 32:   
		    m = 4;
			break;	
		case 33:   
		    m = 5;
			break;
		default :
		    Init_Cease();
			return ;	  

	}
	giv_sys_err_bak = 	giv_sys_err;
	giv_sys_err = 0;
    /******��ʼ����ʾ***********/
    clr_display();	
	/******��ʼ�����õ�ֵ********************/
	clr_ram();
	
	Disable_earth();
	Disable_wall();
	disable_hwincept();       //��ֹ������յ�Դ
	Set_Speed_Send();         //��ֹ���ⷢ��  
	mode.mode = ERR;  
    Dis_On = TRUE;            //��ʾ��־��
	
    Dis_HourOn = TRUE; 		  //Сʱ��
    Dis_MinuteOn = TRUE; 	  //������ 
	Dis_ErrOn = TRUE;
	Dis_ErrGlint = TRUE;
	Dis_Hour = 140;
	Dis_Minute = m;	 			
	WriteWorkState();
}
/******************************************************************
���ܣ�ִ�г��
******************************************************************/
void Do_Err(void)
{
 ///ң������������		     
  if(Sweep_YaoKong_Manage() == 1)
  {
	  giv_sys_err = 0;
      return;
  }
  ///������������������е�����һ��������
  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
  {
      key1.sign = 1;
	  key2.sign = 1;
	  key3.sign = 1;
	  Init_Cease();	 
	  Enable_Beep();
	  giv_sys_err = 0;
	  return;
  }
  if((giv_sys_time % 100000) < 1000)
  {
	  Enable_Beep();
  }
}
