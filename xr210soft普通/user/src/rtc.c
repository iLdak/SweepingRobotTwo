#include "rtc.h"
#include "time1.h"
void RCC_Configuration(void);
uint16  PreengageTime; //定义预约时间，该时间高字节表示小时，
                       //低字节表示分钟
uint16  PreengageDate; //定义预约天数，低字节八位表示从星期1
                       //到星期7、每一天。
uint32  Rtc_time;      //实时时钟的值

uint32  giv_BatteryCapability;  //电池的容量
uint32  giv_BatteryFDCap;//电池放电的电量
uint8   giv_BatteryChargeForbid; //允许电池大电流充电标志，0为允许，1为不允许。
uint32  giv_UsePowerTime;// 用电时间。
uint8   giv_BatteryActivation;    //电池活化标志
uint32  giv_BatteryChargeTime;    //前一次电池的充电时间
bool watchdog_time;
void init_rtc(void)
{
uint16 t;
uint32 m;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
  //将闹钟中断与外部中断联系起来
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	   
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);    
  
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
   
   PWR_BackupAccessCmd(ENABLE);
    t = BKP_ReadBackupRegister(BKP_DR1);
   if( t != 0x55AA)
   {
   //------------在备份域复位后的初始化实时时钟
       /* Reset Backup Domain */
       BKP_DeInit();
	   //修改启动程序

	    for(m=0; m<0xfffff;m++);
       /* Enable LSE */
        RCC_LSEConfig(RCC_LSE_ON);

       /* Wait till LSE is ready */
       while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) ;

       /* Select LSE as RTC Clock Source */
       RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

       /* Enable RTC Clock */
       RCC_RTCCLKCmd(ENABLE);

       /* Wait for RTC registers synchronization */
       RTC_WaitForSynchro();
 
       /* Wait until last write operation on RTC registers has finished */
       RTC_WaitForLastTask();
  
       /* Set RTC prescaler: set RTC period to 1sec */
       RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
  
       /* Wait until last write operation on RTC registers has finished */
       RTC_WaitForLastTask();
	   
       /* Wait until last write operation on RTC registers has finished */
       RTC_WaitForLastTask();
       /* Change the current time */
       RTC_SetCounter(0);
       /* Wait until last write operation on RTC registers has finished */
       RTC_WaitForLastTask();
	   
       BKP_WriteBackupRegister(BKP_DR1, 0x55AA);//备份域标志
       BKP_WriteBackupRegister(BKP_DR2, 0x0000);//预约时间为没有
       BKP_WriteBackupRegister(BKP_DR3, 0x0000);//预约天数为没有 ,错误代码没有
	   BKP_WriteBackupRegister(BKP_DR4, 0x0007);//速度为1，uv灯亮，充电激活；
	   BKP_WriteBackupRegister(BKP_DR5, 0xe080);//  电池容量0X62E080
	   BKP_WriteBackupRegister(BKP_DR6, 0x62);//
	   BKP_WriteBackupRegister(BKP_DR7, 0x0);//	   放电容量 0 ，充饱标志为0
	   BKP_WriteBackupRegister(BKP_DR8, 0x0);//		  
	   BKP_WriteBackupRegister(BKP_DR9, 0x0);//	    充电时间为0	  
	   BKP_WriteBackupRegister(BKP_DR10,0x0);//	   
   }
   else
   {
    RTC_WaitForSynchro();
   }
    Rtc_time = RTC_GetCounter();
	RTC_WaitForLastTask();
	RTC_WaitForLastTask();
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();		
    PWR_BackupAccessCmd(DISABLE);
							  
	PreengageTime = ReadBKPPreengageTime();	  //读出预约时间
	PreengageDate = ReadBKPPreengageDate();	  //读出预约天数
	
	giv_BatteryCapability = ReadBKPCapability();
	giv_BatteryFDCap = ReadBKPFdCap();
    giv_BatteryChargeForbid = ReadBKPBatteryChargeForbid();
	if(giv_BatteryCapability == 0)
	{
	    giv_BatteryCapability = 0x62e080;
	}
	giv_UsePowerTime = Rtc_time;	
	giv_BatteryActivation = ReadBakBatteryActivation();	  //电池激活标志
	giv_BatteryChargeTime = ReadBakBatteryChargeTime();	  //读上一次的电池充电标志
  /* Enable the RTC Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
  //使能闹钟中断
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQChannel;
  NVIC_Init(&NVIC_InitStructure);
}
/************************************************************************
函数名称：uint8 ReadBKPPreengageDate(void)
函数功能：从备份域寄存器中读出预约的天数,预约天数保存在备份域的第三个寄存
          器的低8位。
返回：返回预约的天数
************************************************************************/
uint8 ReadBKPPreengageDate(void)
{
uint16 t;
    t = BKP_ReadBackupRegister(BKP_DR3);
	t &= 0xff;
	return ((uint8)t); 
}
/************************************************************************
函数名称：uint16 ReadBKPPreengageTime(void)
函数功能：从备份域寄存器中读出预约的时间，预约时间保存在备份域的第二个寄
          存器。
返回：返回预约的时间
************************************************************************/
uint16 ReadBKPPreengageTime(void)
{
    return BKP_ReadBackupRegister(BKP_DR2);
}
/************************************************************************
函数名称：void  WritePreengage(void)
函数功能：保存预约的时间和天数，时间保存在第二个寄存器，天数保存在第三个
          寄存器低8位。
*************************************************************************/
void  WritePreengage(void)
{
uint16 t;
       t = BKP_ReadBackupRegister(BKP_DR3);	                   //|读出原来的数据
	   t &= 0xff00 ;										   //|取出高字节数据
	   t += (PreengageDate & 0xff);							   //|加上保存的预约天数
       PWR_BackupAccessCmd(ENABLE);
       BKP_WriteBackupRegister(BKP_DR2, PreengageTime);        //预约时间
       BKP_WriteBackupRegister(BKP_DR3, t);                    //预约天数
       PWR_BackupAccessCmd(DISABLE);
}
/************************************************************************
功能：写实时时钟时间，将现在的设置时间保存到实时时钟寄存器中
*************************************************************************/
void  WriteRtcTime(void)
{
    PWR_BackupAccessCmd(ENABLE);
    RTC_WaitForLastTask();
	RTC_SetCounter(Rtc_time);
    RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);
}

/************************************************************************
函数名称：void  WriteWorkState(void)
功能：保存工作状态	，即将工作中的UV灯状态位和速度灯以及工作模式保存在第四个
      寄存器中，其中0位标志速度，1位标志UV，高8位标志工作模式
*************************************************************************/
void  WriteWorkState(void)
{
uint16 s;				   
    s = BKP_ReadBackupRegister(BKP_DR4) & 0xfc;				//|将第四个寄存器读出并且将要修改的内容屏蔽
    if(gsv_work_mode.uv != FALSE)							//|写UV标志
	{
	    s |= 2;
	}
	if(gsv_work_mode.speed == 1)							//|写速度标志
	{
	    s |= 1;
	}
    PWR_BackupAccessCmd(ENABLE);
    BKP_WriteBackupRegister(BKP_DR4, (s + mode.mode*256)); //|写工作状态
	PWR_BackupAccessCmd(DISABLE);
}
/************************************************************************
函数名称：bool  ReadBKPUV(void)
函数功能：从备份域寄存器中读出UV状态，在第4个寄存器中的第1位，为0时返回TRUE
          为1时返回FALSE
************************************************************************/
bool  ReadBKPUV(void)
{
bool s;
uint16 t;
    s = FALSE;
	t = BKP_ReadBackupRegister(BKP_DR4) & 0x2;
	if(t != 0)
	{
	    s = TRUE;
	}
	return s;
}
/************************************************************************
函数名称：uint8  ReadBKPSpeed(void)
函数功能：从备份域寄存器中读出速度状态，在第4个寄存器中的第0位，为0时返回2
          为1时返回1
************************************************************************/
uint8  ReadBKPSpeed(void)
{
uint8 s;
uint16 t;
    s = 2;
	t = BKP_ReadBackupRegister(BKP_DR4) & 0x1;
	if(t != 0)
	{
	    s = 1;
	}
	return s;
}
/************************************************************************
函数名称：uint8  ReadBKPMode(void)
函数功能：从备份域寄存器中读出工作模式，在第4个寄存器中的高8位
************************************************************************/
uint8  ReadBKPMode(void)
{
uint16 t;
	t = BKP_ReadBackupRegister(BKP_DR4) & 0xff00;
	t = t >> 8;

	return ((uint8) t);
}
/************************************************************************
功能：保存放电容量和充电饱的标志，电池放电容量的低16位保存在第7个寄存器中
      放电容量的高15位保存在第8个寄存器的低15位，电池充饱标志保存在第8个
	  寄存器的最高位。
*************************************************************************/
void  WriteFDCap(void)
{
uint16 s;
    PWR_BackupAccessCmd(ENABLE);
    s = (uint16)(giv_BatteryFDCap&0xffff);
    BKP_WriteBackupRegister(BKP_DR7, s);//    保存放电容量的低16位
    s = (uint16)(giv_BatteryFDCap/0x10000);	  //|计算放电容量的高15位
    if(giv_BatteryChargeForbid != 0)		 //|保存充电饱
	{
	    s |= 0x8000;
	}
    BKP_WriteBackupRegister(BKP_DR8, s);
	PWR_BackupAccessCmd(DISABLE);
}
/************************************************************************
函数名称：uint32 ReadBKPFdCap(void)
函数功能：从备份域中读出放电容量，保存在第7、8个寄存器中，第7个保存低16位
          第8个寄存器保存高15位。
************************************************************************/
uint32 ReadBKPFdCap(void)
{
uint32 s;
    s = BKP_ReadBackupRegister(BKP_DR7) + (BKP_ReadBackupRegister(BKP_DR8)<<16);
	s &= 0x7fffffff;
	return s;
}
/************************************************************************
函数名称：uint8 ReadBKPBatteryChargeForbid(void)
函数功能：从备份域中读出电池充饱标志，保存在第8个寄存器的第16位
          为0则返回0，为1则返回1；
************************************************************************/
uint8 ReadBKPBatteryChargeForbid(void)
{
uint16 s;
    s = BKP_ReadBackupRegister(BKP_DR8)  &  0x8000;
	if(s == 0)
	{
	    return 0;
	}
	return 1;
}
/************************************************************************
功能：保存电池容量，电池容量保存在第5、6个寄存器中，其中第5个寄存器保存低
      16位，第6个寄存器保存高16位
*************************************************************************/
void  WriteBatteryCapability(void)
{
uint16 s;
    PWR_BackupAccessCmd(ENABLE);
    s = (uint16)(giv_BatteryCapability&0xffff);
    BKP_WriteBackupRegister(BKP_DR5, s);//       
    s = (uint16)(giv_BatteryCapability/0x10000);
    BKP_WriteBackupRegister(BKP_DR6, s);//      
	PWR_BackupAccessCmd(DISABLE);
}

/************************************************************************
函数名称：uint32 ReadBKPCapability(void)
函数功能：从备份域中读出电池容量，保存在第5、6个寄存器中，第5个保存低16位
          第6个寄存器保存高16位。
************************************************************************/
uint32 ReadBKPCapability(void)
{
uint32 s;
    s = BKP_ReadBackupRegister(BKP_DR5) + (BKP_ReadBackupRegister(BKP_DR6)<<16);
	return s;
}
/************************************************************************
功能：定时读取实时时钟的值
************************************************************************/
void ReadRealTime(void)
{
    if(Sec_Two != FALSE)
	{
	    Sec_Two = FALSE; 
		Rtc_time =  RTC_GetCounter();
	}
	if( watchdog_time == TRUE)
	{	
	    IWDG_ReloadCounter();
		watchdog_time = FALSE;
	}
}
/**********************************************************************
功能：写入备份域错误代码 错误代码在第3个寄存器的高8位。
**********************************************************************/
void WriteBakErr(uint16 err)
{
uint16 s;
    s = BKP_ReadBackupRegister(BKP_DR3)& 0xff	 ;
	s |= (err << 8);
    PWR_BackupAccessCmd(ENABLE);
    BKP_WriteBackupRegister(BKP_DR3, s);//预约时间
	PWR_BackupAccessCmd(DISABLE);
}
/**********************************************************************
功能：从备份域读错误代码错误代码在第3个字节的高8位
**********************************************************************/	 
uint16 ReadBakErr(void)
{
    return (BKP_ReadBackupRegister(BKP_DR3) / 256);
}

/**********************************************************************
功能：写入备份域错误代码 错误代码在第3个寄存器的高8位。
**********************************************************************/
void WriteBakBatteryActivation(void)
{
uint16 s;
    s = BKP_ReadBackupRegister(BKP_DR4)& 0xfffb	 ;
	if(giv_BatteryActivation != 0)
	{
	    s |= 4;
	}
    PWR_BackupAccessCmd(ENABLE);
    BKP_WriteBackupRegister(BKP_DR4, s);
	PWR_BackupAccessCmd(DISABLE);
}
/**********************************************************************
功能：从备份域读错误代码错误代码在第3个字节的高8位
**********************************************************************/	 
uint8 ReadBakBatteryActivation(void)
{  
uint16 s;
    s = BKP_ReadBackupRegister(BKP_DR4)& 0x4	 ;
	if(s != 0)
	{
	    return 1;
	}
    return 0;
}

/**********************************************************************
功能：写入备份域电池上一次的充电时间。保存在第9、10个寄存器中
      第九个保存低16位，第10个保存高16位
**********************************************************************/
void WriteBakBatteryChargeTime(void)
{
uint16 s;
    PWR_BackupAccessCmd(ENABLE);
    s = (uint16)(giv_BatteryChargeTime&0xffff);
    BKP_WriteBackupRegister(BKP_DR9, s);//       
    s = (uint16)(giv_BatteryChargeTime/0x10000);
    BKP_WriteBackupRegister(BKP_DR10, s);//      
	PWR_BackupAccessCmd(DISABLE);
}
/**********************************************************************
功能：从备份域读上一次的充电时间，保存在第9、10个寄存器中。
**********************************************************************/	 
uint32 ReadBakBatteryChargeTime(void)
{  
uint32 s;
    s = BKP_ReadBackupRegister(BKP_DR9) + (BKP_ReadBackupRegister(BKP_DR9)<<16)	 ;
    return s;
}




