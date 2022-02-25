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
}NB_RX_STATE;



//�����б�
typedef enum {
START_LOCK_SEND,     		//���������ϱ�
OPEN_LOCK_SEND,  				//������������
TICK_LOCK_SEND, 				//����������
OPEN_LOCK_DATA_SEND,    //״̬������ݷ���
SLEEP,		 							//����
GET_DATA,	 							//��ȡNBģ��������
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

uint16_t Start_Lock_Send_Task(void);
uint16_t Open_Lock_Send_Task(void);
uint16_t Tick_Lock_Send_Task(void);
uint16_t Open_Lock_Data_Send_Task(void);


#endif