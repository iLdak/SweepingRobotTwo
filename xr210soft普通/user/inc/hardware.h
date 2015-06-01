#ifndef  __HARDWARE_H
#define  __HARDWARE_H
																  
////////////////////////头文件////////////////////////////////////
#include "stm32f10x_lib.h"
#include "soft.h"

////////////////////////全局变量//////////////////////////////////
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
//-----------------------	
void Set_M_FPMOS(void);	
void Set_M_ZPMOS(void);
void Set_M_ZNMOS(void);
void Set_M_FNMOS(void);
//-------- 
void Set_M_Motor(void);
void Reset_M_Motor(void); 
//-----------
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
//------------------------	
void Reset_M_FPMOS(void);	
void Reset_M_ZPMOS(void);	
void Reset_M_FNMOS(void);	
void Reset_M_ZNMOS(void);	
//------------------------  
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

///////////////////////全局函数////////////////////////////////////
#endif

