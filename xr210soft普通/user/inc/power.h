
#ifndef  __POWER_H
#define  __POWER_H

#include "stm32f10x_lib.h"
#include "soft.h"
void judge_charge(void);
void chargeing(void);
void init_power (void);
void  sampling_temp_voltage(void);
uint8 Get_Dispower(void);
uint8 Get_BatteryTemp(void);
void ChargeControl(void);

extern bool POWER_ready;
extern bool VOL_TEMP_ready;
extern uint16  jt_chargecurrent;   //静态时主板所耗电流
extern uint16  battery_chargecurrent;//电池5秒内的平均电流
extern uint16  battery_voltage;    //电池1秒的电压
extern uint16  battery_temp;       //电池1秒的温度
extern uint16  l_current;          //左轮电流
extern uint16  r_current;          //右轮电流
extern uint16  dust_current;       //灰尘风机电流
extern uint16  m_current;          //中扫电流


extern uint16  battery_chargecurrent_1s;//电池1秒内的平均电流
extern uint16  battery_voltage_1s;    //电池1秒的电压
extern uint16  battery_temp_1s;       //电池1秒的温度
extern uint16  l_current_1s;          //左轮电流
extern uint16  r_current_1s;          //右轮电流
extern uint16  dust_current_1s;       //灰尘风机电流
extern uint16  m_current_1s;          //中扫电流


extern POWER power ;
extern const uint16 temp[];
#endif
