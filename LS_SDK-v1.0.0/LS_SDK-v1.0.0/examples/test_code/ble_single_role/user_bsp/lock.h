#ifndef __LOCK_H
#define __LOCK_H

#include "user_main.h"

extern uint8_t lock_task_flag_1;
extern uint8_t lock_task_flag_2;
extern uint8_t lock_task_flag_1_temp;
extern uint8_t lock_task_flag_2_temp;

void Lock_gpio_init(void);
void Lock_task(void);

#endif 