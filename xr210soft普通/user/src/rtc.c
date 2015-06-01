#include "rtc.h"
#include "time1.h"
void RCC_Configuration(void);
uint16  PreengageTime; //����ԤԼʱ�䣬��ʱ����ֽڱ�ʾСʱ��
                       //���ֽڱ�ʾ����
uint16  PreengageDate; //����ԤԼ���������ֽڰ�λ��ʾ������1
                       //������7��ÿһ�졣
uint32  Rtc_time;      //ʵʱʱ�ӵ�ֵ

uint32  giv_BatteryCapability;  //��ص�����
uint32  giv_BatteryFDCap;//��طŵ�ĵ���
uint8   giv_BatteryChargeForbid; //�����ش��������־��0Ϊ����1Ϊ������
uint32  giv_UsePowerTime;// �õ�ʱ�䡣
uint8   giv_BatteryActivation;    //��ػ��־
uint32  giv_BatteryChargeTime;    //ǰһ�ε�صĳ��ʱ��
bool watchdog_time;
void init_rtc(void)
{
uint16 t;
uint32 m;
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
  //�������ж����ⲿ�ж���ϵ����
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
   //------------�ڱ�����λ��ĳ�ʼ��ʵʱʱ��
       /* Reset Backup Domain */
       BKP_DeInit();
	   //�޸���������

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
	   
       BKP_WriteBackupRegister(BKP_DR1, 0x55AA);//�������־
       BKP_WriteBackupRegister(BKP_DR2, 0x0000);//ԤԼʱ��Ϊû��
       BKP_WriteBackupRegister(BKP_DR3, 0x0000);//ԤԼ����Ϊû�� ,�������û��
	   BKP_WriteBackupRegister(BKP_DR4, 0x0007);//�ٶ�Ϊ1��uv��������缤�
	   BKP_WriteBackupRegister(BKP_DR5, 0xe080);//  �������0X62E080
	   BKP_WriteBackupRegister(BKP_DR6, 0x62);//
	   BKP_WriteBackupRegister(BKP_DR7, 0x0);//	   �ŵ����� 0 ���䱥��־Ϊ0
	   BKP_WriteBackupRegister(BKP_DR8, 0x0);//		  
	   BKP_WriteBackupRegister(BKP_DR9, 0x0);//	    ���ʱ��Ϊ0	  
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
							  
	PreengageTime = ReadBKPPreengageTime();	  //����ԤԼʱ��
	PreengageDate = ReadBKPPreengageDate();	  //����ԤԼ����
	
	giv_BatteryCapability = ReadBKPCapability();
	giv_BatteryFDCap = ReadBKPFdCap();
    giv_BatteryChargeForbid = ReadBKPBatteryChargeForbid();
	if(giv_BatteryCapability == 0)
	{
	    giv_BatteryCapability = 0x62e080;
	}
	giv_UsePowerTime = Rtc_time;	
	giv_BatteryActivation = ReadBakBatteryActivation();	  //��ؼ����־
	giv_BatteryChargeTime = ReadBakBatteryChargeTime();	  //����һ�εĵ�س���־
  /* Enable the RTC Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
  //ʹ�������ж�
  NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQChannel;
  NVIC_Init(&NVIC_InitStructure);
}
/************************************************************************
�������ƣ�uint8 ReadBKPPreengageDate(void)
�������ܣ��ӱ�����Ĵ����ж���ԤԼ������,ԤԼ���������ڱ�����ĵ������Ĵ�
          ���ĵ�8λ��
���أ�����ԤԼ������
************************************************************************/
uint8 ReadBKPPreengageDate(void)
{
uint16 t;
    t = BKP_ReadBackupRegister(BKP_DR3);
	t &= 0xff;
	return ((uint8)t); 
}
/************************************************************************
�������ƣ�uint16 ReadBKPPreengageTime(void)
�������ܣ��ӱ�����Ĵ����ж���ԤԼ��ʱ�䣬ԤԼʱ�䱣���ڱ�����ĵڶ�����
          ������
���أ�����ԤԼ��ʱ��
************************************************************************/
uint16 ReadBKPPreengageTime(void)
{
    return BKP_ReadBackupRegister(BKP_DR2);
}
/************************************************************************
�������ƣ�void  WritePreengage(void)
�������ܣ�����ԤԼ��ʱ���������ʱ�䱣���ڵڶ����Ĵ��������������ڵ�����
          �Ĵ�����8λ��
*************************************************************************/
void  WritePreengage(void)
{
uint16 t;
       t = BKP_ReadBackupRegister(BKP_DR3);	                   //|����ԭ��������
	   t &= 0xff00 ;										   //|ȡ�����ֽ�����
	   t += (PreengageDate & 0xff);							   //|���ϱ����ԤԼ����
       PWR_BackupAccessCmd(ENABLE);
       BKP_WriteBackupRegister(BKP_DR2, PreengageTime);        //ԤԼʱ��
       BKP_WriteBackupRegister(BKP_DR3, t);                    //ԤԼ����
       PWR_BackupAccessCmd(DISABLE);
}
/************************************************************************
���ܣ�дʵʱʱ��ʱ�䣬�����ڵ�����ʱ�䱣�浽ʵʱʱ�ӼĴ�����
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
�������ƣ�void  WriteWorkState(void)
���ܣ����湤��״̬	�����������е�UV��״̬λ���ٶȵ��Լ�����ģʽ�����ڵ��ĸ�
      �Ĵ����У�����0λ��־�ٶȣ�1λ��־UV����8λ��־����ģʽ
*************************************************************************/
void  WriteWorkState(void)
{
uint16 s;				   
    s = BKP_ReadBackupRegister(BKP_DR4) & 0xfc;				//|�����ĸ��Ĵ����������ҽ�Ҫ�޸ĵ���������
    if(gsv_work_mode.uv != FALSE)							//|дUV��־
	{
	    s |= 2;
	}
	if(gsv_work_mode.speed == 1)							//|д�ٶȱ�־
	{
	    s |= 1;
	}
    PWR_BackupAccessCmd(ENABLE);
    BKP_WriteBackupRegister(BKP_DR4, (s + mode.mode*256)); //|д����״̬
	PWR_BackupAccessCmd(DISABLE);
}
/************************************************************************
�������ƣ�bool  ReadBKPUV(void)
�������ܣ��ӱ�����Ĵ����ж���UV״̬���ڵ�4���Ĵ����еĵ�1λ��Ϊ0ʱ����TRUE
          Ϊ1ʱ����FALSE
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
�������ƣ�uint8  ReadBKPSpeed(void)
�������ܣ��ӱ�����Ĵ����ж����ٶ�״̬���ڵ�4���Ĵ����еĵ�0λ��Ϊ0ʱ����2
          Ϊ1ʱ����1
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
�������ƣ�uint8  ReadBKPMode(void)
�������ܣ��ӱ�����Ĵ����ж�������ģʽ���ڵ�4���Ĵ����еĸ�8λ
************************************************************************/
uint8  ReadBKPMode(void)
{
uint16 t;
	t = BKP_ReadBackupRegister(BKP_DR4) & 0xff00;
	t = t >> 8;

	return ((uint8) t);
}
/************************************************************************
���ܣ�����ŵ������ͳ�籥�ı�־����طŵ������ĵ�16λ�����ڵ�7���Ĵ�����
      �ŵ������ĸ�15λ�����ڵ�8���Ĵ����ĵ�15λ����س䱥��־�����ڵ�8��
	  �Ĵ��������λ��
*************************************************************************/
void  WriteFDCap(void)
{
uint16 s;
    PWR_BackupAccessCmd(ENABLE);
    s = (uint16)(giv_BatteryFDCap&0xffff);
    BKP_WriteBackupRegister(BKP_DR7, s);//    ����ŵ������ĵ�16λ
    s = (uint16)(giv_BatteryFDCap/0x10000);	  //|����ŵ������ĸ�15λ
    if(giv_BatteryChargeForbid != 0)		 //|�����籥
	{
	    s |= 0x8000;
	}
    BKP_WriteBackupRegister(BKP_DR8, s);
	PWR_BackupAccessCmd(DISABLE);
}
/************************************************************************
�������ƣ�uint32 ReadBKPFdCap(void)
�������ܣ��ӱ������ж����ŵ������������ڵ�7��8���Ĵ����У���7�������16λ
          ��8���Ĵ��������15λ��
************************************************************************/
uint32 ReadBKPFdCap(void)
{
uint32 s;
    s = BKP_ReadBackupRegister(BKP_DR7) + (BKP_ReadBackupRegister(BKP_DR8)<<16);
	s &= 0x7fffffff;
	return s;
}
/************************************************************************
�������ƣ�uint8 ReadBKPBatteryChargeForbid(void)
�������ܣ��ӱ������ж�����س䱥��־�������ڵ�8���Ĵ����ĵ�16λ
          Ϊ0�򷵻�0��Ϊ1�򷵻�1��
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
���ܣ�������������������������ڵ�5��6���Ĵ����У����е�5���Ĵ��������
      16λ����6���Ĵ��������16λ
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
�������ƣ�uint32 ReadBKPCapability(void)
�������ܣ��ӱ������ж�����������������ڵ�5��6���Ĵ����У���5�������16λ
          ��6���Ĵ��������16λ��
************************************************************************/
uint32 ReadBKPCapability(void)
{
uint32 s;
    s = BKP_ReadBackupRegister(BKP_DR5) + (BKP_ReadBackupRegister(BKP_DR6)<<16);
	return s;
}
/************************************************************************
���ܣ���ʱ��ȡʵʱʱ�ӵ�ֵ
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
���ܣ�д�뱸���������� ��������ڵ�3���Ĵ����ĸ�8λ��
**********************************************************************/
void WriteBakErr(uint16 err)
{
uint16 s;
    s = BKP_ReadBackupRegister(BKP_DR3)& 0xff	 ;
	s |= (err << 8);
    PWR_BackupAccessCmd(ENABLE);
    BKP_WriteBackupRegister(BKP_DR3, s);//ԤԼʱ��
	PWR_BackupAccessCmd(DISABLE);
}
/**********************************************************************
���ܣ��ӱ��������������������ڵ�3���ֽڵĸ�8λ
**********************************************************************/	 
uint16 ReadBakErr(void)
{
    return (BKP_ReadBackupRegister(BKP_DR3) / 256);
}

/**********************************************************************
���ܣ�д�뱸���������� ��������ڵ�3���Ĵ����ĸ�8λ��
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
���ܣ��ӱ��������������������ڵ�3���ֽڵĸ�8λ
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
���ܣ�д�뱸��������һ�εĳ��ʱ�䡣�����ڵ�9��10���Ĵ�����
      �ھŸ������16λ����10�������16λ
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
���ܣ��ӱ��������һ�εĳ��ʱ�䣬�����ڵ�9��10���Ĵ����С�
**********************************************************************/	 
uint32 ReadBakBatteryChargeTime(void)
{  
uint32 s;
    s = BKP_ReadBackupRegister(BKP_DR9) + (BKP_ReadBackupRegister(BKP_DR9)<<16)	 ;
    return s;
}




