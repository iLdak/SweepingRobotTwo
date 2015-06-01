#include "function.h"
#include "hardware.h"
#include "io_driver.h"
#include "key.h"
#include "lcdkey.h"
#include "power.h"
#include "ring.h"
#include "test.h"
#include "A7105.h"
#include "wall_earth.h"
#include "hwincept.h"
#include "ad.h"
#include "pwm.h"
#include "time1.h"
#include "rtc.h"
#include "flash.h"
#include "cease.h" 
#include "sweep.h" 
#include "docking.h"
#include "spot.h"
#include "tele.h"
#include "chargeing.h"
#include "sleep.h"
#include "err.h"
#include "watchdog.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

ErrorStatus HSEStartUpStatus;
GPIO_TypeDef *gpiod;
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);
void  Init_System(void);
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
#ifdef DEBUG
  debug();
#endif			
  gpiod =GPIOD;
  Init_System();
  Init_Mode();
  Rtc_time = RTC_GetCounter();
  /*************����************************/
  
  /*****************************************/
  while (1)
  {
     AccountCapability();
	 ReadRealTime();
     Action_Mode();	//������ģʽ
	 Beep_Driver(); //������������
	 AutoReadKey(); //�Զ���ȡ����
	 AutoDisplay(); //�Զ���ʾ 
     judge_charge(); //�Զ��ж��Ƿ��г���ź�
     sampling_temp_voltage();//�Զ�����������	  
	 A7105_rx();	         //�Զ�����ң�����ź�
	 read_wallearth();		 //�Զ�����ǽ��͵ؼ�����
  }
}

/*******************************************************************************
��������Init_System
���ܣ���ʼ������ϵͳ
���룺��
�������
���أ���
*******************************************************************************/
void  Init_System(void)
{
    RCC_Configuration();///////////��ʼ��ϵͳ�ľ���������ֲ��Ҫ�޸�
    Init_WatchDog();
	NVIC_Configuration();//////////��ʼ��ϵͳ���жϣ�������ֲ��Ҫ�޸�
	Init_Hardware();    ///////////��ʼ��ϵͳ��Ӳ����������ֲ��Ҫ�޸�
	init_hwincept();	///////////��ʼ��������ճ���
	Init_PWM();
	init_ad();
	Init_Io_Driver();
	Init_Lcd();
	init_key();
	Init_Ring();
	init_power ();
	init_wallearth();
	init_rtc();			 
	init_time();        ///////////��ʼ��ʱ�Ӷ�ʱ��
	initRF();
	GPIO_PinLockConfig(GPIOA,GPIO_Pin_All);	  
	GPIO_PinLockConfig(GPIOB,GPIO_Pin_All);
	GPIO_PinLockConfig(GPIOC,0XEFFF);	
	GPIO_PinLockConfig(GPIOD,GPIO_Pin_All);
	GPIO_PinLockConfig(GPIOE,GPIO_Pin_All);	
	
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
//  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
//  HSEStartUpStatus = RCC_WaitForHSEStartUp();

//  if(HSEStartUpStatus == SUCCESS)
//  {
    /* Enable Prefetch Buffer */
//    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
//    FLASH_SetLatency(FLASH_Latency_2);
 	
    /* HCLK = SYSCLK */
//    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
//    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
//    RCC_PCLK1Config(RCC_HCLK_Div1);

    /* PLLCLK = 8MHz/2 * 9 = 36 MHz */
//    RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);

    /* Enable PLL */ 
//    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
//    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
//    {
//    }

    /* Select PLL as system clock source */
//    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
//    while(RCC_GetSYSCLKSource() != 0x08)
//    {
//    }
//  }
//  else
  {
     RCC_HSEConfig(RCC_HSE_OFF);
     FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	 FLASH_SetLatency(FLASH_Latency_2);
	 RCC_HCLKConfig(RCC_SYSCLK_Div1); 
	 RCC_PCLK2Config(RCC_HCLK_Div1); 
	 RCC_PCLK1Config(RCC_HCLK_Div1);
	 RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);
	 RCC_PLLCmd(ENABLE);
	 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
	/* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
