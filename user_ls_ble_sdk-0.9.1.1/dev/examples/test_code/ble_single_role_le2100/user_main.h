#ifndef USER_MAIN_H_
#define USER_MAIN_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lstimer.h"
#include "lsuart.h"
#include "lsdmac.h"

#include "io_config.h"
#include "platform.h"
#include "log.h"


#include "user_function.h"
#include "user_uart.h"
#include "crc.h"
#include "aes-128.h"

extern UART_HandleTypeDef UART_Config; 
extern UART_HandleTypeDef UART_Config_AT; 
extern uint8_t globle_Result; 


#endif