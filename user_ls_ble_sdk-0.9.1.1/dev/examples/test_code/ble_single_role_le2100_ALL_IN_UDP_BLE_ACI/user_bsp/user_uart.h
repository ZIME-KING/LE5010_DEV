#ifndef __USER_UART_H
#define __USER_UART_H

#include "user_main.h"

#define UART_BUFFER_LENTH	255	//帧数据缓存区长度
#define FRAME_QUANTITY  	2   //帧缓存数量
#define FRAME_TIMEOUT   	5   //超时
#define BUSY            	1
#define FREE				      0

typedef struct {
unsigned char buffer[UART_BUFFER_LENTH];	//帧数据缓存区
unsigned char status;       				//帧数据状态
unsigned char length; 						//帧长度
}Frame_Typedef;

typedef struct {
unsigned char status;	    		//串口接收帧状态  0闲 1忙
unsigned char frame_id;	    	//当前处理的帧id
unsigned char time_out;	    	//超时计数
}Uart_Frame_Typedef;

extern Uart_Frame_Typedef uart1;
extern Frame_Typedef frame[FRAME_QUANTITY];     		//开2个帧缓存

extern unsigned char uart_frame_id;
extern uint8_t uart_buffer[2];
//void Uart_Data_Processing(void);
void Uart_Time_Even(void);
void Receive_Interrupt(void);


extern Uart_Frame_Typedef uart_2;
extern Frame_Typedef frame_2[FRAME_QUANTITY];     		//开2个帧缓存

extern unsigned char uart_2_frame_id;
extern uint8_t uart_2_buffer[2];
void Uart_2_Data_Processing(void);
void Uart_2_Time_Even(void);
void Uart_2_Receive_Interrupt(void);




void UART_Transmit_Str(UART_HandleTypeDef *huart,uint8_t *p);

#endif
