/*
 * software_Time.h
 *
 *  Created on: 2020年9月10日
 *      Author: champion
 */
#ifndef __SOFTWARE_TIME_H_
#define __SOFTWARE_TIME_H_

#include "user_main.h"

typedef struct
{
    uint32_t TimeStart;//获取起始时间
    uint32_t TimeInter;//间隔时间
}TIME_TYPE_ST;

extern TIME_TYPE_ST Task_1;
extern TIME_TYPE_ST Task_5;
extern TIME_TYPE_ST Task_50;
extern TIME_TYPE_ST Task_100;
extern TIME_TYPE_ST Task_200;
extern TIME_TYPE_ST Task_500;
extern TIME_TYPE_ST Task_1000;


void Delay_ms(uint32_t Delay);
void GetTime(TIME_TYPE_ST *TimeType);
uint8_t CompareTime(TIME_TYPE_ST *TimeType);//比较函数
 

#endif /* INC_SOFTWARE_TIME_H_ */
 
