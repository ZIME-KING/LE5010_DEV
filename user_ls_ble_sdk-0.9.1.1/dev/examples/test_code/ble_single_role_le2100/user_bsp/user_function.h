#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "user_main.h"
 void User_Init(void);
 void Start_Lock_Send();
 void Open_Lock_Send();
 void Tick_Lock_Send();
 void Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state);
 
 
#endif
