#ifndef __BUZZER_H
#define __BUZZER_H

#include "user_main.h"

extern uint8_t buzzer_task_flag;

void Buzzer_Task(void);
void Basic_PWM_Output_Cfg(void);
void Buzzer_ON(void);
void Buzzer_OFF(void);
#endif


























