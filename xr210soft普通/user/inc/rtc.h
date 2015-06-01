
#ifndef  __RTC_H
#define  __RTC_H
////////////////////////////////////头文件///////////////////
#include "stm32f10x_lib.h"
#include "function.h"
///////////////////////////////////变量定义//////////////////

extern uint16  PreengageTime;
extern uint32  Rtc_time;
extern uint16  PreengageDate;
extern uint32  giv_BatteryCapability;  //电池的容量
extern uint32  giv_BatteryFDCap;//电池放电的电量
extern uint32  giv_UsePowerTime;// 用电时间。
extern uint8   giv_BatteryChargeForbid; //允许电池大电流充电标志，0为允许，1为不允许。
extern bool watchdog_time;		   
extern uint8   giv_BatteryActivation;    //电池活化标志
extern uint32  giv_BatteryChargeTime;    //前一次电池的充电时间


void init_rtc(void);
uint8 ReadBKPPreengageDate(void);  //从备份域中读出预约天数
uint16 ReadBKPPreengageTime(void);//从备份域中读出预约时间

void  WritePreengage(void);//保存预约时间
void  WriteRtcTime(void);//写实时时钟
void  WriteWorkState(void);
bool  ReadBKPUV(void);  //从备份域中读出UV灯状态
uint8  ReadBKPSpeed(void);//从备份域中读出速度状态
uint8  ReadBKPMode(void);//从备份域中读出工作模式
void  WriteFDCap(void);
uint32 ReadBKPFdCap(void);//从备份域中读出放电容量
uint8 ReadBKPBatteryChargeForbid(void); //从备份域中读出电池充饱标志
uint32 ReadBKPCapability(void);//从备份域中读出电池容量
void  WriteBatteryCapability(void);
void ReadRealTime(void);
void WriteBakErr(uint16 err);
uint16 ReadBakErr(void);
void WriteBakBatteryActivation(void); //写电池激活标志
uint8 ReadBakBatteryActivation(void);//读电池激活标志

void WriteBakBatteryChargeTime(void);
uint32 ReadBakBatteryChargeTime(void);

#endif
