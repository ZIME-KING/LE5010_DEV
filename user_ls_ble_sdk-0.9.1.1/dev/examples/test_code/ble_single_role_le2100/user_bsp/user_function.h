#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "user_main.h"

//任务列表
typedef enum {
START_LOCK_SEND,     		//启动数据上报
OPEN_LOCK_SEND,  				//开锁数据请求
TICK_LOCK_SEND, 				//心跳包发送
OPEN_LOCK_DATA_SEND,    //状态变更数据发送
SLEEP,		 							//休眠
GET_DATA,	 							//获取NB模块各项参数
}Typedef_TASK_LIST;

typedef enum 
{
ATQ0,       //
CTM2MSETPM,
CTM2MREG,
ECPCFG,
CPSMS,
ECPMUCFG,
}Typedef_AT;

 void User_Init(void);
 void Start_Lock_Send(void);
 void Open_Lock_Send(void);
 void Tick_Lock_Send(void);
 void Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state);

 void Uart_Data_Processing(void);
 uint8_t Get_Uart_Data_Processing_Result(void);


 uint8_t Get_Task_State(Typedef_TASK_LIST TASK_LIST);
 void  Set_Task_State(Typedef_TASK_LIST TASK_LIST,uint8_t state);


#endif
