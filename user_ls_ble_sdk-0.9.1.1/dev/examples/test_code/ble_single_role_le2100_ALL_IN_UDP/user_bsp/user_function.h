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
CIMI_OK,
CIMI_ERROR,
//CSQ_ERROR,		 //		
}NB_RX_STATE;

//�����б�
typedef enum {
START_LOCK_SEND,     		//���������ϱ�
OPEN_LOCK_SEND,  				//������������
TICK_LOCK_SEND, 				//����������
OPEN_LOCK_DATA_SEND,    // 20��Ϣ�ϱ�
GET_DB_VAL,   				 	//��ȡ�ź�ǿ��
OPEN_LOCK_DATA_SEND_MOTO,
	
GET_MODE_VAL,  //��ȡģʽ�����Գ���������
GET_EMIC_VAL,  //��ȡEMIC���񣨲��Գ���������
TEST_GET_DB_VAL,
SET_SKTCONNECT,
}Typedef_TASK_LIST;


uint16_t AT_Set_SKTCONNECT_Task(void);
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

void Password_Task(void);
void Key_Task(void);

void Once_Send(void);

uint16_t AT_INIT(void);
uint16_t AT_User_Set(uint8_t reset);
uint16_t AT_User_Reply(void);

extern uint8_t AT_tset_flag;
void AT_User_Reply_Task(void);
void AT_User_Set_Task(void);

extern tinyfs_dir_t ID_dir_1;
extern tinyfs_dir_t ID_dir_2;
extern tinyfs_dir_t ID_dir_3;

extern uint8_t last_lock_state;

extern uint8_t AT_RX_DATA_BUF[50];  			//������ܵ��ظ���Ϣ  +NNMI:2,A101 ->0xA1,0x01
extern uint8_t Db_val;  

extern uint8_t user_count;
#endif

//uint8_t open_lock_reply_Result=0;
//uint8_t tick_reply_Result=0;
//uint8_t open_lock_data_reply_Result=0;
//uint8_t start_lock_reply_Result=0;
