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
uint8 Docking_YaoKong_Manage(void) ;
//======================ȫ�ֺ���==================================
//======================˽�к���==================================	
uint8 FindSeat(void);
uint16 ReadHwSign(void);
/******************************************************************
�س����ĳ�ʼ��
******************************************************************/
void Init_Docking(void)
{
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
	Dis_Power = 3;	
	Dis_PowerGlint = TRUE;
	Dis_SpeedGo = TRUE; 
	/******��ʼ�����õ�ֵ********************/
	mode.mode = DOCKING;
	clr_ram();
	Enable_earth();
	Enable_wall();
	enable_hwincept();           //���������յ�Դ
	Reset_Speed_Send();          //������ⷢ��  
	gbv_action_s_motor = TRUE;
	Display_Real();
	Init_Action();
	WriteWorkState();
}

/******************************************************************
����ɨʱ��ң��������
******************************************************************/
uint8 Docking_YaoKong_Manage(void) 
{
    if(gsv_yaokong.sign == 1)     //��ң������������
	{
	    gsv_yaokong.sign = 0;
		switch(	gsv_yaokong.key)
		{
		    case YAOKONG_ESC:	   
			case YAOKONG_DOCKING_SUN:
			    Init_Cease();
				Enable_Beep();
			    return 1;
			case YAOKONG_SPOT_FRI:	
			    Init_Spot();
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
		        //����uv����uvָʾȡ��
	        case YAOKONG_UV_WED:
				Init_Border();
		    	//Turn_UV();
				//Enable_Sweep();
				Enable_Beep();
				return 1;
		}
	}
	return 0;
}
/******************************************************************
�س����
******************************************************************/
void Do_Docking(void)
{
uint32 m;	
uint16 s;		  
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

   //�г��ĵ�Դ����
  if((power.charge_dc == 1) || (power.charge_seat == 1))
  {
     if((mode.step != 3) || ((mode.step_mk != 4)&&(mode.step_mk != 5)))
	 {	
	     stop_rap();
		 DelayMs(200);
	     mode.step = 3;
		 mode.step_mk = 5;	//���
		 mode.step_bp = 0;
	 }
  }

  //ң������������		     
  if(Docking_YaoKong_Manage() == 1)
  {
      return;
  }
  comm_rap();
  clr_hw_effect(&l_hw);
  clr_hw_effect(&m_hw);
  clr_hw_effect(&r_hw);	
  clr_hw_effect(&up_hw);
  clr_hw_effect(&dummy_wall);  
  
  Io_Driver();
  //��س���û�磬������û���ҵ��س���������ֹͣ�ң�����˯��ģʽ
  if((battery_voltage < NOPOWER)&&(mode.step != 3))
  {
     Init_Sleep() ;
	 return;
  }
  
  //������������е�ʱ���ԤԼʱ�䵽����ǰȥɨ��
  if(Check_PreengageTime())
  {
	   Enable_Beep();
	   Init_Sweep();
	   return;
  }
  //��ң����ʱ����ֹͣ����ң�ط����˶�
  if(YaoKongFangXiang())
  {
      return;
  }
  //�Ƿ�����Ҫ����������
  if((mode.step != 3)||((mode.step_mk != 3)&&(mode.step_mk != 5)))
  {
      if(Action_Protect())
      {
          return;
      }
  }

  //��ֱ�ߺ��ر���ʱ�յ��س����ź�
  if( mode.step != 3)
  {		
      s = ReadHwSign();		
      if((s&0x08) != 0)//�����Ƕ��Ϻ���
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 0;//��������
	  }		  	
      if((s&0x5555) != 0)  //0X4444+0X1111 ���������м�����
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 1;//�������
	  }	
      if((s&0x2222) != 0)  //���������ұ�
	  {
	      stop_rap();
		  mode.step = 3;
		  mode.step_bp = 0;
		  mode.step_mk = 2;//�ұ�����
	  }	
  }
  m = read_bump();
  switch(mode.step)
  {
      case 0:
	      if(do_action(3,METE_5))//METE_H5
		  {
		      mode.step = 1;
		  } 
		  if( m != 0)
		  {
		      stop_rap();
			  mode.angle_wall = angle720; //angle720
			  mode.step = 2;
			  mode.step_bp=0;
		  }	
		  Enable_wall();
		  return; 
	  case 1:
	       if(do_action(1,angle360))
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
		  return;
	  case 2:
	      if(ybs() == 0)
		  {
		      mode.step = 4;
		  }	 
		  return;
	  case 3:
	      if( FindSeat() == 0)
		  {
		      mode.step = 0;
		  }		
	 	 Enable_wall(); // Disable_wall();
	      return;
	  case 4:
	      if(do_action(1,angle120))//angle90
		  {
		      mode.step = 0;
		  }
		  Enable_wall();
		  return;
	  default :
	      mode.step = 0;	       
  }
}
/**********************************************
��ȡ�����ź�
**********************************************/
uint16 ReadHwSign(void)
{  //�����յ��ĺ������ݽ��н���
   //�����Ϊ�󡢶��ϡ��м䡢�ұߣ����������㣬�ó�һ������Ĵ������
    uint16 s;
	    s = 0; 
	//-------------1
	    if((l_hw.effect == 1)&&((l_hw.data&0x0f) == 0))
		{
		    //�����ź��Ƕ����ź�
		    if(l_hw.data == 0x70)
			{		 
		        s |= 0x8000;
			}
				 //�����ź�������ź�
			else if((l_hw.data == 0xe0) || (l_hw.data == 0xa0)
			     || (l_hw.data == 0x60) || (l_hw.data == 0x20))
				 {
				     s |= 0x1000;
				 }
				 //�����ź����м������ź�
				 else if((l_hw.data == 0xb0)|| (l_hw.data == 0x30)
			          || (l_hw.data == 0x80)|| (l_hw.data == 0x00)
					  || (l_hw.data == 0xc0)|| (l_hw.data == 0x40))
					  {
					      s |= 0x4000;
					  }
					  //�����ź����ұ��ź�
					  else if((l_hw.data == 0xd0)|| (l_hw.data == 0x90)
					       || (l_hw.data == 0x50)|| (l_hw.data == 0x10))
					       {
					           s |= 0x2000;
					       }
		}
		//---------2
	    if((m_hw.effect == 1)&&((m_hw.data&0x0f) == 0))
		{
		    if(m_hw.data == 0x70)
			{		 
		        s |= 0x800	 ;
			}
			else if((m_hw.data == 0xe0) || (m_hw.data == 0xa0)
			     || (m_hw.data == 0x60) || (m_hw.data == 0x20))
				 {
				     s |= 0x100;
				 }
				 //�����ź����м������ź�
				 else if((m_hw.data == 0xb0)|| (m_hw.data == 0x30)
			          || (m_hw.data == 0x80)|| (m_hw.data == 0x00)
					  || (m_hw.data == 0xc0)|| (m_hw.data == 0x40))
					  {
					      s |= 0x400;
					  }
					  //�����ź����ұ��ź�
					  else if((m_hw.data == 0xd0)|| (m_hw.data == 0x90)
					       || (m_hw.data == 0x50)|| (m_hw.data == 0x10))
					       {
					           s |= 0x200;
					       }
		}
		//-------3
	    if((r_hw.effect == 1)&&((r_hw.data&0x0f) == 0))
		{
		    if(r_hw.data == 0x70)
			{		 
		        s |= 0x80;
			}
			else if((r_hw.data == 0xe0) || (r_hw.data == 0xa0)
			     || (r_hw.data == 0x60) || (r_hw.data == 0x20))
				 {
				     s |= 0x10;
				 }
				 //�����ź����м������ź�
				 else if((r_hw.data == 0xb0)|| (r_hw.data == 0x30)
			          || (r_hw.data == 0x80)|| (r_hw.data == 0x00)
					  || (r_hw.data == 0xc0)|| (r_hw.data == 0x40))
					  {
					      s |= 0x40;
					  }
					  //�����ź����ұ��ź�
					  else if((r_hw.data == 0xd0)|| (r_hw.data == 0x90)
					       || (r_hw.data == 0x50)|| (r_hw.data == 0x10))
					       {
					           s |= 0x20;
					       }
		}
	    //------4
	    if((up_hw.effect == 1)&&((up_hw.data&0x0f) == 0))
		{
		    if(up_hw.data == 0x70)
			{		 
		        s |= 0x08;
			}
			else if((up_hw.data == 0xe0) || (up_hw.data == 0xa0)
			     || (up_hw.data == 0x60) || (up_hw.data == 0x20))
				 {
				     s |= 0x01;
				 }
				 //�����ź����м������ź�
				 else if((up_hw.data == 0xb0)|| (up_hw.data == 0x30)
			          || (up_hw.data == 0x80)|| (up_hw.data == 0x00)
					  || (up_hw.data == 0xc0)|| (up_hw.data == 0x40))
					  {
					      s |= 0x04;
					  }
					  //�����ź����ұ��ź�
					  else if((up_hw.data == 0xd0)|| (up_hw.data == 0x90)
					       || (up_hw.data == 0x50)|| (up_hw.data == 0x10))
					       {
					           s |= 0x02;
					       }
		}
	return s;
}
/**********************************************
���ֻس����źź��һس���
**********************************************/
uint8 FindSeat(void)
 {
    uint32 m;  //��ײ�ź�
	uint16 s;//  �����ź�
    static uint32 piv_stime = 0;
	static uint32 piv_length = 0;
	    if(mode.bump == 0)
		{
		    mode.bump = 9;
		}
	    m = read_bump();
		s = ReadHwSign();
	    if(( l_rap.rap > 750) || (r_rap.rap > 750) )
		{
	        l_rap.rap = l_rap.rap/2;
	        r_rap.rap = r_rap.rap/2;
	    }
		switch (mode.step_mk )
		{
//---------------------�ҵ����ϵ��ź�
		    case 0:
			    if(((s & 0x7777) != 0)&&(mode.step_bp < 7))
				{
				    stop_rap();
					if((s & 0x1111) != 0)    //�ҵ�����ź�
					{
					    mode.step_bp = 7;
					}	 
					if((s & 0x6666) != 0)	//�ҵ��ұ��ź�
					{
					    mode.step_bp = 8;
					}
			
					return 1;
				}
			    switch(mode.step_bp)
				{
				    case 0:
					    do_action(3,angle720);
						if((m != 0)&&(m < 7))     //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;	  //��ײ�źŵ���ȡ
							break ;
						} 
					    if(((s & 0x0f) == 0)&&(l_ring.length > angle60)) //���Ƕ��Ϻ���
						{
						    stop_rap();
							mode.step_bp = 1;
						}
						break;
					case 1:
					    if(do_action(1,angle360+angle30))
						{
						    mode.step_bp = 2;
						}
						if((m != 0)&&(m < 7))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
						if((s & 0x0f) != 0) 
						{
						    stop_rap();
							mode.step_bp = 5;
						}
						break;
					case 2:
					    if(do_action(3,angle90))	//��ʼʱ��ת360�ȣ�ϣ������߻����ұ߽���ͷ�յ��ź�
						{
							mode.step_bp = 1;
						}
						if((m != 0)&&(m < 7))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
					    if(((s & 0x0f) != 0))
						{
						    stop_rap();
							mode.step_bp = 5;
						}
						break;
					case 3:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							return 1;
						}
						if((m == 0)||(m > 6))
						{
						    stop_rap();
							mode.step_bp = 4;
						}
						break;
				    case 4:
					    if(do_action(1,angle30))
						{
						    mode.step_bp = 2;
						}
						break;
						
					//����ת��ϣ���ҵ��ź�
					case 5:					  
					    if(do_action(3,angle150))
						{
						    mode.step_bp = 6;
						}
						if((m != 0)&&(m < 7))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
							break ;
						} 
					    if(((s & 0x0f) == 0))
						{
						    stop_rap();
							mode.step_bp = 1;
						}
						break;
						
					//����ת��360�ȣ�ϣ���м����ͷ�յ��ź�
					case 6:		   			   //����ת��
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 5;
						}	 
						break;
					case 7:
					    if(do_action(3,angle75))
						{
					        mode.step_bp = 13;
						    mode.step_mk = 1;
						}
						break;
					case 8:
					    if(do_action(1,angle90))
						{
					        mode.step_bp = 9;
						}
						break;
					case 9:
					    if(do_action(3,angle90))
						{
					        mode.step_bp = 12;
						}
						if((m != 0)&&(m < 7))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 10;
						} 
						break;
					case 10:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							return 1;
						}
						if((m == 0)||(m > 6))
						{
						    stop_rap();
							mode.step_bp = 11;
						}
						break;
					case 11:
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 9;
						}
						return 1;
					case 12:
					    if(do_action(1,angle330))
						{
						    mode.step_bp = 9;
						}
						if((s & 0x0600) != 0)
						{
						    stop_rap();
							mode.step_bp = 12;
							mode.step_mk = 2;
						}
						break;
					default :
					    mode.step_bp = 0;
				}
				return 1;	
		    //�ҵ���ߵ��ź�				    
			case 1:
			    if( ((up_hw.effect != 0) && ((up_hw.data & 0x80) == 0 ))
				   && (mode.step_bp < 12))	 //�м����ͷ�յ����ϵ��ź�
				{
				    stop_rap();
					mode.step_bp = 12;
				}
				if(((s&0x0200) != 0) ) 
				{
				    if((mode.step_bp < 12))  //�յ��ұ߻����м��ź�
					{			 
					    mode.step_mk = 2;	
					}
					else
					{
					    if(mode.step_bp <17)
						{
						    mode.step_bp = 17;
						}
					}	
				}
			    switch(mode.step_bp)
			    {
			        //������תϣ���м�Ľ���ͷ���յ�����ź�
			        case 0:
				        if(do_action(2,angle360))
					    {
					        mode.step_bp = 1; //ת��һȦ���м�û���յ��ź�
					    }
					    if(m_hw.effect != 0) //�м����ͷ�յ��źţ�������һ����ǰ��
					    {                     
							DelayMs(100);
					        stop_rap();
							DelayMs(200);
						    mode.step_bp = 9;
					    }
						if((m != 0)&&((m <7)))  //����ײ�źš���һ��
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    return 1;

				    //ת��һȦ��ǰ����ͷ�����յ��źţ�������ǰ��1��
				    case 1:
				        if(do_action(3,METE))
						{		 
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
						}
						if((m_hw.effect == 0)&&(up_hw.effect == 0)&&(l_ring.length > angle60)) //����ͷ�ղ����ź�
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break;
				   	//û���յ��źŵ�ͷ
					case 2:
					    if(do_action(6,angle720+angle120))
						{
							mode.step = 0;
							mode.step_mk = 0;
						}
					    if((m_hw.effect != 0)||(up_hw.effect != 0)) //����ͷ�յ��ź�
					    {
					        stop_rap();
						    mode.step_bp = 0;
					    }
						if((m != 0)&&((m <7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step = 0;
							mode.step_mk = 0;
						}
						break;
					//���������ϰ�����
					case 3:	
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //û����ײ�ź�
					    {
					        stop_rap();
						    mode.step_bp = 4;
					    }
						break;
                    //��������ת90��
				    case 4:
					    if(do_action(2,angle90))
						{
						    mode.step_bp = 5;
						}
						break;
					//����ǰ��15����
					case 5:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //����ײ�źţ��������ұߵ��ϰ��
						{
						    stop_rap();
							mode.step_bp = 6;
						}
						if((m_hw.effect == 0)&&(up_hw.effect == 0)) //����ͷ�ղ����ź�
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break ;
					//�������˵������ϰ�
					case 6:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //û����ײ�ź�
					    {
					        stop_rap();
						    mode.step_bp = 7;
					    }
						break;
					//��������ת��30��
					case 7:
					    if(do_action(1,angle30))
						{
						    mode.step_bp = 5;
						} 
						break;
					//������ǰ������
					case 9:        
				        do_action(3,METE_TWO);	 
						if((m != 0)&&((m < 7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if((m_hw.effect == 0)&&(l_ring.length > angle90))
						{
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 10;
						}
				   	    break;
						
					//ǰ��Ľ���ͷû���յ��źţ���������ת��ϣ���ҵ��ź�
				    case 10:
					    if(do_action(6,angle60))
						{
						    mode.step_bp = 11;
						}
						if((m != 0)&&((m < 7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(m_hw.effect != 0)
						{   
							DelayMs(200);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//ǰ��Ľ���ͷû���յ��źţ���������ת��ϣ���ҵ��ź�
					case 11:
					    if(do_action(1,angle360))
						{
						    mode.step_bp = 1;
						}
						if((m != 0)&&((m < 7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(m_hw.effect != 0)
						{
							DelayMs(100);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//�����յ������źţ�Ȼ������ת120�ȣ�ֱ����߽���ͷ�յ��ź�
					case 12:	
				        if(do_action(2,angle120))
						{
							mode.step_bp = 13;
						}
						if(( s&0xf000) != 0)  
						{
						    stop_rap();
							mode.step_bp = 13;
						}
						if((s&0x0600) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
				   	    break;
					
					//����ǰ��ϣ���ҵ��м�����ұ��ź�
					case 13:
					    if(do_action(3,angle75))
						{
						    if((s & 0x0001) == 0)
						    {
						        stop_rap();
							    mode.step_bp = 16;
								break;
						    }
						}
					    if((m != 0)&&(m < 7))
						{
						    stop_rap();
						    mode.step_bp = 14;
						}
						if((s & 0x0606) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
						break;	
					
					//�����ҵ����м�����ұ��źţ���������ǰ��15����
					case 14:   
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //û����ײ�ź�
					    {
					        stop_rap();
						    mode.step_bp = 15;
					    }
						break;
					case 15	:
					    if(do_action(1,angle30))
						{
						    mode.step_bp = 13;
						}
						break;
					case 16:
					    if(do_action(1,angle360))
						{
						    mode.step_bp = 0;
							mode.step = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0100) != 0)
						{
						    stop_rap();
							mode.step_bp = 12;
						}
						if((s & 0x0606) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
						break;
					case 17:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 19;
						}
						if((m != 0)&&(m<7))
						{
						    stop_rap();
							mode.step_bp = 18;
						}
						break;
					case 18:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //û����ײ�ź�
					    {
					        stop_rap();
						    mode.step_bp = 19;
					    }
						break;	
					//�����ҵ��м���������ת����ϣ��ǰ�����ͷ���յ��м��ź�
					case 19:
					    if(do_action(1,angle360))
						{
						    mode.step = 0;
							mode.step_bp = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0500) != 0)
						{
						    stop_rap();
							mode.step_bp = 0;
							mode.step_mk = 3;
						}
						break;
					default :
					    mode.step_bp = 0;
			    }
			    return 1; 
		    //�ҵ��ұߵ��ź�				    
			case 2:
			    if(((up_hw.effect != 0) && ((up_hw.data & 0x80) == 0 ))
				   && (mode.step_bp < 12))	 //�м����ͷ�յ����ϵ��ź�
				{
				    stop_rap();
					mode.step_bp = 12;
				}
				if(((s&0x0100) != 0) )
				{
				    if((mode.step_bp < 12))  //�յ��ұ߻����м��ź�
					{	   
					    mode.step_mk = 1;	
					}
					else
					{
					    if(mode.step_bp <17)
						{
						    mode.step_bp = 17;
						}
					}	
				}
			    switch(mode.step_bp)
			    {
			        //������תϣ���м�Ľ���ͷ���յ�����ź�
			        case 0:
				        if(do_action(1,angle360))
					    {
					        mode.step_bp = 1; //ת��һȦ���м�û���յ��ź�
					    }
					    if(m_hw.effect != 0) //�м����ͷ�յ��źţ�������һ����ǰ��
					    {                     
							DelayMs(100);
					        stop_rap();
							DelayMs(200);
						    mode.step_bp = 9;
					    }
						if((m != 0)&&((m <7)))  //����ײ�źš���һ��
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    return 1;

				    //ת��һȦ��ǰ����ͷ�����յ��źţ�������ǰ��1��
				    case 1:
				        if(do_action(3,METE))
						{		 
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
						}
						if((m_hw.effect == 0)&&(up_hw.effect == 0)&&(l_ring.length > angle60)) //����ͷ�ղ����ź�
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break;
				   	//û���յ��źŵ�ͷ
					case 2:
					    if(do_action(5,angle720+angle120))
						{
							mode.step = 0;
							mode.step_mk = 0;  //�ź�����
						}
					    if((m_hw.effect != 0)||(up_hw.effect != 0)) //����ͷ�յ��ź�
					    {
					        stop_rap();
						    mode.step_bp = 0;
					    }
						if((m != 0)&&((m <7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step = 0;
							mode.step_mk = 0;
						}
						break;
					//���������ϰ�����
					case 3:	
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //û����ײ�ź�
					    {
					        stop_rap();
						    mode.step_bp = 4;
					    }
						break;
                    //��������ת90��
				    case 4:
					    if(do_action(1,angle90))
						{
						    mode.step_bp = 5;
						}
						break;
					//����ǰ��15����
					case 5:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 0;
						}
						if((m != 0)&&((m <7)))  //����ײ�źţ��������ұߵ��ϰ��
						{
						    stop_rap();
							mode.step_bp = 6;
						}
						if((m_hw.effect == 0)&&(up_hw.effect == 0)) //����ͷ�ղ����ź�
						{
						    stop_rap();
							mode.step_bp = 2;
						}
						break ;
					//�������˵������ϰ�
					case 6:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //û����ײ�ź�
					    {
					        stop_rap();
						    mode.step_bp = 7;
					    }
						break;
					//��������ת��30��
					case 7:
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 5;
						} 
						break;
					//������ǰ������
					case 9:        
				        do_action(3,METE_TWO);	 
						if((m != 0)&&((m < 7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if((m_hw.effect == 0)&&(l_ring.length > angle90))
						{
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 10;
						}
				   	    break;
						
					//ǰ��Ľ���ͷû���յ��źţ���������ת��ϣ���ҵ��ź�
				    case 10:
					    if(do_action(5,angle60))
						{
						    mode.step_bp = 11;
						}
						if((m != 0)&&((m < 7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(m_hw.effect != 0)
						{   
							DelayMs(200);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//ǰ��Ľ���ͷû���յ��źţ���������ת��ϣ���ҵ��ź�
					case 11:
					    if(do_action(2,angle360))
						{
						    mode.step_bp = 1;
						}
						if((m != 0)&&((m < 7)))  //����ײ�ź�
						{
						    stop_rap();
							mode.step_bp = 3;
						}
					    if(m_hw.effect != 0)
						{
							DelayMs(100);
						    stop_rap();
							DelayMs(200);
							mode.step_bp = 9;
						}
					    break;
						
					//�����յ������źţ�Ȼ������ת120�ȣ�ֱ����߽���ͷ�յ��ź�
					case 12:	
				        if(do_action(1,angle120))
						{
							mode.step_bp = 13;
						}
						if(( s&0x00f0) != 0)  
						{
						    stop_rap();
							mode.step_bp = 13;
						}			
						if((s&0x0500) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
				   	    break;
					
					//����ǰ��ϣ���ҵ��м�����ұ��ź�
					case 13:
					    if(do_action(3,angle75))
						{	
						    if((s & 0x0002) == 0)
						    {
						        stop_rap();
							    mode.step_bp = 16;
						    }
						}
					    if((m != 0)&&(m < 7))
						{
						    stop_rap();
						    mode.step_bp = 14;
						}
						if((s & 0x0505) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
						break;	
					
					//�����ҵ����м�����ұ��źţ���������ǰ��15����
					case 14:   
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //û����ײ�ź�
					    {
					        stop_rap();
						    mode.step_bp = 15;
					    }
						break;
					case 15	:
					    if(do_action(2,angle30))
						{
						    mode.step_bp = 13;
						}
						break;
					case 16:
					    if(do_action(2,angle360))
						{
						    mode.step_bp = 0;
							mode.step = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0200) != 0)
						{
						    stop_rap();
							mode.step_bp = 12;
						}
						if((s & 0x0505) != 0)
						{
						    stop_rap();
							mode.step_bp = 17;
						}
						break;
					case 17:
					    if(do_action(3,angle60))
						{
						    mode.step_bp = 19;
						}
						if((m != 0)&&(m<7))
						{
						    stop_rap();
							mode.step_bp = 18;
						}
						break;
					case 18:
					    if(do_action(4,angle360))
						{
						    giv_sys_err = m;
							Init_Err();
							break;
						}
					    if((m == 0)||(m > 7))  //û����ײ�ź�
					    {
					        stop_rap();
						    mode.step_bp = 19;
					    }
						break;	
					//�����ҵ��м���������ת����ϣ��ǰ�����ͷ���յ��м��ź�
					case 19:
					    if(do_action(2,angle360))
						{
						    mode.step = 0;
							mode.step_bp = 0;
							mode.step_mk = 0;
							mode.bump = 0;
						}
						if((s & 0x0600) != 0)
						{
						    stop_rap();
							mode.step_bp = 0;
							mode.step_mk = 3;
						}
						break;
					default :
					    mode.step_bp = 0;
			    }
			    return 1; 

			//�ҵ����м���ź�
			case 3:	
				if((l_bump.key != 0)&&(r_bump.key != 10)&&(mode.step_bp != 10))  //����ײ�ź�
				{
					stop_rap();
					piv_stime = giv_sys_time;
					mode.step_bp = 10;
				}
	            if(( l_rap.rap > 500) || (r_rap.rap > 500) )
		        {
	                l_rap.rap = l_rap.rap/2;
	                r_rap.rap = r_rap.rap/2;
		        }
			   	switch (mode.step_bp)
				{
				    //���Ͻ���ͷ�յ��ұ��źţ�����ת360��ϣ�����ҵ��ź�
				    case 0:
					    do_action(3,METE);
						if((s & 0x0400) != 0)  //�м����ͷ�յ��м��ź�
						{
						     stop_rap();
							 DelayMs(200);
							 mode.step_bp = 6;
						}
						else
						{
						    stop_rap();
							DelayMs(200);
						    if((s & 0x0200) != 0) //�м����ͷ�յ�����ź�
						    {
							    mode.step_bp = 4;
						    }
							else
							{
							    mode.step_bp = 1;
							}
						}	 
					    break;
				    //���Ͻ���ͷ�ҵ�����ź�����ת
					case 1:
					    if(do_action(2,angle90))
						{
						   mode.step_bp = 11;
						}
						else
						{
						    if((s & 0x0200) != 0) //�յ�����ź�
						    {
						        stop_rap();
							    DelayMs(200);
								if(	l_ring.length > angle30)
								{
								    piv_length = l_ring.length;
							        mode.step_bp = 2;
								}
								else
								{
								    enable_rap(FRONT, angle20,FRONT,angle60);
								    mode.step_bp = 7;
								}
						    }
						}
						break;	
					//�м����ͷ�ҵ����м��źţ�������ǰǰ��	
				    case 2:
					    if(do_action(1,(piv_length/2)))
						{
						   mode.step_bp = 3;
						}
						break;	
					//�м����ͷ�յ��ұ��źţ���������ת��ϣ���ҵ��м��ź�
				    case 3:		   
					    if(do_action(3,angle30))
						{
						    mode.step_bp = 0;
						}
						if((s & 0x0100) != 0)
						{
						    stop_rap();
							enable_rap(FRONT, angle60,FRONT,angle20);
						    mode.step_bp = 8;
						} 
						if((s & 0x0200) != 0)
						{
						    stop_rap();
							enable_rap(FRONT, angle20,FRONT,angle60);
						    mode.step_bp = 7;
						}
					    break;
				    case 4:
					    if(do_action(1,angle90))
						{
						   mode.step_bp = 12;
						}
						else
						{
						    if((s & 0x0100) != 0) //�յ�����ź�
						    {
						        stop_rap();
							    DelayMs(200);		
								if(	l_ring.length > angle30)
								{
								    piv_length = l_ring.length;
							        mode.step_bp = 5;
								}
								else
								{						  
								    enable_rap(FRONT, angle60,FRONT,angle20);
								    mode.step_bp = 8;
								}
						    }
						}
						break;
					//�м����ͷ�յ�����źţ���������ת��ϣ���ҵ��м��ź�	  
					case 5:		
					    if(do_action(2,(piv_length/2)))
						{
						   mode.step_bp = 3;
						}
						break;	
					case 6:
					    if(do_action(2,angle90))
						{
						   mode.step_bp = 11;
						}
						if((s & 0x0200) != 0) //�յ�����ź�
						{
						    stop_rap();
						    DelayMs(200);
							mode.step_bp = 4;
						}
						break;
					
					case 7:
					    if((l_rap.sign == 0)&&(l_rap.sign == 0))
						{			  
						   mode.step_bp = 0;
						}	 
						if((s & 0x0400) != 0)  //�յ��м��ź�
						{			 
						     stop_rap();
							 mode.step_bp = 9;
							
						}
						else
						{		
						    if((s & 0x0100) != 0) //�յ�����ź�
						    {					   
						        stop_rap();
							    enable_rap(FRONT, angle60,FRONT,angle20);
							    mode.step_bp = 8;
						    }
						}
						break;
					case 8:	
					    if((l_rap.sign == 0)&&(l_rap.sign == 0))
						{			  
						   mode.step_bp = 0;
						}	 
						if((s & 0x0400) != 0)  //�յ��м��ź�
						{			 
						     stop_rap();
							 mode.step_bp = 9;
							
						}
						else
						{		
						    if((s & 0x0200) != 0) //�յ��ұ��ź�
						    {				   
						        stop_rap();
							    enable_rap(FRONT, angle20,FRONT,angle60);
							    mode.step_bp = 7;
						    }
						}
						break;				  
					case 9:
					    if((l_rap.sign == 0)&&(l_rap.sign == 0))
						{
						    enable_rap(FRONT, angle270,FRONT,angle270);
						}
						if((s & 0x0200) != 0) //�յ��ұ��ź�
						{				   
						    stop_rap();
							enable_rap(FRONT, angle20,FRONT,angle60);
							mode.step_bp = 7;
						}  	
						if((s & 0x0100) != 0) //�յ�����ź�
						{					   
						    stop_rap();
							enable_rap(FRONT, angle60,FRONT,angle20);
							mode.step_bp = 8;
						}
					    break;
					case 10:
					//�ڷ�����ײ�ź�ʱͣ�µȴ�2��	
					    if((giv_sys_time - piv_stime) > 10000)
						{
						    mode.step_mk = 4;	 //�ȴ�
							mode.step_bp = 0;
						}
						break;
					case 11:
					    if(do_action(1,angle90))
						{
						    mode.step_bp = 13;
						}
						if((s & 0x0400) != 0) //�м����ͷ�յ��м��ź�
						{
						    stop_rap();
							mode.step_bp = 9;
						}
						break; 
					case 12:
					    if(do_action(2,angle90))
						{
						    mode.step_bp = 13;
						}
						if((s & 0x0400) != 0) //�м����ͷ�յ��м��ź�
						{
						    stop_rap();
							mode.step_bp = 9;
						}
						break;
					case 13:
					    if(do_action(3,angle75))
						{
						    if((s & 0x0100) != 0)
							{
							    mode.step_mk = 1;
								mode.step_bp = 12;
							}	
						    if((s & 0x0200) != 0)
							{
							    mode.step_mk = 2;
								mode.step_bp = 12;
							}
							return 1;
						}
						if((s & 0x0400) != 0) //�м����ͷ�յ��м��ź�
						{
						    stop_rap();
							mode.step_bp = 9;
						}
						break;
					default :
					    mode.step_bp = 0;
						stop_rap();
				}
				return 1;   //�����յ��м�����ĳ���
			//�������м�����û�ж�׼�缫ʱ��ͷ������
			case 4:
			    switch(mode.step_bp)
				{
				    case 0:	
					    if(do_action(4,angle45))
						{
						    mode.step_bp = 1;
						}
						break;
					case 1:
					    if(do_action(1,angle180))
						{
						   mode.step_bp = 2;
						}
						break;
					case 2:
					    if(do_action(3,METE_H5))
						{
						   mode.step_bp = 4;
						}
						if((m != 0)&&(m < 7))
						{
						    stop_rap();
							mode.step_bp = 3;
						}
						break;
					case 3:
					    if(do_action(4,angle45))
						{
						    mode.step_bp = 4;
						}
						if((m == 0)||(m > 6))
						{
						    stop_rap();
							mode.step_bp = 4;
						}
						break;
					case 4:
					    if(do_action(2,angle180))
						{
						   mode.step_bp = 0;
						   mode.step = 0;
						   mode.step_mk = 0;
						}
						break;
					default :
					    mode.step_bp = 0;
						stop_rap();

				}
				return 1;
			case 5:
			    if(do_action(3,120))
				{
	                    if((power.charge_dc == 1) || (power.charge_seat == 1))
	                    {
	                        Init_Chargeing();
	                        return 1;
	                    } 
	                    else
	                    {
		                    mode.step_mk = 4;
	                    }
				}
	            if(( l_rap.rap > 200) || (r_rap.rap > 200) )
		        {
	                l_rap.rap = 200;
	                r_rap.rap = 200;
		        } 
//					  
				{
				    if((l_ring.length > 30)||(r_ring.length > 30)||((giv_sys_time-action.time) > 500))
					{
				        stop_rap();
	                    if((power.charge_dc == 1) || (power.charge_seat == 1))
	                    {
	                        Init_Chargeing();
	                        return 1;
	                    } 
	                    else
	                    {
		                    mode.step_mk = 4;
	                    }
					}
				}
				return 1;
		}
 return 1;
}
