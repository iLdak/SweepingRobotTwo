
//======================ͷ�ļ�====================================
#include "key.h"
#include "hardware.h"

//======================˽�б���==================================
//======================ȫ�ֱ���==================================

KEY key1,key2,key3,l_lidi,r_lidi,l_bump,r_bump,charge_dc,charge_seat,s_current;
bool key_time;
//======================˽�ж���==================================

KEY *hightaction[4] = {&charge_dc,&charge_seat,&l_bump,&r_bump,};    //����Ч�Ŀ���������
KEY *lowaction[6]  =  {&key1,&key2,&key3,&l_lidi,&r_lidi,
                      &s_current};    //����Ч�Ŀ���������
//======================ȫ�ֺ���==================================
void init_key(void);
void AutoReadKey(void);
//======================˽�к���==================================
void read_key( KEY *key);
void read_scurrent( KEY *key);
//======================����ʵ��==================================
/*************************************************************
��ʼ�������ͷ�����
*************************************************************/
void init_key(void)
{
uint8 x;
NVIC_InitTypeDef NVIC_InitStructure;
//=====================�����ʼ����������״̬��ȡ����==============
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
   { //���а���ʱΪ1����ô��ʼ��Ϊ0
       hightaction[x]->sign = 0;
	   hightaction[x]->number = 0;
	   hightaction[x]->key = 0;
   }
   for(x=0; x<6; x++)
   { //���а���ʱΪ0����ô��ʼ��Ϊ1
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
��ȡ�����ļ�ֵ
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
���ܣ���ȡ����Ч�İ���
*****************************************************/
void read_key( KEY *key)
{
      if(key->key == 1)               //û�а���
	  {
	        if(key->read_start() == Bit_RESET)
		    {
	           key->number ++;        //ȥ��
			   if(key->number > 20)
			   {
			     key->sign = 0;       //ȷ�ϰ��°���
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
	  {	//�а������� ��ȷ�������Ƿ��Ѿ�̧���Ա���һ��ʹ��
		    if(key->read_start() != Bit_RESET)
		    {
	           key->number++;
			   if(key->number > 20)     //����ȥ��
			   {
				 key->key = 1;          //ȷ�ϰ���̧��
				 key->sign = 1;         //ȷ�ϰ��°���
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
���ܣ���ȡ����Ч�İ���
*****************************************************/
void read_scurrent( KEY *key)
{
      if(key->key == 1)          //û�а���
	  {
        if(key->read_start() == Bit_RESET)
	    {
           key->number ++;       //ȥ��
		   if(key->number > 20)
		   {
		     key->sign = 0;      //ȷ�ϰ��°���
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
	  {	 //�а�������
	    if(key->read_start() != Bit_RESET)
	    {
           key->number++;
		   if(key->number > 20)  //����ȥ��
		   {
			 key->key = 1;        //ȷ�ϰ���̧��
			 key->sign = 1;       //ȷ�ϰ��°���
			 key->number = 0;
		   }
	    }
		else
	    {
		    key->number = 0;
		}
	  }
}
