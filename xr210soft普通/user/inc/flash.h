#ifndef  __FLASH_H
#define  __FLASH_H

#include "stm32f10x_lib.h"

uint8 init_flash(void);
uint32 read_flash(void);
uint8 write_flash(uint32 data);
#endif
