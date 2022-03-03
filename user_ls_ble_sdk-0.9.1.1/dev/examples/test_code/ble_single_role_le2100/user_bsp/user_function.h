#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "user_main.h"

//接收应答
typedef enum{
OK_AT,     //
ERROE_AT,  //
OPEN_LOCK, //服务器下发开锁
OK_ASK,    //服务器有响应
NO_ASK,		 //服务器无响应
TIME_OUT,	 //超时未相应
CGSN_OK,
CSQ_OK,
		
}NB_RX_STATE;

//任务列表
typedef enum {
START_LOCK_SEND,     		//启动数据上报
OPEN_LOCK_SEND,  				//开锁数据请求
TICK_LOCK_SEND, 				//心跳包发送
OPEN_LOCK_DATA_SEND,    // 20信息上报
SLEEP,		 							//休眠
GET_DATA,	 							//获取NB模块各项参数
}Typedef_TASK_LIST;


 void User_Init(void);
 void Start_Lock_Send(void);
 void Open_Lock_Send(void);
 void Tick_Lock_Send(void);
 void Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state);
 void State_Change_Task(void);
 void Uart_Data_Processing(void);
 uint8_t Get_Uart_Data_Processing_Result(void);


 uint8_t Get_Task_State(Typedef_TASK_LIST TASK_LIST);
 void  Set_Task_State(Typedef_TASK_LIST TASK_LIST,uint8_t state);

uint16_t Start_Lock_Send_Task(void);
uint16_t Open_Lock_Send_Task(void);
uint16_t Tick_Lock_Send_Task(void);
uint16_t Open_Lock_Data_Send_Task(void);

extern tinyfs_dir_t ID_dir;

void AT_GET_DATA(void);
uint16_t Get_Vbat_val(void);
uint8_t Get_dBm(void);
void LED_TASK(void);
void Get_Vbat_Task(void);
//void NB_WAKE_Task(void);
void Sleep_Task(uint16_t time_s);
#endif
