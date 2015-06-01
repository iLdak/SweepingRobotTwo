 //======================头文件=================================
#include "wall_earth.h"
#include "hardware.h"
#include "ring.h"
#include "time1.h"
#include "pwm.h"
#include "ad.h"
#include "lcdkey.h"
//======================私有定义===============================
//======================全局变量=============================== 
WALL_DATA  e_l,e_r,e_lm,e_rm,w_l,w_lm,w_r,w_rm,w_m;
bool wall_earth_time;

bool WallSensitivity;   //墙检信号灵敏度，为FALSE时灵敏度低，为ture时灵敏度高
//======================私有变量===============================	  
bool action_wall;       //允许检测墙检标志
bool action_earth;      //允许检测地检标志

//=======================全局函数===============================
void Disable_wall(void);			  
void Disable_earth(void);
void Enable_wall(void);
void Enable_earth(void);
void init_wallearth(void);
void read_wallearth(void);
//=======================私有函数================================

void  check_earth(WALL_DATA *earth,uint16 difference);	
void  check_near(WALL_DATA *earth);
//=======================函数体==================================
/****************************************************************
功能：初始化墙检和地检的数据
输入：无
输出：无
****************************************************************/
void init_wallearth(void)
{
  action_wall = FALSE;             
  action_earth = FALSE;            //禁止读取地检墙检的数据
  
  /*************设置地检和墙检的默认状态************/
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
  //=========设置地检和墙检的门槛电压值
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
功能: 定时读取地检和墙检的数据,1ms读一次
输入: 无
输出：无
*****************************************/
void read_wallearth(void)
{
static bool ligth = FALSE;
    if( wall_earth_time == FALSE)
    {
	  return;
    }
	wall_earth_time = FALSE;
	if(ligth == FALSE)        //熄灯的信号
	{	 //没有亮的情况下，进行采样
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
	{	//有亮的情况下，进行采样
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
        Set_Wall_Send();              //关闭发射
        Set_Earth_Send();             //关闭发射
	}
}
/****************************************************
函数名： check_earth
说明：判断地检信号
输入：   WALL_DATA *earth         地检数据指针
输出：   改变  	earth  内的数据
返回：   无	 
****************************************************/
void  check_earth( WALL_DATA *earth,uint16 difference)
{	   //不同情况的采样，做对比，而后作运算，作判定
    if(earth->darkness > (earth->brightness + difference/30))
	{
	    earth->difference = earth->darkness - (earth->brightness + difference/30);
	}
	else
	{
	    earth->difference =  0;
	}
	if( earth->difference > earth->low_door) //测试时信号大于设定值	，
	{
	    earth->near++;
		earth->farn = 0;
		if(earth->near > 9)
		{
		    earth->sign = NEAR;
		    earth->near = 10;
		}
    }  
	else                           //小于设定值
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
函数名： check_earth
说明：判断地检信号
输入：   WALL_DATA *earth         地检数据指针
输出：   改变  	earth  内的数据
返回：   无	 
****************************************************/
void  check_near( WALL_DATA *earth)	 
{ //与门限高位进行比对
	if( earth->difference > earth->high_door) //测试时信号大于设定值
	{
	    earth->high_near++;
		earth->high_farn = 0;
		if(earth->high_near > 5)
		{
		    earth->high_sign = NEAR;
		    earth->high_near = 10;
		}
    }  
	else                           //小于设定值
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
功能：允许测量地检
输入：无
输出：无
****************************************************/
void Enable_earth(void)
{
action_earth = TRUE;
} 
/****************************************************
功能：允许测量墙检
输入：无
输出：无
****************************************************/
void Enable_wall(void)
{
action_wall = TRUE;
}
/****************************************************
功能：禁止测量地检
输入：无
输出：无
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
功能：禁止测量墙检
输入：无
输出：无
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
功能：将墙检的灵敏度设置为低
输入：无
输出：无
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
功能：将墙检的灵敏度设置为高
输入：无
输出：无
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

