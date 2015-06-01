#include "hardware.h"

void Init_Hardware (void);
void Set_Beep(void);
void Set_Dust_Motor(void);
void Set_Earth_Send(void);
void Set_HW_Power(void);
void Set_L_BackOnOff(void);
void Set_L_FrontOnOff(void);
void Set_LCD_BG(void);
void Set_LCD_Clk(void);
void Set_LCD_Data(void);
void Set_LCD_Power(void);
void Set_LCD_Stb(void);
void Set_M_Motor(void);
void Set_R_BackOnOff(void);
void Set_R_FrontOnOff(void);
void Set_S_Motor(void);
void Set_Speed_Send(void);
void Set_UV(void);
void Set_Wall_Send(void);						   
void Reset_Beep(void);
void Reset_Dust_Motor(void);
void Reset_Earth_Send(void);
void Reset_HW_Power(void);
void Reset_L_BackOnOff(void);
void Reset_L_FrontOnOff(void);
void Reset_LCD_BG(void);
void Reset_LCD_Clk(void);
void Reset_LCD_Data(void);
void Reset_LCD_Power(void);
void Reset_LCD_Stb(void);
void Reset_M_Motor(void);
void Reset_R_BackOnOff(void);
void Reset_R_FrontOnOff(void);
void Reset_S_Motor(void);
void Reset_Speed_Send(void);
void Reset_UV(void);
void Reset_Wall_Send(void);
uint8 Read_Charge_Dc(void);
uint8 Read_Charge_Seat(void);
uint8 Read_Front_Wheel(void);
uint8 Read_Key1(void);
uint8 Read_Key2(void);
uint8 Read_Key3(void);
uint8 Read_L_Bump(void);
uint8 Read_L_HW(void);
uint8 Read_L_LIDI(void);
uint8 Read_L_Speed(void);
uint8 Read_M_HW(void);
uint8 Read_R_Bump(void);
uint8 Read_R_HW(void);
uint8 Read_R_LIDI(void);
uint8 Read_R_Speed(void);
uint8 Read_S_Current(void);
uint8 Read_Up_HW(void);
uint8 Read_LCD_Data(void);
///////////////////////˽�к���////////////////////////////////////
///////////////////////����ʵ��////////////////////////////////////
void Init_Hardware (void)
{
GPIO_InitTypeDef GPIO_InitStructure; //�ܽŽṹ

RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB 
                     | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD 
					 | RCC_APB2Periph_GPIOE , ENABLE);
			 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//////////////////////////PA�������������ɨ���///////////////////
  GPIO_InitStructure.GPIO_Pin = S_MOTOR_DRIVER;  //��ɨ���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  GPIO_ResetBits(GPIOA, S_MOTOR_DRIVER);
  GPIO_Init(GPIOA, &GPIO_InitStructure);
											
/////////////////////////PB�������������������ź�////////////
  GPIO_InitStructure.GPIO_Pin = M_MOTOR_ZPMOS | UV_DRIVER;           //���������
  GPIO_ResetBits(GPIOB, M_MOTOR_ZPMOS);		
  GPIO_ResetBits(GPIOB, UV_DRIVER);	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
			  								
/////////////////////////PC�������������ɨ�������������������//	
  GPIO_InitStructure.GPIO_Pin = M_MOTOR_FPMOS | DUST_MOTOR_DRIVER;
  GPIO_ResetBits(GPIOC, M_MOTOR_FPMOS); 
  GPIO_ResetBits(GPIOC, DUST_MOTOR_DRIVER);	   
  GPIO_Init(GPIOC, &GPIO_InitStructure);

/////////////////////////PD�����������������////////////////////	 
  GPIO_InitStructure.GPIO_Pin = BEEP;	 
  GPIO_ResetBits(GPIOD, BEEP);  
  GPIO_Init(GPIOD, &GPIO_InitStructure);
		  		  
/////////////////////////PA�ڿ�©�����ǽ�췢�����//////////////

  GPIO_InitStructure.GPIO_Pin = WALL_SEND ;    //ǽ�췢��ܿ���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_SetBits(GPIOA, WALL_SEND);              //�رշ���
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  	  
/////////////////////////PB�ڿ�©�����Һ��ָ�����/////////////	
  GPIO_InitStructure.GPIO_Pin = LCD_STB;	 
  GPIO_SetBits(GPIOB, LCD_STB);  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
								 
/////////////////////////PD�ڿ�©�����
  GPIO_InitStructure.GPIO_Pin = LCD_POWER | LCD_CLK | LCD_DATA 
                              | LCD_BG | SPEED_SEND | EARTH_SEND; 	 
 // GPIO_WriteBit(GPIOD,GPIO_InitStructure.GPIO_Pin,Bit_SET);		
  GPIOD->BSRR = GPIO_InitStructure.GPIO_Pin;					    
  GPIO_Init(GPIOD, &GPIO_InitStructure);
							 
/////////////////////////PE�ڿ�©���	 
  GPIO_InitStructure.GPIO_Pin = L_FRONTONOFF | L_BACKONOFF | R_FRONTONOFF
                              | R_BACKONOFF	| HW_POWER | M_MOTOR_ZNMOS | M_MOTOR_FNMOS;	 
  //GPIO_WriteBit(GPIOE,GPIO_InitStructure.GPIO_Pin,Bit_SET);	
  GPIOE->BSRR = GPIO_InitStructure.GPIO_Pin;												     
  GPIO_Init(GPIOE, &GPIO_InitStructure);

/////////////////////////PB�ڸ������룺	
  GPIO_InitStructure.GPIO_Pin = FRONT_WHEEL;	   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   										  
			  	
/////////////////////////PC�ڸ������룺
  GPIO_InitStructure.GPIO_Pin = S_CURRENT;	   
  GPIO_Init(GPIOC, &GPIO_InitStructure);

/////////////////////////PD�ڸ������룺	
  GPIO_InitStructure.GPIO_Pin = KEY1 | KEY2 | KEY3 | L_SPEED | L_LIDI
                              | CHARGE_DC | CHARGE_SEAT; 	   
  GPIO_Init(GPIOD, &GPIO_InitStructure);

/////////////////////////PE�ڸ������룺
  GPIO_InitStructure.GPIO_Pin = R_SPEED | R_LIDI | L_HW | M_HW
                                | R_HW | UP_HW | L_BUMP | R_BUMP;	   
  GPIO_Init(GPIOE, &GPIO_InitStructure);

}
 
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_S_Motor(void)
{           
  GPIO_SetBits(GPIOA, S_MOTOR_DRIVER);
}
//===========================
void Set_M_Motor(void)
{           
  Reset_M_ZNMOS();
  Set_M_ZPMOS();

}
void Reset_M_Motor(void)
{           
  Set_M_ZNMOS();
  Reset_M_ZPMOS();
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_UV(void)
{           
  GPIO_SetBits(GPIOB, UV_DRIVER);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_M_FPMOS(void)
{           
  GPIO_SetBits(GPIOC, M_MOTOR_FPMOS );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_M_ZPMOS(void)
{           
  GPIO_SetBits(GPIOB, M_MOTOR_ZPMOS );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_M_FNMOS(void)
{           
  GPIO_SetBits(GPIOE, M_MOTOR_FNMOS );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_M_ZNMOS(void)
{           
  GPIO_SetBits(GPIOE, M_MOTOR_ZNMOS );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_Dust_Motor(void)
{           
  GPIO_SetBits(GPIOC, DUST_MOTOR_DRIVER);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_Beep(void)
{           
  GPIO_SetBits(GPIOD, BEEP);
}

/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_Wall_Send(void)
{           
  GPIO_SetBits(GPIOA, WALL_SEND);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_Stb(void)
{           
  GPIO_SetBits(GPIOB, LCD_STB);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_Power(void)
{           
  GPIO_SetBits(GPIOD,  LCD_POWER );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_Clk(void)
{           
  GPIO_SetBits(GPIOD, LCD_CLK);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_Data(void)
{           
  GPIO_SetBits(GPIOD, LCD_DATA);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_LCD_BG(void)
{           
  GPIO_SetBits(GPIOD, LCD_BG);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_Speed_Send(void)
{           
  GPIO_SetBits(GPIOD, SPEED_SEND );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_Earth_Send(void)
{           
  GPIO_SetBits(GPIOD, EARTH_SEND);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_L_FrontOnOff(void)
{           
  GPIO_SetBits(GPIOE, L_FRONTONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_L_BackOnOff(void)
{           
  GPIO_SetBits(GPIOE, L_BACKONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_R_FrontOnOff(void)
{           
  GPIO_SetBits(GPIOE, R_FRONTONOFF);                            
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_R_BackOnOff(void)
{           
  GPIO_SetBits(GPIOE, R_BACKONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Set_HW_Power(void)
{           
  GPIO_SetBits(GPIOE, HW_POWER);
}

/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_S_Motor(void)
{           
  GPIO_ResetBits(GPIOA, S_MOTOR_DRIVER);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_UV(void)
{           
  GPIO_ResetBits(GPIOB, UV_DRIVER);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_M_FPMOS(void)
{           
  GPIO_ResetBits(GPIOC, M_MOTOR_FPMOS );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_M_ZPMOS(void)
{           
  GPIO_ResetBits(GPIOB, M_MOTOR_ZPMOS );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_M_FNMOS(void)
{           
  GPIO_ResetBits(GPIOE, M_MOTOR_FNMOS );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_M_ZNMOS(void)
{           
  GPIO_ResetBits(GPIOE, M_MOTOR_ZNMOS );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_Dust_Motor(void)
{           
  GPIO_ResetBits(GPIOC, DUST_MOTOR_DRIVER);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_Beep(void)
{           
  GPIO_ResetBits(GPIOD, BEEP);
}

/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_Wall_Send(void)
{           
  GPIO_ResetBits(GPIOA, WALL_SEND);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_Stb(void)
{           
  GPIO_ResetBits(GPIOB, LCD_STB);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_Power(void)
{           
  GPIO_ResetBits(GPIOD,  LCD_POWER );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_Clk(void)
{           
  GPIO_ResetBits(GPIOD, LCD_CLK);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_Data(void)
{           
  GPIO_ResetBits(GPIOD, LCD_DATA);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_LCD_BG(void)
{           
  GPIO_ResetBits(GPIOD, LCD_BG);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_Speed_Send(void)
{           
  GPIO_ResetBits(GPIOD, SPEED_SEND );
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_Earth_Send(void)
{           
  GPIO_ResetBits(GPIOD, EARTH_SEND);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_L_FrontOnOff(void)
{           
  GPIO_ResetBits(GPIOE, L_FRONTONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_L_BackOnOff(void)
{           
  GPIO_ResetBits(GPIOE, L_BACKONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_R_FrontOnOff(void)
{           
  GPIO_ResetBits(GPIOE, R_FRONTONOFF);                            
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_R_BackOnOff(void)
{           
  GPIO_ResetBits(GPIOE, R_BACKONOFF);
}
/***********************************************************
���ܣ��ø�
***********************************************************/
void Reset_HW_Power(void)
{           
  GPIO_ResetBits(GPIOE, HW_POWER);
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_Front_Wheel(void)
{
return GPIO_ReadInputDataBit(GPIOB, FRONT_WHEEL)  ;
}
 
/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_S_Current(void)
{
return GPIO_ReadInputDataBit(GPIOC, S_CURRENT)  ;
}
 
/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_Key1(void)
{
GPIO_TypeDef* GPIOx;
//uint8 piv_state = 0;
    GPIOx = GPIOD;
	if ((GPIOx->IDR & KEY1) == 0)
	return 0;
	else 
	return 1;
//return GPIO_ReadInputDataBit(GPIOD, KEY1)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_Key2(void)
{	
GPIO_TypeDef* GPIOx;
//uint8 piv_state = 0;
    GPIOx = GPIOD;
	if ((GPIOx->IDR & KEY2) == 0)
	return 0;
	else 
	return 1;
//return GPIO_ReadInputDataBit(GPIOD, KEY2)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_Key3(void)
{		
GPIO_TypeDef* GPIOx;
//uint8 piv_state = 0;
    GPIOx = GPIOD;
	if ((GPIOx->IDR & KEY3) == 0)
	return 0;
	else 
	return 1;
//return GPIO_ReadInputDataBit(GPIOD, KEY3)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_L_Speed(void)
{
return GPIO_ReadInputDataBit(GPIOD, L_SPEED)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_L_LIDI(void)
{
return GPIO_ReadInputDataBit(GPIOD, L_LIDI)  ;
}
 
/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_Charge_Dc(void)
{
return GPIO_ReadInputDataBit(GPIOD, CHARGE_DC)  ;
}
 
/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_Charge_Seat(void)
{
return GPIO_ReadInputDataBit(GPIOD, CHARGE_SEAT)  ;
}
 
/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_R_Speed(void)
{
return GPIO_ReadInputDataBit(GPIOE, R_SPEED)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_R_LIDI(void)
{
return GPIO_ReadInputDataBit(GPIOE, R_LIDI)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_L_HW(void)
{
return GPIO_ReadInputDataBit(GPIOE, L_HW)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_M_HW(void)
{
return GPIO_ReadInputDataBit(GPIOE, M_HW)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_R_HW(void)
{
return GPIO_ReadInputDataBit(GPIOE, R_HW)  ;
}
   
/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_Up_HW(void)
{
return GPIO_ReadInputDataBit(GPIOE, UP_HW)  ;
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_L_Bump(void)
{
return GPIO_ReadInputDataBit(GPIOE, L_BUMP);
}
 
/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_R_Bump(void)
{
return GPIO_ReadInputDataBit(GPIOE, R_BUMP);
}

/**********************************************************
��ȡIO״̬
**********************************************************/
uint8 Read_LCD_Data(void)
{
return GPIO_ReadInputDataBit(GPIOD, LCD_DATA);
}
