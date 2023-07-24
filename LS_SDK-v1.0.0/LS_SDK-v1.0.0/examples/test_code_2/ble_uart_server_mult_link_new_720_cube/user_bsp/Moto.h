#ifndef __MOTO_H
#define __MOTO_H

#include "user_main.h"

extern uint8_t moro_task_flag;
extern uint8_t test_moro_task_flag ;

void Moto_Task(void);
void Test_Moto_Task(void);


void Moto_IO_Init(void);
void Moto_IO_DeInit(void);

#endif 

//void AES_main(void);
//void AES_ceshi(void);

//void aes_jia_mi(void);
//void aes_jie_mi(void);



























