#include "ring.h"
#include "time1.h"
#include "pwm.h"
#include "ad.h"
#include "lcdkey.h"
#include "hardware.h"
#include "function.h"
//===============================私有定义=========================
//===============================全局变量=========================
RING  l_ring,r_ring,w_ring;       //测试车轮速度的结构
RAP  l_rap,r_rap,w_rap;           //两轮速度距离的控制结构
bool rap_time;                    //两轮速度调整时间标志
uint32 Speed ;                    //两轮的最大速度  ，表示为脉冲时间
uint16 jt_l_current;              //左轮由于1n60反向漏电流引起的静态电流。
uint16 jt_r_current;              //右轮由于1n60反向漏电流引起的静态电流。
ACTION action;

//================================私有变量==============================
uint8 Wheel_State = 0;            //左右轮驱动状态 0为合格，1为驱动坏
uint8 err;
//================================全局函数===============================
//================================私有函数===============================
void stop(void);
void Set_WheelState(void);

//=================================函数实体=========================================
void Init_Ring(void)
{
 //================================初始化变量的值================================== 
l_ring.length = 0;                 //左轮的脉冲为0
l_ring.dq_time = giv_sys_time;     //复位脉冲调整时间
l_ring.old_time = giv_sys_time ;
l_ring.state = GOOD;
l_ring.odds = 0;
l_rap.sign = 0;

r_ring.length = 0;
r_ring.dq_time = giv_sys_time;
r_ring.old_time = giv_sys_time ;
r_ring.state = GOOD;
r_ring.odds = 0;
r_rap.sign = 0;
delay(500000);
jt_r_current = account_current( R_CURRENT);
jt_l_current = account_current( L_CURRENT);
Speed = HIGHSPEED;//1500;   // 1/1000秒
Dis_Speed = 1;
}
/************************************************************
功能  ：    每10ms 执行一次，自动根据车轮信息控制车轮
输入： 无
输出 ：无
************************************************************/
void  comm_rap(void)
{
uint32 t,l;
int32 ec;
uint16 dqcurrent;
//===============================控制两轮的速度比
if(rap_time != FALSE)
  {
    rap_time = FALSE;
//===============================两轮同步=========================
     if((l_rap.sign)&&(r_rap.sign))   //只有在两轮都转时才调节，并且只调速度慢的轮子
	 {
	   if(l_rap.master == 1)          //左轮是主轮
	   {    
	        ec = (r_ring.length*l_rap.rap)/r_rap.rap - l_ring.length;  //根据比例计算两轮的速度差
			//车轮走过的脉冲数，车轮每秒的脉冲数
			if (ec > 0)
			{
			   if(r_rap.pwm > 0)       //右轮速度过快
			   r_rap.pwm--;
			} 
			else
			{
			   if(ec < 0)              //右轮速度过慢
			   r_rap.pwm++;
			}
	   }
	   else              
	   {                                //右轮是主轮
	        ec = (l_ring.length*r_rap.rap)/l_rap.rap - r_ring.length; //根据比例计算两轮的速度差
			if (ec > 0)
			{                          //左轮速度过快
			   if(l_rap.pwm > 0)
			   l_rap.pwm--;
			} 
			else
			{
			   if(ec < 0)              //左轮速度过慢
			   l_rap.pwm++;
			}
	   }
	 }	
 //============================右轮============================ 
	   if(r_rap.sign)
	   {	  
		        //=============右轮脉冲所走时间   
		   t = (r_ring.dq_time - r_ring.old_time );	//^T
		   r_ring.old_time = r_ring.dq_time;
		        //=============右轮所走的脉冲数 
		   l = subbtime(r_rap.pulse,r_ring.length);	//l=r_ring.length-r_rap.pulse ,^S
		   r_rap.pulse = r_ring.length;	//重新定基准
		   
		   if(l == 0)
	       {
		       r_ring.odds++;
		       if(r_ring.odds > 200)r_ring.state = BAD;	 //连续2秒钟的时间，在启动的情况下都没有移动，则判定故障
		   }
	       else
	       {
		       r_ring.odds = 0;
		       r_ring.state = GOOD;
		   }
		  r_ring.ek[2] = r_ring.ek[1];	 //运动的推进，逼近换位推算
		  r_ring.ek[1] = r_ring.ek[0];
		  if(t)//已经运动了，有时间的推移，时间标量已经出现	 //重新计算	r_ring.ek[0]
		  r_ring.ek[0] = r_rap.rap - (l*10000)/t;  //V`=V-^S/^T
		  else
		  r_ring.ek[0] = r_rap.rap;
		  //[PID]--P减小偏差，I消除静差，D偏差信号的变化趋势
		  ec =(int16)((r_ring.ek[0]-r_ring.ek[1])/KP 
		       + r_ring.ek[0]/KI
			   +(r_ring.ek[0]+r_ring.ek[2]-2*r_ring.ek[1])/KD);
		  //KP=4,KI=10,KD=80
		  if(ec > 100)
		  {
		      ec = 100;
		  }
		  //再次根据反馈电流，重新鉴定，算出给定，对速度进行调节
		  dqcurrent = account_current( R_CURRENT);   	//采样轮子的电流
		  if(dqcurrent > (466 + jt_r_current))			//电流大于800ma
		  {
		      if(ec > 0)
			  ec = 0;
			  ec -= 10;
		  }
		  ec = ec + r_rap.pwm;      
		 if(ec<0)
		 {
		     r_rap.pwm = 0;
		 }
		 else
		 { 
		     if(ec > 1000)	//如果因为抖动，导致速度超过了最大，将根据最大重新给定
		     ec = 1000;
		     r_rap.pwm = (uint16)ec;  //每秒钟的脉冲数，是速度调节的基准
		 }
		 //方向的确定
		if(r_rap.ori == FRONT)
		{
		 	  enable_pwm(R_FRONT,r_rap.pwm);	//前进
		}
		else
		{  
		 	  enable_pwm(R_BACK,r_rap.pwm); //后退
		}
//----------------------------------------------------		
		if(r_rap.length<=(r_ring.length+100))
		{
			  if(r_rap.length >(r_ring.length+10))	//
			  if((r_rap.master))
			  {
				   if((--r_rap.sign) == 0)
				   {
			          stop();
				   }				  			 
			  }
		}
	   }
//================================左轮=======================================
	if(l_rap.sign)
	   {	  
//====================左轮脉冲所走时间
		   t = (l_ring.dq_time - l_ring.old_time );
		   l_ring.old_time = l_ring.dq_time;
//====================左轮所走的脉冲数 
		   l = subbtime(l_rap.pulse,l_ring.length);
		   l_rap.pulse = l_ring.length;
		   
			if(l == 0)
			{
				l_ring.odds++;
				if(l_ring.odds > 200)
				  l_ring.state = BAD;
			}
			else
			{
				l_ring.odds = 0;
				l_ring.state = GOOD;
			}
		  l_ring.ek[2] = l_ring.ek[1];
		  l_ring.ek[1] = l_ring.ek[0];
		  if(t)
		  l_ring.ek[0] = l_rap.rap - (l*10000)/t;
		  else
		  l_ring.ek[0] = l_rap.rap;
		  ec = (l_ring.ek[0]-l_ring.ek[1])/KP 
		       +l_ring.ek[0]/KI
			   +(l_ring.ek[0]+l_ring.ek[2]-2*l_ring.ek[1])/KD;
		  if(ec > 100)
		  {
		      ec = 100;
		  }
		  dqcurrent = account_current( L_CURRENT);
		  if(dqcurrent > (466 + jt_l_current))
		  {
		      if(ec > 0)
			  ec = 0;
			  ec -= 10;
		  }
		  ec = ec + l_rap.pwm;
		      if(ec<0)
	          {
			       l_rap.pwm = 0;
		      }
	          else
	          { 
			      if(ec > 1000)
			      ec = 1000;
			      l_rap.pwm = (uint16)ec; 
		      }
		  if(l_rap.ori == FRONT)
		    {
		     enable_pwm(L_FRONT,l_rap.pwm);
			}
		  else
		    {  
		     enable_pwm(L_BACK,l_rap.pwm);
			}

		if(l_rap.length<=l_ring.length+100)
		{
			if(l_rap.length > l_ring.length+5)
			if(l_rap.master)
			{
				if((--l_rap.sign) == 0)
				{
					 stop();
				}			  			 
			}
		}
	   }

  }
//======================================万向轮有效检测
//======================================增加红外数据延长时间处理
}

/****************使能车轮做一定的动作**************************
输入
         ori            车轮动作的方向
         length    		车轮动作的距离
		 rap            车轮动作的速度
		 sign           车轮动作模式
*/
void  enable_rap(uint32 ori_l, uint32 length_l,uint32 ori_r,uint32 length_r)
{
		  if(action.step ==0)
		  {
              if(Test_DriveOfWheel() == 1)
		      {
		          return;
		      } 												  
		      l_rap.pwm=200;
		      r_rap.pwm=200;
		  }
	      l_rap.ori=ori_l;
	      l_rap.length=length_l; //	变量数据已经传递了
		  l_rap.pulse=0;     
		  l_ring.length=0;
		  l_ring.odds=0;
		  l_ring.ek[0]=0;
		  l_ring.ek[1]=0;
		  l_ring.ek[2]=0;


		  r_rap.ori=ori_r;
	      r_rap.length=length_r; //	变量数据已经传递了
		  r_rap.pulse=0;     
		  r_ring.length=0; 
		  r_ring.odds=0;
		  r_ring.ek[0]=0;
		  r_ring.ek[1]=0;
		  r_ring.ek[2]=0;
if(	length_l == length_r)
{
    l_rap.rap=Speed;
	r_rap.rap=Speed;
	l_rap.sign=1;
	l_rap.master=1;
	r_rap.master=0;	   
	r_rap.sign=1;
}	  
if((length_l>length_r))
  {
    l_rap.rap=Speed;
	l_rap.sign=1;
	l_rap.master=1;
	r_rap.master=0;
	if(length_r>0) //速度调节
	{ 
	  r_rap.rap=(Speed*r_rap.length)/l_rap.length;
	  r_rap.sign=1;
	}
	else
	r_rap.sign=0;
  }
if((length_r>length_l))  
  {
    r_rap.rap=Speed;
	r_rap.sign=1;
	r_rap.master=1;
	l_rap.master=0;
	if(length_l>0)
	{ 
      l_rap.rap=(l_rap.length*Speed)/r_rap.length ;	 //距离与速度之间的转换
	  l_rap.sign=1;
	}
	else
	l_rap.sign=0;    
  } 

 //----------------------------------------
   if(l_rap.sign)
    {
		 if(l_rap.ori==FRONT)
		    {
			
              enable_pwm(L_FRONT,l_rap.pwm);
			}
		  else
		    {  
			 enable_pwm(L_BACK,l_rap.pwm);
			}
	}
   if(r_rap.sign)
    {
		 if(r_rap.ori==FRONT)
		    {
              enable_pwm(R_FRONT,r_rap.pwm);
			}
		  else
		    {  
			  enable_pwm(R_BACK,r_rap.pwm);
			}
	}
}
/******************************************************
停止两个轮子的转动
******************************************************/
void stop_rap(void)
{
       stop();
	   action.sign=0;
}
/************************************************************
功能：32位无符号数减法
输入：    little   减数
          big      被减数
返回：差值
************************************************************/
uint32 subbtime(uint32 little,uint32 big)
{
 return(big-little);
}

/***********************************************************
功能：停止两轮转动
***********************************************************/
void stop(void)
{
     l_rap.sign=0;
     r_rap.sign=0;
	 disable_pwm(L_FRONT);
	 disable_pwm(L_BACK);
	 disable_pwm(R_BACK);
	 disable_pwm(R_FRONT) ;
	 GPIO_SetBits(GPIOE, L_BACKONOFF);
	 GPIO_SetBits(GPIOE, R_BACKONOFF);
	 GPIO_SetBits(GPIOE,L_FRONTONOFF );
	 GPIO_SetBits(GPIOE,R_FRONTONOFF );	 
	 
}
/*********************************************************
功能：测试轮子的驱动之间是否存在有贯通的情况
输入：无
输出：在有贯通的情况下返回1，否则返回0
*********************************************************/
uint8 Test_DriveOfWheel(void) 
{
//临时读取的左右轮电流
uint16  dqcurrent;
    /********关闭驱动,测量左右轮AD转换的误差电流********/
    stop();
	DelayMs(10);
	/******打开下面N沟道管，测量P沟道管是否有漏电流******/
	GPIO_ResetBits(GPIOE, L_BACKONOFF);
	GPIO_ResetBits(GPIOE, R_BACKONOFF);
	GPIO_ResetBits(GPIOE,L_FRONTONOFF);
	GPIO_ResetBits(GPIOE,R_FRONTONOFF);
	DelayMs(50);
	dqcurrent = account_current(L_CURRENT);
	if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))  //左轮P沟道管有漏电流大于40ma
	{/********关闭驱动,测量左右轮AD转换的误差电流********/
        stop();
	    DelayMs(20);
	    GPIO_ResetBits(GPIOE, L_BACKONOFF);
	    GPIO_ResetBits(GPIOE, R_BACKONOFF);
	    GPIO_ResetBits(GPIOE,L_FRONTONOFF);
	    GPIO_ResetBits(GPIOE,R_FRONTONOFF);
	    DelayMs(200);
	    dqcurrent = account_current(L_CURRENT);
		if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))
		{
		    giv_sys_err = 27;
			stop();
		    return 1;
		}
	}
	dqcurrent = account_current(R_CURRENT);
	if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))  //右轮P沟道管有漏电流大于40ma
	{			
        stop();
	    DelayMs(20);
	    GPIO_ResetBits(GPIOE, L_BACKONOFF);
	    GPIO_ResetBits(GPIOE, R_BACKONOFF);
	    GPIO_ResetBits(GPIOE,L_FRONTONOFF);
	    GPIO_ResetBits(GPIOE,R_FRONTONOFF);
	    DelayMs(200);
		dqcurrent = account_current(R_CURRENT);
		if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))
		{
		    giv_sys_err = 28;
			stop();
		    return 1;
		}
	}
    stop();
	DelayMs(10);
	/***********打开上面P沟道管，测量N沟道管是否有漏电流********/
    Set_Pwm(L_FRONT,1000);
    Set_Pwm(L_BACK,1000);
    Set_Pwm(R_FRONT,1000);
    Set_Pwm(R_BACK,1000);
	DelayMs(50);
	dqcurrent = account_current(L_CURRENT);
	if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))  //左轮n沟道管有漏电流大于40ma
	{			
        stop();
	    DelayMs(20);
        Set_Pwm(L_FRONT,1000);
        Set_Pwm(L_BACK,1000);
        Set_Pwm(R_FRONT,1000);
        Set_Pwm(R_BACK,1000);
	    DelayMs(200);
	    dqcurrent = account_current(L_CURRENT);
		if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))
		{
		    giv_sys_err = 29;
			stop();
		    return 1;
		}
	}
	dqcurrent = account_current(R_CURRENT);
	if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))  //右轮n沟道管有漏电流大于40ma
	{			  
        stop();
	    DelayMs(20);
        Set_Pwm(L_FRONT,1000);
        Set_Pwm(L_BACK,1000);
        Set_Pwm(R_FRONT,1000);
        Set_Pwm(R_BACK,1000);
	    DelayMs(200);
		dqcurrent = account_current(R_CURRENT);
		if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))
		{
		    giv_sys_err = 30;
			stop();
		    return 1;
		}
	}
    stop();
	DelayMs(10);
    return 0;	
}
/*********************************************************
功能：测试中扫的驱动之间是否存在有贯通的情况
输入：无
输出：在有贯通的情况下返回1，否则返回0
*********************************************************/
uint8 Test_DriveOfBrush(void) 
{
//===================临时读取的左右轮电流
uint16  dqcurrent;
    /********关闭驱动,测量AD转换的误差电流********/
    Reset_M_FPMOS();
    Reset_M_ZPMOS();
	Set_M_FNMOS();
	Set_M_ZNMOS();

	DelayMs(10);
	/******打开下面N沟道管，测量P沟道管是否有漏电流******/
 /*   Set_M_FPMOS();
    Set_M_ZPMOS();
	DelayMs(50);
	dqcurrent = account_current(M_CURRENT);
	if((dqcurrent > 500))  //左轮P沟道管有漏电流大于40ma
	{
	    DelayMs(200);
	    dqcurrent = account_current(M_CURRENT);
		if((dqcurrent  > 500))
		{
            Reset_M_FPMOS();
            Reset_M_ZPMOS();
	        Set_M_FNMOS();
	        Set_M_ZNMOS();
		    return 1;
		}
	}
    Reset_M_FPMOS();
    Reset_M_ZPMOS();
	Set_M_FNMOS();
	Set_M_ZNMOS();
	DelayMs(10); */
	/***********打开上面P沟道管，测量N沟道管是否有漏电流********/
    Set_M_Motor();

	DelayMs(50);
	dqcurrent = account_current(M_CURRENT);
	if((dqcurrent  > 500))  //左轮n沟道管有漏电流大于40ma
	{
	    DelayMs(200);
	    dqcurrent = account_current(M_CURRENT);
		if((dqcurrent  > 500))
		{
		    giv_sys_err = 29;
			stop();
		    return 1;
		}
	}
	Reset_M_Motor();
   
   /* Reset_M_FPMOS();
    Reset_M_ZPMOS();
	Set_M_FNMOS();
	Set_M_ZNMOS();
	*/
	DelayMs(10);
    return 0;	
}
