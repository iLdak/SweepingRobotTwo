#ifndef  __TIME1_H
#define  __TIME1_H
#include "stm32f10x_lib.h"
void init_time(void);
void DelayMs(uint32 Ms);
extern volatile  uint32  giv_sys_time;      //定义为整个系统的系统时钟
extern volatile bool Sec;											   
extern volatile bool Sec_Two;
extern bool gbv_minute;   //分钟标志，用于定时采样电池的电压。

#endif
