 //======================ͷ�ļ�=================================
#include "wall_earth.h"
#include "hardware.h"
#include "ring.h"
#include "time1.h"
#include "pwm.h"
#include "ad.h"
#include "lcdkey.h"
//======================˽�ж���===============================
//======================ȫ�ֱ���=============================== 
WALL_DATA  e_l,e_r,e_lm,e_rm,w_l,w_lm,w_r,w_rm,w_m;
bool wall_earth_time;

bool WallSensitivity;   //ǽ���ź������ȣ�ΪFALSEʱ�����ȵͣ�Ϊtureʱ�����ȸ�
//======================˽�б���===============================	  
bool action_wall;       //������ǽ���־
bool action_earth;      //������ؼ��־

//=======================ȫ�ֺ���===============================
void Disable_wall(void);			  
void Disable_earth(void);
void Enable_wall(void);
void Enable_earth(void);
void init_wallearth(void);
void read_wallearth(void);
//=======================˽�к���================================

void  check_earth(WALL_DATA *earth,uint16 difference);	
void  check_near(WALL_DATA *earth);
//=======================������==================================
/****************************************************************
���ܣ���ʼ��ǽ��͵ؼ������
���룺��
�������
****************************************************************/
void init_wallearth(void)
{
  action_wall = FALSE;             
  action_earth = FALSE;            //��ֹ��ȡ�ؼ�ǽ�������
  
  /*************���õؼ��ǽ���Ĭ��״̬************/
  e_l.sign=NEAR;  //low 
  e_r.sign=NEAR;
  e_lm.sign=NEAR;
  e_rm.sign=NEAR;
  w_l.sign=FARN;  //high
  w_lm.sign=FARN;
  w_r.sign=FARN;
  w_rm.sign=FARN;
  w_m.sign=FARN;
  	
  w_l.on = 1;
  w_lm.on = 1;
  w_r.on = 1;
  w_rm.on = 1;
  w_m.on = 1;
  					 
  w_l.high_sign=FARN;
  w_lm.high_sign=FARN;
  w_r.high_sign=FARN;
  w_rm.high_sign=FARN;
  w_m.high_sign=FARN;
  //=========���õؼ��ǽ����ż���ѹֵ
  e_l.low_door=30;
  e_r.low_door=30;
  e_lm.low_door=30;
  e_rm.low_door=30;
  w_l.high_door=50;
  w_lm.high_door=50;
  w_m.high_door=50;
  w_rm.high_door=50;
  w_r.high_door=50;
  
  w_l.low_door=150;
  w_lm.low_door=120;
  w_m.low_door=120;
  w_rm.low_door=120;
  w_r.low_door=150;
  WallSensitivity = FALSE;
}
/*****************************************
����: ��ʱ��ȡ�ؼ��ǽ�������,1ms��һ��
����: ��
�������
*****************************************/
void read_wallearth(void)
{
static bool ligth = FALSE;
    if( wall_earth_time == FALSE)
    {
	  return;
    }
	wall_earth_time = FALSE;
	if(ligth == FALSE)        //Ϩ�Ƶ��ź�
	{	 //û����������£����в���
	    if(action_wall != FALSE)
		{
		    w_l.darkness = account_current(W_L);   
		    w_lm.darkness = account_current(W_LM);  
		    w_m.darkness = account_current(W_M);  
		    w_rm.darkness = account_current(W_RM);  
		    w_r.darkness = account_current(W_R);
			Reset_Wall_Send();
		} 
		if(action_earth != FALSE)
		{
		    e_l.darkness = account_current(E_L);
			e_lm.darkness = account_current(E_LM);
			e_rm.darkness = account_current(E_RM);
			e_r.darkness = account_current(E_R);
			Reset_Earth_Send();
		}
		ligth = TRUE;
	}
	else
	{	//����������£����в���
	    ligth = FALSE;			   
		if(action_earth != FALSE)
		{
		    e_l.brightness = account_current(E_L);
			e_lm.brightness = account_current(E_LM);
			e_rm.brightness = account_current(E_RM);
			e_r.brightness = account_current(E_R);
	        check_earth( &e_l,0);   
	        check_earth( &e_lm,e_l.difference); 
	        check_earth( &e_rm,e_lm.difference);  
	        check_earth( &e_r,e_rm.difference); 
		}
		if(action_wall != FALSE)
		{
		    w_l.brightness = account_current(W_L);   
		    w_lm.brightness = account_current(W_LM);  
		    w_m.brightness = account_current(W_M);  
		    w_rm.brightness = account_current(W_RM);  
		    w_r.brightness = account_current(W_R);
            check_earth( &w_l,e_r.difference);	
	        check_earth(&w_rm,w_l.difference); 
	        check_earth(&w_r,w_rm.difference); 
	        check_earth( &w_lm,0);
	        check_earth(&w_m,w_lm.difference); 	
            check_near( &w_l);	
	        check_near(&w_rm); 
	        check_near(&w_r); 
	        check_near( &w_lm);
	        check_near(&w_m); 
		}
        Set_Wall_Send();              //�رշ���
        Set_Earth_Send();             //�رշ���
	}
}
/****************************************************
�������� check_earth
˵�����жϵؼ��ź�
���룺   WALL_DATA *earth         �ؼ�����ָ��
�����   �ı�  	earth  �ڵ�����
���أ�   ��	 
****************************************************/
void  check_earth( WALL_DATA *earth,uint16 difference)
{	   //��ͬ����Ĳ��������Աȣ����������㣬���ж�
    if(earth->darkness > (earth->brightness + difference/30))
	{
	    earth->difference = earth->darkness - (earth->brightness + difference/30);
	}
	else
	{
	    earth->difference =  0;
	}
	if( earth->difference > earth->low_door) //����ʱ�źŴ����趨ֵ	��
	{
	    earth->near++;
		earth->farn = 0;
		if(earth->near > 9)
		{
		    earth->sign = NEAR;
		    earth->near = 10;
		}
    }  
	else                           //С���趨ֵ
	{
	    earth->near = 0;
		earth->farn++;
		if(earth->farn > 4)
		{
		    earth->sign = FARN;
		    earth->farn = 10;
		}
	} 
}
/****************************************************
�������� check_earth
˵�����жϵؼ��ź�
���룺   WALL_DATA *earth         �ؼ�����ָ��
�����   �ı�  	earth  �ڵ�����
���أ�   ��	 
****************************************************/
void  check_near( WALL_DATA *earth)	 
{ //�����޸�λ���бȶ�
	if( earth->difference > earth->high_door) //����ʱ�źŴ����趨ֵ
	{
	    earth->high_near++;
		earth->high_farn = 0;
		if(earth->high_near > 5)
		{
		    earth->high_sign = NEAR;
		    earth->high_near = 10;
		}
    }  
	else                           //С���趨ֵ
	{
	    earth->high_near = 0;
		earth->high_farn++;
		if(earth->high_farn > 9)
		{
		    earth->high_sign = FARN;
		    earth->high_farn = 10;
			earth->on = 1;
		}
	} 
}
/****************************************************
���ܣ���������ؼ�
���룺��
�������
****************************************************/
void Enable_earth(void)
{
action_earth = TRUE;
} 
/****************************************************
���ܣ��������ǽ��
���룺��
�������
****************************************************/
void Enable_wall(void)
{
action_wall = TRUE;
}
/****************************************************
���ܣ���ֹ�����ؼ�
���룺��
�������
****************************************************/
void Disable_earth(void)
{
  action_earth = FALSE;
  e_l.sign=NEAR;
  e_r.sign=NEAR;
  e_lm.sign=NEAR;
  e_rm.sign=NEAR;
}
/****************************************************
���ܣ���ֹ����ǽ��
���룺��
�������
****************************************************/
void Disable_wall(void)
{
action_wall = FALSE;
  w_l.sign=FARN;
  w_lm.sign=FARN;
  w_r.sign=FARN;
  w_rm.sign=FARN;
  w_m.sign=FARN;
  				 
  w_l.high_sign=FARN;
  w_lm.high_sign=FARN;
  w_r.high_sign=FARN;
  w_rm.high_sign=FARN;
  w_m.high_sign=FARN;
}
/****************************************************
���ܣ���ǽ�������������Ϊ��
���룺��
�������
****************************************************/
void Wall_SensitivityLow(void)
{
    if(WallSensitivity != FALSE)
	{
	    WallSensitivity = FALSE;	
        w_l.low_door=200;
        w_lm.low_door=200;
        w_m.low_door=200;
        w_rm.low_door = 200;
        w_r.low_door = 200;
	}
}  
/****************************************************
���ܣ���ǽ�������������Ϊ��
���룺��
�������
****************************************************/
void Wall_SensitivityHigh(void)
{
    if(WallSensitivity == FALSE)
	{
	    WallSensitivity = TRUE;	
        w_l.low_door=50;
        w_lm.low_door=50;
        w_m.low_door=50;
        w_rm.low_door = 50;
        w_r.low_door = 50;
	}
}

