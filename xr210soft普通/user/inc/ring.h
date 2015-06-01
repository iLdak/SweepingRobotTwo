#ifndef	 __RING_H
#define  __RING_H
#include "stm32f10x_lib.h"
#include "soft.h"

extern RING  l_ring,r_ring,w_ring;       //测试车轮速度的结构
extern RAP  l_rap,r_rap,w_rap;           //两轮速度距离的控制结构
extern bool rap_time;                    //两轮速度调整时间标志
extern uint16 jt_l_current;              //左轮由于1n60反向漏电流引起的静态电流。
extern uint16 jt_r_current;              //右轮由于1n60反向漏电流引起的静态电流。
extern uint8 Wheel_State ;            //左右轮驱动状态 0为合格，1为驱动坏	 
extern uint32 Speed ;                    //两轮的最大速度  ，表示为脉冲时间
extern uint8 err;
extern ACTION action;
//////////////////////////////////子程序定义////////////////
void Init_Ring(void);
uint32 subbtime(uint32 little,uint32 big);
void  enable_rap(uint32 ori_l, uint32 length_l,uint32 ori_r,uint32 length_r);
void stop_rap(void);
void  comm_rap(void);
uint8 Test_DriveOfWheel(void) ;
uint8 Test_DriveOfBrush(void);
void Change_Speed(void);
#endif
