#ifndef __LPM2100_N_H
#define __LPM2100_N_H

#include "user_main.h"
#define LOCK_NUM 0x00   //�������� 0x01��0x02
#define START 0x01
#define STOP  0x00

typedef enum 
{
ATQ0,       //
CTM2MSETPM,
CTM2MREG,
ECPCFG,
CPSMS,
ECPMUCFG,
CSQ,    //�ź�
CGSN,		//����
AT_SLEEP,
AECPMUCFG,
}Typedef_AT;


void AT_Command_Send(Typedef_AT AT_COM);

void WAKE_UP(void);

void Start_Lock_Send(void);
void Open_Lock_Send(void);
void Tick_Lock_Send(void);
void Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state);


extern uint8_t send_count;							 //���ͼ���
extern uint8_t lock_state[LOCK_NUM+1];  //��״̬�洢

#endif 


























