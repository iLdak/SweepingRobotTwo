
//======================头文件====================================
#include "key.h"
#include "hardware.h"

//======================私有变量==================================
//======================全局变量==================================

KEY key1,key2,key3,l_lidi,r_lidi,l_bump,r_bump,charge_dc,charge_seat,s_current;
bool key_time;
//======================私有定义==================================

KEY *hightaction[4] = {&charge_dc,&charge_seat,&l_bump,&r_bump,};    //高有效的开关量输入
KEY *lowaction[6]  =  {&key1,&key2,&key3,&l_lidi,&r_lidi,
                      &s_current};    //低有效的开关量输入
//======================全局函数==================================
void init_key(void);
void AutoReadKey(void);
//======================私有函数==================================
void read_key( KEY *key);
void read_scurrent( KEY *key);
//======================函数实体==================================
/*************************************************************
初始化按键和蜂鸣器
*************************************************************/
void init_key(void)
{
uint8 x;
NVIC_InitTypeDef NVIC_InitStructure;
//=====================下面初始化开关量的状态读取函数==============
   key1.read_start = Read_Key1;
   key2.read_start = Read_Key2;
   key3.read_start = Read_Key3;
   l_lidi.read_start = Read_L_LIDI;
   r_lidi.read_start = Read_R_LIDI;
   charge_dc.read_start = Read_Charge_Dc;
   charge_seat.read_start = Read_Charge_Seat;
   s_current.read_start = Read_S_Current;
   l_bump.read_start = Read_L_Bump;
   r_bump.read_start = Read_R_Bump;
   for(x=0; x<4; x++)
   { //当有按键时为1，那么开始就为0
       hightaction[x]->sign = 0;
	   hightaction[x]->number = 0;
	   hightaction[x]->key = 0;
   }
   for(x=0; x<6; x++)
   { //当有按键时为0，那么开始就为1
       lowaction[x]->sign = 1;
	   lowaction[x]->number = 0;
	   lowaction[x]->key = 1;
   }
   key_time = FALSE;
   
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource3);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);
  EXTI->FTSR |= 0xe;
  EXTI->RTSR |= 0x1800;
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQChannel;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
  NVIC_Init(&NVIC_InitStructure);
}
/************************************************************
读取按键的键值
************************************************************/
void AutoReadKey(void)
{
   if(key_time != FALSE)
   {
       key_time = FALSE;
	   read_key(&key1);
	   read_key(&key2);
	   read_key(&key3);
	   read_key(&l_lidi);
	   read_key(&r_lidi);
	   read_key(&l_bump);
	   read_key(&r_bump);
	   read_key(&charge_dc);
	   read_key(&charge_seat);
	   read_scurrent(&s_current);
   }
}
/*****************************************************
功能：读取高有效的按键
*****************************************************/
void read_key( KEY *key)
{
      if(key->key == 1)               //没有按键
	  {
	        if(key->read_start() == Bit_RESET)
		    {
	           key->number ++;        //去抖
			   if(key->number > 20)
			   {
			     key->sign = 0;       //确认按下按键
				 key->key = 0;
				 key->number = 0;
			   }
		    }
			else
		    {
			   key->number = 0;
			}
	  }
	  else
	  {	//有按键按下 ，确定按键是否已经抬起，以被下一次使用
		    if(key->read_start() != Bit_RESET)
		    {
	           key->number++;
			   if(key->number > 20)     //按键去抖
			   {
				 key->key = 1;          //确认按键抬起
				 key->sign = 1;         //确认按下按键
				 key->number = 0;
			   }
		    }
			else
		    {
			  key->number = 0;
			}
	  }
}
/*****************************************************
功能：读取高有效的按键
*****************************************************/
void read_scurrent( KEY *key)
{
      if(key->key == 1)          //没有按键
	  {
        if(key->read_start() == Bit_RESET)
	    {
           key->number ++;       //去抖
		   if(key->number > 20)
		   {
		     key->sign = 0;      //确认按下按键
			 key->key = 0;
			 key->number = 0;
		   }
	    }
		else
	    {
		  key->number = 0;
		}
	  }
	  else
	  {	 //有按键按下
	    if(key->read_start() != Bit_RESET)
	    {
           key->number++;
		   if(key->number > 20)  //按键去抖
		   {
			 key->key = 1;        //确认案件抬起
			 key->sign = 1;       //确认按下按键
			 key->number = 0;
		   }
	    }
		else
	    {
		    key->number = 0;
		}
	  }
}
