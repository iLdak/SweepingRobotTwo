
#ifndef  __SLEEP_H
#define  __SLEEP_H
#include "stm32f10x_lib.h"
#include "soft.h"
extern bool ExitInput;     //有按键、外部电源外部输入
void Init_Sleep(void); 
void Do_Sleep(void);

#endif
