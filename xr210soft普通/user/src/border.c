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

		if(action.sign==0)//�ڵ�ǰ����ģʽ�£������˶�������������
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
	if(ybs() == 1)//�˴�����ִ���ر�ɨ
		return 1;

	Enable_wall();
	return 0;
}


/******************************************************************/
void Init_Border(void)
{					 
    /******��ʼ����ʾ***********/
    clr_display();
    Dis_On = TRUE;            //��ʾ��־��
    Dis_PrOn = TRUE; 			//ԤԼ������
    Dis_HourOn = TRUE; 		//Сʱ��
    Dis_MinuteOn = TRUE; 		//������
    Dis_ColOn = TRUE; 		//ʱ�����
    Dis_WeekOn = TRUE; 		//������
    Dis_SpeedOn = TRUE; 		//�ٶ���
    Dis_PowerOn = TRUE; 		//���ͼ����
    Dis_ChargeOn = FALSE ;		//���ͼ����
	Dis_ColGlint = TRUE ;	    //��ʾʱ���Ϊ��˸
	Dis_AuGlint = TRUE;
	
	Dis_SpeedGo = TRUE; 
	/******��ʼ�����õ�ֵ********************/
	gsv_work_mode.work_mode = BORDER;	 //����ģʽΪ��ɨ
	mode.mode = BORDER;
	clr_ram();
    Enable_Sweep();
	Enable_earth();
	Enable_wall();
	enable_hwincept();//���������յ�Դ
	Reset_Speed_Send();  //������ⷢ��  
	Display_Real();
	Init_Action();
	mode.step = 0xfe;
	WriteWorkState();
}

/*****************************************************************
���ܣ���ǽ��ɨ����
*****************************************************************/
void Do_Border(void)
{
//static uint8 y = 0;//����ײ�ź�ʱ y = 1;����Ϊ0
//static uint8 z = 0;//��ִ������ɨʱ z = 1,����Ϊ0

   //NVIC_GenerateSystemReset();
  //-----------��ص�ѹ��ʱ�����Զ��س�
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
  
  //-----------���ʼ����ʱ�ж��������Ƿ���أ������˳�ɨ��ģʽ
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
  //----------ң������������		     
  if(Sweep_YaoKong_Manage() == 1)
  {
      return;
  }

  //----------������������������е�����һ��������
  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
  {
      key1.sign = 1;
	  key2.sign = 1;
	  key3.sign = 1;
	  Init_Cease();	 
	  Enable_Beep();
	  return;
  }
  //-----------��ʾ��ʾ��������
  Display_Real();
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&m_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw); 	
  clr_hw_effect(&dummy_wall); 
  //----------������һЩ��Ҫ�������ж�-----------------
  Io_Driver();	 
  Dis_Power = Get_Dispower();	

	if((l_rap.rap < Speed)&&(r_rap.rap < Speed)&&(mode.bump == 0))
	{
	    l_rap.rap = (l_rap.rap*3)/2;
	    r_rap.rap = (r_rap.rap*3)/2;
	}

     //-----------�ڻس���������ǽ�������������Ϊ��
  if((up_hw.sign != 0) && ((up_hw.data&0x8f)==0))
  {
      Wall_SensitivityHigh();
	  mode.angle_wall = 0;
  }
  else
  {
      Wall_SensitivityLow();
  }

  //-------------��ң����ʱ����ֹͣ����ң�ط����˶�
  if(YaoKongFangXiang())
  {
      return;
  }
  //-------------�Ƿ�����Ҫ����������
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


