#ifndef	 WALL_EARTH_H
#define  WALL_EARTH_H

////////////////////////////////////ͷ�ļ�///////////////////
#include "stm32f10x_lib.h"
#include "soft.h"

///////////////////////////////////��������//////////////////
extern WALL_DATA  e_l,e_r,e_lm,e_rm,w_l,w_lm,w_r,w_rm,w_m;  //�ؼ��ǽ������ݽṹ
extern bool wall_earth_time;  //�ؼ��ǽ���

extern bool action_wall;     //������ǽ���־
//////////////////////////////////�ӳ�����////////////////
void read_wallearth(void);//��ǿ��ؼ��ֵ���������ж�
void init_wallearth(void);
void Disable_wall(void);
void Disable_earth(void);
void Enable_wall(void);
void Enable_earth(void);
void Wall_SensitivityHigh(void);
void Wall_SensitivityLow(void);
#endif

