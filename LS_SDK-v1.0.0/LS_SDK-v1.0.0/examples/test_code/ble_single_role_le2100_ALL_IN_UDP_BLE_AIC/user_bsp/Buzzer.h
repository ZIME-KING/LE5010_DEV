#ifndef __BUZZER_H
#define __BUZZER_H

#include "user_main.h"

extern uint8_t buzzer_task_flag;
extern uint8_t buzzer_task_flag_2;
void Buzzer_Task(void);
void Basic_PWM_Output_Cfg(void);
void Buzzer_ON(void);
void Buzzer_OFF(void);
void Buzzer_DeInit(void);
void Buzzer_Init(void);
#endif 
