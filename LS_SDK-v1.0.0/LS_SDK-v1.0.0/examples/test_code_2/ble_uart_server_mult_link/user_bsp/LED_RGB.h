#ifndef LED_RGB_H_
#define LED_RGB_H_

#include "user_main.h"

void LED_Functon(void);
void LED_Init(void);

 void Set_LED_Function_val(uint8_t r,uint8_t g,uint8_t b,
													uint8_t min,uint8_t max,
													uint8_t T0,uint8_t T1,uint8_t T2,uint8_t T3);

#endif
