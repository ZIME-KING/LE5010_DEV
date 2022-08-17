#ifndef USER_FUNCTION_H_
#define USER_FUNCTION_H_

#define LOG_TAG "MAIN"
#include "ls_ble.h"
#include "platform.h"
#include "prf_diss.h"
#include "log.h"
#include "ls_dbg.h"
#include "cpu.h"
#include "lsuart.h"
#include "builtin_timer.h"
#include <string.h>
#include "co_math.h"
#include "io_config.h"
#include "SEGGER_RTT.h"
#include "user_function.h"
#include "stdio.h"
#include "key.h"
#include "lsadc.h"


extern uint16_t adc_value_num;
extern uint8_t key_status_num;
extern uint8_t serial_num;
extern uint8_t id_num[8];
extern uint8_t sent_buf[12];
extern uint32_t globle_count;

extern uint32_t no_act_count;
extern uint32_t led_open_count;
extern uint8_t  led_open_flag;


extern uint16_t adc_value;
extern volatile uint8_t recv_flag ; //adc中断处理标记

extern UART_HandleTypeDef UART_Server_Config; 
extern ADC_HandleTypeDef hadc;

uint16_t crc16_check(const uint8_t *buff, uint16_t size);
void gpio_init(void);
void user_init(void);
void ls_sleep_enter_LP3(void);
void Get_vbat_val(void);



#endif
