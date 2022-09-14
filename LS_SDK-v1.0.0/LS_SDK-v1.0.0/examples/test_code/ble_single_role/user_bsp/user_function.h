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
CGSN_OK,	 //
CSQ_OK,		 //
CIMI_OK,
CIMI_ERROR,
//CSQ_ERROR,		 //		
}RX_STATE;

//任务列表
typedef enum {
START_LOCK_SEND,     		//启动数据上报
OPEN_LOCK_SEND,  				//开锁数据请求
TICK_LOCK_SEND, 				//心跳包发送
OPEN_LOCK_DATA_SEND,    // 20信息上报
GET_DB_VAL,   				 	//获取信号强度
OPEN_LOCK_DATA_SEND_MOTO,
	
GET_MODE_VAL,  //获取模式（测试程序用任务）
GET_EMIC_VAL,  //获取EMIC任务（测试程序用任务）
TEST_GET_DB_VAL
}Typedef_TASK_LIST;


 void User_Init(void);
// void Start_Lock_Send(void);
// void Open_Lock_Send(void);
// void Tick_Lock_Send(void);
// void Open_Lock_Data_Send(void);
 void State_Change_Task(void);

 void Uart_Data_Processing(void);
 uint8_t Get_Uart_Data_Processing_Result(void);

 uint8_t Get_Task_State(Typedef_TASK_LIST TASK_LIST);
 void  Set_Task_State(Typedef_TASK_LIST TASK_LIST,uint8_t state);

uint16_t Start_Lock_Send_Task(void);
uint16_t Open_Lock_Send_Task(void);
uint16_t Tick_Lock_Send_Task(void);
uint16_t Open_Lock_Data_Send_Task(void);
uint16_t Open_Lock_Data_Send_Moto_Task(void);

uint16_t AT_GET_DB_TASK(void);

uint16_t Get_Vbat_val(void);
uint8_t  Get_dBm(void);

void LED_TASK(void);
void Get_Vbat_Task(void);
void NB_WAKE_Task(void);
void Sleep_Task(void);
void Set_Sleep_Time(uint16_t time_s);
void Read_Last_Data(void); 
uint8_t UDP_INIT(void);
void Password_Task(void);
void Key_Task(void);

void Once_Send(void);

uint16_t AT_INIT(void);
uint16_t AT_User_Set(void);
uint16_t AT_User_Reply(void);

extern uint8_t AT_tset_flag;
void AT_User_Reply_Task(void);
void AT_User_Set_Task(void);

extern tinyfs_dir_t ID_dir_1;
extern tinyfs_dir_t ID_dir_2;
extern tinyfs_dir_t ID_dir_3;

extern uint8_t last_lock_state_0;
extern uint8_t last_lock_state_1;


extern uint8_t AT_RX_DATA_BUF[50];  			//保存接受到回复信息  +NNMI:2,A101 ->0xA1,0x01
extern uint8_t Db_val;  

extern uint8_t user_count;






extern uint8_t T0_enable;  //启动数据上报  			Start_Lock_Send_Task
extern uint8_t T1_enable;	//开锁数据请求				Open_Lock_Send
extern uint8_t T2_enable;	//心跳包发送					Tick_Lock_Send
extern uint8_t T3_enable;	//状态变更数据发送  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)
extern uint8_t T4_enable;  //获取信号强度				AT指令相关
extern uint8_t T5_enable;  //电机动作完成发送包




#endif

//uint8_t open_lock_reply_Result=0;
//uint8_t tick_reply_Result=0;
//uint8_t open_lock_data_reply_Result=0;
//uint8_t start_lock_reply_Result=0;
