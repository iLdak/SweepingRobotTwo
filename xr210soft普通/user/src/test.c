#include "test.h"													  
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
//======================˽�ж���==================================
#define  VER   350	//109
//======================ȫ�ֱ���==================================
//======================˽�б���==================================
//======================ȫ�ֺ���==================================
void frontandback(void) ;
//======================˽�к���==================================
void Test_Earth(uint8 state);
void Test_Wall(uint8 state);
void Test_HwIncept(uint8 state);
void Test_Current(uint8 state);
void Test_Action(uint8 state);
void Test_KeyAndYaoKong(uint8 state);
void Test_Display(uint8 state);
void Test_MainBoard(uint8 state);
//=======================����ʵ��===================================
/******************************************************************
�Լ���������
******************************************************************/
void Do_Test(void)
{
uint32 piv_time;
bool pbv_test;                   //�Ƿ�����Լ�
uint8 piv_test_state;            //�Լ������
uint8 piv_test_item;             //�Լ����Ŀ 0 ���ؼ죬 1ǽ�� ��2������� ��3������
                                 //4 ��ѹ������ 5���� ��6 ��ʾ���汾ϵͳ��Ϣ
    piv_time = giv_sys_time;   
    Dis_On = TRUE;              //��ʾ��־��
    Dis_ErrOn = TRUE ;    	    //����ͼ����
    Dis_UvOn = TRUE ;			//uvͼ����
    Dis_PrOn = TRUE ;			//ԤԼ������
    Dis_AuOn = TRUE ;			//�Զ���ɨ��
    Dis_SpOn = TRUE ;			//�ֲ���ɨ��
    Dis_HourOn = TRUE ;		    //Сʱ��
    Dis_MinuteOn = TRUE ;		//������
    Dis_ColOn = TRUE ;		    //ʱ�����
    Dis_WeekOn = TRUE ;		    //������
    Dis_SpeedOn = TRUE ;		//�ٶ���
    Dis_PowerOn = TRUE ;		//���ͼ����
    Dis_ChargeOn = TRUE ;		//���ͼ����
    Dis_ErrGlint = TRUE ;		//����ͼ����˸
    Dis_UvGlint = TRUE ;		//uvͼ����˸
    Dis_PrGlint = TRUE ;		//ԤԼ������˸
    Dis_AuGlint = TRUE ;		//�Զ���ɨ��˸
    Dis_SpGlint = TRUE ;		//�ֲ���ɨ��˸
    Dis_HourGlint = TRUE ;	    //Сʱ��˸
    Dis_MinuteGlint = TRUE ;	//������˸
    Dis_ColGlint = TRUE ;		//ʱ�����˸
    Dis_WeekGlint = TRUE ;	    //������˸
    Dis_SpeedGlint = TRUE ;	    //�ٶ���˸
    Dis_PowerGlint = TRUE ;	    //���ͼ����˸
    Dis_ChargeGlint = TRUE ;	//���ͼ����˸
    Dis_Preengage = 0xff;	    //ԤԼ����
    Dis_Hour = 0; 		        //��ʾ��Сʱ
    Dis_Minute = 0;  		    //��ʾ�ķ���
    Dis_Week = 0;			    //��ʾ����������  ��Χ 1����7
    Dis_Speed = 1;		        //��ʾ���ٶȣ���Χ1����3
    Dis_Power = 3;		        //��ʾ�ĵ�Դ���� ����Χ0����3	
	Enable_earth();
	Enable_wall();
	enable_hwincept();          //���������յ�Դ
	Reset_Speed_Send();         //������ⷢ��  
	pbv_test = FALSE;
	piv_test_state = 0;
	piv_test_item = 0;
	Enable_Beep(); 
	do
	{	  
	    Beep_Driver();          //������������
	    AutoReadKey();          //�Զ���ȡ����
	    AutoDisplay();          //�Զ���ʾ
		IWDG_ReloadCounter();
		if((key3.sign == 0)  && (key1.sign == 0))  //�޸Ľ�����԰���2011/10/29
        { 
            key1.sign = 1;
	        key2.sign = 1;
	        key3.sign = 1;
	        pbv_test = TRUE;	 
	        Enable_Beep(); 
			clr_display();
			ReadBakBatteryActivation();
			giv_BatteryActivation=0;
            WriteBakBatteryActivation();
			ReadBakBatteryActivation();
			Dis_On = TRUE;        
			piv_test_state = 0;
			giv_sys_err = 0;
		    break;
        }
	}while(giv_sys_time - piv_time < 30000)  ;
	
	while(pbv_test == TRUE)
	{
	    Beep_Driver();            //������������
	    AutoReadKey();            //�Զ���ȡ����
	    AutoDisplay();            //�Զ���ʾ 
        judge_charge();           //�Զ��ж��Ƿ��г���ź�
        sampling_temp_voltage();  //�Զ�����������	  
	    A7105_rx();	              //�Զ�����ң�����ź�
	    read_wallearth();		  //�Զ�����ǽ��͵ؼ�����
		IWDG_ReloadCounter();
        clr_hw_effect(&l_hw);
        clr_hw_effect(&m_hw);
        clr_hw_effect(&r_hw);	
        clr_hw_effect(&up_hw); 	 
        clr_hw_effect(&dummy_wall); 

        chargeing();
		//����
		if(key3.sign == 0)
		{			
		    key3.sign = 1;
		    piv_test_state++;
			giv_sys_err = 0;
			if (((piv_test_state == 4)&&(piv_test_item == 0))
			 || ((piv_test_state == 4)&&(piv_test_item == 1))
			 || ((piv_test_state == 5)&&(piv_test_item == 2))
			 || ((piv_test_state == 4)&&(piv_test_item == 3))
			 || ((piv_test_state == 2)&&(piv_test_item == 4))
			 || ((piv_test_state == 8)&&(piv_test_item == 5))
			 || ((piv_test_state == 6)&&(piv_test_item == 6))
			 || ((piv_test_state == 3)&&(piv_test_item == 7)))
			{
			    piv_test_state = 0;
			}  	 
	        Enable_Beep(); 
			clr_display();
			Dis_On = TRUE;       
			clr_ram();
		}
		if((key2.sign == 0))
		{	
	        key2.sign = 1;
		    piv_test_item++;   
			giv_sys_err = 0;
			if(piv_test_item == 8)
			{
			    piv_test_item = 0;
			}
	        Enable_Beep(); 
			clr_display();
			Dis_On = TRUE;  
			piv_test_state = 0;
			clr_ram();	
		}
		if((key1.sign == 0))
		{
	        key1.sign = 1;	  
	        Enable_Beep(); 
		    pbv_test = FALSE;
			return;
		}
		if(giv_sys_err != 0)
		{					
            /******��ʼ����ʾ***********/
            clr_display();	
	        /******��ʼ�����õ�ֵ********************/
	        //clr_ram();
            Dis_On = TRUE;              //��ʾ��־��
	
            Dis_HourOn = TRUE; 		    //Сʱ��
            Dis_MinuteOn = TRUE; 		//������ 
	        Dis_ErrOn = TRUE;
	        Dis_ErrGlint = TRUE;
	        Dis_Hour = giv_sys_err / 100 + 140;
	        Dis_Minute = giv_sys_err % 100;
		}
		else
		{
		    switch(piv_test_item)
		    {    	
		        //����mainboard������
		        case 0:
				    giv_sys_err = ReadBakErr();
				    if(giv_sys_err)
					break;
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 0;
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE;   
				    Dis_Hour = piv_test_state*10;
				    Dis_Minute = 0;  
				    Test_MainBoard(piv_test_state);
				    break;
		        //���Եؼ������
		        case 1:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 0;
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_Earth(piv_test_state);
				    break;
				
			    //����ǽ�������
			    case 2:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 1;
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_Wall(piv_test_state);
				    break;
				
			    //�������������
		 	    case 3:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 2;
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_HwIncept(piv_test_state);
				    break;
						   
			    //��⿪���ź�
			    case 4:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 3;	  
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
                    Test_KeyAndYaoKong(piv_test_state);
				    break;
			    //���������ֵĵ�����ѹ
			    case 5:	  
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 4;	
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_Current(piv_test_state);
			        break;
			    //���Ը��ֶ�������ʾ�������ٶ�
			    case 6:		
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 5;	
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE; 
				    Test_Action(piv_test_state);
			        break;
			    case 7:
				    Dis_WeekOn = TRUE; 
				    Dis_Week = 6;	
				    Dis_HourOn = TRUE; 
				    Dis_MinuteOn = TRUE;
				    Test_Display(piv_test_state);
			        break;
			    default :
			        piv_test_item = 0;
			
		    }
		}
	} 
}
/******************************************************************
����ң�����Ϳ�����
******************************************************************/	 
void Test_Display(uint8 state)
{
uint8 i;//ѭ����������
uint8 m;
        switch(state)
		{
		    case 0:	
			    Clr_DisplayData();	  
                lcdkey_bgon();     //�򿪱���
	            lcdkey_dison();    //����ʾ
				for(m=0; m<10;m++)
				{
					Dis_Data[m] = (0xff);
				}			  	
		        lcdkey_disdata(12, Dis_Data, 10); 
				while((key1.sign == 1)&&(key2.sign == 1)&&(key3.sign == 1))
				{					 
					IWDG_ReloadCounter();	
				    AutoReadKey();
				}
			    break;
			case 1:
			    Clr_DisplayData();
				i = 0;
                lcdkey_bgon();     //�򿪱���
	            lcdkey_dison();    //����ʾ
			    while((key1.sign == 1)&&(key2.sign == 1)&&(key3.sign == 1))
				{
			        if(Sec == TRUE)
	                {
	                    Sec = FALSE;
						i++;
						Clr_DisplayData();
						//�޸��Լ����ʾ
						i = i%24;
						if(i<4)
						{
						    for(m=0; m<10;m++)
							{
							    Dis_Data[m] = (0x11<<(i));
							}
						}
						else
						{
						    if((i%2)==0)
							{	  
						        Dis_Data[(i-4)/2] = 0x0f;
							}
							else
							{						  
						        Dis_Data[(i-4)/2] = 0xf0;
							}
						}
		                lcdkey_disdata(12, Dis_Data, 10); 
					}	
					IWDG_ReloadCounter();
				    AutoReadKey();
				}
			    break;
			case 2:		   
				Dis_Hour =VER/100;// 10+VER/100; 
				Dis_Minute = VER%100;
			    break;
		}
}	   
/******************************************************************
����ң�����Ϳ�����
******************************************************************/	 
void Test_KeyAndYaoKong(uint8 state)
{
        switch(state)
	    {
		    //���Կ�����
	        case 0:		  
				//������ؿ���	 
				Dis_Hour = 10; 
				Dis_Minute = 0; 
				if(l_lidi.key == 1)
				{
				    Dis_ErrOn = FALSE;
				}
				else 
				{
				    Dis_ErrOn = TRUE;
				}	 
				//������ؿ���  
				if(r_lidi.key == 1)
				{
				    Dis_SpOn = FALSE;
				}
				else 
				{
				    Dis_SpOn = TRUE;
				}
				//�����ײ����
				if(l_bump.key == 0)
				{
				    Dis_UvOn = FALSE;
				}
				else 
				{
				    Dis_UvOn = TRUE;
				}
				//�ұ���ײ����
				if(r_bump.key == 0)
				{
				    Dis_AuOn = FALSE;
				}
				else 
				{
				    Dis_AuOn = TRUE;
				}

			    break;
			//����ң����
			case 1:		   
				Dis_Hour = 20; 
				if(gsv_yaokong.effect == 1)
				{
				    Dis_Minute = gsv_yaokong.key;
				}
				else
				{
					Dis_MinuteOn = FALSE;
				}
				 
			    break;
	    }
}
/******************************************************************
���Լ�������
******************************************************************/
void Test_Action(uint8 state)
{
static uint32 piv_length = 0;
        switch(state)
		{
		    //��ǰ��ʾ�����ٶ�
		    case 0:		  
			    comm_rap();
				do_action(3,0xffffff);
			    if(Sec == TRUE)
	            {
	                Sec = FALSE;
					Dis_Hour = (uint8)((l_ring.length - piv_length)/1000 + 10);
					Dis_Minute = (uint8)(((l_ring.length - piv_length)%1000)/10);
					piv_length = l_ring.length ;
				}
			    break;
			//��ǰ��ʾ�����ٶ�
			case 1:	
			    comm_rap();
				do_action(3,0xffffff);
			    if(Sec == TRUE)
	            {
	                Sec = FALSE;
					Dis_Hour = (uint8)((r_ring.length - piv_length)/1000 + 20);
					Dis_Minute = (uint8)(((r_ring.length - piv_length)%1000)/10);
					piv_length = r_ring.length ;
				}	
			    break;
			//�����ʾ�����ٶ�
			case 2:			  
			    comm_rap();
				do_action(4,0xffffff);
			    if(Sec == TRUE)
	            {
	                Sec = FALSE;
					Dis_Hour = (uint8)((l_ring.length - piv_length)/1000 + 30);
					Dis_Minute = (uint8)(((l_ring.length - piv_length)%1000)/10);
					piv_length = l_ring.length ;
				}
			    break;
			//�����ʾ�����ٶ�
			case 3:	   	   
			    comm_rap();
				do_action(4,0xffffff);
			    if(Sec == TRUE)
	            {
	                Sec = FALSE;
					Dis_Hour = (uint8)((r_ring.length - piv_length)/1000 + 40);
					Dis_Minute = (uint8)(((r_ring.length - piv_length)%1000)/10);
					piv_length = r_ring.length ;
				}
			    break;
			//����򿪹ر�uv�ơ�
			case 4:
				Io_Driver();
				Dis_Hour = 50;
				Dis_Minute = 0;
				if(Sec == TRUE)
				{			
	                Sec = FALSE;
					if(gbv_action_uv == TRUE)
					{
					   gbv_action_uv = FALSE; 
					   Dis_UvOn = FALSE;
					}
					else
					{
					   gbv_action_uv = TRUE; 
					   Dis_UvOn = TRUE;
					}
				}
			    break; 
		    //�ϻ�����
			case 5:	   
				Io_Driver();  
			    comm_rap();	
				Dis_Hour = 60;
				Dis_Minute = 0;
				gbv_action_uv = TRUE;  
				gbv_action_m_motor = TRUE;	
				gbv_action_s_motor = TRUE;
				gbv_action_dust_motor = TRUE;
				frontandback();
				//----------2011/10/29
			   if(key1.key == 0)//��������
				{
				    Dis_SpOn =  TRUE;
				}
				else 
				{
				    Dis_SpOn = FALSE;
				}
			    if(key2.key == 0)//��������
				{
				    Dis_PrOn =  TRUE;
				}
				else 
				{
				    Dis_PrOn = FALSE;
				}
			    if(key3.key == 0)//��������
				{
				    Dis_UvOn =  TRUE;
				}
				else 
				{
				    Dis_UvOn = FALSE;
				}
				
			    break;
		}
}
/******************************************************************
���Ը������ֵĵ�����ѹ
******************************************************************/
void Test_Current(uint8 state)
{
uint32 piv_current;	 //��ʱ���ڵĵ���
uint8 i;			   //ѭ��������
        switch(state)
		{
		    //���ֵĵ���
		    case 0:
			    do_action(6,0xffffff);
			    comm_rap();
				piv_current = (l_current_1s * 33000)/(4096 * 47);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 10);
				Dis_Minute = (uint8)(piv_current%100);
			    break;  
			//������ֵĵ���
			case 1:		
			    do_action(5,0xffffff); 
			    comm_rap();
				piv_current = (r_current_1s * 33000)/(4096 * 47);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 20);
				Dis_Minute = (uint8)(piv_current%100);
			    break;  
				  		 
			//�����ɨ�ĵ��� ---------��ת���ֵ������رշ�ת���ֵ���
			case 2:			
			    gbv_action_m_motor = TRUE;
				Io_Driver();
				piv_current = (m_current_1s * 33000)/(4096 * 47);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 30);
				Dis_Minute = (uint8)(piv_current%100);
			    break; 

			//������ĵ���   
			case 3:			
			    gbv_action_dust_motor = TRUE;
				Io_Driver();
				piv_current = (dust_current_1s * 33000)/(4096 * 47);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 40);
				Dis_Minute = (uint8)(piv_current%100);	
			    break;  

			//����ɨ�ĵ���  
			case 4:		   
			    gbv_action_s_motor = TRUE;
				Io_Driver();
			    Dis_Hour = 50;
				if(s_current.key == 0)
				{
				    Dis_Minute = 1;
				}
				else
				{
				    Dis_Minute = 0;
				}
			    break;  
			
			//����ص�ѹ	  
			case 5:		 
				piv_current = (battery_voltage_1s * 363)/(4096);
				Dis_Hour = (uint8)((piv_current%1000)/100 + 60);
				Dis_Minute = (uint8)(piv_current%100);	
			    break; 
				
			//������¶�
			case 6:		
				if((battery_temp < 356)||(battery_temp > 3902))
				{ 
				    Dis_Hour =  79;
				    Dis_Minute = 99;	
				}
				else
				{
				    i = 119;
					for(i =118; i > 0; i--)
					{
					    if((battery_temp > temp[i+1])&&(battery_temp < temp[i]))
						{
						    Dis_Hour =  70 + i/100;
							Dis_Minute = i%100;
							break ;
						}	
					}
				}
			    break;
				
			//�����ĵ���  
			case 7:
			    chargeing();	 
				piv_current = account_current(CHARGE_CURRENT);
				if(piv_current > jt_chargecurrent)
				{
				    piv_current = ((piv_current-jt_chargecurrent)*165)/4096;
				}
				else
				{
				    piv_current = 0;
				}
				Dis_Hour = (uint8)((piv_current%1000)/100 + 80);
				Dis_Minute = (uint8)(piv_current%100);	
				
			    break;
				
		}
}
/******************************************************************
���Եؼ칦��
******************************************************************/
void Test_HwIncept(uint8 state)
{

		if(l_hw.effect == 1)
		{
			Dis_UvOn = TRUE;   
		}
		else 
		{
			Dis_UvOn = FALSE;
		} 		
		if(m_hw.effect == 1)
		{
			Dis_PrOn = TRUE;   
		}
		else 
		{
			Dis_PrOn = FALSE;
		}			
		if(up_hw.effect == 1)
		{
			Dis_AuOn = TRUE;   
		}
		else 
		{
			Dis_AuOn = FALSE;
		}		
		if(r_hw.effect == 1)
		{
			Dis_SpOn = TRUE;   
		}
		else 
		{
			Dis_SpOn = FALSE;
		}
        switch(state)
	    {
		    //�����ߺ������	
		    case 0:
				Dis_Hour = 10; 
				if(l_hw.effect == 1)
				{
				    Dis_Minute = l_hw.data%100; 
					Dis_Hour += l_hw.data/100;
				}
				else
				{
				    Dis_MinuteOn = FALSE; 
				}
			    break;	 
				
		    //����м�������	
		    case 1:
				Dis_Hour = 20; 
				if(m_hw.effect == 1)
				{
				    Dis_Minute = m_hw.data%100;   
					Dis_Hour += m_hw.data/100;
				}
				else
				{
				    Dis_MinuteOn = FALSE; 
				}
			    break;
				
		    //����ұߺ������	
		    case 2:
				Dis_Hour = 30; 
				if(r_hw.effect == 1)
				{
				    Dis_Minute = r_hw.data%100;   
					Dis_Hour += r_hw.data/100;
				}
				else
				{
				    Dis_MinuteOn = FALSE; 
				}
			    break;
				
		    //��ⶥ�Ϻ������	
		    case 3:
				Dis_Hour = 40; 
				if(up_hw.effect == 1)
				{
				    Dis_Minute = up_hw.data%100;   
					Dis_Hour += up_hw.data/100;
				}
				else
				{
				    if (dummy_wall.effect == 1)
					{				
				        Dis_Minute = dummy_wall.data%100;   
					    Dis_Hour += dummy_wall.data/100;
					}
					else
					{				  
				        Dis_MinuteOn = FALSE; 
					}				 
				}
			    break;
	    }
}
/******************************************************************
���Եؼ칦��
******************************************************************/
void Test_Wall(uint8 state)
{
uint32 piv_current;		 		
		if(w_l.sign == NEAR)
		{
			Dis_ErrOn = TRUE;   
		}
		else 
		{
			Dis_ErrOn = FALSE;
		}				
		if(w_lm.sign == NEAR)
		{
			Dis_UvOn = TRUE;   
		}
		else 
		{
			Dis_UvOn = FALSE;
		} 		
		if(w_m.sign == NEAR)
		{
			Dis_PrOn = TRUE;   
		}
		else 
		{
			Dis_PrOn = FALSE;
		}			
		if(w_rm.sign == NEAR)
		{
			Dis_AuOn = TRUE;   
		}
		else 
		{
			Dis_AuOn = FALSE;
		}		
		if(w_r.sign == NEAR)
		{
			Dis_SpOn = TRUE;   
		}
		else 
		{
			Dis_SpOn = FALSE;
		}
        switch(state)
		{	//������ǽ��	
		    case 0:
			    piv_current = ((w_l.difference)*330)/4096;
				Dis_Hour = (uint8)((piv_current%1000)/100+10); 
				Dis_Minute = (uint8)(piv_current%100); 		
			    break;
				
		    //�������ǽ��	
		    case 1:
				piv_current =  (((w_lm.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+ 20); 
				Dis_Minute = (uint8)(piv_current %100); 	
			    break;
				
		    //����м�ǽ��	
		    case 2: 
			    piv_current = (((w_m.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+ 30); 
				Dis_Minute = (uint8)(piv_current %100); 		
			    break;
				
		    //�������ǽ��	
		    case 3:
			    piv_current = (((w_rm.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+40); 
				Dis_Minute = (uint8)(piv_current%100); 		
			    break;
				
		    //����ұ�ǽ��	
		    case 4:
			    piv_current = (((w_r.difference)*330)/4096);
				Dis_Hour =  (uint8)((piv_current%1000)/100+50); 
				Dis_Minute = (uint8)(piv_current%100); 		
			    break;
				
		}
}
/******************************************************************
���Եؼ칦��
******************************************************************/
void Test_Earth(uint8 state)
{
uint32 piv_current ;
				  	
		if(e_l.sign == NEAR)
		{
		    Dis_ErrOn = TRUE;   
		}
		else 
		{
			Dis_ErrOn = FALSE;
		}						
		if(e_lm.sign == NEAR)
		{
			Dis_UvOn = TRUE;   
		}
		else 
		{
			Dis_UvOn = FALSE;
		}		 				
		if(e_rm.sign == NEAR)
		{
			Dis_AuOn = TRUE;   
		}
		else 
		{
			Dis_AuOn = FALSE;
		}	  			
		if(e_r.sign == NEAR)
		{
			Dis_SpOn = TRUE;   
		}
		else 
		{
			Dis_SpOn = FALSE;
		}
        switch(state)
		{	//�����ߵؼ�
		    case 0:
			    piv_current = (((e_l.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+10); 
				Dis_Minute = (uint8)(piv_current %100); 			
			    break;
				
		    //������еؼ�			
		    case 1: 
			    piv_current = (((e_lm.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+20); 
				Dis_Minute = (uint8)(piv_current %100); 	
			    break;
				
		    //������еؼ�	
		    case 2: 
			    piv_current = (((e_rm.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+30); 
				Dis_Minute = (uint8)(piv_current %100); 
			    break;
				
		    //����ұߵؼ�	
		    case 3:
			    piv_current = (((e_r.difference)*330)/4096);
				Dis_Hour = (uint8)((piv_current%1000)/100+40); 
				Dis_Minute = (uint8)(piv_current %100); 	
			    break;
		}				
}
/******************************************************************
���ܣ���������ת����ǰ0.5�������0.5��
******************************************************************/
void frontandback(void)
{
 static uint8 direction = 0 ;     //����
 static uint32 tt;
    switch(direction )
	 {
	    case 0:
		     if ((l_rap.sign == 0) && (r_rap.sign == 0))
			 {
			      direction = 1;
				  tt = giv_sys_time;
			 }
			 break ;
	    case 1:
			 if ((giv_sys_time - tt) > 3000) 
			 {
				 enable_rap(FRONT, 2000,FRONT,2000);
				 direction = 2 ;
			 }
			 break;
		case 2:
		     if ((l_rap.sign == 0) && (r_rap.sign == 0))
			 {
			      direction = 3;
				  tt = giv_sys_time;
			 }
			 break ;
		case 3:
			 if ((giv_sys_time - tt) > 3000) 
			 {
			     direction = 0;
				 enable_rap(BACK, 2000,BACK,2000);
			 }
			 break ;
		default :
			 direction = 0; 
	 }
}
/*************************************************************************
���ܣ��������ĳ���
���룺��
�������
˵��������������Լ���������������Լ죬������Ŀ�����������ֿ����������·��
��ɨ�����������·����ɨ�����������·����������������·��UV���Ƶ�·��ǰ�ּ�
���·��ǽ��ؼ�������ǰײ��·�ļ�⡣�ڲ���ͨ�����������һ�£�û��ͨ������
���¡�
*************************************************************************/
void Test_MainBoard(uint8 state)
{
static uint8 step = 0;	   
static uint8 step2 = 0;
static uint8 step3 = 0;
static uint32 symbol = 0;
static uint32 stime = 0;
static uint8  test_err = 0;  
uint32 t; 

uint8 i;//ѭ����������
uint8 m;
	Io_Driver();
	comm_rap();
    switch(state)
	{
	    case 0:
		    switch(step)
			{
			    case 0:
				    if(Test_DriveOfWheel())
					{
						step = 6;
						test_err = 1;
						break;
					}
					if(Test_DriveOfBrush())
					{
						step = 6;
						test_err = 2;
						break;
					}
					step++;
					symbol = 0;
					stime = giv_sys_time;
					
                    gbv_action_uv = TRUE;	         /*����uv�Ƶ�״̬�����ⲿģ���ȡ�͸ı�*/
                    gbv_action_s_motor = TRUE;       /*���ñ�ɨ����״̬�����ⲿģ���ȡ�͸ı�*/
                    gbv_action_m_motor = TRUE;       /*������ɨ����״̬�����ⲿģ���ȡ�͸ı�*/ 
                    gbv_action_dust_motor = TRUE;	 /*���ûҳ������״̬�����ⲿģ���ȡ�͸ı�*/

				case 1:
				    do_action(3,0xffffff);
				    if((e_l.difference < 1737)&&(e_l.difference > 1414))
					{
					    symbol |= 0x1;
					}	
				    if((e_lm.difference < 1737)&&(e_lm.difference > 1414))
					{
					    symbol |= 0x2;
					}
				    if((e_rm.difference < 1737)&&(e_rm.difference > 1414))
					{
					    symbol |= 0x4;
					}
				    if((e_r.difference < 1737)&&(e_r.difference > 1414))
					{
					    symbol |= 0x8;
					}
				    if((w_l.difference < 1737)&&(w_l.difference > 1414))
					{
					    symbol |= 0x10;
					}
				    if((w_lm.difference < 1737)&&(w_lm.difference > 1414))
					{
					    symbol |= 0x20;
					}
				    if((w_m.difference < 1737)&&(w_m.difference > 1414))
					{
					    symbol |= 0x40;
					}
				    if((w_rm.difference < 1737)&&(w_rm.difference > 1414))
					{
					    symbol |= 0x80;
					}
				    if((w_r.difference < 1737)&&(w_r.difference > 1414))
					{
					    symbol |= 0x100;
					}
					if(l_hw.sign == 1)
					{
					    symbol |= 0x200;
					}	
					if(m_hw.sign == 1)
					{
					    symbol |= 0x400;
					}
					if(up_hw.sign == 1)
					{
					    symbol |= 0x800;
					}
					if(r_hw.sign == 1)
					{
					    symbol |= 0x1000;
					}
					if(l_lidi.key == 1)
					{ 
					    symbol |= 0x2000;
					}
					else
					{
					    symbol |= 0x4000;
					}
					if(r_lidi.key == 1)
					{ 
					    symbol |= 0x8000;
					}
					else
					{
					    symbol |= 0x10000;
					}  
					if(l_bump.key == 1)
					{ 
					    symbol |= 0x20000;
					}
					else
					{
					    symbol |= 0x40000;
					}	
					if(r_bump.key == 1)
					{ 
					    symbol |= 0x80000;
					}
					else
					{
					    symbol |= 0x100000;
					}
						  
					if(l_ring.length > 3)
					{
					    symbol |= 0x400000;
					}
					if(r_ring.length > 3)
					{
					    symbol |= 0x800000;
					}		
					if(s_current.key == 1)
					{ 
					    symbol |= 0x1000000;
					}
					else
					{
					    symbol |= 0x2000000;
					} 
					if((giv_sys_time - stime) > 20000)
					{
					    if(symbol != 0x3dfffff)	//3ffffff
						{
						    step = 6;
						    break;
						}
						step ++;
					}
					break;
				case 2:
				    if((battery_temp_1s > 2253)||(battery_temp_1s < 1843))
					{
						step = 6;
						test_err = 3;
						break;
					}	
				    if((l_current_1s > 291)||(l_current_1s < 175))	//300--500mA
					{											   
						step = 6;
						test_err = 4;
						break;
					} 			  
				    if((r_current_1s > 291)||(r_current_1s < 175))//300--500mA
					{
						step = 6;
						test_err = 5;
						break;
					}
				    if((m_current_1s > 291)||(m_current_1s < 175))	 //300--500mA
					{
						step = 6;
						test_err = 6;
						break;
					}
				    if((dust_current_1s > 291)||(dust_current_1s < 175)) //300--500mA
					{
						step = 6;
						test_err = 7;
						break;
					}
				    ///////�������֡���ɨ�Ŀ��Ʒ����෴ת��
					 
                    gbv_action_uv = TRUE;	/*����uv�Ƶ�״̬�����ⲿģ���ȡ�͸ı�*/
                    gbv_action_s_motor = FALSE;/*���ñ�ɨ����״̬�����ⲿģ���ȡ�͸ı�*/
                    gbv_action_m_motor = FALSE;//TRUE;/*������ɨ����״̬�����ⲿģ���ȡ�͸ı�*/ 
                    gbv_action_dust_motor = FALSE;	/*���ûҳ������״̬�����ⲿģ���ȡ�͸ı�*/

					///////��ʼ��ʱ�䣬׼���ȴ�2��
					stime = giv_sys_time;
					step ++;
					break;
				case 3:
				    if((giv_sys_time - stime) > 20000)
					{
					    step++;
					}  
				    do_action(4,0xffffff);
				    break;
				case 4:	
				    if((l_current_1s > 291)||(l_current_1s < 175))//300--500mA
					{
						step = 6;
						test_err = 8;
						break;
					} 			  
				    if((r_current_1s > 291)||(r_current_1s < 175))//300--500mA
					{
						step = 6;
						test_err = 9;
						break;
					}
				    
					////�ر�������	  
				    stop_rap();	
                    gbv_action_uv = FALSE;	/*����uv�Ƶ�״̬�����ⲿģ���ȡ�͸ı�*/
                    gbv_action_s_motor = FALSE;/*���ñ�ɨ����״̬�����ⲿģ���ȡ�͸ı�*/
                    gbv_action_m_motor = FALSE;/*������ɨ����״̬�����ⲿģ���ȡ�͸ı�*/ 
                    gbv_action_dust_motor = FALSE;	/*���ûҳ������״̬�����ⲿģ���ȡ�͸ı�*/
					step = 8;	
					stime = giv_sys_time;
					break;
				case 5:
				    Enable_Beep();
					step = 7;
					break;
				case 6:
				    Enable_Beep_number(2);
					step++;
				case 7:
				    stop_rap();	
                    gbv_action_uv = FALSE;	/*����uv�Ƶ�״̬�����ⲿģ���ȡ�͸ı�*/
                    gbv_action_s_motor = FALSE;/*���ñ�ɨ����״̬�����ⲿģ���ȡ�͸ı�*/
                    gbv_action_m_motor = FALSE;/*������ɨ����״̬�����ⲿģ���ȡ�͸ı�*/ 
                    gbv_action_dust_motor = FALSE;	/*���ûҳ������״̬�����ⲿģ���ȡ�͸ı�*/
				    Dis_Minute = test_err; 
				    break;
				case 8:		
				    if((giv_sys_time - stime) > 20000)
					{		   
				        if((battery_voltage_1s > 1748)||(battery_voltage_1s < 1500))	//15.5V--13.3V
					    {
						    step = 6;
							test_err = 11;
					    } 
					    if(step!=6)step = 5;
					}
					break;  
								    
			}
		    break;
		case 1:
		    switch(step2)
			{
			    case 0:	  
					stime = giv_sys_time;
					step2 ++;
				    break;
				case 1:
				    if((giv_sys_time - stime) > 20000)
					{
					    if(power.charge_dc == 1)
						{
						    jt_chargecurrent = battery_chargecurrent_1s;
							stime = giv_sys_time;
							enable_pwm(CHARGE_PWM,1000);	
					        step2++;
						}
						else
						{
						    step2 = 4;
							break;
						}
					} 
					break;
				case 2:
				    if((giv_sys_time - stime) > 20000)
					{
					    t = battery_chargecurrent_1s - jt_chargecurrent	;
						if((t<993)||(t>1241))	 //400--500mA
						{
						    step2 = 4;
							break;
						}
						step2++;
					} 
					break;
				case 3:	 
				    Enable_Beep();
					step2 = 5;
					break;
				case 4:
				    Enable_Beep_number(2);	 
					step2++;
					break;
				case 5:
					enable_pwm(CHARGE_PWM,0);
					break;
			}
			break;
		case 2:
		    switch(step3)
			{
			    case 0:	  
					stime = giv_sys_time;
					step3 ++;
				    break;
				case 1:
				    if((giv_sys_time - stime) > 20000)
					{
					    if(power.charge_seat == 1)
						{
						    jt_chargecurrent = battery_chargecurrent_1s;
							stime = giv_sys_time;
							enable_pwm(CHARGE_PWM,1000);	
					        step3++;
						}
						else
						{
						    step3 = 4;
							break;
						}
					} 
					break;
				case 2:
				    if((giv_sys_time - stime) > 20000)
					{
					    t = battery_chargecurrent_1s - jt_chargecurrent	;
						if((t<993)||(t>1241))	 //400--500mA
						{
						    step3 = 4;
							break;
						}
						step3++;
					} 
					break;
				case 3:	 
				    Enable_Beep();
					step3 = 5;
					break;
				case 4:
				    Enable_Beep_number(2);	 
					step3++;
					break;
				case 5:
					enable_pwm(CHARGE_PWM,0);
					break;
			}
			break;	 
		case 3:
			    Clr_DisplayData();
				i = 0;
                lcdkey_bgon();     //�򿪱���
	            lcdkey_dison();    //����ʾ
			    while((key1.sign == 1)&&(key2.sign == 1)&&(key3.sign == 1))
				{
			        if(Sec == TRUE)
	                {
	                    Sec = FALSE;
						i++;
						Clr_DisplayData();
						//�޸��Լ����ʾ
						i = i%26;
						if(i < 2)
						{	  
				            for(m=0; m<10;m++)
				            {
					            Dis_Data[m] = (0xff);
				            }
						}
						else
						{
						if(i<6)
						{
						    for(m=0; m<10;m++)
							{
							    Dis_Data[m] = (0x11<<(i-2));
							}
						}
						else
						{
						    if((i%2)==0)
							{	  
						        Dis_Data[(i-6)/2] = 0xf;
							}
							else
							{						  
						        Dis_Data[(i-6)/2] = 0xf0;
							}
						}
						}
		                lcdkey_disdata(12, Dis_Data, 10); 
					}	
					IWDG_ReloadCounter();
				    AutoReadKey();
				}
			break;
	}    
}
