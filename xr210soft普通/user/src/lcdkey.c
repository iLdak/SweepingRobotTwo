#include  "lcdkey.h"
#include  "time1.h"
#include  "hardware.h"

//======================˽�ж���===========================
//======================ȫ�ֱ���===========================
uint8 Dis_Data[10] ; 	//��ʾ��������
bool Dis_On;            //��ʾ��־��
bool Dis_ErrOn ;    	//����ͼ����
bool Dis_UvOn ;			//uvͼ����
bool Dis_PrOn ;			//ԤԼ������
bool Dis_AuOn ;			//�Զ���ɨ��
bool Dis_SpOn ;			//�ֲ���ɨ��
bool Dis_HourOn ;		//Сʱ��
bool Dis_MinuteOn ;		//������
bool Dis_ColOn ;		//ʱ�����
bool Dis_WeekOn ;		//������
bool Dis_SpeedOn ;		//�ٶ���
bool Dis_PowerOn ;		//���ͼ����
bool Dis_ChargeOn ;		//���ͼ����
bool Dis_SpeedGo;       //����ͼ��ת��

bool Dis_ErrGlint ;		//����ͼ����˸
bool Dis_UvGlint ;		//uvͼ����˸
bool Dis_PrGlint ;		//ԤԼ������˸
bool Dis_AuGlint ;		//�Զ���ɨ��˸
bool Dis_SpGlint ;		//�ֲ���ɨ��˸
bool Dis_HourGlint ;	//Сʱ��˸
bool Dis_MinuteGlint ;	//������˸
bool Dis_ColGlint ;		//ʱ�����˸
bool Dis_WeekGlint ;	//������˸
bool Dis_SpeedGlint ;	//�ٶ���˸
bool Dis_PowerGlint ;	//���ͼ����˸
bool Dis_ChargeGlint ;	//���ͼ����˸

uint8 Dis_Preengage;	//ԤԼ����
uint8 Dis_Hour; 		//��ʾ��Сʱ
uint8 Dis_Minute;  		//��ʾ�ķ���
uint8 Dis_Week;			//��ʾ����������  ��Χ 1����7
uint8 Dis_Speed;		//��ʾ���ٶȣ���Χ1����3
uint8 Dis_Power;		//��ʾ�ĵ�Դ���� ����Χ0����3

bool dis_updata;        //��ʾ���±�־

//========================˽�б���==================================
const uint8 disnumber[16] = {0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,
                             0xef,0xce,0xee,0xe3,0xa9,0x67,0xe9,0xe8};//���ֽ���
const uint8 disspeed[6] = {0x80,0x01,
                           0x61,0x11,
						   0xa1,0x11};  //�ٶ����ֽ���
const uint8 dispower[4] = {0x0,0x4,0x6,0x7}; 	

bool Action_display;  // ��ǰ����ʾ״̬
//=======================ȫ�ֺ���===================================

void dis_Preengage (void);//����
void dis_disPreengage(void);//
void dis_Hour(void);
void dis_disHour(void);
void dis_Minute(void);
void dis_disMinute(void);
void dis_Week(void);
void dis_disWeek(void);
void dis_Speed(void);
void dis_disSpeed(void);
void dis_Power(void);
void dis_disPower(void);
//=====================�ֲ���������=======================
/*********************************************************
������void  AutoDisplay(void)
���룺��
�������
˵����������ʾ�Ĵ���������ʾ����ʾ��0.1�����һ��
      ��˸ʱ��Ƶ��Ϊ1�롣
	  ����ת������Ƶ��Ϊ0.6�롢0.4�롢0.2�����
*********************************************************/
void  AutoDisplay(void)
{
  static uint8 number = 0;
  uint32 l;
  if(Dis_On == TRUE)     //������ʾ
  {
	  if(dis_updata == TRUE)   //���˸�����ʾ��
	  {
	  	dis_updata = FALSE;
		if(Action_display == FALSE)
		{
		    Action_display = TRUE;
            lcdkey_bgon();     //�򿪱���
	        lcdkey_dison();    //����ʾ
		}
		number ++;			 
	   // Dis_Data[7] |= 1; 
	    Dis_Data[2] |= 0X10;
		if(number == 20)   //��˸��1�붨ʱ
		{
		   number =0;
		}
		if(( Dis_SpeedOn  != FALSE)&&(Dis_SpeedGlint == FALSE)) //��ʾ����
		{
			//Dis_Data[0] &= 0x11;
			//Dis_Data[6] &= 0x11;
			Dis_Data[4] &= 0x11;
			Dis_Data[3] &= 0x11;
		 	if(( Dis_Speed == 0) || (Dis_Speed >3))
			{
			 	Dis_Speed = 1;
			}
			l = giv_sys_time /(1000+500*Dis_Speed);
			l = l%4;
			if(Dis_SpeedGo == FALSE)
			{
			    l = 0;
			}
			switch(l)
			{
			     case 0:
				 //Dis_Data[6] |= 0x2;    //����*3  ��ʾ����
				 //Dis_Data[0] |= 0x84;    //����*3  ��ʾ����
				 Dis_Data[3] |= 0x20;    //����*3  ��ʾ����
				 Dis_Data[4] |= 0x84;    //����*3  ��ʾ����
				 break;
			     case 1:
				 //Dis_Data[6] |= 0x84;    //����*3  ��ʾ����
				 //Dis_Data[0] |= 0x2;    //����*3  ��ʾ����
				 Dis_Data[3] |= 0x48;    //����*3  ��ʾ����
				 Dis_Data[4] |= 0x2;    //����*3  ��ʾ����
				 break;
			     case 2:
				 //Dis_Data[6] |= 0x48;    //����*3  ��ʾ����
				 //Dis_Data[0] |= 0x20;    //����*3  ��ʾ����
				 Dis_Data[3] |= 0x84;    //����*3  ��ʾ����
				 Dis_Data[4] |= 0x20;    //����*3  ��ʾ����
				 break;
			     case 3:
				 //Dis_Data[6] |= 0x20;    //����*3  ��ʾ����
				 //Dis_Data[0] |= 0x48;    //����*3  ��ʾ����
				 Dis_Data[3] |= 0x2;    //����*3  ��ʾ����
				 Dis_Data[4] |= 0x48;    //����*3  ��ʾ����
				 break;
			}
		}	
		if (number < 10)    //��ǰ������
		{
		   if( Dis_ErrOn == FALSE)  //�����־
		   {
		      //Dis_Data[4] &= 0xef;
		      Dis_Data[8] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[4] |= 0x10;
		   	  Dis_Data[8] |= 0x10;
		   }
		   if( Dis_UvOn == FALSE)//UV��־
		   {
		      //Dis_Data[5] &= 0xef;
		      Dis_Data[9] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[5] |= 0x10;
		   	  Dis_Data[9] |= 0x10;
		   }
		   if( Dis_PrOn == FALSE) //ԤԼ����
		   {
		     dis_disPreengage();
		   }
		   else
		   {
		   	  dis_Preengage();
		   }
		   if( Dis_AuOn == FALSE) //�Զ���ʾͼ��
		   {
		      //Dis_Data[2] &= 0xef;
		      Dis_Data[6] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[2] |= 0x10;
		   	  Dis_Data[6] |= 0x10;
		   }
		   if( Dis_SpOn == FALSE)//����ͼ��
		   {
		      //Dis_Data[1] &= 0xef;
		      Dis_Data[5] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[1] |= 0x10;
		   	  Dis_Data[5] |= 0x10;
		   }
		   if( Dis_HourOn == FALSE)//��ʾСʱ
		   {
		      dis_disHour();
		   }
		   else
		   {
		   	  dis_Hour();
		   }
		   if( Dis_MinuteOn  == FALSE)//��ʾ����
		   {
		      dis_disMinute();
		   }
		   else
		   {
		   	  dis_Minute();
		   }
		   if( Dis_ColOn == FALSE)//��ʾʱ���ͼ��
		   {
		      //Dis_Data[3] &= 0xef;
		      Dis_Data[7] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[3] |= 0x10;
		   	  Dis_Data[7] |= 0x10;
		   }
		   if( Dis_WeekOn  == FALSE)//��ʾ����
		   {
		      dis_disWeek();
		   }
		   else
		   {
		   	  dis_Week();
		   }
		   if( Dis_SpeedOn  == FALSE)//��ʾ�ٶ�
		   {
		      dis_disSpeed();
		   }
		   else
		   {
		      if(Dis_SpeedGlint != FALSE)
			  {
			    //Dis_Data[0] |= 0xee;
				//Dis_Data[6] |= 0xee;
			    Dis_Data[4] |= 0xee;
				Dis_Data[3] |= 0xee;
			  }
		   	  dis_Speed();
		   }
		   if( Dis_PowerOn  == FALSE)//��ʾ���
		   {
		      dis_disPower();
		   }
		   else
		   {
		   	  dis_Power();
		   }
		   if( Dis_ChargeOn == FALSE)  //����־
		   {
		      //Dis_Data[7] &= 0x7f;
		      Dis_Data[2] &= 0xf7;
		   }
		   else
		   {
		   	  //Dis_Data[7] |= 0x80;
		   	  Dis_Data[2] |= 0x8;
		   }
		}
		////////////////////////��˸���°���///////////////////
		else
		{
		   if( Dis_ErrGlint != FALSE)  //�����־
		   {
		      //Dis_Data[4] &= 0xef;
		      Dis_Data[8] &= 0xef;
		   }
		   if( Dis_UvGlint != FALSE)//UV��־
		   {
		      //Dis_Data[5] &= 0xef;
		      Dis_Data[9] &= 0xef;
		   }
		   if( Dis_PrGlint != FALSE) //ԤԼ����
		   {
		     dis_disPreengage();
		   }
		   if( Dis_AuGlint != FALSE) //�Զ���ʾͼ��
		   {
		      //Dis_Data[2] &= 0xef;
		      Dis_Data[6] &= 0xef;
		   }
		   if( Dis_SpGlint != FALSE)//����ͼ��
		   {
		      //Dis_Data[1] &= 0xef;
		      Dis_Data[5] &= 0xef;
		   }
		   if( Dis_HourGlint != FALSE)//��ʾСʱ
		   {
		      dis_disHour();
		   }
		   if( Dis_MinuteGlint  != FALSE)//��ʾ����
		   {
		      dis_disMinute();
		   }
		   if( Dis_ColGlint != FALSE)//��ʾʱ���ͼ��
		   {
		      //Dis_Data[3] &= 0xef;
		      Dis_Data[7] &= 0xef;
		   }
		   if( Dis_WeekGlint  != FALSE)//��ʾ����
		   {
		      dis_disWeek();
		   }
		   if( Dis_SpeedGlint  != FALSE)//��ʾ�ٶ�
		   {
		      dis_disSpeed();
		   }
		   if( Dis_PowerGlint  != FALSE)//��ʾ���
		   {
		      dis_disPower();
		   }
		   if( Dis_ChargeGlint != FALSE)  //����־
		   {
		      //Dis_Data[7] &= 0x7f;
		      Dis_Data[2] &= 0xf7;
		   }
		
		}
		lcdkey_disdata(12, Dis_Data, 10); 
	  } 
  }
  else
  {
      if(Action_display != FALSE)
	  {
	      Action_display = FALSE;
          lcdkey_bgoff();
	      lcdkey_disoff();
	  }
  }
}
/*********************************************************
�������ƣ�delay(uint32 delaynumbe)
��������ʱ�ӳ���
���룺delaynumber   ��ʱʱ��ĳ���
�������
**********************************************************/
void delay(uint32 delaynumber)
{
  uint32 l;
  for(l=0; l<delaynumber; l++)
  ;
}
/***********************************************************
�������֣�void Init_Lcd(void)
��������ʼ��Һ����ʾ��оƬTM1723
���룺��
�������
***********************************************************/
void Init_Lcd(void)
{
  Set_LCD_Power();
  Set_LCD_Stb();
  Set_LCD_BG();
  Set_LCD_Data();
  Set_LCD_Clk();
  //ȫ���رսӿ�
  delay(100);
  Reset_LCD_Power();
  delay(100);
  Lcd_WriteCommand(0x1c6);//����Ϊ��ͨģʽ
  Lcd_WriteCommand(0X40);
  Lcd_WriteCommand(2);
  Lcd_WriteCommand(0X50);
  Lcd_WriteCommand(0x30);//ϵͳʱ��ԴΪ�ڲ�rc����
  clr_display();
  Action_display = FALSE;
}
/*************************************************************
�������ƣ�lcdkey_write
��������LCD����оƬTM1723д��һ���ֽ�����
���룺data   Ҫд�������
�������
*************************************************************/
void Lcd_WriteCommand(uint16 command)
{
uint8 l;
   Reset_LCD_Stb();
   delay(10);  
   Reset_LCD_Clk();
   Set_LCD_Data();
   delay(10);  
   Set_LCD_Clk();
   delay(10); 
   Reset_LCD_Clk();
   Reset_LCD_Data();
   delay(10);  
   Set_LCD_Clk();
   delay(10); 
   Reset_LCD_Clk();
   Reset_LCD_Data();
   delay(10);  
   Set_LCD_Clk();
   delay(10); 
   for(l=0; l<9; l++)
   {
	  Reset_LCD_Clk();
	  if((command & 0x100) != 0)
	   {
	     Set_LCD_Data();
	   }
	  else
	   {
	     Reset_LCD_Data();
	   } 
	  command *=2;
	  delay(10);
	  Set_LCD_Clk();
	  delay(10);  
   }
   Set_LCD_Data();
   Set_LCD_Stb();
}
/**************************************************************
�������ƣ�cdkey_dison
����������ʾ
���룺��
�������
**************************************************************/
void  lcdkey_dison(void)
{
  Lcd_WriteCommand(0x6);
}
/**************************************************************
�������ƣ�lcdkey_disoff
�������ر���ʾ
���룺��
�������
**************************************************************/
void  lcdkey_disoff(void)
{
  Lcd_WriteCommand(0x4);
}
/**************************************************************
�������ƣ�cdkey_bgon
�������򿪱���
���룺��
�������
**************************************************************/
void  lcdkey_bgon(void)
{
  Reset_LCD_BG();
}
/**************************************************************
�������ƣ�cdkey_bgoff
�������رձ���
���룺��
�������
**************************************************************/
void  lcdkey_bgoff(void)
{
  Set_LCD_BG();
}


/**************************************************************
�������ƣ�lcdkey_disdata
��������ָ��λ����ʾ����
���룺��
�����addr   д����ʵ��λ��,��ַ���ܴ���15
      str    Ҫд������ݵ�ַ
	  number Ҫд����ֽ���
**************************************************************/
void  lcdkey_disdata(uint8  addr, uint8*  str, uint8  number)
{
uint8 l,m,n;
   Reset_LCD_Stb();
   delay(1);  
   Reset_LCD_Clk();
   Set_LCD_Data();
   delay(1);  
   Set_LCD_Clk();
   delay(1); 
   Reset_LCD_Clk();
   Reset_LCD_Data();
   delay(1);  
   Set_LCD_Clk();
   delay(1); 
   Reset_LCD_Clk();
   Set_LCD_Data();
   delay(1);  
   Set_LCD_Clk();
   delay(1); 
   for(l=0; l<6; l++)
   {
	  Reset_LCD_Clk();
	  if((addr & 0x20) != 0)
	   {
	     Set_LCD_Data();
	   }
	  else
	   {
	     Reset_LCD_Data();
	   } 
	  addr *=2;
	  delay(1);
	  Set_LCD_Clk();
	  delay(1);  
   }
   for(l = 0;l<number ;l++)
   {
       n = str[l];
       for(m=0; m<8; m++)
	   {
	       Reset_LCD_Clk();
	       if((n & 0x1) != 0)
	       {
	           Set_LCD_Data();
	       }
	       else
	       {
	           Reset_LCD_Data();
	       } 
	       n /=2;
	       delay(1);
	       Set_LCD_Clk();
	       delay(1);  
	   }
   }
   Set_LCD_Data();
   Set_LCD_Stb();
}
/************************************************************
��ʾԤԼͼ��
************************************************************/
void dis_Preengage (void)
{
  //Dis_Data[9] = 0;
  //Dis_Data[8] = 0;
  //Dis_Data[7] |= 0x8;
  Dis_Data[0] = 0;
  Dis_Data[1] = 0;
  Dis_Data[2] |= 0x80;
  if(Dis_Preengage != 0)
  {
	{
	 	if((Dis_Preengage&0x1) != 0)  //��ʾ����1
		{
		 	//Dis_Data[8] |= 0x44;
		 	Dis_Data[1] |= 0x44;
		}
	 	if((Dis_Preengage&0x2) != 0)  //��ʾ����2
		{
		 	//Dis_Data[8] |= 0x22;
		 	Dis_Data[1] |= 0x22;
		}
	 	if((Dis_Preengage&0x4) != 0)  //��ʾ����3
		{
		 	//Dis_Data[8] |= 0x9;
		 	Dis_Data[1] |= 0x90;
		}
	 	if((Dis_Preengage&0x8) != 0)  //��ʾ����4
		{
		 	//Dis_Data[8] |= 0x90;
		 	Dis_Data[1] |= 0x9;
		}
	 	if((Dis_Preengage&0x10) != 0)  //��ʾ����5
		{
		 	//Dis_Data[9] |= 0x11;
		 	Dis_Data[0] |= 0x11;
		}
	 	if((Dis_Preengage&0x20) != 0)  //��ʾ����6
		{
		 	//Dis_Data[9] |= 0x88;
		 	Dis_Data[0] |= 0x88;
		}
	 	if((Dis_Preengage&0x40) != 0)  //��ʾ����7
		{
		 	//Dis_Data[9] |= 0x22;
		 	Dis_Data[0] |= 0x22;
		}	  
	 	if((Dis_Preengage&0x80) != 0)  //ÿ��
		{
		 	//Dis_Data[9] |= 0x22;
		 	Dis_Data[0] |= 0x44;
		}
	}
  } 
  
}

/************************************************************
�ر�ԤԼͼ��
************************************************************/
void dis_disPreengage(void)
{
	  //Dis_Data[7] &= 0xf7;
	  //Dis_Data[8] &= 0x0;
	  //Dis_Data[9] &= 0x0;
	  Dis_Data[2] &= 0x7f;
	  Dis_Data[1] &= 0x0;
	  Dis_Data[0] &= 0x0;
}
/***********************************************************
��ʾСʱ
***********************************************************/
void dis_Hour(void)
{
 uint8 h,l;
/*    if(Dis_Hour > 23)
	Dis_Hour = 0;*/
 	h = Dis_Hour /10;//
 	l = Dis_Hour % 10;//
    //Dis_Data[5] &= 0x10;
	//Dis_Data[4] &= 0x10;
	//Dis_Data[5] |= disnumber[h];
	//Dis_Data[4] |= disnumber[l];
    Dis_Data[9] &= 0x10;
	Dis_Data[8] &= 0x10;
	Dis_Data[9] |= disnumber[h];
	Dis_Data[8] |= disnumber[l];
}
/***********************************************************
�ر�Сʱ
***********************************************************/
void dis_disHour(void)
{
    //Dis_Data[5] &= 0x10;
	//Dis_Data[4] &= 0x10;
    Dis_Data[9] &= 0x10;
	Dis_Data[8] &= 0x10;
}
/***********************************************************
��ʾ����
***********************************************************/
void dis_Minute(void)
{
 uint8 h,l;
    if(Dis_Minute > 99)
	Dis_Minute = 0;
 
 	h = Dis_Minute / 10;//
 	l = Dis_Minute % 10;//
    //Dis_Data[3] &= 0x10;
	//Dis_Data[2] &= 0x10;
	//Dis_Data[3] |= disnumber[h];
	//Dis_Data[2] |= disnumber[l];
    Dis_Data[7] &= 0x10;
	Dis_Data[6] &= 0x10;
	Dis_Data[7] |= disnumber[h];
	Dis_Data[6] |= disnumber[l];
}
/***********************************************************
�رշ���
***********************************************************/
void dis_disMinute(void)
{
   // Dis_Data[3] &= 0x10;
	//Dis_Data[2] &= 0x10;
    Dis_Data[7] &= 0x10;
	Dis_Data[6] &= 0x10;
}
/***********************************************************
��ʾ����
***********************************************************/
void dis_Week(void)
{  
    if((Dis_Week > 6))
	Dis_Week = 0;
	//Dis_Data[1] &= 0x10;
	//Dis_Data[1] |= disnumber[Dis_Week+1];
	Dis_Data[5] &= 0x10;
	Dis_Data[5] |= disnumber[Dis_Week+1];
}
/***********************************************************
�ر�����
***********************************************************/
void dis_disWeek(void)
{
    //Dis_Data[1] &= 0x10;
    Dis_Data[5] &= 0x10;
}
/***********************************************************
��ʾ�ٶ�ͼ��
***********************************************************/
void dis_Speed(void)
{
	//Dis_Data[0] &= 0xee;
	//Dis_Data[6] &= 0xee;
	//Dis_Data[7] &= 0xf9;
	Dis_Data[4] &= 0xee;
	Dis_Data[3] &= 0xee;
	Dis_Data[2] &= 0x9f;
	if((Dis_Speed > 3) || (Dis_Speed == 0))
	Dis_Speed = 1;
	if(Dis_Speed == 1)
	{
	    //Dis_Data[0] |= 0x10;
		//Dis_Data[6] |= 0x1;
	    Dis_Data[4] |= 0x10;
		Dis_Data[3] |= 0x10;
	}
	else
	{
	   if(Dis_Speed == 2)
	   {
	       //Dis_Data[0] |= 1;
		   //Dis_Data[6] |= 0x11;
		   //Dis_Data[7] |= 0x6;
	       Dis_Data[4] |= 1;
		   Dis_Data[3] |= 0x11;
		   Dis_Data[2] |= 0x60;
	   }
	   else
	   {
	       //Dis_Data[0] |= 0x11;
		   //Dis_Data[6] |= 0x11;
		   //Dis_Data[7] |= 0x2;
	       Dis_Data[4] |= 0x11;
		   Dis_Data[3] |= 0x11;
		   Dis_Data[2] |= 0x20;
	   }
	}
}
/***********************************************************
�ر��ٶ�ͼ��
***********************************************************/
void dis_disSpeed(void)
{
	//Dis_Data[0] &= 0x0;
	//Dis_Data[6] &= 0x0;
	//Dis_Data[7] &= 0xf9;
	Dis_Data[4] = 0x0;
	Dis_Data[3] = 0x0;
	Dis_Data[2] &= 0x9f;
}
/***********************************************************
��ʾ��ص���
***********************************************************/
void dis_Power(void)
{
	//Dis_Data[7] &= 0x8f;
	if((Dis_Power > 3))
	Dis_Power = 0;
	//Dis_Data[7] |= dispower[Dis_Power];
	Dis_Data[2] &= 0xf8;
	Dis_Data[2] |= dispower[Dis_Power];
}
/***********************************************************
�رյ�ص���
***********************************************************/
void dis_disPower(void)
{
	//Dis_Data[7] &= 0x8f;
	Dis_Data[2] &= 0xf8;
}
/***********************************************************
���ܣ������ʾ����
***********************************************************/
void clr_display(void)
{
Dis_On = FALSE;            //��ʾ��־��
Dis_ErrOn = FALSE ;    	//����ͼ����
Dis_UvOn = FALSE ;			//uvͼ����
Dis_PrOn = FALSE ;			//ԤԼ������
Dis_AuOn = FALSE ;			//�Զ���ɨ��
Dis_SpOn = FALSE ;			//�ֲ���ɨ��
Dis_HourOn = FALSE ;		//Сʱ��
Dis_MinuteOn = FALSE ;		//������
Dis_ColOn = FALSE ;		//ʱ�����
Dis_WeekOn = FALSE ;		//������
Dis_SpeedOn = FALSE ;		//�ٶ���
Dis_PowerOn = FALSE ;		//���ͼ����
Dis_ChargeOn = FALSE ;		//���ͼ����
Dis_SpeedGo = FALSE;
      
Dis_ErrGlint = FALSE ;    	//����ͼ����
Dis_UvGlint = FALSE ;			//uvͼ����
Dis_PrGlint = FALSE ;			//ԤԼ������
Dis_AuGlint = FALSE ;			//�Զ���ɨ��
Dis_SpGlint = FALSE ;			//�ֲ���ɨ��
Dis_HourGlint = FALSE ;		//Сʱ��
Dis_MinuteGlint = FALSE ;		//������
Dis_ColGlint = FALSE ;		//ʱ�����
Dis_WeekGlint = FALSE ;		//������
Dis_SpeedGlint = FALSE ;		//�ٶ���
Dis_PowerGlint = FALSE ;		//���ͼ����
Dis_ChargeGlint = FALSE ;		//���ͼ����
}
 /*****************************************
 �����ʾ����
 *****************************************/
 void Clr_DisplayData(void)
 {
uint8 i;//ѭ����������	  
	for(i=0; i<10; i++)
    {
	    Dis_Data[i] = 0;
	}
 }
