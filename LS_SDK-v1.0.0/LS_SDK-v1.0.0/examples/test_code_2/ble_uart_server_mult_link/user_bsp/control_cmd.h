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

extern uint8_t  address ;

uint8_t* CMD_Processing(uint8_t *p,uint16_t length);

#endif
