#ifndef	 WALL_EARTH_H
#define  WALL_EARTH_H

////////////////////////////////////头文件///////////////////
#include "stm32f10x_lib.h"
#include "soft.h"

///////////////////////////////////变量定义//////////////////
extern WALL_DATA  e_l,e_r,e_lm,e_rm,w_l,w_lm,w_r,w_rm,w_m;  //地检和墙检的数据结构
extern bool wall_earth_time;  //地检和墙检的

extern bool action_wall;     //允许检测墙检标志
//////////////////////////////////子程序定义////////////////
void read_wallearth(void);//读强检地检的值并且做出判断
void init_wallearth(void);
void Disable_wall(void);
void Disable_earth(void);
void Enable_wall(void);
void Enable_earth(void);
void Wall_SensitivityHigh(void);
void Wall_SensitivityLow(void);
#endif

