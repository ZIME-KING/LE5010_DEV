#ifndef __AES_128_H
#define __AES_128_H

#include "user_main.h"

void aes_jia_mi(void);
void aes_jie_mi(void);

void User_Decoden(unsigned char *in_data,unsigned char *out_data , unsigned char len);
void User_Encryption(unsigned char *in_data,unsigned char *out_data , unsigned char len);

#endif 



























