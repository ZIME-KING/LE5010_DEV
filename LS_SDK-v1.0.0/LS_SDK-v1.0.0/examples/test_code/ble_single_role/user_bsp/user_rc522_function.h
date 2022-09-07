#ifndef __USER_RC522_FUNCTION_H
#define __USER_RC522_FUNCTION_H

#include "user_main.h"
#define  KEYA        0x60
#define  REQ_ALL     0x52


typedef struct{
  unsigned char TagType[2];    //������
  unsigned char buf[16];       //����������
  char snr;                    //Ҫ������������
}Card_TypeDef;

extern Card_TypeDef  M1_Card;

void User_Mfrc522(Card_TypeDef *card,char snr);

#endif