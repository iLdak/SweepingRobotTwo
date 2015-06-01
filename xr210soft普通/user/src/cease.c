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
uint32 giv_cease_time ;
//======================˽�б���==================================
uint8 piv_yaokongstate = 0;	   //������ʱ������״̬,0û�����ã�1����
                               //��ǰʱ�䣬2����ԤԼʱ��
uint8 piv_dis_hour;			   //��ʾ��Сʱ��
uint8 piv_dis_minute;		   //��ʾ�ķ�����
uint8 piv_dis_week;			   //��ʾ��Сʱ��
uint8 piv_dis_preengage;	   //��ʾ��ԤԼ����
uint8 piv_set_content;         //���õ�����	0Ϊû�����ã�1Ϊ����������2Ϊ����Сʱ��3Ϊ���÷���
                               //4Ϊ��������
//=======================ȫ�ֺ���====================================
void Do_Cease(void);           //ִ���ڴ���״̬�ĳ���
void Init_Cease(void);
void Cease_No_Key(void);
void Cease_YaoKong_Manage(void);
//=======================˽�к���=====================================	
void Cease_display_set(void);
void Cease_SetTime(void);
void Cease_Timing(void);
void Cease_Lode_Time(void);
void Cease_Save_Time(void);
void Cease_Save_Preengage(void);
void Cease_Lode_Preengage(void);
void Cease_Big_Subb_Time(void);
void Cease_Subb_Time(void);
void Cease_Big_Add_Time(void);
void Cease_Add_Time(void);

//=========================����ʵ��======================================
/*****************************************************************
���ܣ��ڴ���ʱ�Ķ���
*****************************************************************/
void Do_Cease(void)
{
    /**************�����ң��������*/
	Cease_YaoKong_Manage() ;

	//������ϵİ��� ����
	if(key1.sign == 0)		//����SPOT����
	{
	   key1.sign = 1;
	   Enable_Beep();
	   Init_Spot();
	   return ;
	}			 
	if(key2.sign == 0)	    //����AUTO����
	{
	   key2.sign = 1; 
	   Enable_Beep();
	   Init_Sweep();
	   return;
	}
	if(key3.sign == 0)	    //����UV����
	{
	   key3.sign = 1;  
	   Enable_Beep();
	   //Turn_UV();
	   //Init_Cease();
	   Init_Border();
	}
    if((power.charge_dc == 1) || (power.charge_seat == 1))
	{
	   if(((giv_sys_err_bak != 25)&&(giv_sys_err_bak != 26)))
	   {
	       Init_Chargeing(); 
	       return;
	   }
	}
	/*********�������ò�ͬ��ʾ��ͬ����******/
    if(piv_yaokongstate == 0)
	{		
	    Display_Real();
	}
	else
	{
	    Cease_display_set();
	}
	Dis_Power = Get_Dispower();	  
	//=========������ԤԼʱ�䵽����������ɨ��ģʽ
	if(Check_PreengageTime())
	{
	   Enable_Beep();
	   Init_Sweep();
	   return;
	}
    /**********���������5������û�в���,��������˯��ģʽ**********/
	if((giv_sys_time - giv_cease_time) > 3000000)
	{
	   Init_Sleep();
	   return;
	}
}
/******************************************************************
�ڴ���״̬����ʾ��ʼ��
******************************************************************/
void Init_Cease(void)
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
    Dis_SpeedGo = FALSE;        //����ͼ�겻ת��
	   
    Display_Real();	            //��ʾ��ǰ������
	
	/******��ʼ�����õ�ֵ********************/
	piv_set_content = 0;		//��ʼ����Ϊû������
	piv_yaokongstate = 0;

	/*******��ʼ������Ŀ���***************/
	stop_rap();                 //�ر�������
	Init_Io_Driver();           //�ر���ɨ������
	Disable_earth();
	Disable_wall();	

	disable_hwincept();         //�رպ�����յ�Դ
	Set_Speed_Send();           //�رպ��ⷢ��    
	
	/****���û����Ĺ���ģʽ******/   
	mode.mode = CEASE; 
	giv_cease_time = giv_sys_time;   
	WriteWorkState();
}
/*****************************************************************
���ܣ���ʾ����ʱ��״̬
*****************************************************************/
void Cease_display_set(void)
{
	Dis_Preengage = piv_dis_preengage;
    Dis_Week = piv_dis_week;
	Dis_Minute = piv_dis_minute;	
	Dis_Hour = piv_dis_hour;
	Display_Work_Mode();
	Dis_PrGlint = FALSE;
	Dis_HourGlint = FALSE;
	Dis_MinuteGlint = FALSE;
	Dis_WeekGlint =FALSE;
	switch (piv_set_content	)
	{
		case 1:	 
	        Dis_PrGlint = TRUE;	 
		    break;
	    case 2:
	        Dis_HourGlint = TRUE; 	
		    break;
	    case 3:
	        Dis_MinuteGlint = TRUE;	
		    break;
	    case 4:						 
	        Dis_WeekGlint =TRUE; 
		    break;
	}
}
/*****************************************************************
���ܣ�����ң�����������
******************************************************************/
void Cease_YaoKong_Manage(void)
{
    if(gsv_yaokong.sign == 1) //--------------��ң������������
	{
	    gsv_yaokong.sign = 0; 
	   Enable_Beep();
	    switch( piv_yaokongstate)
		{
		    //����ǰû�а�������ʱ�����
		    case 0:
			    Cease_No_Key();
				break;

		    //ԭ�������趨ʱ���
			case 1:
			    Cease_SetTime();
			    break;

			//ԭ�������趨ԤԼ��
			case 2:
			    Cease_Timing();
			    break;
			default :
			piv_yaokongstate = 0;
		}
	    giv_cease_time = giv_sys_time;     
	}
}
/*******************************************************
���ܣ���û��Ԥ�Ȱ��°���ʱ�Ĵ������
*******************************************************/
void Cease_No_Key(void)
{
uint32 liv_time;
    switch(	gsv_yaokong.key)
	{
		    //------------���µ�Դ������ȡ����
	    case YAOKONG_ESC:
			liv_time = giv_sys_time;
		    //============�ȴ���Դ��������===================
		    do
			{
			    A7105_rx();	
				Beep_Driver();	
				IWDG_ReloadCounter();			   
		        if((giv_sys_time - liv_time) > 30000)  //����5��
			    {
			        PreengageDate = 0;                 //���ԭ����ԤԼ����	  
				    PreengageTime = 0;
				    WritePreengage();	
					Enable_Beep();
					break;			                         
			    }
			}
			while(( gsv_yaokong.key == YAOKONG_ESC) &&(gsv_yaokong.effect != 0)) ;

		    //-------------------�жϵ�Դ���Ƿ���ʱ�䳬��3��
			if((giv_sys_time - liv_time) < 30000)
			{
			    Init_Sleep();    //����˯��ģʽ
			}
		    break;	
		
		    //-------------------����ԤԼ��������ԤԼ����	 
	    case YAOKONG_TIMINE_MON:
			Cease_Lode_Preengage();//��ԤԼ����װ����ʱ������
			piv_set_content = 1;
		    piv_yaokongstate = 2;
		    break;

		    //����ʱ�䰴������ʱ���趨
	    case YAOKONG_SETTIME_TUE:	
		    Cease_Lode_Time();
			piv_set_content = 4;
		    piv_yaokongstate = 1;
		    break;

		    //����uv����uvָʾȡ��
	    case YAOKONG_UV_WED:
		    Turn_UV();
			Init_Border();
		    //Turn_UV();
		    break;

		    //���¶�����ɨ���붨����ɨģʽ
	    case YAOKONG_AUTO_THU:
		    Init_Sweep();;
		    break;

		    //�����Զ���ɨ��
	    case YAOKONG_SPOT_FRI:
		    Init_Spot();
		    break;

	    case YAOKONG_V_SAT:
		    Change_Speed();
		    break;

	    case YAOKONG_DOCKING_SUN:  
			Init_Docking();
		    break;
	    case YAOKONG_LEFT:
	    case YAOKONG_RIGHT:
	    case YAOKONG_FRONT:
	    case YAOKONG_BACK:
		    Init_Tele();
		    break;
	}
}
/*************************************************************
װ��ʱ��
*************************************************************/
void Cease_Lode_Time(void)
{	
    piv_dis_preengage = (uint8)PreengageDate;	//ԤԼ����	
    piv_dis_week = (uint8)(((Rtc_time%604800) / 86400)) ;
    piv_dis_minute = (uint8)((Rtc_time % 3600) / 60);
    piv_dis_hour = (uint8)((Rtc_time % 86400) / 3600) ;

}
/*************************************************************
����ʱ��
*************************************************************/
void  Cease_Save_Time(void)
{
uint32 t;
    t = Rtc_time - (Rtc_time%604800);
    Rtc_time = t + (piv_dis_hour*3600) + (piv_dis_minute*60) + ((piv_dis_week)*86400);
	WriteRtcTime();
}
/*************************************************************
װ��ʱ��
*************************************************************/
void Cease_Lode_Preengage(void)
{	
    piv_dis_preengage = (uint8)PreengageDate;	//ԤԼ����	
    piv_dis_week = (uint8)((Rtc_time%604800) / 86400 );
    piv_dis_minute = (uint8)(PreengageTime % 256);
    piv_dis_hour = (uint8)(PreengageTime / 256);

}
/*************************************************************
����ʱ��
*************************************************************/
void  Cease_Save_Preengage(void)
{
    PreengageDate = piv_dis_preengage ;
	PreengageTime = piv_dis_minute + (piv_dis_hour<<8);
	WritePreengage();
}

/*************************************************************
�����ö�ʱԤԼ����
*************************************************************/
void Cease_Timing(void)
{	 
uint32 liv_time;	 //�����Ŀ�ʼ����ʱ��
uint8 liv_number;	 
uint8 liv_key_down_time;  //��������ʱ����ʱ��
    switch(	gsv_yaokong.key)
	{
		//////���µ�Դ������ȡ����
	    case YAOKONG_ESC:
			 liv_time = giv_sys_time;
		    ////////�ȴ���Դ��������//////////////////
		    do
			{
			    A7105_rx();		//	
				Beep_Driver();	 
				IWDG_ReloadCounter(); 
		        if((giv_sys_time - liv_time) > 30000)  //����5��
			    {
			        PreengageDate = 0;        //���ԭ����ԤԼ����	
				    PreengageTime = 0;
				    WritePreengage();         //
					Enable_Beep();
			    }
			}
			while(( gsv_yaokong.key == YAOKONG_ESC) &&(gsv_yaokong.effect != 0)) ;

		    ////�жϵ�Դ���Ƿ���ʱ�䳬��5��//////
			Init_Cease();
		    break;	
		
		///////����ԤԼ��������ԤԼ����	 
	    case YAOKONG_TIMINE_MON:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x02;   
			}
		    break;

		///////����ʱ�䰴������ʱ���趨
	    case YAOKONG_SETTIME_TUE:	
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x04;    
			}
		    break;

		//////����uv����uvָʾȡ��
	    case YAOKONG_UV_WED:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x08;  
			}
		    break;

		/////���¶�����ɨ���붨����ɨģʽ
	    case YAOKONG_AUTO_THU:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x10;   
			}
		    break;

		////�����Զ���ɨ��///////////////
	    case YAOKONG_SPOT_FRI:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x20;   
			}
		    break;

	    case YAOKONG_V_SAT:
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x40; 
			}
		    break;

	    case YAOKONG_DOCKING_SUN:  
		    if(	piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x1; 
			}
		    break;

	    case YAOKONG_ED:	   
		    if( piv_set_content == 1)
			{
			    piv_dis_preengage ^= 0x80;    
			}
		    break;

	    case YAOKONG_LEFT:		 
		    piv_set_content--; 
		    if(piv_set_content == 0)
			{
			   piv_set_content = 3;
			}
			break;
	    case YAOKONG_RIGHT:	  
		    piv_set_content++; 
		    if(piv_set_content == 4)
			{
			   piv_set_content = 1;
			}
			break;
	    case YAOKONG_FRONT:
		    liv_time = giv_sys_time; //���濪ʼ���°�����ʱ��
			liv_number = 0;
		    do
			{
			    A7105_rx();		// ��ȡ����	  
				Beep_Driver();
				liv_key_down_time = (uint8)((giv_sys_time - liv_time)/10000) ; //10000
			    //����ʱ�����3�벢�ҵ�����ʱ��
				if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
				{
				   liv_number =  liv_key_down_time;
				   Cease_Big_Add_Time();
				}			 
				Cease_display_set();
				AutoDisplay();
				IWDG_ReloadCounter();
			}
			while(( gsv_yaokong.key == YAOKONG_FRONT) &&(gsv_yaokong.effect != 0)) ;	
		    Cease_Add_Time();
			break;
	    case YAOKONG_BACK:
		    liv_time = giv_sys_time; //���濪ʼ���°�����ʱ��
			liv_number = 0;
		    do
			{
			    A7105_rx();		// ��ȡ����	  
				Beep_Driver();
				liv_key_down_time = (uint8)((giv_sys_time - liv_time)/10000) ;//10000
			    //����ʱ�����3�벢�ҵ�����ʱ��
				if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
				{
				   liv_number =  liv_key_down_time;
				   Cease_Big_Subb_Time();
				}			 
				Cease_display_set();
				AutoDisplay();
				IWDG_ReloadCounter();
			}
			while(( gsv_yaokong.key == YAOKONG_BACK) &&(gsv_yaokong.effect != 0)) ;
		    Cease_Subb_Time();
			break;

	    case YAOKONG_OK:
		    Cease_Save_Preengage();	 
			Init_Cease();
		    break;
	}

}

/*************************************************************
������ʱ�����
*************************************************************/
void Cease_SetTime(void)
{
uint32 liv_time;	 //�����Ŀ�ʼ����ʱ��
uint8 liv_number;	 
uint8 liv_key_down_time;  //��������ʱ����ʱ��
    switch(	gsv_yaokong.key)
	{
		//////���µ�Դ������ȡ����
	    case YAOKONG_ESC:				   
			Init_Cease();
		    break;
		///////����ԤԼ��������ԤԼ����	 
	    case YAOKONG_TIMINE_MON:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 1;   
			}
		    break;

		///////����ʱ�䰴������ʱ���趨
	    case YAOKONG_SETTIME_TUE:	
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 2;  
			}
		    break;

		//////����uv����uvָʾȡ��
	    case YAOKONG_UV_WED:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 3;  
			}
		    break;

		/////���¶�����ɨ���붨����ɨģʽ
	    case YAOKONG_AUTO_THU:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 4;  
			}
		    break;

		////�����Զ���ɨ��///////////////
	    case YAOKONG_SPOT_FRI:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 5; 
			}
		    break;

	    case YAOKONG_V_SAT:
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 6;
			}
		    break;

	    case YAOKONG_DOCKING_SUN:  
		    if(	piv_set_content == 4)
			{
			    piv_dis_week = 0;
			}
		    break;
	
	    case YAOKONG_LEFT:	   
		    piv_set_content--;
			if(piv_set_content == 1)
			{
			    piv_set_content = 4;
			}
			break;
	    case YAOKONG_RIGHT:
		    piv_set_content++;
			if(piv_set_content == 5)
			{
			    piv_set_content = 2;
			}	 
			break;
	    case YAOKONG_FRONT:
		    liv_time = giv_sys_time; //���濪ʼ���°�����ʱ��
			liv_number = 0;
		    do
			{
			    A7105_rx();		// ��ȡ����	
				Beep_Driver();
				liv_key_down_time = (uint8)((giv_sys_time - liv_time)/10000) ;
			    //����ʱ�����3�벢�ҵ�����ʱ��
				if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
				{
				   liv_number =  liv_key_down_time;
				   Cease_Big_Add_Time();
				}			 
				Cease_display_set();
				AutoDisplay();
				IWDG_ReloadCounter();
			}
			while(( gsv_yaokong.key == YAOKONG_FRONT) &&(gsv_yaokong.effect != 0)) ;	
		    Cease_Add_Time();
			break;
	    case YAOKONG_BACK:
		    liv_time = giv_sys_time; //���濪ʼ���°�����ʱ��
			liv_number = 0;
		    do
			{
			    A7105_rx();		// ��ȡ����	 
				Beep_Driver();
				liv_key_down_time = (uint8)((giv_sys_time - liv_time)/10000) ;
			    //����ʱ�����3�벢�ҵ�����ʱ��
				if((liv_key_down_time > 2 )&&(liv_number< liv_key_down_time))
				{
				   liv_number =  liv_key_down_time;
				   Cease_Big_Subb_Time();
				}			 
				Cease_display_set();
				AutoDisplay();
				IWDG_ReloadCounter();
			}
			while(( gsv_yaokong.key == YAOKONG_BACK) &&(gsv_yaokong.effect != 0)) ;
		    Cease_Subb_Time();
			break;

	    case YAOKONG_OK:
		    Cease_Save_Time();	  	   
			Init_Cease();
		    break;
	}
}
/*******************************************************************************
��������ʱ��
*******************************************************************************/
void Cease_Big_Add_Time(void)
{
    if(piv_set_content == 3)
    {
	    piv_dis_minute = (piv_dis_minute+3)%60;
	}
	else if(piv_set_content == 2)
	     {
	         piv_dis_hour = (piv_dis_hour + 3) % 24;
	     }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+3)%7;
			  }	  */
}

/*******************************************************************************
��������ʱ��
*******************************************************************************/
void Cease_Add_Time(void)
{
    if(piv_set_content == 3)
	{
		piv_dis_minute = (piv_dis_minute+1)%60;
	}
	else if(piv_set_content == 2)
		 {
		     piv_dis_hour = (piv_dis_hour + 1) % 24;
		 }
		 /*else if(piv_set_content == 4)
			 {
				 piv_dis_week = (piv_dis_week+1)%7;
			 }	 */
}

/*******************************************************************************
��������ʱ��
*******************************************************************************/
void Cease_Big_Subb_Time(void)
{
    if(piv_set_content == 3)
    {
	    piv_dis_minute = (piv_dis_minute+57)%60;
	}
	else if(piv_set_content == 2)
		 {
			 piv_dis_hour = (piv_dis_hour + 21) % 24;
		 }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+4)%7;
		      }*/
}

/*******************************************************************************
��һ����ʱ��
*******************************************************************************/
void Cease_Subb_Time(void)
{
    if(piv_set_content == 3)
	{
	    piv_dis_minute = (piv_dis_minute+59)%60;
	}
	else if(piv_set_content == 2)
		 {
		     piv_dis_hour = (piv_dis_hour + 23) % 24;
		 }
		 /*else if(piv_set_content == 4)
			  {
			      piv_dis_week = (piv_dis_week+6)%7;
			  }	*/
}
