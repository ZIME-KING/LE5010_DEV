#ifndef __LPM2100_N_H
#define __LPM2100_N_H

#include "user_main.h"
#define LOCK_NUM 0x00   //从锁数量 0x01，0x02
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
ATE1,    //开启回显
CSQ,     //信号
CGSN,		 //设备号
AT_SLEEP,
CIMI,
CEREG_R,
CPSMS_R,
CEREG,
//CPSMS_SET,
//AECPMUCFG,
SKTCREATE ,
SKTCONNECT, //连接远程服务器和端口	 
SKTDELETE, //删除SKTCREATE
}Typedef_AT;


void AT_Command_Send(Typedef_AT AT_COM);

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


























