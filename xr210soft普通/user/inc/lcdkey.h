#ifndef __LCDKEY_H
#define __LCDKEY_H
/////////////////*****INCLUDE *************************
#include "stm32f10x_lib.h"

////////////////全局变量定义//////////////////////////
extern bool Dis_On;            //显示标志。
extern bool Dis_ErrOn ;    	//错误图标亮
extern bool Dis_UvOn ;			//uv图标亮
extern bool Dis_PrOn ;			//预约天数亮
extern bool Dis_AuOn ;			//自动清扫亮
extern bool Dis_SpOn ;			//局部清扫亮
extern bool Dis_HourOn ;		//小时亮
extern bool Dis_MinuteOn ;		//分钟亮
extern bool Dis_ColOn ;		//时间点亮
extern bool Dis_WeekOn ;		//星期亮
extern bool Dis_SpeedOn ;		//速度亮
extern bool Dis_PowerOn ;		//电池图标亮
extern bool Dis_ChargeOn ;		//充电图标亮

extern bool Dis_SpeedGo;       //轮子图标转动

extern bool Dis_ErrGlint ;		//错误图标闪烁
extern bool Dis_UvGlint ;		//uv图标闪烁
extern bool Dis_PrGlint ;		//预约天数闪烁
extern bool Dis_AuGlint ;		//自动清扫闪烁
extern bool Dis_SpGlint ;		//局部清扫闪烁
extern bool Dis_HourGlint ;	//小时闪烁
extern bool Dis_MinuteGlint ;	//分钟闪烁
extern bool Dis_ColGlint ;		//时间点闪烁
extern bool Dis_WeekGlint ;	//星期闪烁
extern bool Dis_SpeedGlint ;	//速度闪烁
extern bool Dis_PowerGlint ;	//电池图标闪烁
extern bool Dis_ChargeGlint ;	//充电图标闪烁

extern uint8 Dis_Preengage;	//预约内容
extern uint8 Dis_Hour; 		//显示的小时
extern uint8 Dis_Minute;  		//显示的分钟
extern uint8 Dis_Week;			//显示的星期内容  范围 1——7
extern uint8 Dis_Speed;		//显示的速度，范围1——3
extern uint8 Dis_Power;		//显示的电源电量 ，范围0——3

extern bool dis_updata;        //显示更新标志

extern uint8 Dis_Data[10] ; 	//显示的数据亮
/////////////////***FUC*******************************
void Init_Lcd(void);	//初始化芯片的IO脚
uint8 lcdkey_read(void);     // 读一个数
void Lcd_WriteCommand(uint16 command);
void  lcdkey_disdata(uint8  addr, uint8*  str, uint8  number);
void  lcdkey_bgoff(void);
void  lcdkey_bgon(void);
void  lcdkey_disoff(void);
void  lcdkey_dison(void);
void  AutoDisplay(void);
void clr_display(void);
void delay(uint32 delaynumber);
 void Clr_DisplayData(void);
#endif /* __MAIN_H */
