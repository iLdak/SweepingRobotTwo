#ifndef  __TIME1_H
#define  __TIME1_H
#include "stm32f10x_lib.h"
void init_time(void);
void DelayMs(uint32 Ms);
extern volatile  uint32  giv_sys_time;      //����Ϊ����ϵͳ��ϵͳʱ��
extern volatile bool Sec;											   
extern volatile bool Sec_Two;
extern bool gbv_minute;   //���ӱ�־�����ڶ�ʱ������صĵ�ѹ��

#endif
