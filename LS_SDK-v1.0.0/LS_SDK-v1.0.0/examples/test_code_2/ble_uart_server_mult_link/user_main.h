#ifndef USER_MAIN_H_
#define USER_MAIN_H_

#define  u8  uint8_t
#define  u16 uint16_t
#define  u32 uint32_t
#define  u64 uint64_t

#define RECORD_KEY0  0  //485ͨ�ŵ�ַ 
#define RECORD_KEY1  1	//��16λid��+ "IN"
#define RECORD_KEY2  2  //��16λ�������
#define RECORD_KEY3  3  //��16λԤ������
#define RECORD_KEY4  4  //SN

#define RECORD_KEY5  5  //��ʼ����ɱ��

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




#endif
