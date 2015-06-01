
#ifndef  __RTC_H
#define  __RTC_H
////////////////////////////////////ͷ�ļ�///////////////////
#include "stm32f10x_lib.h"
#include "function.h"
///////////////////////////////////��������//////////////////

extern uint16  PreengageTime;
extern uint32  Rtc_time;
extern uint16  PreengageDate;
extern uint32  giv_BatteryCapability;  //��ص�����
extern uint32  giv_BatteryFDCap;//��طŵ�ĵ���
extern uint32  giv_UsePowerTime;// �õ�ʱ�䡣
extern uint8   giv_BatteryChargeForbid; //�����ش��������־��0Ϊ����1Ϊ������
extern bool watchdog_time;		   
extern uint8   giv_BatteryActivation;    //��ػ��־
extern uint32  giv_BatteryChargeTime;    //ǰһ�ε�صĳ��ʱ��


void init_rtc(void);
uint8 ReadBKPPreengageDate(void);  //�ӱ������ж���ԤԼ����
uint16 ReadBKPPreengageTime(void);//�ӱ������ж���ԤԼʱ��

void  WritePreengage(void);//����ԤԼʱ��
void  WriteRtcTime(void);//дʵʱʱ��
void  WriteWorkState(void);
bool  ReadBKPUV(void);  //�ӱ������ж���UV��״̬
uint8  ReadBKPSpeed(void);//�ӱ������ж����ٶ�״̬
uint8  ReadBKPMode(void);//�ӱ������ж�������ģʽ
void  WriteFDCap(void);
uint32 ReadBKPFdCap(void);//�ӱ������ж����ŵ�����
uint8 ReadBKPBatteryChargeForbid(void); //�ӱ������ж�����س䱥��־
uint32 ReadBKPCapability(void);//�ӱ������ж����������
void  WriteBatteryCapability(void);
void ReadRealTime(void);
void WriteBakErr(uint16 err);
uint16 ReadBakErr(void);
void WriteBakBatteryActivation(void); //д��ؼ����־
uint8 ReadBakBatteryActivation(void);//����ؼ����־

void WriteBakBatteryChargeTime(void);
uint32 ReadBakBatteryChargeTime(void);

#endif
