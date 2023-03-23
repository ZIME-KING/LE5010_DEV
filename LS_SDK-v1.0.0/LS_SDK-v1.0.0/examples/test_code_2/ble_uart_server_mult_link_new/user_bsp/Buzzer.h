#ifndef __BUZZER_H
#define __BUZZER_H

#include "user_main.h"
#include "Buzzer.h"
extern uint8_t buzzer_task_flag;
//extern uint8_t buzzer_task_flag_2;




void Set_buzzer_Task_val(uint16_t a,uint8_t b,uint8_t c);


void Buzzer_Task_100(void);

void Buzzer_ON(void);
void Buzzer_OFF(void);
void Buzzer_IO_Init(void);

#endif 


























