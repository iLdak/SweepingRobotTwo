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

//======================˽�ж���===============================
//======================ȫ�ֱ���===============================
//======================˽�б���===============================
//======================ȫ�ֺ���===============================
void Init_Sweep(void);
void Do_Sweep(void);		   
uint8 Sweep_YaoKong_Manage(void) ;
//======================˽�к���===============================	
//======================����ʵ��===============================
/*****************************************************************
���ܣ���ʼ���Զ���ɨ
*****************************************************************/
void Init_Sweep(void)
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
	gsv_work_mode.work_mode = SWEEP;	 //����ģʽΪ��ɨ
	mode.mode = SWEEP;
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
/******************************************************************
���ܣ�����ɨʱ�Ĺ���
******************************************************************/
void Do_Sweep(void)
{

static uint8 y = 0;//����ײ�ź�ʱ y = 1;����Ϊ0
//static uint8 z = 0;//��ִ������ɨʱ z = 1,����Ϊ0

   //NVIC_GenerateSystemReset();
  //-----------��ص�ѹ��ʱ�����Զ��س�
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

  //----------�ڿ���ǽ��ʱ��������
  if((w_l.high_sign == NEAR)||(w_lm.high_sign == NEAR)||(w_m.high_sign == NEAR)||
     (w_r.high_sign == NEAR)||(w_rm.high_sign == NEAR))
  {
      if((l_rap.rap > 1000)||(r_rap.rap > 1000))
		{
		    l_rap.rap = (l_rap.rap*2)/3;
		    r_rap.rap = (r_rap.rap*2)/3;
		}
  }
  else
  {
      if((l_rap.rap < Speed)&&(r_rap.rap < Speed)&&(mode.bump == 0))
		{
		    l_rap.rap = (l_rap.rap*3)/2;
		    r_rap.rap = (r_rap.rap*3)/2;
		}
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
  
  //--------------����������ɨ�ص�·��
  if(action_bump())
  {
      y=1;
      return;
  } 
//============================������������ɨ��ʱ��·��===========================
  //--------------------------�й���ײ����ײ����·��ֻ����ֱ�ߺ�Z���ζ���
  if(1 == y)	
  { 
      y = 0;
	  if((mode.step==0xfe) || (mode.step==0xff))
	  {
	      mode.step = 0;
	  }
	  do
      {
          mode.step++;
	      if(22 < mode.step)
	      {
	          mode.step = 0;
	      }
      }
      while(3 != zhulujing[mode.step]);
  } 
  	  
  if(mode.step==0xfe)	  //���ʼ��ʱ��
  {
      if(!do_action(3,METE))
      {
          return;
      }
          mode.step++;
  }
  if(mode.step ==0xff)	 //��ͷ
  {
      if(!do_action(1,angle180))
      {
          return;
      }
      mode.step++;
  }

 
  if((18 == zhulujing[mode.step]) || (19 == zhulujing[mode.step]))
  {	//mode.step=1,5,9,11,17,21
	  mode.sum_bump = 0;
	  mode.fangxiang = 0;
  }
  if((18 == zhulujing[mode.step]) || (19 == zhulujing[mode.step])||(mode.sum_bump > 10))
  {	//mode.step=1,5,9,11,17,21
	  mode.ybs_sign = 1;
	  mode.angle_wall=angle720;	  
	  mode.long_wall = 0;
	 
  }
  else
  {
	  mode.ybs_sign = 0;
	  Enable_wall();
  }
 
if(do_action(zhulujing[mode.step],zhulujinglong[mode.step]))
 {
     mode.step++;
     if(mode.step > 22)
     {
         mode.step=0;
     }
 }
}
/******************************************************************
����ɨʱ��ң��������
******************************************************************/
uint8 Sweep_YaoKong_Manage(void) 
{
    if(gsv_yaokong.sign == 1) /////��ң������������
	{
	    gsv_yaokong.sign = 0;
		switch(	gsv_yaokong.key)
		{
		    case YAOKONG_ESC:
			case YAOKONG_AUTO_THU:
			    Init_Cease();
				Enable_Beep();
			    return 1;
			case YAOKONG_SPOT_FRI:	
			    Init_Spot();
				Enable_Beep();
			    return 1;
			case YAOKONG_DOCKING_SUN: 
			    Init_Docking();
				Enable_Beep();
			    return 1;
	        case YAOKONG_V_SAT:
		        Change_Speed();
				Enable_Beep();
		        return 0;
		    //////����uv����uvָʾȡ��
	        case YAOKONG_UV_WED:
		        //Turn_UV();
				//Enable_Sweep();
				//Init_Border();
				Init_Cease();
				Enable_Beep();
				return 1;
		}
	}
	return 0;
}
/***************************************************
���ܣ�����ң������ǰ�����Ұ���֮�󣬻�������ң�����ķ����˶�
      ��Ҫ���������ײ
***************************************************/
uint8 YaoKongFangXiang(void)
{
static uint8 piv_yaokong = 0;
    //����ң�ص�ʱ������ײ�źţ���������
    if(piv_yaokong == 1)
	{
        if(read_bump() != 0)        //��������ײ
        {
			    stop_rap();
			    mode.step = 0;
			    mode.bump = 0;
				mode.step_yaokong = 0;
				piv_yaokong = 0;
	            return 0;
        }
	}
    if((gsv_yaokong.effect == 1)||(gsv_yaokong.sign == 1))
	{
	    gsv_yaokong.sign = 0;
		if(read_bump() != 0)
		{			   		 
			piv_yaokong = 0;
		    mode.step_yaokong = 0;
		    return 0;
		}
	    switch(gsv_yaokong.key)
		{
	        case YAOKONG_LEFT:
			    if(mode.step_yaokong != 1)
				{		   
				    stop_rap();
				    mode.step_yaokong = 1;
				}
				break;
	        case YAOKONG_RIGHT:	   
			    if(mode.step_yaokong != 2)
				{		   
				    stop_rap();
				    mode.step_yaokong = 2;
				}
				break;
	        case YAOKONG_FRONT:		 
			    if(mode.step_yaokong != 4)
				{		   
				    stop_rap();
				    mode.step_yaokong = 4;
				}
				break;
	        case YAOKONG_BACK:	   
			    if(mode.step_yaokong != 3)
				{		   
				    stop_rap();
				    mode.step_yaokong = 3;
				}
				break;
			default :
			    piv_yaokong = 0;
				return 0;
		}
		if(piv_yaokong == 0)
		{
		    stop_rap();
			piv_yaokong = 1;
		}
	}
    else 
	{
	    if((piv_yaokong != 0)&&(mode.step_yaokong != 5))
		{
			stop_rap();
			mode.step_yaokong = 5;
		}
	}
    if(piv_yaokong != 0)
	{			 
	    switch(mode.step_yaokong)
	    {
		    case 0:
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
		    case 4:
			    do_action(3,METE_TWO);
		        break;
		    case 5:
			    if(do_action(3,angle120))
		        {
	                piv_yaokong = 0;
			        mode.bump = 0;
		            mode.step_yaokong = 0;
			        mode.step = 0;
			    }
	    }
	}
	return piv_yaokong;
}

