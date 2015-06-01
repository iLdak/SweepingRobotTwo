#ifdef  __WATCHDOG_C
#define EXTERN_WATCHDOG 
#else
#define EXTERN_WATCHDOG  extern
#endif 
#include "stm32f10x_lib.h"
#include "soft.h"
EXTERN_WATCHDOG void Init_WatchDog(void);
