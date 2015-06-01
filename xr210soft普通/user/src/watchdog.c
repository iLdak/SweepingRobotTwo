#define __WATCHDOG_C
#include "watchdog.h"

void Init_WatchDog(void)
{	   
  #ifdef  DEBUG   
  *((uint32 *)0xe0042004) |= 10;
  #endif  
  	   
      IWDG_ReloadCounter();	 
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_SetPrescaler(IWDG_Prescaler_256);//สนำร
      IWDG_SetReload(1000);
      IWDG_ReloadCounter();
}
