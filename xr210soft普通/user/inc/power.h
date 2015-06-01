
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
extern uint16  jt_chargecurrent;   //��̬ʱ�������ĵ���
extern uint16  battery_chargecurrent;//���5���ڵ�ƽ������
extern uint16  battery_voltage;    //���1��ĵ�ѹ
extern uint16  battery_temp;       //���1����¶�
extern uint16  l_current;          //���ֵ���
extern uint16  r_current;          //���ֵ���
extern uint16  dust_current;       //�ҳ��������
extern uint16  m_current;          //��ɨ����


extern uint16  battery_chargecurrent_1s;//���1���ڵ�ƽ������
extern uint16  battery_voltage_1s;    //���1��ĵ�ѹ
extern uint16  battery_temp_1s;       //���1����¶�
extern uint16  l_current_1s;          //���ֵ���
extern uint16  r_current_1s;          //���ֵ���
extern uint16  dust_current_1s;       //�ҳ��������
extern uint16  m_current_1s;          //��ɨ����


extern POWER power ;
extern const uint16 temp[];
#endif
