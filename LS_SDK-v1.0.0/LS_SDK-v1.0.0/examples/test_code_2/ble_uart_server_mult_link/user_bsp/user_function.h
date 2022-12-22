#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "user_main.h"


#define __USER_FUNCTION_H
#define __USER_FUNCTION_H
#define __USER_FUNCTION_H


extern UART_HandleTypeDef UART2_Config;
extern UART_HandleTypeDef UART1_Config;
extern uint16_t DYP_distance;


extern uint8_t lockid[18];

void User_BLE_Ready(void);
void loop_task(void);

#endif
