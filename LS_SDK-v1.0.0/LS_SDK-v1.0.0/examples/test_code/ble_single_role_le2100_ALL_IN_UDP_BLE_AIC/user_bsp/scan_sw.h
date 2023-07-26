#ifndef __SCAN_SW_H
#define __SCAN_SW_H

#include "user_main.h"
void Scan_Key(void);
void Button_Gpio_Init(void);
uint8_t Check_SW1(void);
uint8_t Check_SW2(void);
uint8_t Check_SW3(void);
extern uint8_t KEY_ONCE;
void ls_sleep_enter_lp2(void);
extern uint8_t wd_FLAG;
void Button_io_init_exti(void);
#endif 

//void AES_main(void);
//void AES_ceshi(void);

//void aes_jia_mi(void);
//void aes_jie_mi(void);




























