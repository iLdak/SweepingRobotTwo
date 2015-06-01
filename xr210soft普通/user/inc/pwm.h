#ifndef  __PWM_H
#define  __PWM_H

#include "stm32f10x_lib.h"

void  Init_PWM(void);
void  enable_pwm(uint16 channel,uint16  pwm);
void  disable_pwm(uint16 channel);
void  Set_Pwm(uint16 channel,uint16  pwm);
#endif
