#ifndef USER_MAIN_H_
#define USER_MAIN_H_

#define  u8  uint8_t
#define  u16 uint16_t
#define  u32 uint32_t
#define  u64 uint64_t

#define RECORD_KEY0  0  //485通信地址 
#define RECORD_KEY1  1	//锁16位id号+ "IN"
#define RECORD_KEY2  2  //锁16位随机密码
#define RECORD_KEY3  3  //锁16位预置密码
#define RECORD_KEY4  4  //SN

#define RECORD_KEY5  5  //初始化完成标记

#define RECORD_KEY6  6   //上升时间1
#define RECORD_KEY7  7   //上升时间2
#define RECORD_KEY8  8   //

#define RECORD_KEY9  9   //
#define RECORD_KEY10  10   //
#define RECORD_KEY11  11   //
#define RECORD_KEY12  12   //







#define USER_SET 0
#define USER_RUN 1


#include "tinyfs.h"
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
#include "reg_base_addr.h"
#include "sleep.h"

//#include "user_sw_check.h"
#include "user_function.h" 
//#include "user_uart.h"
//#include "moto.h"
//#include "Buzzer.h"
//#include "LED_RGB.h"
//#include "control_cmd.h"
//#include "crc16.h"

#include "lsadc.h"
#include "user_adc.h"
#include "md5.h"
#include "lsiwdg.h"
#include "key.h"



extern uint8_t con_idx_client;

extern uint16_t uart_client_tx_pointer_handle;

extern uint16_t uart_client_rx_attribute_handle;

extern uint16_t uart_client_rx_pointer_handle;



#endif
