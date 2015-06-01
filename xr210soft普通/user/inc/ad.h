#ifndef  __AD_H
#define  __AD_H
#include "stm32f10x_lib.h"
void init_ad(void);
uint16 account_current (uint8 channel);
void ReInitAd(void);
extern uint16  ADC_ConvertedValue[256];
extern bool ADC_ready;
#endif
