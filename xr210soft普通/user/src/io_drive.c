#include "io_driver.h"
#include "hardware.h"
#include "time1.h"
//======================˽�ж���================================
//======================ȫ�ֱ���================================
bool gbv_action_uv;	          //����uv�Ƶ�״̬�����ⲿģ���ȡ�͸ı�
bool gbv_action_s_motor;      //���ñ�ɨ����״̬�����ⲿģ���ȡ�͸ı�
bool gbv_action_m_motor;      //������ɨ����״̬�����ⲿģ���ȡ�͸ı� 
bool gbv_action_dust_motor;	  //���ûҳ������״̬�����ⲿģ���ȡ�͸ı�

//======================˽�б���================================	 
bool pbv_state_uv;            //ָʾ��ǰuv�Ƶ�״̬
bool pbv_state_m_motor;       //ָʾ��ǰ��ɨ����״̬

bool pbv_state_s_motor;       //ָʾ��ǰ��ɨ����״̬
bool pbv_state_dust_motor;    //ָʾ��ǰ�ҳ������״̬
uint32 piv_beep_time;         //���������ʱ��

bool pbv_beep_on;             //��������ı�־
uint8 piv_beep_number;        //������������
//======================ȫ�ֺ���================================
//======================˽�к���================================
//======================����ʵ��================================
/******************************************************************
���ܣ���ʼ����ɨ�������ҳ��з����������ɨ�����������ɨ��������
      �����������
���룺��
�������
******************************************************************/
void Init_Io_Driver(void)
{  
		gbv_action_uv = FALSE;
		gbv_action_s_motor = FALSE;
		gbv_action_m_motor = FALSE;
		gbv_action_dust_motor = FALSE;
		
		Reset_UV();
		pbv_state_uv = FALSE;
		Reset_S_Motor();
		pbv_state_s_motor = FALSE;
		Reset_Dust_Motor();
		pbv_state_dust_motor = FALSE;
		Reset_M_FPMOS();
		Reset_M_ZPMOS();
		Set_M_FNMOS();
		Set_M_ZNMOS();
		pbv_state_m_motor = FALSE;
		Reset_Beep();    //�رշ�����
}
/*****************************************************************
���ܣ�������ɨ�����ɨ�����
���룺��
�������
˵��������Ҫ������ɨʱ�Ϳ���ѭ������
*****************************************************************/
void Io_Driver(void)
{
    /*********************���������*********************/
    if((gbv_action_uv == TRUE) && (pbv_state_uv == FALSE))
    {
        Set_UV();
        pbv_state_uv = TRUE;
    }
	else if((gbv_action_uv == FALSE) && (pbv_state_uv == TRUE))
	{
        Reset_UV();
        pbv_state_uv = FALSE;
	}
	 
    /*********************������ɨ*********************/
    if((gbv_action_s_motor == TRUE) && (pbv_state_s_motor == FALSE))
    {
        Set_S_Motor();
        pbv_state_s_motor = TRUE;
		DelayMs(2);
    }

	else if((gbv_action_s_motor == FALSE) && (pbv_state_s_motor == TRUE))
	{
        Reset_S_Motor();
        pbv_state_s_motor = FALSE;
	}
	//��ɨ�����޸�
	//======================================================================
	 if((gbv_action_m_motor == TRUE) && (pbv_state_m_motor == FALSE))
    {
        Set_M_Motor();	   //��ǰ�˶�	//�˿ڲ�ȷ��ע���޸�
        pbv_state_m_motor = TRUE;
    }
	else if((gbv_action_m_motor == FALSE) && (pbv_state_m_motor == TRUE))
	{
        Reset_M_Motor();   //�ر��˶�
        pbv_state_m_motor = FALSE;
	}
  //====================================================================
   
    /*********************�����ҳ����*********************/
    if((gbv_action_dust_motor == TRUE) && (pbv_state_dust_motor == FALSE))
    {
        Set_Dust_Motor();
        pbv_state_dust_motor = TRUE;
    }
	else if((gbv_action_dust_motor == FALSE) && (pbv_state_dust_motor == TRUE))
	{
         Reset_Dust_Motor();
         pbv_state_dust_motor = FALSE;
	}
} 
/**********************************************************
ʹ�ܷ�������0.2��
**********************************************************/
void Enable_Beep_Five(void)
{
		Set_Beep();
		piv_beep_time = giv_sys_time;
		pbv_beep_on = TRUE;
		piv_beep_number = 10;
}
/**********************************************************
ʹ�ܷ�������0.2��
**********************************************************/
void Enable_Beep(void)
{
		Set_Beep();
		piv_beep_time = giv_sys_time;
		pbv_beep_on = TRUE;
		piv_beep_number = 1;
}	  
/**********************************************************
ʹ�ܷ�������0.2��
**********************************************************/
void Enable_Beep_number(uint8 number)
{
		Set_Beep();
		piv_beep_time = giv_sys_time;
		pbv_beep_on = TRUE;
		piv_beep_number = number;
}
/**********************************************************
������������
**********************************************************/
void Beep_Driver(void)
{
    if((pbv_beep_on != FALSE)&&(piv_beep_number!=0))
	{
        if((giv_sys_time - piv_beep_time) > 2000)
	    {
	        Reset_Beep();
			if((giv_sys_time - piv_beep_time) > 5000)
			{
			    piv_beep_time = giv_sys_time;  
			    piv_beep_number --;
			}
			if(piv_beep_number == 0)
			pbv_beep_on = FALSE;
	    }
	    else
	    {
	        Set_Beep();
	    }
	}
}
/*********************************************************
���ܣ�ʹ�ܲ����Լ�ǰײ
*********************************************************/
void Enable_Speed(void)
{
        Reset_Speed_Send();
}
/*********************************************************
���ܣ��رղ����Լ�ǰײ
*********************************************************/
void Disable_Speed(void)
{
        Set_Speed_Send();
}

