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
#include "builtin_timer.h"

#include "user_function.h" 
#include "test_user_function.h"
#include "user_uart.h"
#include "crc.h"
#include "aes-128.h"
#include "moto.h"
#include "Buzzer.h"
#include "scan_sw.h"
#include "LPM2100_N.h"


//#define USER_TEST 


#define DEF                   //Ĭ��������
//#define MAG                   //�ְ���
//#define METAL_PADLOCK         //��������
//#define WHITE         //����



#define POWER_L 0
#define POWER_H 1


#ifdef USER_TEST 
 extern uint8_t open_count;
#endif


#define RECORD_KEY1  1	//��������
#define RECORD_KEY11 12	//IMEI/�豸��
#define RECORD_KEY2  2  //���ģ���ʼ�����
#define RECORD_KEY3  3  //������������
#define RECORD_KEY4  4  //����ͨ����Կ ��8λ
#define RECORD_KEY5  5  //����ͨ����Կ ��8λ
#define RECORD_KEY6  6  //��flash����д���ʼֵ���
#define RECORD_KEY7  7  //last_Vbat
#define RECORD_KEY8  8  //last_Vbat_max
#define RECORD_KEY9  9  //CIMI /IMSI ����

#define RECORD_KEY_T  11  //����ģʽ���


#ifdef USER_TEST 
	#define RECORD_KEY10 10 
#endif


void start_adv(void);


extern UART_HandleTypeDef UART_Config;
extern UART_HandleTypeDef UART_Config_AT;
extern uint8_t globle_Result;
extern uint8_t user_ble_send_flag;

extern uint8_t MAC_ADDR[6];
extern uint8_t CIMI_DATA[15];
extern uint8_t MICI_DATA[15];
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

extern uint8_t NEW_PASSWORD_BUF[6]; //�ݴ��password������
extern uint8_t NEW_KEY_BUF[8];      //�ݴ��key�����ݣ������ν���

extern uint8_t	psaaword_task_flag;  //����������ʼ���
extern uint8_t	key_task_flag;			 //����Կ����ʼ���

extern uint8_t  reset_flag;

extern uint16_t send_time_delay;

extern uint8_t test_mode_flag;

extern uint8_t look_status_send_count;
extern uint16_t s_VBat_value;



void ls_app_timer_deinit(void);
void ls_app_timer_init(void);

void AT_uart_deinit(void);
void AT_uart_init(void);


void ls_user_event_timer_cb_0(void *param);


extern uint32_t user_event_period;
extern struct builtin_timer *user_event_timer_inst_0;
extern uint8_t power_mode;


#endif
