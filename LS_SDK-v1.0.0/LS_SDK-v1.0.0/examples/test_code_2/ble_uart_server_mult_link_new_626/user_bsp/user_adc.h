#ifndef USER_ADC_H_
#define USER_ADC_H_

#include "user_main.h"

extern ADC_HandleTypeDef hadc;
void User_ADC_Init(void);
uint16_t Get_ADC_value(void);

#endif
