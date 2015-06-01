#include "stm32f10x_it.h"

#include "time1.h"
#include "ad.h"
#include "lcdkey.h"
#include "rtc.h"
#include "key.h"
#include "ring.h"
#include "power.h"
#include "wall_earth.h"
#include "A7105.h"
#include "hwincept.h"
#include "sleep.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : NMIException
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMIException(void)
{
}

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFaultException(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  //while (1)
  //{
  //}
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManageException(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  //while (1)
  //{
  //}
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFaultException(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  //while (1)
  //{
  //}
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : UsageFaultException
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFaultException(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  //while (1)
  //{
  //}
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : DebugMonitor
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMonitor(void)
{
}

/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVCHandler(void)
{
}

/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSVC(void)
{
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTickHandler(void)
{
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
/*  Rtc_time = RTC_GetCounter();
  if(Rtc_time >= 604800)
  {
    Rtc_time = Rtc_time % 604800;
	PWR_BackupAccessCmd(ENABLE);
    RTC_WaitForLastTask();
	RTC_SetCounter(Rtc_time);
    RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);
  }
  Sec  = TRUE;
  RTC_ClearITPendingBit(RTC_IT_SEC);*/
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
uint8 A7105_rx_data[8],a;
uint32 sign_id;
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);	  
		if(gbv_a7105_reset == FALSE)
		{
	        RxPacket(A7105_rx_data);
			sign_id =  ((A7105_rx_data[1]<<24)+(A7105_rx_data[2]<<16)+(A7105_rx_data[3]<<8)+A7105_rx_data[4]);
	       // if(((Telecontrol_ID == sign_id)||(sign_id == 0xffffffff))
			if((Telecontrol_ID == sign_id)
		        &&(A7105_rx_data[0] == 0x55))//是本机的遥控并且是发送按键
	        {
     	         //数据校验
	             a=(A7105_rx_data[0]+A7105_rx_data[1]+A7105_rx_data[2]+A7105_rx_data[3]+A7105_rx_data[4]+A7105_rx_data[5]+A7105_rx_data[6]);
                 if(A7105_rx_data[7]==a)//接收正确 
	             {
			         if((A7105_rx_data[6] & 0x10) != 0)
				     {
				         gsv_yaokong.effect = 0;
				     }
				     else
				     {
				         if((gsv_yaokong.effect == 0) || (gsv_yaokong.old_key != A7105_rx_data[6]))
			             {
			                 gsv_yaokong.effect = 1;
			                 gsv_yaokong.sign = 1;
				             gsv_yaokong.key = A7105_rx_data[6];
						     gsv_yaokong.old_key = gsv_yaokong.key ;
							 if(sign_id == 0xffffffff)
							 {
							     gsv_yaokong.test = 1;
							 }
							 else
							 {
							     gsv_yaokong.test = 0;
							 }
			             }
					     gsv_yaokong.effect_time = giv_sys_time;
					 
				     }
	   	        }
			}
		}
		init_a7105();
        StrobeCmd(CMD_RFR);        //清除RX FIFO
        StrobeCmd(CMD_RX);         //进入接收模式  
        EXTI_ClearITPendingBit(EXTI_Line0);	  
    }
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
	    ExitInput = TRUE;
        EXTI_ClearITPendingBit(EXTI_Line1);	  
	}
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
	    ExitInput = TRUE;
        EXTI_ClearITPendingBit(EXTI_Line2);	  
	}
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
	    ExitInput = TRUE;
        EXTI_ClearITPendingBit(EXTI_Line3);	  
	}
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel1_IRQHandler
* Description    : This function handles DMA Stream 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel2_IRQHandler
* Description    : This function handles DMA Stream 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel3_IRQHandler
* Description    : This function handles DMA Stream 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel4_IRQHandler
* Description    : This function handles DMA Stream 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel5_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel6_IRQHandler
* Description    : This function handles DMA Stream 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel7_IRQHandler
* Description    : This function handles DMA Stream 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
	 ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
	 ADC_ready = TRUE;
	 if(((DMA_Channel1->CNDTR)&0XF) != 0)
	 {
		 ReInitAd();
	 }
	 else
	 {			 
	     ADC_SoftwareStartConvCmd(ADC1, ENABLE);  
	 }	   
	 ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_RX1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SCE_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and Commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
static uint8 r_step = 0;
static uint8 l_step = 0;   


TIM_ClearITPendingBit(TIM2, TIM_IT_Update );
giv_sys_time ++;
POWER_ready = TRUE;           //电池充电时间
if((giv_sys_time % 2) == 0)
{
  VOL_TEMP_ready = TRUE;      //马达电流采样   
}
if((giv_sys_time % 500) == 0)
{
    dis_updata = TRUE;        //显示更新时间
}
if((giv_sys_time % 100) == 0)
{
    rap_time = TRUE;          //左右轮速度调节时间
	gbv_yaokong_time = TRUE;
}
if((giv_sys_time %10) == 0)
{
    key_time = TRUE;                 //按键时间	   
    wall_earth_time = TRUE;          //按键时间	
    if(ADC_ready	==FALSE)
    {
       ReInitAd();     
    }
    ADC_ready = FALSE;
}
if((giv_sys_time % 600000) == 0)     //分钟标志
{
    gbv_minute = TRUE;
}
if((giv_sys_time % 10000) == 0)     //秒标志
{
    Sec  = TRUE;	 
}	 
if((giv_sys_time % 20000) == 0)     //两秒标志
{
    Sec_Two  = TRUE;   
    watchdog_time = TRUE;
}
if(Action_hwincept == TRUE)
{
    read_hw_data(&l_hw);
    read_hw_data(&m_hw);
    read_hw_data(&r_hw);
    read_hw_data(&up_hw);
	read_dummy_wall(&dummy_wall);
}

 switch (r_step)
 {
    case 0:
	    if(GPIO_ReadInputDataBit(GPIOE, R_SPEED) == Bit_RESET)
		{
		   if((giv_sys_time - r_ring.dq_time) > 3)
		    {
			  r_ring.length++; 
			  r_ring.dq_time = giv_sys_time;
			}
		   r_step = 1;	
		}
		break;
	case 1:
	    if(GPIO_ReadInputDataBit(GPIOE, R_SPEED) != Bit_RESET)
		{
		   r_step = 0;	
		}
	   break;
    default :
	   r_step = 0;	  
 } 
 switch (l_step)
 {
    case 0:
	    if(GPIO_ReadInputDataBit(GPIOD, L_SPEED) == Bit_RESET)
		{
		   if((giv_sys_time - l_ring.dq_time) > 3)
		    {
			  l_ring.length++; 
			  l_ring.dq_time = giv_sys_time;
			}
		   l_step = 1;	
		}
		break;
	case 1:
	    if(GPIO_ReadInputDataBit(GPIOD, L_SPEED) != Bit_RESET)
		{
		   l_step = 0;	
		}
	   break;
    default :
	   l_step = 0;	  
 } 	
 
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
 if(EXTI_GetITStatus(EXTI_Line11) != RESET)
  {
      ExitInput = TRUE;
      EXTI_ClearITPendingBit(EXTI_Line11);
  }
 if(EXTI_GetITStatus(EXTI_Line12) != RESET)
  {
      ExitInput = TRUE;
      EXTI_ClearITPendingBit(EXTI_Line12);
  }
}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
  if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
  {
	  RTC_ClearITPendingBit(RTC_IT_ALR);
      EXTI_ClearITPendingBit(EXTI_Line17);
  }
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
