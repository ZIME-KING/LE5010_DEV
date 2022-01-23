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


#include "software_Time.h"
#include "user_uart.h"
#include "user_main.h"


extern uint32_t uwTick;


extern TIM_HandleTypeDef TimHandle_TIM; 	//TMIER: LSBSTIM
extern TIM_HandleTypeDef TimHandle_PWM;  //TMIER: LSGPTIMB
extern UART_HandleTypeDef UART_Config; 

#endif
