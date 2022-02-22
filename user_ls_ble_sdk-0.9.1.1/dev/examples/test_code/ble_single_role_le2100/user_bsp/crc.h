#ifndef __CRC_H
#define __CRC_H

#include "user_main.h"

extern uint16_t crc_temp;
extern uint16_t  crc_data[50];


unsigned short CRC16_8005Modbus( unsigned char *buff, unsigned long len);
uint16_t calc_crc16_1(uint8_t const *p_data, int32_t data_len);

void crc_char(char char_data);

#endif