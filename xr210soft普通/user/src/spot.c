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

//======================˽�ж���================================
//======================ȫ�ֱ���================================
//======================˽�б���================================
//======================ȫ�ֺ���================================
void Do_Spot(void);
void Init_Spot(void);
//======================˽�к���================================
uint8 Spot_YaoKong_Manage(void);	
uint32  read_bump1(void);
//======================����ʵ��================================
/*****************************************************************
���ܣ���ʼ��������ɨ�ĳ���
*****************************************************************/
void Init_Spot(void)
{					 
    /******��ʼ����ʾ***********/
    clr_display();
    Dis_On = TRUE;              //��ʾ��־��
    Dis_PrOn = TRUE; 			//ԤԼ������
    Dis_HourOn = TRUE; 		    //Сʱ��
    Dis_MinuteOn = TRUE; 		//������
    Dis_ColOn = TRUE; 		    //ʱ�����
    Dis_WeekOn = TRUE; 		    //������
    Dis_SpeedOn = TRUE; 		//�ٶ���
    Dis_PowerOn = TRUE; 		//���ͼ����
    Dis_ChargeOn = FALSE ;		//���ͼ����
	Dis_ColGlint = TRUE ;	    //��ʾʱ���Ϊ��˸
	Dis_SpeedGo = TRUE; 
	Dis_SpGlint = TRUE;
	/******��ʼ�����õ�ֵ********************/
	gsv_work_mode.work_mode = SPOT;	 //����ģʽΪ��ɨ
	mode.mode = SPOT;
	clr_ram();
	
    Enable_Sweep();
	Enable_earth();
	Enable_wall();
	enable_hwincept(); //���������յ�Դ
	Reset_Speed_Send();//������ⷢ��  
	 
	Display_Real();
	Init_Action();
	//==============���ǽ������
    w_l.sign=FARN;
    w_lm.sign=FARN;
    w_r.sign=FARN;
    w_rm.sign=FARN;
    w_m.sign=FARN;
	WriteWorkState();
}

/******************************************************************
����ɨʱ��ң��������
******************************************************************/
uint8 Spot_YaoKong_Manage(void) 
{
    if(gsv_yaokong.sign == 1) /////��ң������������
	{
	    gsv_yaokong.sign = 0;
		switch(	gsv_yaokong.key)
		{
		    case YAOKONG_ESC:	
			case YAOKONG_SPOT_FRI:	   
			    Init_Cease();
				Enable_Beep();
			    return 1;
			case YAOKONG_DOCKING_SUN:
			    Init_Docking();
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
		    //=================����uv����uvָʾȡ��
	        case YAOKONG_UV_WED:
				Init_Border();
				//Init_Cease();
		    	//Turn_UV();
				//Enable_Sweep();
				Enable_Beep();
				return 1;
		}
	}
	return 0;
}
/*****************************************************************
���ܣ�������ɨ����
*****************************************************************/
void Do_Spot(void)
{
  static uint8 piv_out;  //�����Ƿ�����չ����1Ϊ����չ����0Ϊ������С
  static uint8 piv_left; //�����Ƿ�����ת��1Ϊ����ת��0Ϊ����ת
  uint32 m ;
  		
  //=============��ص�ѹ��ʱ�����Զ��س�
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
  //============���ʼ����ʱ�ж��������Ƿ���أ������˳�ɨ��ģʽ
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
  //===========ң������������		     
  if(Spot_YaoKong_Manage() == 1)
  {
      return;
  }
  //===========������������������е�����һ��������
  if((key1.sign == 0)  || (key2.sign == 0) || (key3.sign == 0))
  {
      key1.sign = 1;
	  key2.sign = 1;
	  key3.sign = 1;
	  Init_Cease();	 
	  Enable_Beep();
	  return;
  }
  //===========��ʾ��ʾ��������
  Display_Real();
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&m_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw); 	
  clr_hw_effect(&dummy_wall); 
  //=========������һЩ��Ҫ�������ж�========
  Io_Driver();	 
  Dis_Power = Get_Dispower();	

  //=========��ң����ʱ����ֹͣ����ң�ط����˶�
  if(YaoKongFangXiang())
  {
      return;
  }
  //========�Ƿ�����Ҫ����������
  if(Action_Protect())
  {
     return;
  }
  m=read_bump1();//���ϼ��
    switch(mode.bump) //������������Լ�����
    {
  //---------------------------
	    case 1://E_L		   
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(7,angle360)) //�������ת������ת����߳������
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return ; 
		            }
				    if((m==1))	  // mode.bump=mode.step_bp=m; do not action
				    {
				     return ;
				    }		   
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(7,angle30))   //�����������
				    {
				        mode.step_bp++;
				    }
				    return ;	 
			    case  2:
			        if(do_action(8,angle15))   //�ұ߰���������
				    {
				        mode.step_bp++;
				    }	  
				    read_earth();
				    return ;
		        case  3:		
	                if(do_action(2,angle30))   //ȫ��������ת
		            {
		                mode.bump = 0;
		            }
				    read_earth();	
					return;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  ;
 //--------------------------------------------
	    case 4://E_R: 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(8,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return ; 
		            }
				    if((m==4))
				    {
				         return ;
				    }		   
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(8,angle30))
				    {
				        mode.step_bp++;
				    }
				    return ;	 
			    case  2:
			        if(do_action(7,angle15))
				    {
				        mode.step_bp++;
				    }	
				    read_earth();
				    return ;
		        case  3:		
	                if(do_action(1,angle30))
		            {
		                mode.bump = 0;
		            }
				    read_earth();	
		            return;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  ;
//-------------------------------mode.bump				 
		case 2://E_LM 
	    case 3://E_RM: 	 
	    case 5://L_BUMP:
	    case 6://R_BUMP:	
	    case 7://W_L:
	    case 8://W_LM:	
	    case 9://W_M:		 		    
	    case 10://up_hw			 	
	    case 11://l_hw	 	
	    case 12://m_hw
	    case 13://r_hw	
	    case 14://W_R:	
	    case 15://W_RM:	
            switch(mode.step_bp)
            {
                case 0:
	                if(do_action(4,angle360))
		            {
					    giv_sys_err = mode.bump;
						Init_Err();
					    return ; 
		            }		
				    read_sideearth();
		            if(m == 0)
		            {
		                stop_rap();
			            mode.step_bp = 1;
		            }
		            return ;
				case 1:
	                if(do_action(1,angle30))
		            {
		                mode.bump = 0;
		            }		 
				    read_earth();	
		            return;
	            default :
	                mode.bump = 0;
            }
			return ;
	    default :
	        mode.bump = 0;
	        break;
	} //����  switch(mode.bump)
//======================================================
  switch (mode.step)
  {
      case 0:
	      left(angle360);//ԭ��תȦ1Ȧ
		  mode.step++;
		  piv_out = 1;
		  piv_left = 1;
		  break;
	  case 1:
	  case 2:
	  case 3:
	  case 4:
	      if((!l_rap.sign)&&(!r_rap.sign))
		  {   
		      action.step++;
		      if(piv_out == 0)
			  {
	              Init_Cease();
	              return;
			  }
		      if(piv_left != 0)	//ȫ��������ת
			  {
		          enable_rap(BACK,(angle180-mode.step*angle45), FRONT,(angle180+mode.step*angle45)); //1940
			  }
			  else	//ȫ��������ת
			  {
			      enable_rap(FRONT,(angle180+mode.step*angle45),BACK,(angle180-mode.step*angle45) ); 
			  }
			  mode.step++;   
		  }
		  break;
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
	      if((!l_rap.sign)&&(!r_rap.sign))
	      {
		      action.step++;
		      if(piv_left != 0)
			  {
		          enable_rap(FRONT, (mode.step-4)*angle45, FRONT, (mode.step-4)*angle45+angle360);
			  }
			  else
			  {
			      enable_rap(FRONT, (mode.step-4)*angle45+angle360, FRONT, (mode.step-4)*angle45);
			  }
			  if(mode.step == 17)
			  {
			      piv_out = 0;
			  }
			  if(piv_out != 0) //����չ��
			  {
			      mode.step++;
			  }
			  else              //������£
			  {
			      mode.step--;
			  }
		  } 
  }
}
/******************************************
��ȡ��ײ��Ϣ
*******************************************/
uint32  read_bump1(void)
{
		if((e_l.sign == FARN))
		{
			if((mode.bump > 4) || (mode.bump == 0))	    //��ؼ�����
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
		if (1 == l_bump.key)
		{
			if((mode.bump == 0))	 //��ǰײ
			{
				mode.bump=5;//L_BUMP;
				mode.step_bp=0;
				stop_rap();
			}
			return 5;//L_BUMP;
		}
		if (1 == r_bump.key)
		{
			if((mode.bump == 0))	 //��ǰײ
			{
				mode.bump=6;//R_BUMP;
				mode.step_bp=0;
				stop_rap();
			}
			return 6;//R_BUMP;
		}      
		if((w_lm.sign==NEAR))
		{
			if((mode.bump == 0))		 //����ǽ�쿿��ǽ
			{
				mode.bump=8;//W_LM;
				mode.step_bp=0;
				stop_rap();
			}
			return 8;//W_LM;
		}
		if((w_m.sign==NEAR))
		{
			if((mode.bump == 0))	   //��ǽ�쿿��ǽ
			{
				mode.bump=9;//W_M;
				mode.step_bp=0;
				stop_rap();
			}
			return 9;//W_M;
		}	   
		if((w_rm.sign==NEAR))
		{
			if((mode.bump == 0))		//����ǽ�쿿��ǽ
			{
				mode.bump=15;//W_RM;
				mode.step_bp=0;
				stop_rap();
			}
			return 15;//W_RM;
		}
		return 0;
}
