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
#define RECORD_KEY13  13   //

#define USER_SET 0
#define USER_RUN 1

//#define DYPA06     //超声波
#define URA196     //毫米波



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
#include "sys_stat.h"

#include "reg_lsgpio.h"
#include "reg_rcc.h"




#include "user_sw_check.h"
#include "user_function.h" 
#include "user_uart.h"
#include "moto.h"
#include "Buzzer.h"
#include "LED_RGB.h"
#include "control_cmd.h"
#include "crc16.h"

#include "lsadc.h"
#include "user_adc.h"
#include "md5.h"
#include "lsiwdg.h"


#define USER_EVENT_PERIOD_0 1		 	 	// 1ms
#define USER_EVENT_PERIOD_1 20     	// 20ms
#define USER_EVENT_PERIOD_2 500     // 500ms


void ls_app_timer_init(void);

void ls_user_event_timer_cb_0(void *param);
void ls_user_event_timer_cb_1(void *param);
void ls_user_event_timer_cb_2(void *param);

extern struct builtin_timer_0 *user_event_timer_inst_0 ;
extern struct builtin_timer_1 *user_event_timer_inst_1 ;
extern struct builtin_timer_2 *user_event_timer_inst_2 ;





void start_adv(void);



#endif
