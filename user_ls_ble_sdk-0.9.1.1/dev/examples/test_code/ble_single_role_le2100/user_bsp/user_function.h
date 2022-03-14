#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "user_main.h"

//����Ӧ��
typedef enum{
OK_AT,     //
ERROE_AT,  //
OPEN_LOCK, //�������·�����
OK_ASK,    //����������Ӧ
NO_ASK,		 //����������Ӧ
TIME_OUT,	 //��ʱδ��Ӧ
CGSN_OK,	 //
CSQ_OK,		 //
//CSQ_ERROR,		 //		
}NB_RX_STATE;

//�����б�
typedef enum {
START_LOCK_SEND,     		//���������ϱ�
OPEN_LOCK_SEND,  				//������������
TICK_LOCK_SEND, 				//����������
OPEN_LOCK_DATA_SEND,    // 20��Ϣ�ϱ�
GET_DB_VAL,   				 	//��ȡ�ź�ǿ��
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
uint16_t AT_GET_DB_TASK(void);

uint16_t Get_Vbat_val(void);
uint8_t  Get_dBm(void);

uint16_t AT_INIT(void);
void LED_TASK(void);
void Get_Vbat_Task(void);
void NB_WAKE_Task(void);
void Sleep_Task();
void Set_Sleep_Time(uint16_t time_s);
//void AT_GET_DB(void);
void Read_Last_Data(void); 

extern tinyfs_dir_t ID_dir;
extern uint8_t last_lock_state;

#endif

//uint8_t open_lock_reply_Result=0;
//uint8_t tick_reply_Result=0;
//uint8_t open_lock_data_reply_Result=0;
//uint8_t start_lock_reply_Result=0;
