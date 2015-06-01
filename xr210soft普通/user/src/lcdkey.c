#include  "lcdkey.h"
#include  "time1.h"
#include  "hardware.h"

//======================私有定义===========================
//======================全局变量===========================
uint8 Dis_Data[10] ; 	//显示的数据亮
bool Dis_On;            //显示标志。
bool Dis_ErrOn ;    	//错误图标亮
bool Dis_UvOn ;			//uv图标亮
bool Dis_PrOn ;			//预约天数亮
bool Dis_AuOn ;			//自动清扫亮
bool Dis_SpOn ;			//局部清扫亮
bool Dis_HourOn ;		//小时亮
bool Dis_MinuteOn ;		//分钟亮
bool Dis_ColOn ;		//时间点亮
bool Dis_WeekOn ;		//星期亮
bool Dis_SpeedOn ;		//速度亮
bool Dis_PowerOn ;		//电池图标亮
bool Dis_ChargeOn ;		//充电图标亮
bool Dis_SpeedGo;       //轮子图标转动

bool Dis_ErrGlint ;		//错误图标闪烁
bool Dis_UvGlint ;		//uv图标闪烁
bool Dis_PrGlint ;		//预约天数闪烁
bool Dis_AuGlint ;		//自动清扫闪烁
bool Dis_SpGlint ;		//局部清扫闪烁
bool Dis_HourGlint ;	//小时闪烁
bool Dis_MinuteGlint ;	//分钟闪烁
bool Dis_ColGlint ;		//时间点闪烁
bool Dis_WeekGlint ;	//星期闪烁
bool Dis_SpeedGlint ;	//速度闪烁
bool Dis_PowerGlint ;	//电池图标闪烁
bool Dis_ChargeGlint ;	//充电图标闪烁

uint8 Dis_Preengage;	//预约内容
uint8 Dis_Hour; 		//显示的小时
uint8 Dis_Minute;  		//显示的分钟
uint8 Dis_Week;			//显示的星期内容  范围 1——7
uint8 Dis_Speed;		//显示的速度，范围1——3
uint8 Dis_Power;		//显示的电源电量 ，范围0——3

bool dis_updata;        //显示更新标志

//========================私有变量==================================
const uint8 disnumber[16] = {0xaf,0x06,0x6d,0x4f,0xc6,0xcb,0xeb,0x0e,
                             0xef,0xce,0xee,0xe3,0xa9,0x67,0xe9,0xe8};//数字解码
const uint8 disspeed[6] = {0x80,0x01,
                           0x61,0x11,
						   0xa1,0x11};  //速度数字解码
const uint8 dispower[4] = {0x0,0x4,0x6,0x7}; 	

bool Action_display;  // 当前的显示状态
//=======================全局函数===================================

void dis_Preengage (void);//；；
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
//=====================局部函数定义=======================
/*********************************************************
函数：void  AutoDisplay(void)
输入：无
输出：无
说明：根据显示寄存器内容显示，显示屏0.1秒更新一次
      闪烁时的频率为1秒。
	  轮盘转动更新频率为0.6秒、0.4秒、0.2秒更新
*********************************************************/
void  AutoDisplay(void)
{
  static uint8 number = 0;
  uint32 l;
  if(Dis_On == TRUE)     //允许显示
  {
	  if(dis_updata == TRUE)   //到了更新显示屏
	  {
	  	dis_updata = FALSE;
		if(Action_display == FALSE)
		{
		    Action_display = TRUE;
            lcdkey_bgon();     //打开背光
	        lcdkey_dison();    //打开显示
		}
		number ++;			 
	   // Dis_Data[7] |= 1; 
	    Dis_Data[2] |= 0X10;
		if(number == 20)   //闪烁的1秒定时
		{
		   number =0;
		}
		if(( Dis_SpeedOn  != FALSE)&&(Dis_SpeedGlint == FALSE)) //显示车轮
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
				 //Dis_Data[6] |= 0x2;    //单段*3  显示跳动
				 //Dis_Data[0] |= 0x84;    //单段*3  显示跳动
				 Dis_Data[3] |= 0x20;    //单段*3  显示跳动
				 Dis_Data[4] |= 0x84;    //单段*3  显示跳动
				 break;
			     case 1:
				 //Dis_Data[6] |= 0x84;    //单段*3  显示跳动
				 //Dis_Data[0] |= 0x2;    //单段*3  显示跳动
				 Dis_Data[3] |= 0x48;    //单段*3  显示跳动
				 Dis_Data[4] |= 0x2;    //单段*3  显示跳动
				 break;
			     case 2:
				 //Dis_Data[6] |= 0x48;    //单段*3  显示跳动
				 //Dis_Data[0] |= 0x20;    //单段*3  显示跳动
				 Dis_Data[3] |= 0x84;    //单段*3  显示跳动
				 Dis_Data[4] |= 0x20;    //单段*3  显示跳动
				 break;
			     case 3:
				 //Dis_Data[6] |= 0x20;    //单段*3  显示跳动
				 //Dis_Data[0] |= 0x48;    //单段*3  显示跳动
				 Dis_Data[3] |= 0x2;    //单段*3  显示跳动
				 Dis_Data[4] |= 0x48;    //单段*3  显示跳动
				 break;
			}
		}	
		if (number < 10)    //在前半秒内
		{
		   if( Dis_ErrOn == FALSE)  //错误标志
		   {
		      //Dis_Data[4] &= 0xef;
		      Dis_Data[8] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[4] |= 0x10;
		   	  Dis_Data[8] |= 0x10;
		   }
		   if( Dis_UvOn == FALSE)//UV标志
		   {
		      //Dis_Data[5] &= 0xef;
		      Dis_Data[9] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[5] |= 0x10;
		   	  Dis_Data[9] |= 0x10;
		   }
		   if( Dis_PrOn == FALSE) //预约天数
		   {
		     dis_disPreengage();
		   }
		   else
		   {
		   	  dis_Preengage();
		   }
		   if( Dis_AuOn == FALSE) //自动显示图标
		   {
		      //Dis_Data[2] &= 0xef;
		      Dis_Data[6] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[2] |= 0x10;
		   	  Dis_Data[6] |= 0x10;
		   }
		   if( Dis_SpOn == FALSE)//定点图标
		   {
		      //Dis_Data[1] &= 0xef;
		      Dis_Data[5] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[1] |= 0x10;
		   	  Dis_Data[5] |= 0x10;
		   }
		   if( Dis_HourOn == FALSE)//显示小时
		   {
		      dis_disHour();
		   }
		   else
		   {
		   	  dis_Hour();
		   }
		   if( Dis_MinuteOn  == FALSE)//显示分钟
		   {
		      dis_disMinute();
		   }
		   else
		   {
		   	  dis_Minute();
		   }
		   if( Dis_ColOn == FALSE)//显示时间点图标
		   {
		      //Dis_Data[3] &= 0xef;
		      Dis_Data[7] &= 0xef;
		   }
		   else
		   {
		   	  //Dis_Data[3] |= 0x10;
		   	  Dis_Data[7] |= 0x10;
		   }
		   if( Dis_WeekOn  == FALSE)//显示星期
		   {
		      dis_disWeek();
		   }
		   else
		   {
		   	  dis_Week();
		   }
		   if( Dis_SpeedOn  == FALSE)//显示速度
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
		   if( Dis_PowerOn  == FALSE)//显示电池
		   {
		      dis_disPower();
		   }
		   else
		   {
		   	  dis_Power();
		   }
		   if( Dis_ChargeOn == FALSE)  //充电标志
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
		////////////////////////闪烁的下半秒///////////////////
		else
		{
		   if( Dis_ErrGlint != FALSE)  //错误标志
		   {
		      //Dis_Data[4] &= 0xef;
		      Dis_Data[8] &= 0xef;
		   }
		   if( Dis_UvGlint != FALSE)//UV标志
		   {
		      //Dis_Data[5] &= 0xef;
		      Dis_Data[9] &= 0xef;
		   }
		   if( Dis_PrGlint != FALSE) //预约天数
		   {
		     dis_disPreengage();
		   }
		   if( Dis_AuGlint != FALSE) //自动显示图标
		   {
		      //Dis_Data[2] &= 0xef;
		      Dis_Data[6] &= 0xef;
		   }
		   if( Dis_SpGlint != FALSE)//定点图标
		   {
		      //Dis_Data[1] &= 0xef;
		      Dis_Data[5] &= 0xef;
		   }
		   if( Dis_HourGlint != FALSE)//显示小时
		   {
		      dis_disHour();
		   }
		   if( Dis_MinuteGlint  != FALSE)//显示分钟
		   {
		      dis_disMinute();
		   }
		   if( Dis_ColGlint != FALSE)//显示时间点图标
		   {
		      //Dis_Data[3] &= 0xef;
		      Dis_Data[7] &= 0xef;
		   }
		   if( Dis_WeekGlint  != FALSE)//显示星期
		   {
		      dis_disWeek();
		   }
		   if( Dis_SpeedGlint  != FALSE)//显示速度
		   {
		      dis_disSpeed();
		   }
		   if( Dis_PowerGlint  != FALSE)//显示电池
		   {
		      dis_disPower();
		   }
		   if( Dis_ChargeGlint != FALSE)  //充电标志
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
程序名称：delay(uint32 delaynumbe)
描述：延时子程序
输入：delaynumber   延时时间的长短
输出：无
**********************************************************/
void delay(uint32 delaynumber)
{
  uint32 l;
  for(l=0; l<delaynumber; l++)
  ;
}
/***********************************************************
程序名字：void Init_Lcd(void)
描述：初始化液晶显示器芯片TM1723
输入：无
输出：无
***********************************************************/
void Init_Lcd(void)
{
  Set_LCD_Power();
  Set_LCD_Stb();
  Set_LCD_BG();
  Set_LCD_Data();
  Set_LCD_Clk();
  //全部关闭接口
  delay(100);
  Reset_LCD_Power();
  delay(100);
  Lcd_WriteCommand(0x1c6);//设置为普通模式
  Lcd_WriteCommand(0X40);
  Lcd_WriteCommand(2);
  Lcd_WriteCommand(0X50);
  Lcd_WriteCommand(0x30);//系统时钟源为内部rc振荡器
  clr_display();
  Action_display = FALSE;
}
/*************************************************************
程序名称：lcdkey_write
描述：向LCD控制芯片TM1723写入一个字节数据
输入：data   要写入的内容
输出：无
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
程序名称：cdkey_dison
描述：打开显示
输入：无
输出：无
**************************************************************/
void  lcdkey_dison(void)
{
  Lcd_WriteCommand(0x6);
}
/**************************************************************
程序名称：lcdkey_disoff
描述：关闭显示
输入：无
输出：无
**************************************************************/
void  lcdkey_disoff(void)
{
  Lcd_WriteCommand(0x4);
}
/**************************************************************
程序名称：cdkey_bgon
描述：打开背光
输入：无
输出：无
**************************************************************/
void  lcdkey_bgon(void)
{
  Reset_LCD_BG();
}
/**************************************************************
程序名称：cdkey_bgoff
描述：关闭背光
输入：无
输出：无
**************************************************************/
void  lcdkey_bgoff(void)
{
  Set_LCD_BG();
}


/**************************************************************
程序名称：lcdkey_disdata
描述：在指定位置显示数据
输入：无
输出：addr   写入现实区位置,地址不能大于15
      str    要写入的数据地址
	  number 要写入的字节数
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
显示预约图标
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
	 	if((Dis_Preengage&0x1) != 0)  //显示星期1
		{
		 	//Dis_Data[8] |= 0x44;
		 	Dis_Data[1] |= 0x44;
		}
	 	if((Dis_Preengage&0x2) != 0)  //显示星期2
		{
		 	//Dis_Data[8] |= 0x22;
		 	Dis_Data[1] |= 0x22;
		}
	 	if((Dis_Preengage&0x4) != 0)  //显示星期3
		{
		 	//Dis_Data[8] |= 0x9;
		 	Dis_Data[1] |= 0x90;
		}
	 	if((Dis_Preengage&0x8) != 0)  //显示星期4
		{
		 	//Dis_Data[8] |= 0x90;
		 	Dis_Data[1] |= 0x9;
		}
	 	if((Dis_Preengage&0x10) != 0)  //显示星期5
		{
		 	//Dis_Data[9] |= 0x11;
		 	Dis_Data[0] |= 0x11;
		}
	 	if((Dis_Preengage&0x20) != 0)  //显示星期6
		{
		 	//Dis_Data[9] |= 0x88;
		 	Dis_Data[0] |= 0x88;
		}
	 	if((Dis_Preengage&0x40) != 0)  //显示星期7
		{
		 	//Dis_Data[9] |= 0x22;
		 	Dis_Data[0] |= 0x22;
		}	  
	 	if((Dis_Preengage&0x80) != 0)  //每天
		{
		 	//Dis_Data[9] |= 0x22;
		 	Dis_Data[0] |= 0x44;
		}
	}
  } 
  
}

/************************************************************
关闭预约图标
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
显示小时
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
关闭小时
***********************************************************/
void dis_disHour(void)
{
    //Dis_Data[5] &= 0x10;
	//Dis_Data[4] &= 0x10;
    Dis_Data[9] &= 0x10;
	Dis_Data[8] &= 0x10;
}
/***********************************************************
显示分钟
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
关闭分钟
***********************************************************/
void dis_disMinute(void)
{
   // Dis_Data[3] &= 0x10;
	//Dis_Data[2] &= 0x10;
    Dis_Data[7] &= 0x10;
	Dis_Data[6] &= 0x10;
}
/***********************************************************
显示星期
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
关闭星期
***********************************************************/
void dis_disWeek(void)
{
    //Dis_Data[1] &= 0x10;
    Dis_Data[5] &= 0x10;
}
/***********************************************************
显示速度图标
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
关闭速度图标
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
显示电池电量
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
关闭电池电量
***********************************************************/
void dis_disPower(void)
{
	//Dis_Data[7] &= 0x8f;
	Dis_Data[2] &= 0xf8;
}
/***********************************************************
功能：清除显示内容
***********************************************************/
void clr_display(void)
{
Dis_On = FALSE;            //显示标志。
Dis_ErrOn = FALSE ;    	//错误图标亮
Dis_UvOn = FALSE ;			//uv图标亮
Dis_PrOn = FALSE ;			//预约天数亮
Dis_AuOn = FALSE ;			//自动清扫亮
Dis_SpOn = FALSE ;			//局部清扫亮
Dis_HourOn = FALSE ;		//小时亮
Dis_MinuteOn = FALSE ;		//分钟亮
Dis_ColOn = FALSE ;		//时间点亮
Dis_WeekOn = FALSE ;		//星期亮
Dis_SpeedOn = FALSE ;		//速度亮
Dis_PowerOn = FALSE ;		//电池图标亮
Dis_ChargeOn = FALSE ;		//充电图标亮
Dis_SpeedGo = FALSE;
      
Dis_ErrGlint = FALSE ;    	//错误图标亮
Dis_UvGlint = FALSE ;			//uv图标亮
Dis_PrGlint = FALSE ;			//预约天数亮
Dis_AuGlint = FALSE ;			//自动清扫亮
Dis_SpGlint = FALSE ;			//局部清扫亮
Dis_HourGlint = FALSE ;		//小时亮
Dis_MinuteGlint = FALSE ;		//分钟亮
Dis_ColGlint = FALSE ;		//时间点亮
Dis_WeekGlint = FALSE ;		//星期亮
Dis_SpeedGlint = FALSE ;		//速度亮
Dis_PowerGlint = FALSE ;		//电池图标亮
Dis_ChargeGlint = FALSE ;		//充电图标亮
}
 /*****************************************
 清除显示数据
 *****************************************/
 void Clr_DisplayData(void)
 {
uint8 i;//循环计数变量	  
	for(i=0; i<10; i++)
    {
	    Dis_Data[i] = 0;
	}
 }
