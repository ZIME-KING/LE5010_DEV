#include "user_main.h"
#include "user_function.h"
#include <string.h>
void User_Init(){
	 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"usart_OK",8);
}
//发送数据的值
const uint8_t	Frame_header[2]={0x58,0x59};
uint8_t	Frame_len; 	 				//通讯帧长度	
uint8_t	Frame_ID;   				//事务ID
uint8_t	Frame_Function_ID;  //功能码
uint8_t	Frame_Data[10];     //数据缓存最大10 byte
uint8_t	Frame_CRC[2];     	//数据缓存,CRC16


/*************************************************
Function: 		hex2string
Description: 	hex字符数组转换成string字符串，用于printf显示等
Input: 			hex:原字符数组
                len:hex数组的长度
Output: 		ascII:输出的字符串
                newlen:输出的字符串长度
Return: 		
*************************************************/
void hex2string(char *hex,char *ascII,int len,int *newlen)
{
	int i = 0;
	char newchar[100] = {0};
	*newlen=len*3;
	for (i = 0; i< len; i++)
	{
		sprintf(newchar,"%02X ", hex[i]);
		strcat(ascII, newchar);
	}
}
//————————————————
//版权声明：本文为CSDN博主「雾里赏花」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/weixin_43727672/article/details/104532443


//锁信号强度获取
void _f(){

}
//启动信息上报、
void Lock_POST(){
	
	char F_RX_BUF[100];
	uint8_t RX_BUF[16+1];
	uint16_t temp;

	
	RX_BUF[0] = Frame_header[0];//帧头
	RX_BUF[1] = Frame_header[1];//帧头
	
	RX_BUF[2] = 0X11;		//长度17
	RX_BUF[3] = 0X07;   //事务ID  发送一次++ 0~255
	RX_BUF[4] = 0X01;   //功能码  启动
	
	RX_BUF[5] = 0XA0;   //锁硬件版本号
	RX_BUF[6] = 0X00;
	
	RX_BUF[7] = 0XA1;		//锁固件版本号
	RX_BUF[8] = 0X00;
	
	RX_BUF[9]  = 0XA2;		//锁信号强度（0-31，99）
	RX_BUF[10] = 0X00;

	RX_BUF[10] = 0XA6;		//支持的从锁数量
	RX_BUF[11] = 0X00;

	RX_BUF[12] = 0XB0; 	//电量 0~100%;
	RX_BUF[13] = 0X32;	//50%

	temp=CRC16_8005Modbus(&RX_BUF[0],13);
	RX_BUF[14]=(temp&0xff00) >>8;
	RX_BUF[15]=temp&0xff;
	RX_BUF[15]='\0';

	sprintf(&F_RX_BUF[0],"AT+CTM2MSEND=%s",&RX_BUF[0]);
	
	
	HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"usart_OK",8);
}
//  携带数据同启动  请求开锁（如果20秒内没有收到服务器回复，则需要再次发送请求开锁指令，如果超过6次没有收到服务器开锁指令则放弃。）
void Lock_POST_(){

}