#ifndef __USER_RC522_FUNCTION_H
#define __USER_RC522_FUNCTION_H

#include "user_main.h"
#define  KEYA        0x60
#define  REQ_ALL     0x52


typedef struct{
  unsigned char TagType[2];    //卡类型
  unsigned char buf[16];       //读出的数据
  char snr;                    //要操作的扇区号
}Card_TypeDef;

extern Card_TypeDef  M1_Card;

void User_Mfrc522(Card_TypeDef *card,char snr);

#endif