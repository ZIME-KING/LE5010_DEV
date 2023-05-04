#ifndef __LPM2100_N_H
#define __LPM2100_N_H

#include "user_main.h"
#define LOCK_NUM 0x00   //�������� 0x01��0x02
#define START 0x01
#define STOP  0x00

typedef enum 
{
AT,
ATQ0,       //
CFUN_R,
CFUN,
CGATT_R,
CGATT,
CTM2MSETPM,
CTM2MREG,
//ECPCFG,
CPSMS,
ECPMUCFG,
CSQ,    //�ź�
CGSN,		//�豸��
AT_SLEEP,
CIMI,
CEREG_R,
CPSMS_R,
CEREG,
CTM2MUPDATE,	
CCID
//CPSMS_SET,
//AECPMUCFG,
}Typedef_AT;


void Json_init();

void AT_Command_Send(Typedef_AT AT_COM);

void WAKE_UP(void);
void RESET_NB(void);

void Start_Lock_Send(void);
void Open_Lock_Send(void);
void Tick_Lock_Send(void);
void Open_Lock_Data_Send(void);
void Open_Lock_Data_Send_Moto(void);
void Reply_send(void);

extern uint8_t send_count;							 //���ͼ���
extern uint8_t lock_state[LOCK_NUM+1];  //��״̬�洢
extern uint8_t C0_lock_state[LOCK_NUM+1];

#endif 

























