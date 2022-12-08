#ifndef CRC16_H_
#define CRC16_H_
#include <stdint.h>

uint16_t crc16ccitt(uint16_t crc,const void *in_data,uint32_t len);
uint16_t crc16_ccitt(const uint8_t *buff, uint16_t length);

#endif

