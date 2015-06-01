
#ifndef  __FUNCTION_H
#define  __FUNCTION_H
////////////////////////头文件////////////////////////////////////	 
#include "stm32f10x_lib.h"										
#include "soft.h"
//--------------------------- 
extern MODE mode;		 //当前机器所在的状态 
extern WORK_MODE gsv_work_mode;	 //当前机器所在的工作状态，其取值范围为SWEEP、
                             //SPOT,当机器在其他状态时不能改变，只有在进入SWEE或者PSPOT
							 //才会变为SWEEP或者SPOT
extern  uint8 zhulujing[];
extern  uint32 zhulujinglong[];
extern  uint32 angle[52];
extern  ACTION action;
extern uint8 giv_sys_err;         //系统的错误代码		
extern uint8 giv_sys_err_bak;         //系统的错误代码

///////////////////////全局函数////////////////////////////////////
void Init_Mode(void);
void Action_Mode(void);
uint8 Action_Protect(void);
void Turn_UV(void);
void Change_Speed(void);
void Display_Real(void);  
void Display_Work_Mode(void);
void  Enable_Sweep(void);
void  Disable_Sweep(void);

uint32  read_bump(void);
uint8 Read_Protect(void);  
void  back(uint32 longth);
void  left(uint32 longth);
void  right(uint32 longth);
void  forward(uint32 longth);
void  clr_ram(void);
uint8 do_action(uint8 dm,uint32 length);
uint8 z_road(void);
uint8 louxuan(  uint8  f);
uint8 read_earth(void);
void yanqiangzou(void);
uint8 ybs(void);
uint8 ybs_randrm(void);
uint8 action_bump(void);
uint8 Check_PreengageTime(void);
void Init_Action(void);
void AccountCapability(void);
void  AccountCapabilityReal(void);
uint8 CheckMDustMotor(void);


uint8 read_sideearth(void);	

#endif
