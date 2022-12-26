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
extern uint8_t rand_password[16]; //Ëø16Î»Ëæ»úÃÜÂë
extern uint8_t def_password[16];  //Ëø16Î»Ô¤ÖÃÃÜÂë
extern uint8_t lock_sn[16];  //
extern uint8_t lock_mac[6];  //
extern uint8_t reset_flag;

extern uint8_t  address ;
extern uint32_t time_count;


void User_BLE_Ready(void);
void loop_task(void);

bool Check_Password(uint8_t *password);


#endif
