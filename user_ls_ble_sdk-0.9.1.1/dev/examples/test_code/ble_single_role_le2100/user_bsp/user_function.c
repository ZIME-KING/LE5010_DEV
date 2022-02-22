#include "user_main.h"
#include "user_function.h"
#include <string.h>
void User_Init(){
	 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"usart_OK",8);
}
//�������ݵ�ֵ
const uint8_t	Frame_header[2]={0x58,0x59};
uint8_t	Frame_len; 	 				//ͨѶ֡����	
uint8_t	Frame_ID;   				//����ID
uint8_t	Frame_Function_ID;  //������
uint8_t	Frame_Data[10];     //���ݻ������10 byte
uint8_t	Frame_CRC[2];     	//���ݻ���,CRC16


/*************************************************
Function: 		hex2string
Description: 	hex�ַ�����ת����string�ַ���������printf��ʾ��
Input: 			hex:ԭ�ַ�����
                len:hex����ĳ���
Output: 		ascII:������ַ���
                newlen:������ַ�������
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
//��������������������������������
//��Ȩ����������ΪCSDN�����������ͻ�����ԭ�����£���ѭCC 4.0 BY-SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
//ԭ�����ӣ�https://blog.csdn.net/weixin_43727672/article/details/104532443


//���ź�ǿ�Ȼ�ȡ
void _f(){

}
//������Ϣ�ϱ���
void Lock_POST(){
	
	char F_RX_BUF[100];
	uint8_t RX_BUF[16+1];
	uint16_t temp;

	
	RX_BUF[0] = Frame_header[0];//֡ͷ
	RX_BUF[1] = Frame_header[1];//֡ͷ
	
	RX_BUF[2] = 0X11;		//����17
	RX_BUF[3] = 0X07;   //����ID  ����һ��++ 0~255
	RX_BUF[4] = 0X01;   //������  ����
	
	RX_BUF[5] = 0XA0;   //��Ӳ���汾��
	RX_BUF[6] = 0X00;
	
	RX_BUF[7] = 0XA1;		//���̼��汾��
	RX_BUF[8] = 0X00;
	
	RX_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
	RX_BUF[10] = 0X00;

	RX_BUF[10] = 0XA6;		//֧�ֵĴ�������
	RX_BUF[11] = 0X00;

	RX_BUF[12] = 0XB0; 	//���� 0~100%;
	RX_BUF[13] = 0X32;	//50%

	temp=CRC16_8005Modbus(&RX_BUF[0],13);
	RX_BUF[14]=(temp&0xff00) >>8;
	RX_BUF[15]=temp&0xff;
	RX_BUF[15]='\0';

	sprintf(&F_RX_BUF[0],"AT+CTM2MSEND=%s",&RX_BUF[0]);
	
	
	HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"usart_OK",8);
}
//  Я������ͬ����  �����������20����û���յ��������ظ�������Ҫ�ٴη���������ָ��������6��û���յ�����������ָ�����������
void Lock_POST_(){

}