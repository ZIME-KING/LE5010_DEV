#include "user_main.h"
#include "user_function.h"
#include <string.h>
uint8_t send_count;

#define LOCK_NUM 0x00;  //�������� 0x01��0x02
const uint8_t	Frame_header[2]={0x58,0x59};

static void test_delay(){
		int i=65535;
		while(i--);
}
void User_Init(){
	   HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"OK\r\n",5);
		 Start_Lock_Send();
			test_delay();
	
		 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"\r\nOK\r\n",7);
		 Open_Lock_Send();
			test_delay();

		 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"\r\nOK\r\n",7);
		 Tick_Lock_Send();
		 test_delay(); 
	
		 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"\r\nOK\r\n",7);
		 Open_Lock_Data_Send(1,1);
		 test_delay(); 
}
//hex תchar���
//����ַ�����Ϊ�����ַ�����2��
void hex2string(uint8_t *IN_DATA,uint8_t *OUT_DATA,uint16_t len){
	int i = 0;
	char newchar[10] = {0};
	for (i = 0; i< len; i++)
	{
	    sprintf(newchar,"%02X ", *(IN_DATA+i));
			memcpy( OUT_DATA+i*2, &newchar[0], 2);
	}
}
//���ź�ǿ�Ȼ�ȡ
uint8_t Get_dBm(){
	uint8_t x;
	return x;
}
//��ȡ��ص��� 0~100
uint8_t Get_Vbat_val(){
	uint8_t x;
	return x;
}

enum{
ATQ0,       //
CTM2MSETPM,
CTM2MREG,
ECPCFG,
CPSMS,
ECPMUCFG,
}AT_COM;

//AT�����
void AT_Command_Send(AT_COM){
switch(AT_COM){
	case ATQ0:
				 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"ATQ0",sizeof("ATQ0"));
	break;
	case CTM2MSETPM:
				 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+ CTM2MSETPM =221.229.214.202,5683,38400",sizeof("AT+ CTM2MSETPM =221.229.214.202,5683,38400"));
	break;
	case CTM2MREG:
				 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+CTM2MREG",sizeof("AT+CTM2MREG"));
	break;
	case ECPCFG:
				 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+ECPCFG=\"slpWaitTime\",5000",sizeof("AT+ECPCFG=\"slpWaitTime\",5000"));
	break;
	case CPSMS:
				 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+CPSMS=1",sizeof("AT+CPSMS=1"));
	break;
	case ECPMUCFG:
				 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+ECPMUCFG=1,4",sizeof("AT+ECPMUCFG=1,4"));
	break;
}
//test_delay();
}
//01���� ������
void Start_Lock_Send(){
	uint8_t RX_BUF[50];
	uint8_t F_RX_BUF[100];
	uint8_t DATA_BUF[16];
	uint16_t temp;

	DATA_BUF[0] = Frame_header[0];//֡ͷ
	DATA_BUF[1] = Frame_header[1];//֡ͷ
	
	DATA_BUF[2] = 0X11;		//����17
	DATA_BUF[3] = send_count;     //����ID  ����һ��++ 0~255
	DATA_BUF[4] = 0X01;   //������  ����
	
	DATA_BUF[5] = 0XA0;   //��Ӳ���汾��
	DATA_BUF[6] = 0X00;
	
	DATA_BUF[7] = 0XA1;		//���̼��汾��
	DATA_BUF[8] = 0X00;
	
	DATA_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
	DATA_BUF[10] = 0X00;

	DATA_BUF[10] = 0XA6;		//֧�ֵĴ�������
	DATA_BUF[11] = 0X00;

	DATA_BUF[12] = 0XB0; 	//���� 0~100%;
	DATA_BUF[13] = 0X32;	//50%
	
	temp=CRC16_8005Modbus(&DATA_BUF[0],13);
	DATA_BUF[14]=(temp&0xff00) >>8;
	DATA_BUF[15]=temp&0xff;

	hex2string(DATA_BUF,RX_BUF,16);
	RX_BUF[16]='\0';
	sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s",(char*)RX_BUF);
	HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}
//10������   Я������ͬ����    �����������20����û���յ��������ظ�������Ҫ�ٴη���������ָ��������6��û���յ�����������ָ�����������
void Open_Lock_Send(){
	uint8_t RX_BUF[50];
	uint8_t F_RX_BUF[100];
	uint8_t DATA_BUF[16];
	uint16_t temp;

	static uint16_t temp2;

	
	DATA_BUF[0] = Frame_header[0];//֡ͷ
	DATA_BUF[1] = Frame_header[1];//֡ͷ
	
	DATA_BUF[2] = 0X11;		//����17
	DATA_BUF[3] = send_count;   //����ID  ����һ��++ 0~255
	DATA_BUF[4] = 0X10;   //������  ������
	
	DATA_BUF[5] = 0XA0;   //��Ӳ���汾��
	DATA_BUF[6] = 0X00;
	
	DATA_BUF[7] = 0XA1;		//���̼��汾��
	DATA_BUF[8] = 0X00;
	
	DATA_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
	DATA_BUF[10] = 0X00;

	DATA_BUF[10] = 0XA6;		//֧�ֵĴ�������
	DATA_BUF[11] = 0X00;

	DATA_BUF[12] = 0XB0; 	//���� 0~100%;
	DATA_BUF[13] = 0X32;	//50%
	
	temp=CRC16_8005Modbus(&DATA_BUF[0],13);
	DATA_BUF[14]=(temp&0xff00) >>8;
	DATA_BUF[15]=temp&0xff;

	hex2string(DATA_BUF,RX_BUF,16);
	RX_BUF[16]='\0';
	sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s",(char*)RX_BUF);
	HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}


//02 ���� RTC��ʱ���ѷ������� ���ϴ���ƽ̨�ظ�����ͬ����
void Tick_Lock_Send(){
	uint8_t RX_BUF[50];
	uint8_t F_RX_BUF[100];
	uint8_t DATA_BUF[16];
	uint16_t temp;

	DATA_BUF[0] = Frame_header[0];//֡ͷ
	DATA_BUF[1] = Frame_header[1];//֡ͷ
	
	DATA_BUF[2] = 0X11;		//����17
	DATA_BUF[3] = send_count;   //����ID  ����һ��++ 0~255
	DATA_BUF[4] = 0X02;   //������  ����
	
	DATA_BUF[5] = 0XA0;   //��Ӳ���汾��
	DATA_BUF[6] = 0X00;
	
	DATA_BUF[7] = 0XA1;		//���̼��汾��
	DATA_BUF[8] = 0X00;
	
	DATA_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
	DATA_BUF[10] = 0X00;

	DATA_BUF[10] = 0XA6;		//֧�ֵĴ�������
	DATA_BUF[11] = 0X00;

	DATA_BUF[12] = 0XB0; 	//���� 0~100%;
	DATA_BUF[13] = 0X32;	//50%
	
	temp=CRC16_8005Modbus(&DATA_BUF[0],13);
	DATA_BUF[14]=(temp&0xff00) >>8;
	DATA_BUF[15]=temp&0xff;

	hex2string(DATA_BUF,RX_BUF,16);
	hex2string(DATA_BUF,RX_BUF,16);
	RX_BUF[16]='\0';
	sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s",(char*)RX_BUF);
	HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}
//20��Ϣ�ϱ����� ��ID�ţ�������״̬
void Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state){
	uint8_t RX_BUF[50];
	uint8_t F_RX_BUF[100];
	uint8_t DATA_BUF[16+2];
	uint16_t temp;

	DATA_BUF[0] = Frame_header[0];//֡ͷ
	DATA_BUF[1] = Frame_header[1];//֡ͷ
	
	DATA_BUF[2] = 0X11;		//����17
	DATA_BUF[3] = send_count;     //����ID  ����һ��++ 0~255
	DATA_BUF[4] = 0X20;   //������  ����
	
	DATA_BUF[5] = 0XA0;   //��Ӳ���汾��
	DATA_BUF[6] = 0X00;
	
	DATA_BUF[7] = 0XA1;		//���̼��汾��
	DATA_BUF[8] = 0X00;
	
	DATA_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
	DATA_BUF[10] = 0X00;

	DATA_BUF[10] = 0XA6;		//֧�ֵĴ�������
	DATA_BUF[11] = LOCK_NUM;

	DATA_BUF[12] = 0XB0; 	//���� 0~100%;
	DATA_BUF[13] = 0X32;	//50%
	
	DATA_BUF[14] = lock_ID; 	  //lock_ID
	DATA_BUF[15] = lock_state;	//lock_state
	
	temp=CRC16_8005Modbus(&DATA_BUF[0],15);
	DATA_BUF[16]=(temp&0xff00) >>8;
	DATA_BUF[17]=temp&0xff;


	hex2string(DATA_BUF,RX_BUF,16+2);
	RX_BUF[16+2]='\0';
	sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s",(char*)RX_BUF);
	HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}

//�����б�
enum{
T0,       //���������ϱ�
T1,				//������������
T2,				//����������
T3,				//״̬������ݷ���
T4,				//��������
T5,				//��ѯ�ź�
}Task_Table;
#define READ 0
#define SEND 1

//����Ӧ��
enum{
OK_AT,    //
ERROE_AT, //
OPEN_LOCK,//�������·�����
OK_ASK,   //����������Ӧ
NO_ASK,		//����������Ӧ
TIME_OUT,		//����������Ӧ
}READ_STATE;

uint8_t Get_Uart_Data_Processing_Result(){
		int x;
		return x;
}

void Send_Task(Task_Table){
	switch(Task_Table){
	case T0:
				Start_Lock_Send();
	break;
	
	case T1:
				
	break;
	
	case T2:
				
	break;
	
	case T3:
				 
	break;
	
	case T4:
				 
	break;
	
	case T5:
				
	break;
	}
}
void READ_Task(Task_Table){
	uint8_t temp;
	temp=Get_Uart_Data_Processing_Result();
	switch(Task_Table){
	case T0:
			if(temp==)
	break;
	
	case T1:
				
	break;
	
	case T2:
				
	break;
	
	case T3:
				 
	break;
	
	case T4:
				 
	break;
	
	case T5:
				
	break;
	}	
}
uint8_t task;
	//100ms��һ��
void Time_Loop_Task(){
	static uint8_t state;
	if(state==READ){
		
		READ_Task(task);
	}
	else if(state==SEND){
		Send_Task(task);
		state=READ;
	}
	
}