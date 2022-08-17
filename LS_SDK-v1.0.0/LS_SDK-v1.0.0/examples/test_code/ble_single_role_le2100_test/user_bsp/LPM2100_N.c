#include "user_main.h"
#include "LPM2100_N.h"

#define LOCK_NUM 0x00   //从锁数量 0x01，0x02
#define START 0x01
#define STOP  0x00

#define VER_0  0xA0
#define VER_1  0xA0

uint8_t send_count;							 //发送计数
uint8_t lock_state[LOCK_NUM+1];  //锁状态存储
const uint8_t	Frame_header[2]= {0x58,0x59};

void WAKE_UP(){
		io_cfg_output(PA13);               //输出模式                     
    io_pull_write(PA13, IO_PULL_UP);   //设置上拉  
		io_write_pin(PA13,0);  
		DELAY_US(10*1000);
		io_write_pin(PA13,1);             
		DELAY_US(10*1000);
		io_write_pin(PA13,0);
}

void RESET_NB(){
		io_cfg_output(PA05);               //输出模式                     
    io_pull_write(PA05, IO_PULL_UP);   //设置上拉  
		io_write_pin(PA05,0);  
		DELAY_US(10*1000);
		io_write_pin(PA05,1);             
		DELAY_US(10*1000);
		io_write_pin(PA05,0);
}

//hex 转char输出
//输出字符长度为输入字符长度2倍
void hex2string(uint8_t *IN_DATA,uint8_t *OUT_DATA,uint16_t len) {
    int i = 0;
    char newchar[10] = {0};
    for (i = 0; i< len; i++)
    {
        sprintf(newchar,"%02X ", *(IN_DATA+i));
        memcpy( OUT_DATA+i*2, &newchar[0], 2);
    }
}
//AT命令发送
void AT_Command_Send(Typedef_AT AT_COM) {
    switch(AT_COM) {
		case AT:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT\r\n",sizeof("AT\r\n"),100);
        break;
    case ATQ0:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"ATQ0\r\n",sizeof("ATQ0\r\n"),100);
        break;
		case CFUN_R: //功能设置
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CFUN?\r\n",sizeof("AT+CFUN?\r\n"),100);
        break;
			
		case CFUN:		//功能设置
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CFUN=1",sizeof("AT+CFUN=1"),100);
        break;
		
		case CGATT_R:  //附着查询
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CGATT?",sizeof("AT+CGATT?"),100);
        break;
		case CGATT:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CGATT=1",sizeof("AT+CGATT=1"),100);
    break;

    case CTM2MSETPM:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\n",sizeof("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\n"),100);
        break;
    case CTM2MREG:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CTM2MREG\r\n",sizeof("AT+CTM2MREG\r\n"),100);
        break;
    case CPSMS:
        //HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CPSMS=1\r\n",sizeof("AT+CPSMS=1\r\n"),50);
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CPSMS=1,,,\"01000001\",\"00000101\"\r\n",sizeof("AT+CPSMS=1,,,\"10100011\",\"00000101\"\r\n"),100);
				break;
    case ECPMUCFG:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+ECPMUCFG=1,4\r\n",sizeof("AT+ECPMUCFG=1,4\r\n"),100);
        break;
    case CSQ:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CSQ\r\n",sizeof("AT+CSQ\r\n"),100);
        break;
    case CGSN:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CGSN=1\r\n",sizeof("AT+CGSN=1\r\n"),100);
        break;
		case AT_SLEEP:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+ECPCFG=\"slpWaitTime\",15000\r\n",sizeof("AT+ECPCFG=\"slpWaitTime\",15000\r\n"),100);
        break;	
		case CIMI:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CIMI\r\n",sizeof("AT+CIMI\r\n"),100);
        break;	
		case CEREG_R:  //查注册状态 
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CEREG?\r\n",sizeof("AT+CEREG?\r\n"),100);
     break;	
				case CPSMS_R: //查psm设置
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CPSMS?\r\n",sizeof("AT+CPSMS?\r\n"),100);
     break;	
		case CEREG: //设置psm数据上报
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CEREG=0\r\n",sizeof("AT+CEREG=0\r\n"),100);
     break;	
				
//		case CPSMS_SET:
////        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CPSMS=1,,,,\"00100010\"\r\n",sizeof("AT+CPSMS?\r\n"),50);
////     break;	
//		
		//AT+CGSN
	}
}
//01启动 锁发送
void Start_Lock_Send() {
    uint8_t RX_BUF[100];
    uint8_t F_RX_BUF[255];
    uint8_t DATA_BUF[40+2];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X2A;		//长度42
    DATA_BUF[3] = send_count++;   //send_count;     //事务ID  发送一次++ 0~255
    DATA_BUF[4] = 0X01;   //功能码  启动

    DATA_BUF[5] = 0XA0;   //锁硬件版本号
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//锁固件版本号
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//支持的从锁数量
    DATA_BUF[12] = 0X00;

    DATA_BUF[13] = 0XB0; 	//电量 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

    DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state
		
		DATA_BUF[17] = 0xA8;			//MAC_ADDR
		memcpy(&DATA_BUF[18],&MAC_ADDR[0],6);

		DATA_BUF[24] = 0xA7;			//MICI 卡号
		memcpy(&DATA_BUF[25],&CIMI_DATA[0],15);
		
		//MAC_ADDR[0]
		
    temp=CRC16_8005Modbus(&DATA_BUF[0],40);
    DATA_BUF[40]=(temp & 0xff00) >>8;
    DATA_BUF[41]= temp & 0xff;


    hex2string(DATA_BUF,RX_BUF,42);
    RX_BUF[84]='\0';
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("START_LOCK_SEND");
		//HAL_UART_Transmit(&UART_Config   ,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
}
//10请求开锁   携带数据同启动    请求开锁（如果20秒内没有收到服务器回复，则需要再次发送请求开锁指令，如果超过6次没有收到服务器开锁指令则放弃。）
void Open_Lock_Send() {
    uint8_t RX_BUF[50];
    uint8_t F_RX_BUF[100];
    uint8_t DATA_BUF[17+2];
    //uint8_t DATA_BUF[17];
		uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X13;		//长度19
    DATA_BUF[3] = send_count++;   //事务ID  发送一次++ 0~255
    DATA_BUF[4] = 0X10;   //功能码  请求开锁

    DATA_BUF[5] = 0XA0;   //锁硬件版本号
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//锁固件版本号
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//支持的从锁数量
    DATA_BUF[12] = 0X00;

    DATA_BUF[13] = 0XB0; 	//电量 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

    DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state

    temp=CRC16_8005Modbus(&DATA_BUF[0],17);
    DATA_BUF[17]=(temp & 0xff00) >>8;
    DATA_BUF[18]= temp & 0xff;


    hex2string(DATA_BUF,RX_BUF,19);
    RX_BUF[38]='\0';

    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("Open_Lock_Send");
	}

//02 心跳 RTC定时唤醒发送数据 锁上传和平台回复数据同启动
void Tick_Lock_Send() {
    uint8_t RX_BUF[50];
    uint8_t F_RX_BUF[100];
    uint8_t DATA_BUF[17+2];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X13;		//长度17
    DATA_BUF[3] = send_count;   //事务ID  发送一次++ 0~255
    DATA_BUF[4] = 0X02;   //功能码  心跳

    DATA_BUF[5] = 0XA0;   //锁硬件版本号
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//锁固件版本号
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//支持的从锁数量
    DATA_BUF[12] = 0X00;

    DATA_BUF[13] = 0XB0; 	//电量 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

		DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state

    temp=CRC16_8005Modbus(&DATA_BUF[0],17);
    DATA_BUF[17]=(temp & 0xff00) >>8;
    DATA_BUF[18]= temp & 0xff;

    hex2string(DATA_BUF,RX_BUF,19);
    RX_BUF[38]='\0';
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		
		
		LOG_I("Tick_Lock_Send");
}

//21 电机动作 信息上报 
void Open_Lock_Data_Send_Moto() {
    uint8_t RX_BUF[50];
    uint8_t F_RX_BUF[100];
    uint8_t DATA_BUF[17+2];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X13;		//长度17
    DATA_BUF[3] = send_count++;     //事务ID  发送一次++ 0~255
    DATA_BUF[4] = 0X21;   //功能码  启动

    DATA_BUF[5] = 0XA0;   //锁硬件版本号
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//锁固件版本号
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//支持的从锁数量
    DATA_BUF[12] = LOCK_NUM;

    DATA_BUF[13] = 0XB0; 	//电量 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

		DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state

    temp=CRC16_8005Modbus(&DATA_BUF[0],17);
    DATA_BUF[17]=(temp & 0xff00) >>8;
    DATA_BUF[18]= temp & 0xff;

    hex2string(DATA_BUF,RX_BUF,19);
    RX_BUF[38]='\0';
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("Open_Lock_Data_Send_Moto");
}


//20 信息上报输入 锁ID号，锁更新状态
void Open_Lock_Data_Send() {
    uint8_t RX_BUF[50];
    uint8_t F_RX_BUF[100];
    uint8_t DATA_BUF[17+2];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X13;		//长度17
    DATA_BUF[3] = send_count++;     //事务ID  发送一次++ 0~255
    DATA_BUF[4] = 0X20;   //功能码  启动

    DATA_BUF[5] = 0XA0;   //锁硬件版本号
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//锁固件版本号
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//支持的从锁数量
    DATA_BUF[12] = LOCK_NUM;

    DATA_BUF[13] = 0XB0; 	//电量 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

		DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state

    temp=CRC16_8005Modbus(&DATA_BUF[0],17);
    DATA_BUF[17]=(temp & 0xff00) >>8;
    DATA_BUF[18]= temp & 0xff;

    hex2string(DATA_BUF,RX_BUF,19);
    RX_BUF[38]='\0';
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("Open_Lock_Data_Send");
}

