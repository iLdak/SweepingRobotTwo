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

//=========================˽�ж���=========================
const uint16 M_PROTECTCURRENT =   466;//292;//��ɨ�������� 500MA,���㹫ʽ  1A*0.47R*4096/3.3  
const uint16 M_STOPCURRENT =   788;//��ɨֹͣ���� 1350MA,���㹫ʽ  1A*0.47R*4096/3.3
const uint16 DUST_PROTECTCURRENT =   292; //�ҳ�������� 500MA						   
const uint16 WHEEL_PROTECTCURRENT =  292;//234; //�����ֵ��� 400MA

const uint16 BATTERY_PREOTECTTEMP = 815;  //��ر����¶�  60��
//==========================ȫ�ֱ���=========================

MODE mode;
WORK_MODE gsv_work_mode;	

uint8 read_sideearth(void);	
uint8 giv_sys_err;         //ϵͳ�Ĵ������	  
uint8 giv_sys_err_bak;         //ϵͳ�Ĵ������
//����   0:ֹͣ  1:ԭ����ת  2ԭ����ת 3ǰ��   4����   5��ת1  6����ת2  ������18��ת14	��19��������
/***********************************************************
����������ʱ����·����
ǰ��1�ף�ɨ6Ȧ��ǰ��1�ף���ת90�ȣ�ǰ��1.2�ף���ת90�ȣ�
ǰ��1�ף�ɨ6Ȧ��ǰ��1�ף���ת90�ȣ�ǰ��1.2�ף���ת90�ȣ�
ǰ��1�ף�ɨ6Ȧ����ת90�ȣ�ǰ��1.2�ף�ǰ��2�ף� ��ת90�ȣ�
ɨ6Ȧ��ǰ��1�ף���ת90�ȣ�ǰ��1.2�ף���ת90�ȣ�ǰ��1�ף�
ɨ6Ȧ��ǰ��1�ף���ת90�ȣ�ǰ��1.2�ף���ת90�ȣ�ǰ��1�ף�
ɨ6Ȧ����ת135�� ��������ͷ�ٿ�ʼ
************************************************************/
uint8 zhulujing[]={
              3,         19,        3,            2,             3,            18,
			  
              3,         1,         3,            19,            21,           19,
			  
			  3,         1,         3,            2,             3,            19,
			  
			  3,         1,         3,            18,            21,           0}; //��·������
uint32 zhulujinglong[]={
              METE,      15,        METE,         angle60,       METE*4,      15,
			  
              METE_H7,   angle30 ,  METE_5,       15,            2,            15,
			  
              METE,      angle90,   METE_H7,      angle30,       METE_H5,      15,
			  
              METE,      angle45,   METE*4,      15,            2};//��·���ľ���
/******************************************************
������ײ�����ת�ĽǶ�����ײ�����Ĺ�ϵ
*******************************************************/
uint32 angle[52]={angle15,angle15,
			    angle45,  angle15,  angle105, angle15, angle15,
			    angle105, angle75,  angle75,  angle30, angle45,
				angle30,  angle15,  angle15,  angle45, angle15,
				angle15,  angle15,  angle105, angle15, angle15,
				angle105, angle15,	angle30,  angle15, angle60,
				angle15,  angle15,	angle105, angle15, angle15,
				angle45,  angle60,	angle45,  angle15, angle90,
				angle45,  angle75,  angle75,  angle60, angle45,
				angle45,  angle105,	angle45,  angle30, angle30,
				angle15,  angle75,	angle15,  angle60, angle135};//��ײ�������ת�ĽǶ�
/*uint32 qj[52] = { METE,METE,
                METE_TWO, METE_1,   angle360, METE_5,  METE_5,
				METE_0,   angle240, METE_2,   METE_3,  METE_8,
				METE_0,   angle240,	angle240, angle360,METE,
				METE,     METE_8, 	angle300, METE_0,  METE_7,
				angle330, METE_1,   METE_6,   angle270,METE_2,
				METE_5,   angle270, METE_3,   METE_4,  angle360,
				METE,     METE,     METE,     METE_TWO,angle360,
				angle360, angle360, METE_2,   METE_2,  METE_2,
				METE_TWO, METE_TWO, METE_3,	  METE_3,  METE_3,
				angle300, angle300, angle300, METE_1,  METE_1};*/

///////////////////////˽�б���////////////////////////////////////	
//uint8 piv_display;         //ϵͳ��ʾ��Դ����Χ0--3�� 
///////////////////////˽�к���////////////////////////////////////
///////////////////////ȫ�ֺ���////////////////////////////////////
/*****************************************************************
���ܣ���ʼ��·��
*****************************************************************/
void Init_Mode(void)
{
    mode.mode = CEASE;//��ʼϵͳ���ھ�ֹ����״̬
	gsv_work_mode.work_mode = SWEEP;
	gsv_work_mode.uv = ReadBKPUV();				//�ӱ������ж�UV״̬
	gsv_work_mode.speed =  ReadBKPSpeed();		//�ӱ������ж��ٶ�
	if(gsv_work_mode.speed == 1)
	{
		Speed = HIGHSPEED;
	}
	else
	{
		Speed = LOWSPEED;
	}
	if((RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)||
	  (RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET))
    {
	    RCC_ClearFlag();
		mode.mode =  ReadBKPMode();
		Dis_On = TRUE;            //��ʾ��־��
		switch(mode.mode)
		{
			case SWEEP:
			    Init_Sweep();
				break;
			case SPOT:
			    Init_Spot();
				break;
			case TELE:
			    Init_Tele();
				break;
			case DOCKING:
			    Init_Docking();
				break;
			case CHARGEING:
			    Init_Chargeing();
				break;
			case SLEEP:
			    Init_Sleep();
				break;
			case BORDER:
				Init_Border();
				break;
			default :
			    Init_Cease();
		}

    }
	else
	{
	    Do_Test();
        Init_Cease();
	}




}
/*****************************************************************
����: ���й���ģʽ
*****************************************************************/
void Action_Mode(void)
{ 
  switch (mode.mode)
  {    
      /*********************��������״̬*******************/
      case CEASE: 
	      Do_Cease();   
	      break;
	  
	  /*********************������ɨ״̬*******************/
	  case SWEEP:
	      Do_Sweep();
	      break;
	  
	  /*********************�ֲ���ɨ״̬*******************/
	  case SPOT:
	      Do_Spot();    
	      break;
	  
	  /*********************ң��������״̬*****************/
	  case TELE:
	      Do_Tele();
	      break;
	  
	  /*********************�һس���״̬*******************/
	  case DOCKING:
	      Do_Docking();
	      break;
	  
	  /*********************�һس���״̬*******************/
	  case CHARGEING:
	      Do_Chargeing();
	      break;
	  
	  /*********************�һس���״̬*******************/
	  case SLEEP:
	      Do_Sleep();
	      break;
	  
	  /*********************�һس���״̬*******************/
	  case ERR:
	      Do_Err();
	      break;
	  /*********************�ر�ɨ***********************/
	  case BORDER:
		  Do_Border();
		  break;
	  
	  /*********************Ĭ��״̬***********************/
	  default :
	      Init_Cease();	  
  }
}
/**************************************************************************
���ܣ�ȡ��uv��
**************************************************************************/
void Turn_UV(void)
{
    if(gsv_work_mode.uv == FALSE)
	{
	    gsv_work_mode.uv = TRUE;  
	}
	else
	{
	    gsv_work_mode.uv = FALSE;
	}
	WriteWorkState();
}
 
/**************************************************** 
���ܣ��ı���������ٶ�
���룺��
�����Wheel_State = 1;
���أ���
****************************************************/
void Change_Speed(void)
{
    if( gsv_work_mode.speed == 1)
	{
	    gsv_work_mode.speed = 2;
		Speed = LOWSPEED;//750;
		l_rap.rap /= (HIGHSPEED/LOWSPEED);//2;	   
		r_rap.rap /= (HIGHSPEED/LOWSPEED);//2;
	}
	else
	{
	    gsv_work_mode.speed = 1; 
		Speed = HIGHSPEED;//1500;
		l_rap.rap *= (HIGHSPEED/LOWSPEED);//2;	   
		r_rap.rap *= (HIGHSPEED/LOWSPEED);//2;
	}
	WriteWorkState();
}
/*****************************************************************
��ʾʵʱ״̬
*****************************************************************/
void Display_Real(void)
{
uint32 t;
    t = Rtc_time % 604800;		
    Dis_Preengage = (uint8)PreengageDate;	//ԤԼ����	
    Dis_Week = (uint8)(t / 86400);
    Dis_Minute = (uint8)((t % 3600) / 60);
    Dis_Hour = (uint8)((t % 86400) / 3600) ;
	Display_Work_Mode();
}
/*****************************************************************
��ʾ������״̬
*****************************************************************/
void Display_Work_Mode(void)
{					
	Dis_Speed = gsv_work_mode.speed;
	Dis_UvOn = 	gsv_work_mode.uv;
	if(mode.mode == SPOT)
	{
	    Dis_SpOn = TRUE;
		Dis_AuOn = FALSE;
	}
	else if(mode.mode == SWEEP)
	{	
	    Dis_SpOn = FALSE;
		Dis_AuOn = TRUE;
	}
	else
	{		   
	    Dis_SpOn = FALSE;
		Dis_AuOn = FALSE;
	}
}
/*****************************************************************
ʹ�ܻ�����ɨ����
*****************************************************************/
void  Enable_Sweep(void)
{
    gbv_action_uv = gsv_work_mode.uv;
    gbv_action_s_motor = TRUE;
    gbv_action_m_motor = TRUE;
    gbv_action_dust_motor = TRUE;
}
/*****************************************************************
ʹ�ܻ�����ɨ����
*****************************************************************/
void  Disable_Sweep(void)
{
    gbv_action_uv = FALSE;
    gbv_action_s_motor = FALSE;
    gbv_action_m_motor = FALSE;
    gbv_action_dust_motor = FALSE;
}

/******************************************
��ȡ��ײ��Ϣ
*******************************************/
uint32  read_bump(void)
{
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
		
		if((dummy_wall.effect))
		{
		
			       if((mode.bump == 0))
			       {
				           mode.bump=10;
				           mode.step_bp=0;
				           stop_rap();
			       }
			       return 10;
			
		}     
		if((w_l.sign==NEAR)&&(w_l.on==1))
		{
			if((mode.bump == 0))	    //��ǽ�쿿��ǽ
			{
			      mode.bump=7;//W_L;
				  mode.step_bp=0;
				  stop_rap();
			}	  
			return 7;//W_L;
		}
		if((w_lm.sign==NEAR)&&(w_lm.on==1))
		{
			if((mode.bump == 0))		 //����ǽ�쿿��ǽ
			{
			      mode.bump=8;//W_LM;
				  mode.step_bp=0;
				  stop_rap();
			}
			
			return 8;//W_LM;
		}
		if((w_m.sign==NEAR)&&(w_m.on==1))
		{
			if((mode.bump == 0))	   //��ǽ�쿿��ǽ
			{
			      mode.bump=9;//W_M;
				  mode.step_bp=0;
				  stop_rap();
			}
			return 9;//W_M;
		}
		
		if((w_rm.sign==NEAR)&&(w_rm.on==1))
		{
			if((mode.bump == 0))		//����ǽ�쿿��ǽ
			{
			      mode.bump=15;//W_RM;
				  mode.step_bp=0;
				  stop_rap();
			}
			return 15;//W_RM;
		}
		
		if((w_r.sign==NEAR)&&(w_r.on==1))
		{
			if((mode.bump == 0))		//��ǽ�쿿��ǽ		
			{
			      mode.bump=14;//W_R;
				  mode.step_bp=0;
				  stop_rap();
			}	
			return 14;//W_R;
		}				
		return 0;
}

/****************************************************************
�Ƿ��б��������ݣ�����������1�����򷵻�0
****************************************************************/
uint8 Read_Protect(void)
{
static uint32 s_current_time;
    if(dust_current > DUST_PROTECTCURRENT)		//�ҳ������������
    {   
        giv_sys_err = 17;
		Init_Err();
		return 1;	    
    }	
    if(s_current.sign == 0)
	{
	    gbv_action_s_motor = FALSE;
		s_current_time = giv_sys_time;
	}
	if(((giv_sys_time - s_current_time) >  50000)&&((mode.mode == SWEEP)||(mode.mode == DOCKING)))
	{
		gbv_action_s_motor = TRUE;
	}
		   
//=====================�����쳣�ź�====================

    if(m_current > M_PROTECTCURRENT)			//��ɨ��������
    {   		  
		if(mode.abnormity != 16)
	    {
		    mode.abnormity = 16;
		    mode.step_abn = 0;		 
		    stop_rap();
		}
		return mode.abnormity;   
    }	
		 					 
	if(r_current > WHEEL_PROTECTCURRENT)
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 31;
		    mode.step_abn = 0; 
		    stop_rap();
		}
		return mode.abnormity;   
	}
	if(l_current > WHEEL_PROTECTCURRENT)
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 32;
		    mode.step_abn = 0; 
		    stop_rap();
		}
		return mode.abnormity;  	  
	}

	if((l_ring.state == BAD))	   //���ֲ�ת�����
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 19;
		    mode.step_abn = 0;
		    stop_rap();
		}
		return mode.abnormity;
	}

	if((r_ring.state))	   //���ֲ�ת�����
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 20;
		    mode.step_abn = 0;
		    stop_rap();
		}					
		return mode.abnormity;
	}
 
	if (l_lidi.key == 0) 				  //�������
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 22;
		    mode.step_abn = 0;
		    stop_rap();	
//		    Disable_Sweep();	    //�رշ������
		}			   
		return mode.abnormity;
	}

	if (r_lidi.key == 0)				  //�������
	{
		if(mode.abnormity == 0)
	    {
		    mode.abnormity = 23;
		    mode.step_abn = 0;
		    stop_rap();	  
//		    Disable_Sweep();	    //�رշ������
		}		
		return mode.abnormity;
    }	  
	return 0;
}
/*****************************************************************
���ܣ��Ա����źŽ��д���
*****************************************************************/    
uint8 Action_Protect(void)
{
uint8 m;  
uint32 n;
static uint32 piv_abntime;
    m = Read_Protect();
	if(m == 1)
	{
	    return 1;
	}
	if(mode.abnormity != 0)
	{
	    n = read_bump();
	    if((((e_l.sign == FARN)&&(e_lm.sign == FARN))
		 &&((e_rm.sign == FARN)&&(e_r.sign == FARN)))
		 &&((l_lidi.key == 0)&&(r_lidi.key == 0)))  //��������ز��ҵؼ����ʱ����ͣ��
		{
		    Init_Cease();
			return 1;    
		}
		if(( m == 0) && (mode.step_abn != 1))
		{
		    stop_rap();
		    mode.step_abn = 1;
		} 
		if((n != 0)&&(mode.step_abn != 1)&&(mode.step_abn < 7))
		{ 
		    stop_rap();
			mode.step_abn = 7;
		}
		if(mode.abnormity == 16)
		{
	 	    switch (mode.step_abn)
		    {
		        case  0:		   
			        piv_abntime = giv_sys_time;
				    mode.step_abn = 3;
			        return 1;
			    case  1:
				    if(do_action(1, angle15 * (giv_sys_time % 10)))
			        {
				        mode.abnormity = 0;	   
						mode.step = 0;
					    return 0;					
				    }
				    if((giv_sys_time - action.time) > 100000)
				    {  				   
				        stop_rap();
				        mode.step_abn = 3;
				    }
				    return 1;
			    case 3:	   
				    if(do_action(1,angle45))
					{
				        mode.step_abn = 4; 
					} 
				    if((giv_sys_time - action.time) > 30000)
				    {  				   
				        stop_rap();
				        mode.step_abn = 4;	  
				    }
				    break;

			    case 4:	  
				    if(do_action(3,angle360))
					{
				        mode.step_abn = 3; 
					} 
				    if((giv_sys_time - action.time) > 100000)
				    {  				     
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err(); 
				    }
				    break; 
				case 7:	
				    if(do_action(4,angle90))		  
				    {
					    mode.step_abn++;
			            break;
				    }
					if( n == 0)
					{
					    stop_rap();
						mode.bump = 0;
						mode.step_abn = 3;
					}
				    if((giv_sys_time - action.time) > 100000)
				    {  				  
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err();
				    }
				    break;  
				case 8:	
				    if(do_action(1,angle90))		  
				    {
					    mode.step_abn++;
			            break;
				    } 
					if( n == 0)
					{
					    stop_rap();
						mode.bump = 0;
						mode.step_abn = 4;
					}
				    if((giv_sys_time - action.time) > 100000)
				    {  				  
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err();
				    }
				    break;  
				case 9:	
				    if(do_action(4,angle90))		  
				    {
				        giv_sys_err = mode.abnormity;
					    Init_Err();
			            return 1;
				    }  
					if( n == 0)
					{
					    stop_rap();
						mode.bump = 0;
						mode.step_abn = 3;
					}
				    if((giv_sys_time - action.time) > 100000)
				    {  				  
				        stop_rap();
				        giv_sys_err = mode.abnormity;
					    Init_Err();
				    }
				    break;
				default :
				    mode.step_abn = 0;
					return 0;				   
		    }
			if((giv_sys_time - piv_abntime) > 150000)
			{
			    stop_rap();
			    giv_sys_err = mode.abnormity;
				Init_Err();
			}
			return 1;
		}
		else
		{	
		    if(mode.abnormity == 21)
			{	
	 	        switch (mode.step_abn)
		    	{
		        	case  0:	
			        	piv_abntime = giv_sys_time;
				    	mode.step_abn = 2;
			        	return 1;
			    	case  1:
				    	if(do_action(1, angle15 * (giv_sys_time % 10)))
			        	{
				        	mode.abnormity = 0;
							mode.step = 0;
					    	return 0;					
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				   
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
				    	return 1;
					case 2:
			        	if(do_action(4,angle360))
		            	{
				        	mode.step_abn = 3;
			            	return 1;
			        	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
						break;
			    	case 3:
				    	if(do_action(1,angle45))
				    	{
					    	mode.step_abn = 4;
				    	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					
				        	stop_rap();
				        	mode.step_abn = 4;
				    	}
				    	break;

			    	case 4:	 		
			        	if(do_action(3,angle360))
				    	{
					    	mode.step_abn = 5;
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 5;
				    	}
				    	break;
			    	case 5:
				    	if(do_action(2,angle90))
						{
					   		mode.step_abn = 6;
						} 	 
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 6;
				    	}
						break; 
					case 6:	
				    	if(do_action(4,angle180))		  
				    	{
							mode.abnormity = 0;
							mode.step = 0;
			            	return 0;
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
							mode.abnormity = 0;
			            	return 0;
				    	}
				    	break;	 
					case 7:	
				    	if(do_action(4,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	}
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	break;  
					case 8:	
				    	if(do_action(1,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	} 
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 4;
						}
				    	break;  
					case 9:	
				    	if(do_action(4,angle90))		  
				    	{
							mode.abnormity = 0;
							mode.step = 0;
			            	return 0;
				    	}  
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	break;
		    	}  
				if((giv_sys_time - piv_abntime) > 250000)
				{
			    	stop_rap();	
					mode.abnormity = 0;
					mode.step = 0;
			        return 0;
				}
				return 1;
			}
			else
			{  
	 	        switch (mode.step_abn)
		    	{
		        	case  0:	
			        	piv_abntime = giv_sys_time;
				    	mode.step_abn = 2;
			        	return 1;
			    	case  1:
				    	if(do_action(1, angle15 * (giv_sys_time % 10)))
			        	{
				        	mode.abnormity = 0;
							mode.step = 0;
					    	return 0;					
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				   
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
				    	return 1;
					case 2:
			        	if(do_action(4,angle60))
		            	{
				        	mode.step_abn = 3;
			            	return 1;
			        	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  
				        	stop_rap();
				        	mode.step_abn = 3;
				    	}
						break;
			    	case 3:
				    	if(do_action(1,angle45))
				    	{
					    	mode.step_abn = 4;
				    	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					
				        	stop_rap();
				        	mode.step_abn = 4;
				    	}
				    	break;

			    	case 4:	 		
			        	if(do_action(3,angle60))
				    	{
					    	mode.step_abn = 5;
				    	}
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 5;
				    	}
				    	break;
			    	case 5:
				    	if(do_action(2,angle90))
						{
					   	mode.step_abn = 6;
						} 	 
				    	if((giv_sys_time - action.time) > 30000)
				    	{  					 
				        	stop_rap();
				        	mode.step_abn = 6;
				    	}
						break; 
					case 6:	
				    	if(do_action(4,angle90))		  
				    	{
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
			            	return 1;
				    	}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;	 
					case 7:	
				    	if(do_action(4,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	}
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;  
					case 8:	
				    	if(do_action(1,angle90))		  
				    	{
					    	mode.step_abn++;
			            	return 1;
				    	} 
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 4;
						}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;  
					case 9:	
				    	if(do_action(4,angle90))		  
				    	{
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
			            	return 1;
				    	}  
						if( n == 0)
						{
					    	stop_rap();
							mode.bump = 0;
							mode.step_abn = 3;
						}
				    	if((giv_sys_time - action.time) > 100000)
				    	{  				  
				        	stop_rap();
				        	giv_sys_err = mode.abnormity;
					    	Init_Err();
				    	}
				    	break;
		    	}  
				if((giv_sys_time - piv_abntime) > 150000)
				{
			    	stop_rap();
			    	giv_sys_err = mode.abnormity;
					Init_Err();
				}
				return 1;
			}
		}

	}
    mode.abnormity=0;
    return 0;
}

/**************************************
��ת
�������    longth    ��ת��������
���  ��
***************************************/
void  left(uint32 longth)
{
 enable_rap(BACK, longth,FRONT,longth);

}
/**************************************
��ת
�������    longth    ��ת��������
���  ��
***************************************/
void  right(uint32 longth)
{
enable_rap(FRONT, longth,BACK,longth);
}

 /**************************************
��ת
�������    longth    ��ת��������
���  ��
***************************************/

void  back(uint32 longth)
{
  enable_rap(BACK, longth,BACK,longth);  	        
}
/**************************************
��ת
�������    longth    ��ת��������
���  ��
***************************************/
void  forward(uint32 longth)
{
    enable_rap(FRONT, longth,FRONT,longth);
}
/*************************************************
���·���йص��ڴ�
*************************************************/
void clr_ram(void)
{
 mode.bump=0;
 mode.step=0;
 mode.step_bp=0;
 mode.abnormity=0;
 mode.step_abn=0;
 mode.times_bp=0;
 mode.angle_wall=0;
 mode.long_wall=0;
 mode.start_wall=0;
 mode.muck=0;
 mode.step_mk=0;
 mode.times_mk=0;
 mode.ybs_sign =0;

 mode.time = giv_sys_time;
 action.sign=0;
 stop_rap();
 Init_Io_Driver();  //�ر���ɨ��ɨ�Լ�������Ⲣ
 disable_pwm(CHARGE_PWM);
 power.pwm = 0;
}
/*****************************************
���ݶ�������ִ��ָ������
*****************************************/
uint8 do_action(uint8 dm,uint32 length)
{
uint32 m;  
		if(mode.action != dm)
		{
			stop_rap();
			mode.action = dm  ;
		}
		if(action.sign==0)//�ڵ�ǰ����ģʽ�£������˶�������������
		{
			if(length == 0)		//�β�
			{
			   return 1;
			}
			action.sign = 1;
			action.step=0;
			action.time = giv_sys_time;	//���¶����Ŀ�ʼʱ��
			action.length = length;	 //��ڵ�Ԥ�ж�����
		}
		m = (giv_sys_time-action.time);//����Ԥ����ʱ�䣬
		if((m>1200000)&&(dm<18))//�����������״̬������ʱ�䳬������ֹͣ��ǰ����ģʽ
		{
			if(dm != 3)
			{
				stop_rap();
				
				return 1;
			}
		}	   
		switch (dm)
		{
			case  0:
				return 1;
			case  1:					   //��ת
				if(action.step==0)
				{
					left(action.length);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign)&&(!r_rap.sign))//���ֵ�״̬�Ƕ�ͣ����
				{
					action.sign=0;
					return 1;
				}
				return 0;
			case  2:				   //��ת
				if(!action.step)
				{
					right(action.length);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign)&&(!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}
				return 0;
			case  3:				 //ǰ��
				if(!action.step)
				{
					forward(action.length);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign)&&(!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}
				return 0;
				
			case  4:				//����
				if(!action.step)
				{
					back(action.length);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign)&&(!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}
				return 0;
			case  5:				//���ֲ�������ת
				if(!action.step)
				{
					enable_rap(FRONT,0,FRONT,action.length);
					action.step=1;
					return 0;
				}
				if((!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}  
				return 0;
			case  6:			   //���ֲ�������ת
				if(!action.step)
				{
				 	enable_rap(FRONT,action.length,FRONT,0);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign))
				{
					action.sign=0;
					return 1;
				}  
				return 0;	 
			case  7:				//���ֲ�������ת
				if(!action.step)
				{									 
					enable_rap(BACK,action.length,BACK,0);
					action.step=1;
					return 0;
				}
				if((!l_rap.sign))
				{
					action.sign=0;
					return 1;
				}  
				return 0;
			case  8:			   //���ֲ�������ת
				if(!action.step)
				{								 
					enable_rap(BACK,0,BACK,action.length);
					action.step=1;
					return 0;
				}
				if((!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}  
				return 0;
			case  18:
				if(louxuan( 0 ))	//˳ʱ����������	 
				{
					action.sign=0;       
					return 1;
				}
				else
				return 0;
			case  19:
				if(louxuan( 1 ))	//��ʱ����������	 
				{
					action.sign=0;       
					return 1;
				}
				else
				return 0;
			case  20:
				if(!action.step)
				{
					forward(action.length);
					action.step=1;
					return 0;
				}
				if(action.step == 1)
				{  
					if((!l_rap.sign)&&(!r_rap.sign))
					{
						back(angle30);
						action.step++;
					}
					return 0;  
				}
				if(action.step == 2)
				{  
					if((!l_rap.sign)&&(!r_rap.sign))
					{
						forward(angle30);
						action.step++;
					}
					return 0;  
				}
				if((!l_rap.sign)&&(!r_rap.sign))
				{
					action.sign=0;
					return 1;
				}
				return 0;	          
			case  21:
				if(z_road())	//��������	 
				{
					action.sign=0;       
					return 1;
				}
				else
				return 0;
			default :
				stop_rap();
		}
		return 1;
}
/***************************************************
��Z���ߣ���������ǰ������� �˳�����   do_action()���á�
���룺��
����  ���������򷵻�1�����򷵻�0��
***************************************************/
uint8 z_road(void)
{
  switch(action.step)
  {
    case 0:
    case 6:
    case 12:
	case 18:
	  if((!l_rap.sign)&&(!r_rap.sign))
	  {
	     forward(METE_5);		//��ǰ1.5��
	     action.step++;
	  }
	  return 0;
	case 1:
    case 7:
	case 13:
	case 19:
	  if((l_rap.sign)||(r_rap.sign))
	  {
		return 0;
	  }
	  action.step++;
	case 2:
    case 8:
	case 14:
	case 20:
	  if((!l_rap.sign)&&(!r_rap.sign))
	  {
		action.step++; 
		enable_rap(BACK, angle165, FRONT, angle165);
	  }
	  return 0;
	case 3 :
    case 9:
	case 15:
	case 21:
	  if((!l_rap.sign)&&(!r_rap.sign))
	  {
         if( action.step == 21)
           {
			mode.bump = 0;
        	return 1;
            }
	    forward(METE_5);
		action.step++;
	  }  
	  return 0;
	case 4:
    case 10:
	case 16:
	  if((l_rap.sign)||(r_rap.sign))
	  {
		return 0;
	  }
	  action.step++;
	case 5:
    case 11:
	case 17:
	  if((!l_rap.sign)&&(!r_rap.sign))
	  {
		action.step++; 
		enable_rap(FRONT, angle165, BACK, angle165);
	  }
	  return 0;  
	default :
	return 1; 
  }
}
/***************************************************
�������ߣ�Ȧ����ǰ������� �˳�����   do_action()���á�
�����߲��ܴ���20Ȧ  
���룺f = 1,����תȦ��f = 0������תȦ��
����  ���������򷵻�1�����򷵻�0��
***************************************************/
uint8 louxuan(  uint8  f)
{
  switch (action.step)
  {
   case  0:
          if( f != 0)
		  {
	       left(angle360);				 //ԭ��תȦ1Ȧ
		  }
		  else
		  {
		   right(angle360);				 //ԭ��תȦ1Ȧ
		  }
		  action.step++;
		  return 0;
   case   1:
   case   2:
   case   3:
   case   4:
         if((!l_rap.sign)&&(!r_rap.sign))
		  {
		    if( f != 0)
		     {
              enable_rap(BACK,(angle180-action.step*angle45), FRONT,(angle180+action.step*angle45));
			 }
			 else
			 {
			  enable_rap(FRONT, (angle180+action.step*angle45),BACK,(angle180-action.step*angle45)); 
			 }
			action.step++;
		  }
		  return 0;
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
   case   18:
   case   19:
   case   20:
   case   21:
   case   22:
   case   23:
   case   24:
   case   25:
   case   26:
   case   27:
   case   28:
   case   29:
   case   30:
   case   31:
   case   32:
   case   33:
   case   34:
   case   35:
   case   36:
   case   37:
   case   38:
   case   39:
   case   40:
          if((!l_rap.sign)&&(!r_rap.sign))
		  { 

			if( (action.step==(action.length*2-1))|| (action.step==40))
			  {
			    action.sign=0;
				return 1;
			  }	
		    if( f != 0)
		     {
		    	enable_rap(FRONT,(action.step-4)*angle45,FRONT,(action.step-4)*angle45+angle360);
			 }
			 else
			 {
			    enable_rap(FRONT,(action.step-4)*angle45+angle360,FRONT,(action.step-4)*angle45);
			 } 					 
			action.step++;
		  }
		  return 0;
  }
  return 1;
}
/***********************************************
��������read_earth
����˵������ȡ�ؼ��ź�
���أ����еؼ��ź�ʱֹͣ�������� mode.bump��Ϊ��
      �ؼ��ֵ��
***********************************************/
uint8 read_earth(void)
{						   
			if(read_sideearth())
			{
				return 1;
			} 
			if(e_lm.sign == FARN)
			{
				stop_rap();
				mode.step_bp = 0;
				mode.bump = 2;
				return 1;
			}   
			if(e_rm.sign == FARN)
			{
				stop_rap();
				mode.step_bp = 0;
				mode.bump = 3;
				return 1;
			} 
			return 0;		  
}
/***********************************************
��������read_earth
����˵������ȡ�ؼ��ź�
���أ����еؼ��ź�ʱֹͣ�������� mode.bump��Ϊ��
      �ؼ��ֵ��
***********************************************/
uint8 read_sideearth(void)
{						   
			if(e_r.sign == FARN)
			{
				stop_rap();
				mode.step_bp = 0;
				mode.bump = 4;
				return 1;
			} 
			if(e_l.sign == FARN)
			{
				stop_rap();
				mode.step_bp = 0;
				mode.bump = 1;
				return 1;
			} 
			return 0;		  
}

/*****************************
 �ر�ɨ
 ����� mode.long_wall ָʾ��ǽ�ߵľ���
*****************************/
/*
uint8 ybs(void)
{
uint32 y;
static uint8 outybs = 0;
    y=read_bump();
    if(mode.bump)
    {		 
	    mode.sum_bump = 0;	
		mode.fangxiang = 0;
	    if((l_rap.rap > 1000)||(r_rap.rap > 1000))
		{
		    l_rap.rap = (l_rap.rap*2)/3;
		    r_rap.rap = (r_rap.rap*2)/3;
		}
        if((mode.angle_wall<10))
        {	
            if(outybs == 0)
			{
			    stop_rap();
				outybs = 1;
			}
            if(!do_action(1,angle45))	   //�Ѿ����ϰ�һ��
			{		   
	            return 1;
			}
			outybs = 0;
	        mode.bump=0;
	        return 0;
        }  //���� if((mode.angle_wall<10))
        if(mode.angle_wall > angle360 * 5)
        {
		    if(action_wall != FALSE)
			{
		        Disable_wall();
			}
			if(mode.angle_wall > angle360 * 8)
			{			
			    stop_rap();
//			    if(gsv_yaokong.test == 0)	   //ȡ������ǽɨʱ��ֹͣ����
				{			
//				    Init_Cease();
				}
//				else
				{
				    mode.angle_wall = 0;
				}
				return 1;
			}
        }	 
//===================================================		
		 //��ײ��Ϣ
        switch (mode.bump)	 
        {					 
	        case  10: //up_hw 
	        case  11: //l_hw
	        case  12: //m_hw
	        case  13: //r_hw	  
	            switch(mode.step_bp)
	            {	
	                case 0:		  
			            if(do_action(4,angle360))
			            {
						    mode.step_bp = 8;
				            return 1;
				        }
						if((y == 0)||(y == 7)||(y == 14))
						{
						    stop_rap();
							mode.step_bp ++;
						}
		                return 1;
		            case 1:	   
		                if(do_action(4,angle30))
			            { 
				            mode.step_bp++;  	 
				            return 1;
			            }
			            if((y != 0)&&(y != 14))
			            {		   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
				            return  1; 
			            }
		                break;
		            case 2 :
					    if(do_action(1,angle90))
						{
						    mode.angle_wall += l_ring.length+r_ring.length;
							mode.step_bp++;
							return 1;
						}
			            if((y != 0)&&(y != 14))
			            {		   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
						    mode.angle_wall += l_ring.length+r_ring.length;
				            return  1; 
			            }
						break;
					case 3:
					case 12:
					    if(do_action(3,angle180))
						{
			                mode.long_wall +=  l_ring.length;
							mode.step_bp = 4;
							return 1;
						}	
						if(y != 0)
						{	   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
						    mode.long_wall +=  l_ring.length;
						}
						break;		 
		            case 4:
					case 11:
			            if(do_action(2,angle90))
			            {			
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
			                mode.step_bp++;	 
				        }
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
						}
					    return 1;
		            case 5:
					case 10:
		                if(do_action(3,angle90))
			            {
				            mode.step_bp++;	 
			                mode.long_wall +=  l_ring.length;
				            return 1;
			        	}
			            if(y)
			            {
			                mode.bump=y;
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 6:			 
		                if(do_action(2,angle30))
			            {
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
			                mode.step_bp++;	 
				        }
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
						}
						break;	
					case 7:		 
		                if(do_action(3,METE))
			            { 
				            mode.step_bp = 0;
							mode.bump = 0;
							mode.angle_wall = 0;  	 
				            return 0;
			            }	 
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						}
		                break; 
					case 8:		
				        if(do_action(6,angle360+angle180))
					    {
					        mode.step_bp = 9;
							mode.angle_wall += angle720 - l_ring.length;
					    }
					    if(y == 0)
					    {
					        stop_rap();
						    mode.step_bp = 10;
						    return 1;
					    }
					    if((y <10)||(y > 13))
					    {
					        stop_rap();
						    mode.step_bp = 0;
						    mode.bump = y;
					    }
					    return 1; 
					case 9:
					    do_action(3,METE);  
					    if(y == 0)
					    {
					        stop_rap();
						    mode.step_bp = 5;
						    return 1;
					    }
					    if((y <10)||(y > 13))
					    {
					        stop_rap();
						    mode.step_bp = 0;
						    mode.bump = y;
				 	    }
						break;
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }//����switch(mode.step_bp)
			    return 1;
		//==============================================

	        case  7:    //W_L:
	        case  9://W_M:
	        case  8://W_LM:		   			  
        	case  15://W_RM:
	            switch(mode.step_bp)
	            {	
	                case 0:		  
			            if(!do_action(4,angle15))
			            {
				            return 1;
				        }
			            mode.step_bp++;
		                return 1;
		            case 1:	   
		                if(do_action(1,angle180))
			            { 
				            mode.step_bp = 5;  	 
				            return 1;
			            }
			            if((y == mode.bump)||(y == 14))
			            {
				            return  1; 
			            }
			            stop_rap();
			            mode.angle_wall+=l_ring.length*2;
				        if( y == 0)
				        {		
			                mode.step_bp = 2;	
				        }
				        else
				        {
						    mode.step_bp = 0;
					        mode.bump = y;
				        }	
		                break;
		            case 2 :
			            if(!do_action(1,angle30))
			            {
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length*2;  
			            mode.step_bp++;
		            case 3:
		                if(do_action(3,angle90))
			            {
				            mode.step_bp++;	 
			                mode.long_wall +=  l_ring.length;
				            return 1;
			        	}
			            if(y)
			            {
			                mode.bump=y;
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
		            case 4:
		                if(do_action(6,angle360))
			            {						  
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             if(mode.angle_wall>l_ring.length)
				        	 {								 
				                 mode.angle_wall-=l_ring.length;
					         }
				             else 
				        	 {					
				                 mode.angle_wall=0;
				        	 }
				             mode.step_bp=0;
			            }			    
		                break;
	                case 5:
		                if(do_action(3,angle90)) //�޸�ʱ��2009.12.11
			            { 
						    if(mode.bump == 7)
							{
							    w_l.on = 0;
							}
							if(mode.bump == 8)
							{
							   w_lm.on = 0;
							} 
							if(mode.bump == 9)
							{
							   w_m.on = 0;
							}	
							if(mode.bump == 15)
							{
							   w_rm.on = 0;
							}
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
			            }
			            if((y == mode.bump))
			            {
				            return  1; 
			            }
			            stop_rap();
				        if( y == 0)
				        {		
			                mode.step_bp = 1;	
				        }
				        else
				        {
				            mode.step_bp = 0;
					        mode.bump = y;
				        }	
		                return 1;
					case 6:		
		                if(do_action(3,angle90))
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }//����switch(mode.step_bp)
	            return 1;

            case  1://E_L
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(7,angle360))
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}
			            else if((y==1))
			        	     {
			        	        return 1; 
			        	     }
			            stop_rap();
			            mode.step_bp++;	 
			            mode.angle_wall+=l_ring.length;
		                break;
		            case 1:
			            if(!do_action(7,angle30))
			            {			
			        	    return 1;
			        	}
			            mode.step_bp++;		 
			            mode.angle_wall+=l_ring.length;
		                break;	 
		            case 2:
			            if(!do_action(8,angle15))
			            {			 
						    read_earth();
			        	    return 1;
			        	}			   
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }
			            mode.step_bp++;
		                break;
		            case 3 :
			            if(!do_action(1,angle60))
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length*2;
			            mode.step_bp++;
		            case 4:
		                if(do_action(3,angle90))
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 5:
		                if(do_action(6,angle360))
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 6:		
		                if(do_action(3,angle90))
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//����switch(mode.step_bp)
		        return 1; 	  
        	case  4://E_R:	 
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(8,angle360))
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}
			            else if((y==4))
			        	     {
			        	        return 1; 
			        	     }
			            stop_rap();
			            mode.step_bp++;	 
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }	 
		                break;
		            case 1:
			            if(!do_action(8,angle30))
			            {			
			        	    return 1;
			        	}
			            mode.step_bp++;	
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }	 
		                break;	 
		            case 2:
			            if(!do_action(7,angle15))
			            {			 
						    read_earth();
			        	    return 1;
			        	}			   
			            mode.step_bp++;			
			            mode.angle_wall+=l_ring.length;
		                break;
		            case 3 :
			            if(!do_action(1,angle60))
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length*2;
			            mode.step_bp++;
		            case 4:
		                if(do_action(3,angle90))
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 5:
		                if(do_action(6,angle360))
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 6:		
		                if(do_action(3,angle90))
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//����switch(mode.step_bp)
		        return 1; 	 
	        case  2://E_LM
	        case  3://E_RM:	
        	case  5://L_BUMP:
        	case  6://R_BUMP:
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(4,angle360))
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}				
						read_sideearth() ;
			            if((y == mode.bump))
			        	{
			        	     return 1; 
			        	}
			            stop_rap();
			            mode.step_bp++;	
		                break;
		            case 1:
			            if(!do_action(4,angle15))
			            {
			        	    return 1;
			        	}		   
						read_sideearth() ;
			            mode.step_bp++;
		                break;
		            case 2 :
			            if(!do_action(1,angle60))
			            {
						    read_earth();
					        return 1;
				        }
						
			            mode.angle_wall+=l_ring.length*2;
			            mode.step_bp++;
		            case 3:
		                if(do_action(3,angle90))
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 4:
		                if(do_action(6,angle360))
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 5 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 5:		
		                if(do_action(3,angle90))
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//����switch(mode.step_bp)
		        return 1; 
        	case  14://W_R:		
                switch (mode.step_bp)
				{
	                case 0:
					    if(do_action(1,angle180))
						{
						    mode.step_bp = 6;
						}
			            if((y != 14))
			            { 			   
							stop_rap();	
							mode.angle_wall += r_ring.length*2;
				            if(y == 0)
				            {
								mode.step_bp = 1;
				            }
				            else
				            {
				                mode.bump = y;
				            }
			            }
			            return 1;
		            case 1:			  
	                case 4:	 
		                if(do_action(6,angle360))
			            {
			                mode.step_bp += 2;
			                mode.long_wall +=  l_ring.length/2;	 
							if( mode.angle_wall > l_ring.length)
							{
							     mode.angle_wall -= l_ring.length;
							}
							else
							{
							     mode.angle_wall = 0;
							}
			                return 1; 
			            }
			            if(y != 0)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length/2;
							if( mode.angle_wall > l_ring.length)
							{
							     mode.angle_wall -= l_ring.length;
							}
							else
							{
							     mode.angle_wall = 0;
							}
							if(y == 14)
							{
							    mode.step_bp ++;
							}
							else
							{	  
				                mode.bump = y;
				                mode.step_bp = 0;
							}
			            }
			            return 1;
	                case 2:
		                if(do_action(3,angle60))
			            {
			                mode.step_bp = 0;
			                mode.long_wall +=  l_ring.length;
			                return 1; 
			            }
			            if(y != 14)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length;
							if(y == 0)
							{
							    mode.step_bp = 1;
							}
							else
							{	
				                mode.bump = y;
				                mode.step_bp = 0;
							}
			            }
			            return 1;
					case 3:	  
		                if(do_action(3,angle90))
			            {
			                mode.step_bp = 4;
			                mode.long_wall +=  l_ring.length;
			                return 1; 
			            }
			            if(y != 0)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length;
				            mode.bump = y;
				            mode.step_bp = 0;
			            }
			            return 1;
	                case 6:	 
					    //�޸�ʱ��2009.12.11	 
						w_r.on = 0;
				        mode.bump = 0;
						mode.step_bp = 0;
				        mode.angle_wall = 0 ;
			            return 1;				 
		            default :
					     stop_rap();
		                 mode.step_bp=0;
	            }//����	switch (mode.step_bp)
                return  1;   
            default :
	            mode.bump = 0;	   
        }//����swicth(bump)			   
    } //����if(mode.bump)
    return 0;
}  */
 //***************************************************
 //---dengpo modify about port_code----
 //---2010/11/29-----------------------
 //add to times for bump---------------
 //****************************************************
 /*
uint8 action_bump(void)
{
uint32 m;
static uint8 s=0;
    if(mode.ybs_sign == 1)	//��Ҫ����ǽ��ɨ�Ĺ���
    {
		    if( ybs() == 1)
		    {
			        s = s%10;
			        if( 9 == s)
			        {
			            m = 50000;
			        }
			        else
			        {
			            if(s == 6)
				        {
				            m =  30000;
				        }
				        else
				        {
				            m =  10000;
				        }
			        }
			        if(mode.long_wall > m)
			        {
			            s++;
			            stop_rap();
			            mode.long_wall =0;
				        mode.ybs_sign = 0;
				        mode.angle_wall = 0;
						Enable_wall();
			        }
			        return 1;
		    }
			Enable_wall();
		    return 0;
    }//����if(mode.ybs_sign == 1)
	if(mode.bump != 0)
	{
		    if((l_rap.rap > 1000)||(r_rap.rap > 1000))
			{
			    l_rap.rap = (l_rap.rap*2)/3;
			    r_rap.rap = (r_rap.rap*2)/3;
			}
	}	 
    m=read_bump();
    switch(mode.bump)
    {
	    case 1://E_L		   
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(7,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==1))
				    {
				        return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(7,angle30))
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(8,angle15))
				    {
				        mode.step_bp++;
				    }	  
				    read_earth();
				    return 1;
		        case  3:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  4:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
	    case 2://E_LM 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m!=0)&&(m < 7))
				    {  
						read_sideearth() ;
				        return 1;
				    }
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;	
				    }	   
					read_sideearth() ;
				    return 1;
		        case  2:   
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				         mode.times_bp=0;
					     mode.bump=0;
				         return 0;
				    } 
				    read_earth();
					break;
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return 1;	  
	    case 5://L_BUMP:  
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle120))
		            {  	  
					    mode.step_bp = 4;
					    return 1; 
		            }
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;	
				    }	   
				    read_earth();
				    return 1;
		        case  2:   
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				         mode.times_bp=0;
					     mode.bump=0;
				         return 0;
				    } 
				    read_earth();
					break;
				case  4:
				    if(do_action(2,angle120))
					{
					    mode.step_bp = 5;
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;	
				case  5:
				    if(do_action(3,angle120))
					{			  
					    giv_sys_err = mode.bump;
						Init_Err();
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return 1;
	    case 4://E_R: 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(8,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==4))
				    {
				     return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(8,angle30))
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(7,angle15))
				    {
				        mode.step_bp++;
				    }	
				    read_earth();
				    return 1;
		        case  3:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  4:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
	    case 3://E_RM:
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m!=0)&&(m < 7))
				    { 
						read_sideearth() ;
				        return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;
				    }			   
					read_sideearth() ;
				    return 1;
		        case  2:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;	  
	    case 6://R_BUMP:		
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle120))
		            {  
					    mode.step_bp = 4;
					    return 1; 
		            }	 
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
				    if((m!=0)&&(m < 7))
				    { 
						read_earth() ;
				        return 1;
				    }		   
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;
				    }			   
					read_sideearth() ;
				    return 1;
		        case  2:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;		
				case  4:
				    if(do_action(1,angle120))
					{
					    mode.step_bp = 5;
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;	
				case  5:
				    if(do_action(3,angle120))
					{			  
					    giv_sys_err = mode.bump;
						Init_Err();
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
	    case 7://W_L:
	    case 8://W_LM:	 		          
		    switch (mode.step_bp)
		    {
		        case 0:
				    if(!do_action(4,angle15))
					{
					    return  1;
					}
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}	 
					mode.sum_bump++;
					mode.step_bp = 1;
					return 1;
				case 1:
			        if(do_action(mode.fangxiang,angle180))
				    {
					    mode.step_bp = 3;
					    return 1;
				    }
				    if((m == 7)||(m == 8))
				    {
				        return 1;
				    }		   
					stop_rap();	  
				    mode.step_bp ++;
				    if(m != 0)
				    {
					    mode.step_bp = 0;
					    mode.bump = m;
					    return 1;
				    }
			    case  2:
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    }
			        return  1; 
		        case 3:
			        if(do_action(3,angle90))//�޸�ʱ��2009.12.11
			         { 
						    if(mode.bump == 7)
							{
							    w_l.on = 0;
							}
							if(mode.bump == 8)
							{
							   w_lm.on = 0;
							} 
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
				    }
				    if((m == 7)||(m == 8))
				    {
				        return 1;
				    }		 
				    stop_rap();
				    mode.step_bp=2;
				    if(m != 0)
				    {
					    mode.step_bp = 0;
					    mode.bump = m;
				    }
			        return  1; 
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    } 
			return 1;	 	
	    case 10://up_hw			 	
	    case 11://l_hw	 	
	    case 12://m_hw
	    case 13://r_hw		
		    switch(mode.step_bp)
			{
			    case 0:
				    if(do_action(4,angle360))
					{
					    mode.step_bp = 3; 
						mode.sum_bump++;  
					    if(mode.fangxiang == 0)
					    {
					        mode.fangxiang = 1;
					    }	
					} 
					if((m == 0)||(m == 7)||(m == 14))
					{
					    stop_rap();
						mode.step_bp = 1;  
					    if(mode.fangxiang == 0)
					    {
					        mode.fangxiang = 1;
					    }
						return 1;
					}
					return 1;  
			    case 1:
				    if(do_action(4,angle90))
					{
					    mode.step_bp = 2; 
					} 
					if(m == 0)
					{
						return 1;
					}
					stop_rap();
					mode.step_bp = 0;
					mode.bump = m;
					return 1;
			    case 2:
				    if(do_action(1,angle180))
					{
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
					}
					if((m != 0))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
			    case 3:
				    if(do_action(6,angle360+angle180))
					{
					    mode.step_bp = 4;
					}
					if(m == 0)
					{
					    stop_rap();
						mode.step_bp = 5;
						return 1;
					}
					if((m <10)||(m > 13))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
				case 4:
				    do_action(3,METE);	
					if(m == 0)
					{
					    stop_rap();
						mode.step_bp = 5;
						return 1;
					}
					if((m <10)||(m > 13))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}			
					return 1;
			    case 5:
				    if(do_action(3,angle90))
					{
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
					}
					if(m != 0)
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
				default :
				    stop_rap();
				    mode.step_bp = 0;
			}
			return 1;
	    case 9://W_M:									  
	    case 14://W_R:	
	    case 15://W_RM:
	        switch(mode.step_bp)
		    {
		        case 0:			   
			        if(!do_action(4,angle15))
				    {
					    return 1;  
				    }				
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
					mode.step_bp = 1;
			    case  1:
			        if(do_action(mode.fangxiang,angle180))
			        {
					    mode.step_bp = 3;
					    return 1;
				    }
			        if((m == 9)||(m==14)||(m==15))
			        {
				        return  1; 
				    }		 
				    stop_rap();
				    mode.step_bp++;
				    if(m != 0)
				    {
					    stop_rap();
					    mode.step_bp=0;
					    mode.bump=m;
					    return 1;
				    }	 
			    case  2:
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    } 
			        return 1;
		        case 3:
			        if(do_action(3,angle90))//�޸�ʱ��2009.12.11
			        { 
							if(mode.bump == 9)
							{
							   w_m.on = 0;
							}			   
							if(mode.bump == 14)
							{
							   w_r.on = 0;
							} 
							if(mode.bump == 15)
							{
							   w_rm.on = 0;
							}
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
				    }
				    if((m == 9)||(m==14)||(m==15))
				    {
				        return 1;
				    }		
				    stop_rap();
				    mode.step_bp=2;
				    if(m != 0)
				    {
					    mode.step_bp=0;
					    mode.bump=m;
				    }
			        return  1; 
			    default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
			return 1;
	    default :
	        mode.bump = 0;
	        break;
	} //����  switch(mode.bump)
    return 0;
}
*/
/************************************************************
�˶Ե�ǰʱ���Ƿ���ԤԼʱ��һ�£�������򷵻�1�����򷵻�0
************************************************************/
uint8 Check_PreengageTime(void)
{
uint16 piv_t;
uint8  piv_hour;
uint8  piv_minute;
static uint8 piv_preengage;
uint32 t;
    t = Rtc_time % 604800;
    piv_minute = (uint8)((t % 3600) / 60);
    piv_hour = (uint8)((t % 86400) / 3600) ;  
    piv_t = (piv_hour << 8)	+ piv_minute;
	if(piv_t == PreengageTime)
	{
	    if(piv_preengage == 1)
		{
		    piv_preengage = 0;
	        if((((1<<(t/86400))& PreengageDate) != 0))
	        {
		        return 1;
		    }
		}
	}
	else
	{
	    piv_preengage = 1;
	} 
	return 0;  
}
/************************************************************
��ʼ����ײ�Լ�����
************************************************************/
void Init_Action(void)
{
//#define     NEAR            0XFE                              //��
//#define     FARN            0XFD                              //Զ
   e_l.sign = NEAR;
   e_lm.sign =NEAR;
   e_rm.sign = NEAR;	
   e_r.sign = NEAR;
   l_bump.key = 0;
   r_bump.key = 0;
   w_l.sign = FARN;
   w_lm.sign = FARN;
   w_m.sign = FARN;
   w_r.sign = FARN;
   w_rm.sign = FARN;
   l_ring.state = GOOD;
   r_ring.state = GOOD;
}
/****************************************************
���ܣ����ݵ�ص�ѹ�ж���ʾ��ѹ
���룺��
�����
���أ���
****************************************************/
uint8 Get_Dispower(void)
{
uint32  t;
    t = (giv_BatteryFDCap *10)/giv_BatteryCapability;
    if(t > 7)
	{
	    return 1;
	}
	if(t > 4)
	{
	    return 2;
	}
	return 3 ;
}
/***************************************************
���ܣ�ÿ���1�������ʣ������
***************************************************/
void AccountCapability(void)
{
    if(giv_UsePowerTime == Rtc_time)
	{
	    return ;
	}
	AccountCapabilityReal();
}

/****************************************************
���ܣ��������ʹ�õĵ�ص���
���룺��
�����
���أ���
˵����ͨ�������ڸ���ģʽ�¹���ʱ�����ж�ʹ�õĵ���
      �ı����ֵ
	  giv_UsePowerTime    
	  giv_BatteryFDCap
	  
	  �ڵ�ص�ѹ����13.2vʱ�������Ϊ�Ѿ�����磬���ҽ��ŵ��������뱸�ݼĴ���
****************************************************/
void  AccountCapabilityReal(void)
{
static uint8 entercapability = 0;
uint32 t;//�ĵ�ʱ��
uint32 curr;//�ĵ����
	t = 0;
	curr = 0;
	if(Rtc_time > giv_UsePowerTime)
	{
	    t = Rtc_time  - giv_UsePowerTime;
	}
	giv_UsePowerTime = Rtc_time;
    switch(mode.mode)
	{
        case CEASE: 
	    case ERR:
		    curr = 71;
		    break;
        case SLEEP:
		    curr = 20;
	        break;
	    case SWEEP:
	    case SPOT:
	    case TELE:
		    curr = 1200;
		    break ;
	    case DOCKING:
		    curr = 400;
	        break;
	    case CHARGEING:
	        switch(power.step)
			{
				case 2:
				case 4:	  
				case 7:
				case 11:
				case 13:
				    curr = 150;
				    break; 
		        case 5:	  //�����0.05C��缴80ma���5���� 
		        case 9:	
				    curr = 80;
				    break; 
				case 3:
				    curr = 800;
					break;
			}
	        break;
	}
	t = t*curr;
	if(t > 2600)
	{
	    return;
	}
	if(mode.mode == CHARGEING)
	{
	    if(giv_BatteryFDCap > t)
		{
		    giv_BatteryFDCap -= t;
		}
		else
		{
		    giv_BatteryFDCap = 0;
		}
	}
	else 
	{
	    if(giv_BatteryFDCap < 0x83d600)
	    giv_BatteryFDCap += t;
	}
    if((((giv_BatteryFDCap *10)/giv_BatteryCapability) > 2)&&(giv_BatteryChargeForbid == 1))
	{
	    giv_BatteryChargeForbid = 0;
	}
	WriteFDCap();
	if(battery_voltage < LOWPOWER)
	{
	    if(entercapability == 0)
		{
		    entercapability = 1;
			giv_BatteryChargeForbid = 0;
			if((giv_BatteryFDCap > 0x36ee80)&&(giv_BatteryFDCap < 0x83d600))
			{
		        giv_BatteryCapability = giv_BatteryFDCap;
			}
			else
			{
				giv_BatteryFDCap = 	 giv_BatteryCapability;
			}					   
			WriteBatteryCapability();
		}
	}
	else
	{
	    if(battery_voltage > TWOPOWER)
		{
		    entercapability = 0;
		}
	}
    return;
}
/*******************************************************
���ܣ������ɨ�ͷ�����Ƿ���
���أ��ڷ���0�����򷵻�1
*******************************************************/
uint8 CheckMDustMotor(void)
{
uint16 piv_dust,piv_m;
uint16 v;
    piv_dust = account_current(DUST_CURRENT);
	piv_m = account_current(M_CURRENT);
	Set_Dust_Motor();
    pbv_state_dust_motor = TRUE;
	DelayMs(500);
	v = account_current(DUST_CURRENT) ;
	if((v < piv_dust)||(( v - piv_dust) < 10))
	{
	     DelayMs(500);
	     v = account_current(DUST_CURRENT) ;
		 if((v < piv_dust)||(( v - piv_dust) < 10))
		 {
	         giv_sys_err = 33;
		     Init_Err();
		     return 1;
		 }
	}
    Set_M_Motor();
	//Reset_M_ZNMOS();
	//Set_M_ZPMOS();
	pbv_state_m_motor = TRUE;
	DelayMs(500);
	v = account_current(M_CURRENT) ;
	if((v < piv_m)||(( v - piv_m) < 10))
	{
	     DelayMs(500);
	     v = account_current(M_CURRENT) ;
		 if((v < piv_m)||(( v - piv_m) < 10))
		 {
	         giv_sys_err = 34;
		     Init_Err();
		     return 1;
		 }
	}
	return 0;
}
//===================================================================

uint8 action_bump(void)
{
uint32 m;
static uint8 s=0;

    if(mode.ybs_sign == 1)	//��Ҫ����ǽ��ɨ�Ĺ���
    {
		    if( ybs() == 1)		   //�˴�����ִ���ر�ɨ
		    {
			        s = s%10;
			     /*   if( 9 == s)
			        {
			            m = 50000;
			        }
			        else
			        {
			            if(s == 6)
				        {
				            m =  30000;
				        }
				        else
				        {
				            m =  10000;
				        }
			        } */
					if(s==9)m = 30000;
					else if(s==8||s==7||s==6)m = 20000;
					else if(s<6)m = 5000;
				
			        if(mode.long_wall > m)	 //����ǽ��ɨ�ķ�Χ���˳��ر�ɨ
			        {
			            s++;
			            stop_rap();
			            mode.long_wall =0;
				        mode.ybs_sign = 0;
				        mode.angle_wall = 0;
						Enable_wall();
			        }
			        return 1;
		    }
			Enable_wall();
		    return 0;
    }//����if(mode.ybs_sign == 1)
	if(mode.bump != 0)	 //ǰ��������ײ
	{
	        if((l_rap.rap > 1000)||(r_rap.rap > 1000))
			{
			    l_rap.rap = (l_rap.rap*2)/3;
			    r_rap.rap = (r_rap.rap*2)/3;
			}
	}
	 
    m=read_bump();//�������ٶ����� ��//M�����ײ��Ϣ M=mode.bump
	
    switch(mode.bump) //״̬�Ľ����Լ�����ʽ
    {//-------------------------
	    case 1://E_L		   
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(7,angle360)) //���ֲ�������ת	,������������
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==1))
				    {
				        return 1;
				    }		   
					if(mode.fangxiang == 0)	//�����ʼ�ǲ����ģ������Ϊ��ת����
					{
					    mode.fangxiang = 2;
					}	 
					mode.sum_bump++;        //��ײ��������ʱ���㣬�Ա�����ʹ�� 
				 	stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(7,angle30))  //���ֲ�������תһ���Ƕ� ���Ұ���ǰ������ת
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(8,angle15))  //���ֲ�������תһ���ĽǶ�
				    {
				        mode.step_bp++;
				    }	  
				    read_earth();
				    return 1;
		        case  3:		
			        if(do_action(mode.fangxiang,angle45))  //ȫ��������תһ���ĽǶ�
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  4:	//mode.bump=1��mode.step_bp=4��	  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))//����ͬʱת����������תһ���ĽǶ�
			        { //�Է����ǣ��Բ�ͬ�ĽǶ���ת��������
				        mode.times_bp++;
				        if(mode.times_bp>51)mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth(); //�޸�/mode.bump�����ȼ������ߣ��˶��еļ��
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
	   //-----------------------------------
	    case 2://E_LM 
	        switch  (mode.step_bp)
		    {
			    case  0:												  
	                if(do_action(4,angle360))	 //	����
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m!=0)&&(m < 7))	//����ߵؼ�ĵؼ����ײ���״���µģ��Եؼ��
				    {  
						read_sideearth() ;
				        return 1;
				    }
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))   //����
				    {
				        mode.step_bp++;	
				    }	   
					read_sideearth() ;
				    return 1;
		        case  2:   
			        if(do_action(mode.fangxiang,angle45))	//��ת
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))//��תʱ�����
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return 1;
		//--------------------------------------		  
	    case 5://L_BUMP:  	//������ײ�Ĵ�������Զ�����
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle120))	 //����
		            {  	  
					    mode.step_bp = 4;
					    return 1;                //��ʾ�й���ײ
		            }
			//	if((ADC_ConvertedValue[7] & 0XFFF)>(ADC_ConvertedValue[14] & 0XFFF)||
			//	   (ADC_ConvertedValue[14] & 0XFFF)>(ADC_ConvertedValue[15] & 0XFFF))mode.fangxiang=2;
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}	
				    if((m!=0)&&(m < 7))//M=READ_BUMP������
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle30))	//����angle15
				    {
				        mode.step_bp++;	        //ֱ�����˵����,���ǵؼ췢������
				    }	   
				    read_earth();
				    return 1;
		        case  2:   
			        if(do_action(2,angle30));//mode.fangxiang,angle30))	 //��ת������	angle45
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(2,angle[mode.times_bp]))	 //ʱ�����
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				case  4:
				    if(do_action(2,angle75))	 //��ת	 angle120
					{
					    mode.step_bp = 5;
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
					mode.ybs_sign=1;
				    stop_rap();
				    mode.step_bp=1;
					break;	
				case  5:
				    if (mode.ybs_sign==0)
					{
					    if(do_action(3,angle60))	 //��ǰ����	angle120 1293��Լ20CM
						{			  
						    giv_sys_err = mode.bump;
							Init_Err();
							return 1;
						}	
					    if((m!=0)&&(m < 7))
					    {  
							read_earth() ;
					        return 1;
					    }
					}					  
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
					break;
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return 1;
		//-----------------------------------------
	    case 4://E_R: 
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(8,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m==4))
				    {
				     return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(8,angle30))
				    {
				        mode.step_bp++;
				    }
				    return 1;	 
			    case  2:
			        if(do_action(7,angle15))
				    {
				        mode.step_bp++;
				    }	
				    read_earth();
				    return 1;
		        case  3:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  4:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
		//---------------------------------------------
	    case 3://E_RM:
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle360))
		            {  
					    giv_sys_err = mode.bump;
						Init_Err();
					    return 1; 
		            }
				    if((m!=0)&&(m < 7))
				    { 
						read_sideearth() ;
				        return 1;
				    }		   
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle15))
				    {
				        mode.step_bp++;
				    }			   
					read_sideearth() ;
				    return 1;
		        case  2:		
			        if(do_action(mode.fangxiang,angle45))
			        {
				        mode.step_bp++;
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;
		    }
		    return  1;
		//--------------------------------------------------		  
	    case 6://R_BUMP:		
	        switch  (mode.step_bp)
		    {
			    case  0:
	                if(do_action(4,angle120))	 //����
		            {  
					    mode.step_bp = 4;
					    return 1; 
		            }	 
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
				    if((m!=0)&&(m < 7))
				    { 
						read_earth() ;
				        return 1;
				    }		   
					mode.sum_bump++;
				    stop_rap();
				    mode.step_bp=1;
			    case  1:
			        if(do_action(4,angle30))	 //����angle15
				    {
				        mode.step_bp++;
				    }			   
					read_sideearth() ;
				    return 1;
		        case  2:		
			        if(do_action(1,angle30)) //����������ң���ô���� �����ǣ���������angle45
			        {
				        mode.step_bp++;											 
				    } 
				    read_earth();	 
					break;
				case  3:		  
			        if(do_action(1,angle[mode.times_bp])) //����ʱ��
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
				    	mode.bump=0;
				        return 0;
				    } 
				    read_earth();
					break;		
				case  4:
				    if(do_action(1,angle60))	//��angle120
					{
					    mode.step_bp = 5;
						return 1;
					}	
				    if((m!=0)&&(m < 7))
				    {  
						read_earth() ;
				        return 1;
				    }					  
					mode.sum_bump++;
					mode.ybs_sign=1;
				    stop_rap();
				    mode.step_bp=1;
					break;	
				case  5:
				    if(mode.ybs_sign==0)
				    {
					    if(do_action(3,angle60))  //��ǰ���� angle120
						{			  
						    giv_sys_err = mode.bump;
							Init_Err();
							return 1;
						}	
					    if((m!=0)&&(m < 7))
					    {  
							read_earth() ;
					        return 1;
					    }
					}
					mode.sum_bump++;
	          	    stop_rap();
				    mode.step_bp=1;
					break;
				default :  
				    stop_rap();
				    mode.step_bp = 0;//�Է���������ڵ�ʱ��û�в���
		    }
		    return  1;
		//--���ǽ�촦��-------------------------------
	    case 7://W_L:
	    case 8://W_LM:	 		          
		    switch (mode.step_bp)
		    {
		        case 0:
				    if(!do_action(4,angle15))
					{
					    return  1;
					}
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 2;
					}	 
					mode.sum_bump++;
			       	mode.step_bp = 1;
					return 1;
				case 1:
			        if(do_action(mode.fangxiang,angle75)) //angle180
				    {
					    mode.step_bp = 3;
					    return 1;
				    }
				    if((m == 7)||(m == 8))
				    {
				        return 1;
				    }		   
					stop_rap();	  
				    mode.step_bp ++;
				    if(m != 0)
				    {
					    mode.step_bp = 0;
					    mode.bump = m;
					    return 1;
				    }
			    case  2:
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    }
			        return  1; 
		        case 3:
			        if(do_action(3,angle60))//�޸�ʱ��2009.12.11 angle90
			         { 
						    if(mode.bump == 7)
							{
							    w_l.on = 0;
							}
							if(mode.bump == 8)
							{
							   w_lm.on = 0;
							} 
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
				    }
				    if((m == 7)||(m == 8))
				    {
				        return 1;
				    }		 
				    stop_rap();
				    mode.step_bp=2;
				    if(m != 0)
				    {
					    mode.step_bp = 0;
					    mode.bump = m;
				    }
			        return  1; 
				default :
				    stop_rap();
				    mode.step_bp = 0;
		    } 
			return 1;	
		//--------------------------------------------	 	
	    case 10://up_hw			 	
	    case 11://l_hw	 	
	    case 12://m_hw
	    case 13://r_hw		
		    switch(mode.step_bp)
			{
			    case 0:
				    if(do_action(4,angle360))
					{
					    mode.step_bp = 3; 
						mode.sum_bump++; 
						mode.ybs_sign=1;
						if(mode.fangxiang == 0)
					    {
					        mode.fangxiang = 1;
					    }	
					} 
					if((m == 0)||(m == 7)||(m == 14))
					{
					    stop_rap();
						mode.step_bp = 1;  
					    if(mode.fangxiang == 0)
					    {
					        mode.fangxiang = 1;
					    }
						return 1;
					}
					return 1;  
			    case 1:
				    if(do_action(4,angle60))//angle90
					{
					    mode.step_bp = 2; 
					} 
					if(m == 0)
					{
						return 1;
					}
					stop_rap();
					mode.step_bp = 0;
					mode.bump = m;
					return 1;
			    case 2:
				    if(do_action(1,angle180))
					{
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
					}
					if((m != 0))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
			    case 3:
				    if(do_action(6,angle180))//angle360+angle180
					{
					    mode.step_bp = 4;
					}
					if(m == 0)
					{
					    stop_rap();
						mode.step_bp = 5;
						return 1;
					}
					if((m <10)||(m > 13))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
				case 4:
				   if(mode.ybs_sign==0)
				   do_action(3,METE);//1��ǰ������	
					if(m == 0)
					{
					    stop_rap();
						mode.step_bp = 5;
						return 1;
					}
					if((m <10)||(m > 13))
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}			
					return 1;
			    case 5:
				    if(do_action(3,angle30))//angle90
					{
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
					}
					if(m != 0)
					{
					    stop_rap();
						mode.step_bp = 0;
						mode.bump = m;
					}
					return 1;
				default :
				    stop_rap();
				    mode.step_bp = 0;
			}
			return 1;
		//----------------------------------------------
	    case 9://W_M:									  
	    case 14://W_R:	
	    case 15://W_RM:
	        switch(mode.step_bp)
		    {
		        case 0:			   
			        if(!do_action(4,angle15))
				    {
					    return 1;  
				    }				
					if(mode.fangxiang == 0)
					{
					    mode.fangxiang = 1;
					}	 
					mode.sum_bump++;
					mode.step_bp = 1;
			    case  1:
			        if(do_action(mode.fangxiang,angle75))//angle180
			        {
					    mode.step_bp = 3;
					    return 1;
				    }
			        if((m == 9)||(m==14)||(m==15))
			        {
				        return  1; 
				    }		 
				    stop_rap();
				    mode.step_bp++;
				    if(m != 0)
				    {
					    stop_rap();
					    mode.step_bp=0;
					    mode.bump=m;
					    return 1;
				    }	 
			    case  2:
			        if(do_action(mode.fangxiang,angle[mode.times_bp]))
			        {
				        mode.times_bp++;
				        if(mode.times_bp>51)
				        mode.times_bp=0;
					    mode.bump=0;
				        return 0;
				    } 
			        return 1;
		        case 3:
			        if(do_action(3,angle60))//�޸�ʱ��2009.12.11 angle90
			        { 
							if(mode.bump == 9)
							{
							   w_m.on = 0;
							}			   
							if(mode.bump == 14)
							{
							   w_r.on = 0;
							} 
							if(mode.bump == 15)
							{
							   w_rm.on = 0;
							}
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
				    }
				    if((m == 9)||(m==14)||(m==15))
				    {
				        return 1;
				    }		
				    stop_rap();
				    mode.step_bp=2;
				    if(m != 0)
				    {
					    mode.step_bp=0;
					    mode.bump=m;
				    }
			        return  1; 
			    default :
				    stop_rap();
				    mode.step_bp = 0;
		    }
			return 1;
	    default :
	        mode.bump = 0;
	        break;
	} //����  switch(mode.bump)
    return 0;
}
 //---------------------------------------------------------------
uint8 ybs(void)
{
uint32 y;
static uint8 outybs = 0;
    y=read_bump();
    if(mode.bump)
    {		 
	    mode.sum_bump = 0;	
		mode.fangxiang = 0;
	    if((l_rap.rap > 1000)||(r_rap.rap > 1000))
		{
		    l_rap.rap = (l_rap.rap*2)/3;
		    r_rap.rap = (r_rap.rap*2)/3;
		}
        if((mode.angle_wall<10))
        {	
            if(outybs == 0)
			{
			    stop_rap();
				outybs = 1;
			}
            if(!do_action(1,angle45))	   //�Ѿ����ϰ�һ��
			{		   
	            return 1;
			}
			outybs = 0;
	        mode.bump=0;
	        return 0;
        }  //���� if((mode.angle_wall<10))
       
    if(mode.angle_wall > angle360 * 5) //3880*5
        {
		    if(action_wall != FALSE)
			{
		        Disable_wall();
			}
			if(mode.angle_wall > angle360 * 8) //����һ���������˶�����Ȼû������ʱ��ֹͣ
			{			
			    stop_rap();
				mode.angle_wall = 0;
				return 1;
			}
        }	 
//===================================================		
		 //��ײ��Ϣ
        switch (mode.bump)	 
        {
//--------------------------------------
//--��һ���,������ײ,dummy wall							 
	        case  10: //up_hw 
	       // case  11: //l_hw
	      //  case  12: //m_hw
	      //  case  13: //r_hw	  
	            switch(mode.step_bp)	  //��ײʱ�Ĳ����־
	            {	
	                case 0:		  
			            if(do_action(4,angle360))  //����
			            {
						    mode.step_bp = 8;
				            return 1;
				        }
						if((y == 0)||(y == 7)||(y == 14))  //������������ǽ�죬��������
						{
						    stop_rap();
							mode.step_bp ++;
						}
		                return 1;
		            case 1:	   
		                if(do_action(4,angle30))	 //����
			            { 
				            mode.step_bp++;  	 
				            return 1;
			            }
			            if((y != 0)&&(y != 14))	   //ֱ����ǽ�����ź�
			            {		   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
				            return  1; 
			            }
		                break;
		            case 2 :
					    if(do_action(1,angle90))   //��
						{
						    mode.angle_wall += l_ring.length+r_ring.length;	 //��ת�Ƕȵļ���
							mode.step_bp++;
							return 1;
						}
			            if((y != 0)&&(y != 14))	 //û����ߺ��ұ�ǽ��
			            {		   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	      //�µ���ײ��Ϣ
						    mode.angle_wall += l_ring.length+r_ring.length;	 //��ת�Ƕȵļ���
				            return  1; 
			            }
						break;
					case 3:
					case 12:
					    if(do_action(3,angle180))	  //ǰ��
						{
			                mode.long_wall +=  l_ring.length;	 //ǰ������ļ��㣬�������������ߵ�������
							mode.step_bp = 4;
							return 1;
						}	
						if(y != 0)	  //����Ԥ��yӦ��Ϊ0, ����,�����µ���ת
						{	   
			                stop_rap();
						    mode.step_bp = 0;
					        mode.bump = y;	  
						    mode.long_wall +=  l_ring.length;
						}
						break;		 
		            case 4:
					case 11:
			            if(do_action(2,angle90))		//��ת
			            {			
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  //��ת���ӣ���ת����������
							}
							else
							{
							    mode.angle_wall  = 0;
							}
			                mode.step_bp++;	 
				        }
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
						}
					    return 1;
		            case 5:
					case 10:
		                if(do_action(3,angle90))	//ǰ��
			            {
				            mode.step_bp++;	 
			                mode.long_wall +=  l_ring.length;
				            return 1;
			        	}
			            if(y)
			            {
			                mode.bump=y;
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 6:			 
		                if(do_action(2,angle30))	//��ת
			            {
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
			                mode.step_bp++;	 
				        }
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						    if(	mode.angle_wall > (l_ring.length+r_ring.length))
							{
			                    mode.angle_wall -= (l_ring.length+r_ring.length);  
							}
							else
							{
							    mode.angle_wall  = 0;
							}
						}
						break;	
					case 7:		 
		                if(do_action(3,METE))	 //ǰ��������ǽ�ߣ�1��
			            { 
				            mode.step_bp = 0;
							mode.bump = 0;
							mode.angle_wall = 0;  	//��ǽ��Ƕȼ��㣬���¹��㣬�Ա���һ�ε�ʹ�� 
				            return 0;
			            }	 
						if(y != 0)
						{   
				            stop_rap();
				            mode.step_bp=0;
			                mode.bump=y; 
						}
		                break; 
					case 8:		
				        if(do_action(6,angle360+angle180))	 //	  ���ֲ�������ת
					    {
					        mode.step_bp = 9;
							mode.angle_wall += angle720 - l_ring.length;  //ֻ��һ��������תʱ�ģ��Ƕȼ���
					    }
					    if(y == 0)
					    {
					        stop_rap();
						    mode.step_bp = 10;
						    return 1;
					    }
					    else //if((y <10)||(y > 13))
					    {
					        stop_rap();
						    mode.step_bp = 0;
						    mode.bump = y;
					    }
					    return 1; 
					case 9:
					    do_action(3,METE);  	//�뿪ǽ�ߣ�ǰ��1��
					    if(y == 0)
					    {
					        stop_rap();
						    mode.step_bp = 5;
						    return 1;
					    }
					    else //if((y <10)||(y > 13))
					    {
					        stop_rap();
						    mode.step_bp = 0;
						    mode.bump = y;
				 	    }
						break;
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }//����switch(mode.step_bp)
			    return 1;
//--------------------------------------------------
//--�ڶ������ײ,ǽ�죨�����У��У����У�

	        case  7: //W_L:
	        case  9://W_M:
	        case  8://W_LM:		   			  
        	case  15://W_RM:
	            switch(mode.step_bp)
	            {	
	                case 0:		  
			            if(!do_action(4,angle15)) //houtui
			            {
				            return 1;
				        }
			            mode.step_bp++;
		                return 1;
		            case 1:	   
		                if(do_action(1,angle180))  //zuozhuan
			            { 
				            mode.step_bp = 5;  	 
				            return 1;
			            }
			            if((y == mode.bump)||(y == 14))
			            {
				            return  1; 
			            }
			            stop_rap();
			            mode.angle_wall+=l_ring.length+r_ring.length;
				        if( y == 0)
				        {		
			                mode.step_bp = 2;	
				        }
				        else
				        {
						    mode.step_bp = 0;
					        mode.bump = y;
				        }	
		                break;
		            case 2 :
			            if(!do_action(1,angle30))
			            {
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length+r_ring.length;  
			            mode.step_bp++;
		            case 3:
		                if(do_action(3,angle90))  //����ǽ������һ���ľ���
			            {
				            mode.step_bp++;	 
			                mode.long_wall +=  l_ring.length;
				            return 1;
			        	}
			            if(y)
			            {
			                mode.bump=y;
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
		            case 4:
		                if(do_action(6,angle360))	 //������ת�Ƕȵļ��� �������ǽ�죬�׻�[��ͷ�����������Լ�]
			            {						  
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			            }
			            if(y) //Ϊ��õ�����Ϣ����׼��
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall += (int) l_ring.length/2;
				             if(mode.angle_wall>l_ring.length) //�Ƿ���������
				        	 {								 
				                 mode.angle_wall-=l_ring.length;
					         }
				             else 
				        	 {					
				                 mode.angle_wall=0;
				        	 }
				             mode.step_bp=0;
			            }			    
		                break;
	                case 5:
		                if(do_action(3,angle90)) //�޸�ʱ��2009.12.11
			            { 
						    if(mode.bump == 7)
							{
							    w_l.on = 0;
							}
							if(mode.bump == 8)
							{
							   w_lm.on = 0;
							} 
							if(mode.bump == 9)
							{
							   w_m.on = 0;
							}	
							if(mode.bump == 15)
							{
							   w_rm.on = 0;
							}
				            mode.bump = 0;
							mode.step_bp = 0;
				            return 0;
			            }
			            if((y == mode.bump))
			            {
				            return  1; 
			            }
			            stop_rap();
				        if( y == 0)
				        {		
			                mode.step_bp = 1;	
				        }
				        else
				        {
				            mode.step_bp = 0;
					        mode.bump = y;
				        }	
		                return 1;
					case 6:		
		                if(do_action(3,angle90)) //˫����ǰ����
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2; //���ߵľ���
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }//����switch(mode.step_bp)
	            return 1;
//---------------------------------------------
//���������ײ,�����ײ
            case  1://E_L
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(7,angle360)) //���ֲ�������ת�������
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}
			            else if((y==1))
			        	     {
			        	        return 1; 
			        	     }
			            stop_rap();
			            mode.step_bp++;	 
			            mode.angle_wall+=l_ring.length;	//�������߽Ƕȼ���
		                break;
		            case 1:
			            if(!do_action(7,angle30))
			            {			
			        	    return 1;
			        	}
			            mode.step_bp++;		 
			            mode.angle_wall+=l_ring.length;
		                break;	 
		            case 2:
			            if(!do_action(8,angle15))	//���ֲ�������ת���������
			            {			 
						    read_earth();
			        	    return 1;
			        	}			   
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;	 	//�������߽Ƕȼ���
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }
			            mode.step_bp++;
		                break;
		            case 3 :
			            if(!do_action(1,angle60))	  //˫�� ��ת
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length+r_ring.length;
			            mode.step_bp++;
		            case 4:
		                if(do_action(3,angle90)) //�ر�ǰ��
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;	 //�ر�ǰ���ľ���
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 5:
		                if(do_action(6,angle360))	  //�����ǰ������ת����Ŀ�Ŀ���ǽ�ڣ��鿴�ұ�ǽ���ź�
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;	   //������
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 6:		
		                if(do_action(3,angle90))	//˫����ǰ����
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2; //���ߵľ���
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//����switch(mode.step_bp)
		        return 1; 
//----------------------------------------------------------------
//--���������ײ,�ҵ�					  
        	case  4://E_R:	 
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(8,angle360))	//�Ұ�߳������
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}
			            else if((y==4))
			        	     {
			        	        return 1; 
			        	     }
			            stop_rap();
			            mode.step_bp++;	 
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;	   //ת���ĽǶ�
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }	 
		                break;
		            case 1:
			            if(!do_action(8,angle30))
			            {			
			        	    return 1;
			        	}
			            mode.step_bp++;	
				        if(mode.angle_wall>r_ring.length)
				        {								
			        	    mode.angle_wall-=r_ring.length;
			        	}
				        else 
				        {					
				            mode.angle_wall=0;
				        }	 
		                break;	 
		            case 2:
			            if(!do_action(7,angle15))  //���߳������
			            {			 
						    read_earth();
			        	    return 1;
			        	}			   
			            mode.step_bp++;			
			            mode.angle_wall+=l_ring.length;	  //�Ƕ�
		                break;
		            case 3 :
			            if(!do_action(1,angle60))	 //ȫ����ת
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length+r_ring.length;
			            mode.step_bp++;
		            case 4:
		                if(do_action(3,angle90))   //��ǰ����
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length;
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 5:
		                if(do_action(6,angle360))	 //�����������ת��̽�ұ�ǽ��
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 6 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 6:		
		                if(do_action(3,angle90))   //ȫ��ǰ��
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//����switch(mode.step_bp)
		        return 1; 
//----------------------------------------------------------
//--���������ײ					 
	        case  2://E_LM
	        case  3://E_RM:	
        	case  5://L_BUMP:
        	case  6://R_BUMP:
	            switch(mode.step_bp)
	            {	
	                case 0:
		                if(do_action(4,angle360))	  //ȫ�����
			            {	  		
			        	     giv_sys_err = mode.bump;
			        		 Init_Err();
			        	    return 1;
			        	}				
						read_sideearth() ;
			            if((y == mode.bump))   //no changing
			        	{
			        	     return 1; 
			        	}
			            stop_rap();
			            mode.step_bp++;	
		                break;
		            case 1:
			            if(!do_action(4,angle15))	  //ȫ�����
			            {
			        	    return 1;
			        	}		   
						read_sideearth() ;
			            mode.step_bp++;
		                break;
		            case 2 :
			            if(!do_action(1,angle60))	   //ȫ����ת
			            {
						    read_earth();
					        return 1;
				        }
			            mode.angle_wall+=l_ring.length+r_ring.length;
			            mode.step_bp++;
		            case 3:
		                if(do_action(3,angle90))	  //ȫ��ǰ��
			            {
			        	    mode.step_bp++;
			        	    mode.long_wall +=  l_ring.length; //����������Ϊ��׼
			        	    return 1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;	   
			                mode.long_wall +=  l_ring.length;
				            stop_rap();
				            mode.step_bp=0;
			            }
			            return 1;
	                case 4:
		                if(do_action(6,angle360)) //�����ǰ������ת
			            {						   
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				             mode.step_bp = 5 ;
				             return  1;
			        	}
			            read_earth();
			            if(y)
			            {
			                mode.bump=y;			  
				            stop_rap();						
			                mode.long_wall +=  l_ring.length/2;	 //�����߶�λ
				            if(mode.angle_wall>l_ring.length)
				        	{								
			        	        mode.angle_wall-=l_ring.length;
			        		}
				            else 
				        	{					
				                mode.angle_wall=0;
				        	}
				            mode.step_bp=0;
			            }			    
		                break;	  
					case 5:		
		                if(do_action(3,angle90))//ȫ��ǰ��
			            {
				             mode.angle_wall = 0 ;
				             return  1;
			            }
			            if(y)
			            {
			                 mode.bump=y;
				             stop_rap();
			                 mode.long_wall +=  l_ring.length/2;  //�����߶�λ
				             mode.step_bp=0;
			            }
						return 1;	
		            default :
					    stop_rap();
		                mode.step_bp=0;
			            return 1;
	            }	//����switch(mode.step_bp)
		        return 1; 
//----------------------------------------------------------------
//--���������ײ,��ǽ��ײ
        	case  14://W_R:		
                switch (mode.step_bp)
				{
	                case 0:
					    if(do_action(1,angle180))  //ȫ����
						{
						    mode.step_bp = 6;
						}
			            if((y != 14))
			            { 			   
							stop_rap();	
							mode.angle_wall += r_ring.length+l_ring.length;
				            if(y == 0)
				            {
								mode.step_bp = 1;
				            }
				            else
				            {
				                mode.bump = y;
				            }
			            }
			            return 1;
		            case 1:			  
	                case 4:	 
		                if(do_action(6,angle360))	//�����ǰ������ת
			            {
			                mode.step_bp += 2;
			                mode.long_wall +=  l_ring.length/2;	 
							if( mode.angle_wall > l_ring.length)
							{
							     mode.angle_wall -= l_ring.length;
							}
							else
							{
							     mode.angle_wall = 0;
							}
			                return 1; 
			            }
			            if(y != 0)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length/2;
							if( mode.angle_wall > l_ring.length)   //�����߶�λ
							{
							     mode.angle_wall -= l_ring.length;
							}
							else
							{
							     mode.angle_wall = 0;
							}
							if(y == 14)
							{
							    mode.step_bp ++;
							}
							else
							{	  
				                mode.bump = y;
				                mode.step_bp = 0;
							}
			            }
			            return 1;
	                case 2:
		                if(do_action(3,angle60))   //ȫ��ǰ��
			            {
			                mode.step_bp = 0;
			                mode.long_wall +=  l_ring.length;
			                return 1; 
			            }
			            if(y != 14)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length;
							if(y == 0)
							{
							    mode.step_bp = 1;
							}
							else
							{	
				                mode.bump = y;
				                mode.step_bp = 0;
							}
			            }
			            return 1;
					case 3:	  
		                if(do_action(3,angle90))   //ȫ��ǰ��
			            {
			                mode.step_bp = 4;
			                mode.long_wall +=  l_ring.length;
			                return 1; 
			            }
			            if(y != 0)
			            {
			                stop_rap();
				            mode.long_wall +=  l_ring.length;
				            mode.bump = y;
				            mode.step_bp = 0;
			            }
			            return 1;
	                case 6:	 
					    //�޸�ʱ��2009.12.11	 
						w_r.on = 0;
				        mode.bump = 0;
						mode.step_bp = 0;
				        mode.angle_wall = 0 ;
			            return 1;				 
		            default :
					     stop_rap();
		                 mode.step_bp=0;
	            }//����	switch (mode.step_bp)
                return  1;   
            default :
	            mode.bump = 0;	   
        }//����swicth(bump)			   
    } //����if(mode.bump)
    return 0;
} 


