#ifndef __CLM920_YV9V2_H
#define __CLM920_YV9V2_H

#include "user_main.h"
#define LOCK_NUM 0x00   //从锁数量 0x01，0x02
#define START 0x01
#define STOP  0x00

typedef enum 
{    
AT,
CFUN_ASK,
CFUN_0,
CFUN_1,
CFUN_4,
CGATT_ASK,
CSQ,
CGSN,
CIMI,
CEREG_ASK,
QIPCSGP,			//注册服务
QIPACT, 			//激活服务
QIPOPEN,			//启动连接
}Typedef_AT;


void AT_Command_Send(Typedef_AT AT_COM);
void UDP_Data_Send(uint8_t len);

void WAKE_UP(void);
void RESET_NB(void);

void Start_Lock_Send(void);
void Open_Lock_Send(void);
void Tick_Lock_Send(void);
void Open_Lock_Data_Send(void);
void Open_Lock_Data_Send_Moto(void);


extern uint8_t send_count;							 //发送计数
extern uint8_t lock_state[LOCK_NUM+1];  //锁状态存储
extern uint8_t C0_lock_state[LOCK_NUM+1];

#endif 


























