#include "ring.h"
#include "time1.h"
#include "pwm.h"
#include "ad.h"
#include "lcdkey.h"
#include "hardware.h"
#include "function.h"
//===============================˽�ж���=========================
//===============================ȫ�ֱ���=========================
RING  l_ring,r_ring,w_ring;       //���Գ����ٶȵĽṹ
RAP  l_rap,r_rap,w_rap;           //�����ٶȾ���Ŀ��ƽṹ
bool rap_time;                    //�����ٶȵ���ʱ���־
uint32 Speed ;                    //���ֵ�����ٶ�  ����ʾΪ����ʱ��
uint16 jt_l_current;              //��������1n60����©��������ľ�̬������
uint16 jt_r_current;              //��������1n60����©��������ľ�̬������
ACTION action;

//================================˽�б���==============================
uint8 Wheel_State = 0;            //����������״̬ 0Ϊ�ϸ�1Ϊ������
uint8 err;
//================================ȫ�ֺ���===============================
//================================˽�к���===============================
void stop(void);
void Set_WheelState(void);

//=================================����ʵ��=========================================
void Init_Ring(void)
{
 //================================��ʼ��������ֵ================================== 
l_ring.length = 0;                 //���ֵ�����Ϊ0
l_ring.dq_time = giv_sys_time;     //��λ�������ʱ��
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
Speed = HIGHSPEED;//1500;   // 1/1000��
Dis_Speed = 1;
}
/************************************************************
����  ��    ÿ10ms ִ��һ�Σ��Զ����ݳ�����Ϣ���Ƴ���
���룺 ��
��� ����
************************************************************/
void  comm_rap(void)
{
uint32 t,l;
int32 ec;
uint16 dqcurrent;
//===============================�������ֵ��ٶȱ�
if(rap_time != FALSE)
  {
    rap_time = FALSE;
//===============================����ͬ��=========================
     if((l_rap.sign)&&(r_rap.sign))   //ֻ�������ֶ�תʱ�ŵ��ڣ�����ֻ���ٶ���������
	 {
	   if(l_rap.master == 1)          //����������
	   {    
	        ec = (r_ring.length*l_rap.rap)/r_rap.rap - l_ring.length;  //���ݱ����������ֵ��ٶȲ�
			//�����߹���������������ÿ���������
			if (ec > 0)
			{
			   if(r_rap.pwm > 0)       //�����ٶȹ���
			   r_rap.pwm--;
			} 
			else
			{
			   if(ec < 0)              //�����ٶȹ���
			   r_rap.pwm++;
			}
	   }
	   else              
	   {                                //����������
	        ec = (l_ring.length*r_rap.rap)/l_rap.rap - r_ring.length; //���ݱ����������ֵ��ٶȲ�
			if (ec > 0)
			{                          //�����ٶȹ���
			   if(l_rap.pwm > 0)
			   l_rap.pwm--;
			} 
			else
			{
			   if(ec < 0)              //�����ٶȹ���
			   l_rap.pwm++;
			}
	   }
	 }	
 //============================����============================ 
	   if(r_rap.sign)
	   {	  
		        //=============������������ʱ��   
		   t = (r_ring.dq_time - r_ring.old_time );	//^T
		   r_ring.old_time = r_ring.dq_time;
		        //=============�������ߵ������� 
		   l = subbtime(r_rap.pulse,r_ring.length);	//l=r_ring.length-r_rap.pulse ,^S
		   r_rap.pulse = r_ring.length;	//���¶���׼
		   
		   if(l == 0)
	       {
		       r_ring.odds++;
		       if(r_ring.odds > 200)r_ring.state = BAD;	 //����2���ӵ�ʱ�䣬������������¶�û���ƶ������ж�����
		   }
	       else
	       {
		       r_ring.odds = 0;
		       r_ring.state = GOOD;
		   }
		  r_ring.ek[2] = r_ring.ek[1];	 //�˶����ƽ����ƽ���λ����
		  r_ring.ek[1] = r_ring.ek[0];
		  if(t)//�Ѿ��˶��ˣ���ʱ������ƣ�ʱ������Ѿ�����	 //���¼���	r_ring.ek[0]
		  r_ring.ek[0] = r_rap.rap - (l*10000)/t;  //V`=V-^S/^T
		  else
		  r_ring.ek[0] = r_rap.rap;
		  //[PID]--P��Сƫ�I�������Dƫ���źŵı仯����
		  ec =(int16)((r_ring.ek[0]-r_ring.ek[1])/KP 
		       + r_ring.ek[0]/KI
			   +(r_ring.ek[0]+r_ring.ek[2]-2*r_ring.ek[1])/KD);
		  //KP=4,KI=10,KD=80
		  if(ec > 100)
		  {
		      ec = 100;
		  }
		  //�ٴθ��ݷ������������¼�����������������ٶȽ��е���
		  dqcurrent = account_current( R_CURRENT);   	//�������ӵĵ���
		  if(dqcurrent > (466 + jt_r_current))			//��������800ma
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
		     if(ec > 1000)	//�����Ϊ�����������ٶȳ�������󣬽�����������¸���
		     ec = 1000;
		     r_rap.pwm = (uint16)ec;  //ÿ���ӵ������������ٶȵ��ڵĻ�׼
		 }
		 //�����ȷ��
		if(r_rap.ori == FRONT)
		{
		 	  enable_pwm(R_FRONT,r_rap.pwm);	//ǰ��
		}
		else
		{  
		 	  enable_pwm(R_BACK,r_rap.pwm); //����
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
//================================����=======================================
	if(l_rap.sign)
	   {	  
//====================������������ʱ��
		   t = (l_ring.dq_time - l_ring.old_time );
		   l_ring.old_time = l_ring.dq_time;
//====================�������ߵ������� 
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
//======================================��������Ч���
//======================================���Ӻ��������ӳ�ʱ�䴦��
}

/****************ʹ�ܳ�����һ���Ķ���**************************
����
         ori            ���ֶ����ķ���
         length    		���ֶ����ľ���
		 rap            ���ֶ������ٶ�
		 sign           ���ֶ���ģʽ
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
	      l_rap.length=length_l; //	���������Ѿ�������
		  l_rap.pulse=0;     
		  l_ring.length=0;
		  l_ring.odds=0;
		  l_ring.ek[0]=0;
		  l_ring.ek[1]=0;
		  l_ring.ek[2]=0;


		  r_rap.ori=ori_r;
	      r_rap.length=length_r; //	���������Ѿ�������
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
	if(length_r>0) //�ٶȵ���
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
      l_rap.rap=(l_rap.length*Speed)/r_rap.length ;	 //�������ٶ�֮���ת��
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
ֹͣ�������ӵ�ת��
******************************************************/
void stop_rap(void)
{
       stop();
	   action.sign=0;
}
/************************************************************
���ܣ�32λ�޷���������
���룺    little   ����
          big      ������
���أ���ֵ
************************************************************/
uint32 subbtime(uint32 little,uint32 big)
{
 return(big-little);
}

/***********************************************************
���ܣ�ֹͣ����ת��
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
���ܣ��������ӵ�����֮���Ƿ�����й�ͨ�����
���룺��
��������й�ͨ������·���1�����򷵻�0
*********************************************************/
uint8 Test_DriveOfWheel(void) 
{
//��ʱ��ȡ�������ֵ���
uint16  dqcurrent;
    /********�ر�����,����������ADת����������********/
    stop();
	DelayMs(10);
	/******������N�����ܣ�����P�������Ƿ���©����******/
	GPIO_ResetBits(GPIOE, L_BACKONOFF);
	GPIO_ResetBits(GPIOE, R_BACKONOFF);
	GPIO_ResetBits(GPIOE,L_FRONTONOFF);
	GPIO_ResetBits(GPIOE,R_FRONTONOFF);
	DelayMs(50);
	dqcurrent = account_current(L_CURRENT);
	if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))  //����P��������©��������40ma
	{/********�ر�����,����������ADת����������********/
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
	if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))  //����P��������©��������40ma
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
	/***********������P�����ܣ�����N�������Ƿ���©����********/
    Set_Pwm(L_FRONT,1000);
    Set_Pwm(L_BACK,1000);
    Set_Pwm(R_FRONT,1000);
    Set_Pwm(R_BACK,1000);
	DelayMs(50);
	dqcurrent = account_current(L_CURRENT);
	if((dqcurrent > jt_l_current) && ((dqcurrent - jt_l_current) > 500))  //����n��������©��������40ma
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
	if((dqcurrent > jt_r_current) && ((dqcurrent - jt_r_current) > 500))  //����n��������©��������40ma
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
���ܣ�������ɨ������֮���Ƿ�����й�ͨ�����
���룺��
��������й�ͨ������·���1�����򷵻�0
*********************************************************/
uint8 Test_DriveOfBrush(void) 
{
//===================��ʱ��ȡ�������ֵ���
uint16  dqcurrent;
    /********�ر�����,����ADת����������********/
    Reset_M_FPMOS();
    Reset_M_ZPMOS();
	Set_M_FNMOS();
	Set_M_ZNMOS();

	DelayMs(10);
	/******������N�����ܣ�����P�������Ƿ���©����******/
 /*   Set_M_FPMOS();
    Set_M_ZPMOS();
	DelayMs(50);
	dqcurrent = account_current(M_CURRENT);
	if((dqcurrent > 500))  //����P��������©��������40ma
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
	/***********������P�����ܣ�����N�������Ƿ���©����********/
    Set_M_Motor();

	DelayMs(50);
	dqcurrent = account_current(M_CURRENT);
	if((dqcurrent  > 500))  //����n��������©��������40ma
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
