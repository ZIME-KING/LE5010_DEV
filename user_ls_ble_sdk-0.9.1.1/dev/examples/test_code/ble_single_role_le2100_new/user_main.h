#ifndef USER_MAIN_H_
#define USER_MAIN_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lstimer.h"
#include "lsuart.h"
#include "lsdmac.h"
#include "sleep.h"
#include "lsadc.h"
#include "tinyfs.h"
#include "io_config.h"
#include "platform.h"
#include "log.h"
#include "lsiwdg.h"
#include "lsrtc.h"
#include "reg_syscfg.h"

#include "user_function.h"
#include "user_uart.h"
#include "crc.h"
#include "aes-128.h"
#include "moto.h"
#include "Buzzer.h"
#include "scan_sw.h"
#include "LPM2100_N.h"

void start_adv(void);


extern UART_HandleTypeDef UART_Config;
extern UART_HandleTypeDef UART_Config_AT;
extern uint8_t globle_Result;
extern uint8_t user_ble_send_flag;

extern uint8_t SHORT_NAME[10];
extern uint8_t NEW_SHORT_NAME[10];

extern uint8_t SHORT_NAME_LEN;
//extern uint8_t COMPLETE_NAME[7];
extern uint8_t TX_DATA_BUF[16]; //BEL
extern uint8_t TOKEN[4];
extern uint16_t sleep_time;
extern uint8_t KEY_FLAG;
extern uint8_t BLE_connected_flag;
extern uint8_t VBat_value;
extern uint16_t temp_count;

extern uint8_t PASSWORD[6];
extern unsigned char key[16];

extern uint8_t NEW_PASSWORD_BUF[6]; //暂存改password的数据
extern uint8_t NEW_KEY_BUF[8];      //暂存改key的数据，分两次接收

extern uint8_t	psaaword_task_flag;  //改密码任务开始标记
extern uint8_t	key_task_flag;			 //改密钥任务开始标记

#endif
