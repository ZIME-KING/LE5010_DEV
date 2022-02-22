#include "user_main.h"
#include "user_function.h"
#include <string.h>
uint8_t send_count;

#define LOCK_NUM 0x00;  //从锁数量 0x01，0x02
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
//hex 转char输出
//输出字符长度为输入字符长度2倍
void hex2string(uint8_t *IN_DATA,uint8_t *OUT_DATA,uint16_t len){
	int i = 0;
	char newchar[10] = {0};
	for (i = 0; i< len; i++)
	{
	    sprintf(newchar,"%02X ", *(IN_DATA+i));
			memcpy( OUT_DATA+i*2, &newchar[0], 2);
	}
}
//锁信号强度获取
uint8_t Get_dBm(){
	uint8_t x;
	return x;
}
//获取电池电量 0~100
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

//AT命令发送
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
//01启动 锁发送
void Start_Lock_Send(){
	uint8_t RX_BUF[50];
	uint8_t F_RX_BUF[100];
	uint8_t DATA_BUF[16];
	uint16_t temp;

	DATA_BUF[0] = Frame_header[0];//帧头
	DATA_BUF[1] = Frame_header[1];//帧头
	
	DATA_BUF[2] = 0X11;		//长度17
	DATA_BUF[3] = send_count;     //事务ID  发送一次++ 0~255
	DATA_BUF[4] = 0X01;   //功能码  启动
	
	DATA_BUF[5] = 0XA0;   //锁硬件版本号
	DATA_BUF[6] = 0X00;
	
	DATA_BUF[7] = 0XA1;		//锁固件版本号
	DATA_BUF[8] = 0X00;
	
	DATA_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
	DATA_BUF[10] = 0X00;

	DATA_BUF[10] = 0XA6;		//支持的从锁数量
	DATA_BUF[11] = 0X00;

	DATA_BUF[12] = 0XB0; 	//电量 0~100%;
	DATA_BUF[13] = 0X32;	//50%
	
	temp=CRC16_8005Modbus(&DATA_BUF[0],13);
	DATA_BUF[14]=(temp&0xff00) >>8;
	DATA_BUF[15]=temp&0xff;

	hex2string(DATA_BUF,RX_BUF,16);
	RX_BUF[16]='\0';
	sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s",(char*)RX_BUF);
	HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}
//10请求开锁   携带数据同启动    请求开锁（如果20秒内没有收到服务器回复，则需要再次发送请求开锁指令，如果超过6次没有收到服务器开锁指令则放弃。）
void Open_Lock_Send(){
	uint8_t RX_BUF[50];
	uint8_t F_RX_BUF[100];
	uint8_t DATA_BUF[16];
	uint16_t temp;

	static uint16_t temp2;

	
	DATA_BUF[0] = Frame_header[0];//帧头
	DATA_BUF[1] = Frame_header[1];//帧头
	
	DATA_BUF[2] = 0X11;		//长度17
	DATA_BUF[3] = send_count;   //事务ID  发送一次++ 0~255
	DATA_BUF[4] = 0X10;   //功能码  请求开锁
	
	DATA_BUF[5] = 0XA0;   //锁硬件版本号
	DATA_BUF[6] = 0X00;
	
	DATA_BUF[7] = 0XA1;		//锁固件版本号
	DATA_BUF[8] = 0X00;
	
	DATA_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
	DATA_BUF[10] = 0X00;

	DATA_BUF[10] = 0XA6;		//支持的从锁数量
	DATA_BUF[11] = 0X00;

	DATA_BUF[12] = 0XB0; 	//电量 0~100%;
	DATA_BUF[13] = 0X32;	//50%
	
	temp=CRC16_8005Modbus(&DATA_BUF[0],13);
	DATA_BUF[14]=(temp&0xff00) >>8;
	DATA_BUF[15]=temp&0xff;

	hex2string(DATA_BUF,RX_BUF,16);
	RX_BUF[16]='\0';
	sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s",(char*)RX_BUF);
	HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}


//02 心跳 RTC定时唤醒发送数据 锁上传和平台回复数据同启动
void Tick_Lock_Send(){
	uint8_t RX_BUF[50];
	uint8_t F_RX_BUF[100];
	uint8_t DATA_BUF[16];
	uint16_t temp;

	DATA_BUF[0] = Frame_header[0];//帧头
	DATA_BUF[1] = Frame_header[1];//帧头
	
	DATA_BUF[2] = 0X11;		//长度17
	DATA_BUF[3] = send_count;   //事务ID  发送一次++ 0~255
	DATA_BUF[4] = 0X02;   //功能码  心跳
	
	DATA_BUF[5] = 0XA0;   //锁硬件版本号
	DATA_BUF[6] = 0X00;
	
	DATA_BUF[7] = 0XA1;		//锁固件版本号
	DATA_BUF[8] = 0X00;
	
	DATA_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
	DATA_BUF[10] = 0X00;

	DATA_BUF[10] = 0XA6;		//支持的从锁数量
	DATA_BUF[11] = 0X00;

	DATA_BUF[12] = 0XB0; 	//电量 0~100%;
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
//20信息上报输入 锁ID号，锁更新状态
void Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state){
	uint8_t RX_BUF[50];
	uint8_t F_RX_BUF[100];
	uint8_t DATA_BUF[16+2];
	uint16_t temp;

	DATA_BUF[0] = Frame_header[0];//帧头
	DATA_BUF[1] = Frame_header[1];//帧头
	
	DATA_BUF[2] = 0X11;		//长度17
	DATA_BUF[3] = send_count;     //事务ID  发送一次++ 0~255
	DATA_BUF[4] = 0X20;   //功能码  启动
	
	DATA_BUF[5] = 0XA0;   //锁硬件版本号
	DATA_BUF[6] = 0X00;
	
	DATA_BUF[7] = 0XA1;		//锁固件版本号
	DATA_BUF[8] = 0X00;
	
	DATA_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
	DATA_BUF[10] = 0X00;

	DATA_BUF[10] = 0XA6;		//支持的从锁数量
	DATA_BUF[11] = LOCK_NUM;

	DATA_BUF[12] = 0XB0; 	//电量 0~100%;
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

//任务列表
enum{
T0,       //启动数据上报
T1,				//开锁数据请求
T2,				//心跳包发送
T3,				//状态变更数据发送
T4,				//设置休眠
T5,				//查询信号
}Task_Table;
#define READ 0
#define SEND 1

//接收应答
enum{
OK_AT,    //
ERROE_AT, //
OPEN_LOCK,//服务器下发开锁
OK_ASK,   //服务器有响应
NO_ASK,		//服务器无响应
TIME_OUT,		//服务器无响应
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
	//100ms跑一次
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