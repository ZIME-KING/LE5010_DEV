#ifndef CONTROL_CMD_H_
#define CONTROL_CMD_H_

#include "user_main.h"

extern uint8_t hw_lock_status;    //锁状态 真实硬件上的 
extern uint8_t now_lock_status;    //锁状态 当前 设置值
extern uint8_t tag_lock_status;    //锁状态 目标
extern uint8_t lock_mode;          //锁模式

void CMD_Processing(uint8_t *p);

#endif
