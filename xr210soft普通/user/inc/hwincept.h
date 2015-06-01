#ifndef  __HWINCEPT_H
#define  __HWINCEPT_H
////////////////////////ͷ�ļ�////////////////////////////////////	 
#include "stm32f10x_lib.h"
#include "soft.h"

////////////////////////ȫ�ֱ���//////////////////////////////////

extern INFR_DATA l_hw,r_hw,m_hw,up_hw,dummy_wall;	  
extern bool Action_hwincept;
///////////////////////ȫ�ֺ���////////////////////////////////////

void  init_hwincept(void);
void read_hw_data(INFR_DATA *hw_struct);
void read_dummy_wall(INFR_DATA *hw_struct);

void clr_hw_effect(INFR_DATA *hw_struct);
void enable_hwincept(void);
void disable_hwincept(void);
							   
#endif
