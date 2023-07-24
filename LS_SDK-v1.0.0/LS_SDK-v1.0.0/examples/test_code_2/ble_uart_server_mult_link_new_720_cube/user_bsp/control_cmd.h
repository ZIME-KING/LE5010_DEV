#ifndef CONTROL_CMD_H_
#define CONTROL_CMD_H_

#include "user_main.h"

extern uint8_t hw_lock_status;      //锁状态 真实硬件上的 
extern uint8_t now_lock_status;     //锁状态 当前 设置值
extern uint8_t tag_lock_status;     //锁状态 目标
extern uint8_t lock_mode;           //锁模式
extern uint8_t lock_success_flag;   //锁鉴权成功标记


extern uint8_t lockid[18];
extern uint8_t rand_password[16]; //锁16位随机密码
extern uint8_t def_password[16];  //锁16位预置密码
extern uint8_t lock_sn[16];  //
extern uint8_t lock_mac[6];  //
extern uint8_t reset_flag;
extern uint8_t vbat_val;		 //电池电量
extern uint8_t car_val;      //车辆状态
extern uint8_t address ;

extern uint16_t first_rise_time;    //
extern uint16_t second_rise_time;     
extern uint8_t low_vbat_val;     //低电压报警值4.5V   

extern uint8_t hw_err_out_time;     				//超时时间，小于90到0，大于90到90
extern uint8_t hw_err_buzzer_all_time;      //报警总时长 
extern uint8_t hw_err_buzzer_on_time;       //音效     
extern uint8_t hw_err_buzzer_off_time;      //音效     


extern uint8_t  led_sleep_time;     					//单位s
extern uint32_t user_time_cont;       			  //无操作时间计数

extern uint8_t  send_buf[64];

uint8_t* CMD_Processing(uint8_t *p,uint16_t length);


#endif
