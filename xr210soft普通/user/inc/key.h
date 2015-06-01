
#ifndef  __KEY_H
#define  __KEY_H
////////////////////////////////////м╥нд╪Ч///////////////////
#include "stm32f10x_lib.h"
#include "soft.h"
void init_key(void);
void AutoReadKey(void);

extern KEY key1,key2,key3,l_lidi,r_lidi,l_bump,r_bump,charge_dc,charge_seat,s_current;
extern bool key_time;
#endif

