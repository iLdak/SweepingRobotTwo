#ifndef	 __RING_H
#define  __RING_H
#include "stm32f10x_lib.h"
#include "soft.h"

extern RING  l_ring,r_ring,w_ring;       //���Գ����ٶȵĽṹ
extern RAP  l_rap,r_rap,w_rap;           //�����ٶȾ���Ŀ��ƽṹ
extern bool rap_time;                    //�����ٶȵ���ʱ���־
extern uint16 jt_l_current;              //��������1n60����©��������ľ�̬������
extern uint16 jt_r_current;              //��������1n60����©��������ľ�̬������
extern uint8 Wheel_State ;            //����������״̬ 0Ϊ�ϸ�1Ϊ������	 
extern uint32 Speed ;                    //���ֵ�����ٶ�  ����ʾΪ����ʱ��
extern uint8 err;
extern ACTION action;
//////////////////////////////////�ӳ�����////////////////
void Init_Ring(void);
uint32 subbtime(uint32 little,uint32 big);
void  enable_rap(uint32 ori_l, uint32 length_l,uint32 ori_r,uint32 length_r);
void stop_rap(void);
void  comm_rap(void);
uint8 Test_DriveOfWheel(void) ;
uint8 Test_DriveOfBrush(void);
void Change_Speed(void);
#endif
