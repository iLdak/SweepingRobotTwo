#include "io_driver.h"
#include "hardware.h"
#include "time1.h"
//======================私有定义================================
//======================全局变量================================
bool gbv_action_uv;	          //设置uv灯的状态，由外部模块读取和改变
bool gbv_action_s_motor;      //设置边扫马达的状态，由外部模块读取和改变
bool gbv_action_m_motor;      //设置中扫马达的状态，由外部模块读取和改变 
bool gbv_action_dust_motor;	  //设置灰尘风机的状态，由外部模块读取和改变

//======================私有变量================================	 
bool pbv_state_uv;            //指示当前uv灯的状态
bool pbv_state_m_motor;       //指示当前中扫马达的状态

bool pbv_state_s_motor;       //指示当前边扫马达的状态
bool pbv_state_dust_motor;    //指示当前灰尘风机的状态
uint32 piv_beep_time;         //蜂鸣器响的时间

bool pbv_beep_on;             //蜂鸣器响的标志
uint8 piv_beep_number;        //蜂鸣器的声数
//======================全局函数================================
//======================私有函数================================
//======================函数实体================================
/******************************************************************
功能：初始化中扫驱动、灰尘盒风机驱动、边扫马达驱动、边扫电流输入
      紫外灯驱动。
输入：无
输出：无
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
		Reset_Beep();    //关闭蜂鸣器
}
/*****************************************************************
功能：驱动中扫风机边扫紫外脚
输入：无
输出：无
说明：在需要进行清扫时就可以循环调用
*****************************************************************/
void Io_Driver(void)
{
    /*********************驱动紫外灯*********************/
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
	 
    /*********************驱动边扫*********************/
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
	//中扫部分修改
	//======================================================================
	 if((gbv_action_m_motor == TRUE) && (pbv_state_m_motor == FALSE))
    {
        Set_M_Motor();	   //向前运动	//端口不确定注意修改
        pbv_state_m_motor = TRUE;
    }
	else if((gbv_action_m_motor == FALSE) && (pbv_state_m_motor == TRUE))
	{
        Reset_M_Motor();   //关闭运动
        pbv_state_m_motor = FALSE;
	}
  //====================================================================
   
    /*********************驱动灰尘风机*********************/
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
使能蜂鸣器响0.2秒
**********************************************************/
void Enable_Beep_Five(void)
{
		Set_Beep();
		piv_beep_time = giv_sys_time;
		pbv_beep_on = TRUE;
		piv_beep_number = 10;
}
/**********************************************************
使能蜂鸣器响0.2秒
**********************************************************/
void Enable_Beep(void)
{
		Set_Beep();
		piv_beep_time = giv_sys_time;
		pbv_beep_on = TRUE;
		piv_beep_number = 1;
}	  
/**********************************************************
使能蜂鸣器响0.2秒
**********************************************************/
void Enable_Beep_number(uint8 number)
{
		Set_Beep();
		piv_beep_time = giv_sys_time;
		pbv_beep_on = TRUE;
		piv_beep_number = number;
}
/**********************************************************
驱动蜂鸣器响
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
功能：使能测速以及前撞
*********************************************************/
void Enable_Speed(void)
{
        Reset_Speed_Send();
}
/*********************************************************
功能：关闭测速以及前撞
*********************************************************/
void Disable_Speed(void)
{
        Set_Speed_Send();
}

