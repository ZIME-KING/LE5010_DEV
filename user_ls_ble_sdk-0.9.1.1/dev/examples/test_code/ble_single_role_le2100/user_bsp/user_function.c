#include "user_main.h"
#include "user_function.h"
#include <string.h>

#define LOCK_NUM 0x00   //�������� 0x01��0x02
#define START 0x01
#define STOP  0x00

uint8_t T0_enable;  //���������ϱ�  			Start_Lock_Send_Task
uint8_t T1_enable;	//������������				Open_Lock_Send
uint8_t T2_enable;	//����������					Tick_Lock_Send
uint8_t T3_enable;	//״̬������ݷ���  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)
uint8_t T4_enable;  //��������  					ATָ�����
uint8_t T5_enable;  //��ȡNBģ��������	ATָ�����

uint8_t send_count;							 //���ͼ���
uint8_t lock_state[LOCK_NUM+1];  //��״̬�洢
const uint8_t	Frame_header[2]= {0x58,0x59};

uint8_t	IMEI_Flag=1;
uint8_t IMEI[15]= {0};

uint8_t Open_Lock_Moto(){
	
}
uint8_t Close_Lock_Moto(){

}


static void test_delay() {
    int i=65535;
    while(i--);
}

void  AT_TEST() {
    //HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+CGSN=1\r\n",sizeof("AT+CGSN=1\r\n"));//�������豸��IMEI
    //�ظ�  +CGSN:"866804052864384"
    test_delay();
	
    IMEI_Flag=1;
    HAL_UART_Transmit_IT(&UART_Config_AT,(unsigned char*)"AT+CIMI\r\n",sizeof("AT+CIMI\r\n"));

    //�ظ�46011316372 8 7 1 9
    //HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"OK\r\n",5);
    //HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"OK\r\n",5);
}

void User_Init() {
	
	//{0xc6,0x53,0x0c,0x1d,0x64,0x30,0xff,0xB8,0x6D,0x17,0x28,0xa5,0x86,0xdd,0x3c,0x1c};
	static uint8_t user_tmpe[16]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10};
	static uint8_t user_tmpe_2[16]={0xc6,0x53,0x0c,0x1d,0x64,0x30,0xff,0xB8,0x6D,0x17,0x28,0xa5,0x86,0xdd,0x3c,0x1c};
	
	User_Encryption(user_tmpe,user_tmpe,16);
	    test_delay();
	
	User_Decoden(user_tmpe_2,user_tmpe_2,16);
			test_delay();
//    HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"USART_OK\r\n",5);

//    Start_Lock_Send();
//    test_delay();

//    HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"\r\nOK\r\n",7);
//    Open_Lock_Send();
//    test_delay();

//    HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"\r\nOK\r\n",7);
//    Tick_Lock_Send();
//    test_delay();

//    HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"\r\nOK\r\n",7);
//    Open_Lock_Data_Send(1,1);
//    test_delay();
}
//hex תchar���
//����ַ�����Ϊ�����ַ�����2��
void hex2string(uint8_t *IN_DATA,uint8_t *OUT_DATA,uint16_t len) {
    int i = 0;
    char newchar[10] = {0};
    for (i = 0; i< len; i++)
    {
        sprintf(newchar,"%02X ", *(IN_DATA+i));
        memcpy( OUT_DATA+i*2, &newchar[0], 2);
    }
}
//���ź�ǿ�Ȼ�ȡ
uint8_t Get_dBm() {
    uint8_t x;
    return x;
}
//��ȡ��ص��� 0~100
uint8_t Get_Vbat_val() {
    uint8_t x;
    return x;
}

//AT�����
void AT_Command_Send(Typedef_AT AT_COM) {
    switch(AT_COM) {
    case ATQ0:
        HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"ATQ0\r\n",sizeof("ATQ0\r\n"));
        break;
    case CTM2MSETPM:
        HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\n",sizeof("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\n"));
        break;
    case CTM2MREG:
        HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+CTM2MREG\r\n",sizeof("AT+CTM2MREG\r\n"));
        break;
    case ECPCFG:
        HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+ECPCFG=\"slpWaitTime\",5000\r\n",sizeof("AT+ECPCFG=\"slpWaitTime\",5000\r\n"));
        break;
    case CPSMS:
        HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+CPSMS=1\r\n",sizeof("AT+CPSMS=1\r\n"));
        break;
    case ECPMUCFG:
        HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"AT+ECPMUCFG=1,4\r\n",sizeof("AT+ECPMUCFG=1,4\r\n"));
        break;
    }
//test_delay();
}
//01���� ������
void Start_Lock_Send() {
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
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}
//10������   Я������ͬ����    �����������20����û���յ��������ظ�������Ҫ�ٴη���������ָ��������6��û���յ�����������ָ�����������
void Open_Lock_Send() {
    uint8_t RX_BUF[50];
    uint8_t F_RX_BUF[100];
    uint8_t DATA_BUF[16];
    uint16_t temp;

//	static uint16_t temp2;


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
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}


//02 ���� RTC��ʱ���ѷ������� ���ϴ���ƽ̨�ظ�����ͬ����
void Tick_Lock_Send() {
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
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}
//20��Ϣ�ϱ����� ��ID�ţ�������״̬
void Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state) {
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
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit_IT(&UART_Config,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1);
}
/********************
 **���ݴ���, һ֡������� ����һ��
//�ڽ����жϺ�����һ��
******************/
void Uart_Data_Processing() {

    if(frame[uart_frame_id].status!=0) {    			//���յ����ݺ�status=1;
        HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,10);
        //���յ������� ��uart2  ͸��
        frame[uart_frame_id].status=0;					//���������ݺ�status ��0;
    }
}

void Uart_2_Data_Processing() {
    int count;
    uint8_t RX_DATA_BUF[50];  //������ܵ��ķ������ظ���Ϣ  +NNMI:2,A101 ->0xA1,0x01

			
	
    if(frame_2[uart_2_frame_id].status!=0) {    			//���յ����ݺ�status=1;
			
        HAL_UART_Transmit(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length,10);
        //���ؽ��յ������ݵ�uart1��
        if(strncmp("OK",(char*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length)==0) {
            HAL_UART_Transmit(&UART_Config,(uint8_t*)"ok \r\n",sizeof("ok \r\n"),10);
            globle_Result=OK_AT;
        }
        else if(strncmp("ERROR",(char*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length)==0) {
            HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=ERROE_AT;
        }
        else if(IMEI_Flag==1) {
            for(int iii=0; iii<frame_2[uart_2_frame_id].length-count; iii++) {
                IMEI[iii]=frame_2[uart_2_frame_id].buffer[iii]-'0';
            }
            HAL_UART_Transmit(&UART_Config,&IMEI[0],15,10);
            IMEI_Flag=0;
        }
        else if(strncmp("+NNMI",(char*)frame_2[uart_2_frame_id].buffer,5)==0) {
            for(int i=0; i<frame_2[uart_2_frame_id].length; i++) {
                if(frame_2[uart_2_frame_id].buffer[i]==',') {
                    count=i+1;
                    break;
                }
            }
            //Ĭ������Ϊ��д
            for(int i=0; i<frame_2[uart_2_frame_id].length-count; i++) {
                RX_DATA_BUF[i]=0;
                if( frame_2[uart_2_frame_id].buffer[count+i*2]<='9')
                    RX_DATA_BUF[i]+= (frame_2[uart_2_frame_id].buffer[count+i*2]-'0')<<4;
                else
                    RX_DATA_BUF[i]+=(frame_2[uart_2_frame_id].buffer[count+i*2]-'A'+10)<<4;

                if(frame_2[uart_2_frame_id].buffer[count+1+i*2]<='9')
                    RX_DATA_BUF[i]+=(frame_2[uart_2_frame_id].buffer[count+1+i*2]-'0');
                else
                    RX_DATA_BUF[i]+=(frame_2[uart_2_frame_id].buffer[count+1+i*2]-'A'+10);
            }
						 HAL_UART_Transmit(&UART_Config,&RX_DATA_BUF[0],(frame_2[uart_2_frame_id].length-count)/2,10);

            //��ǡ���������������Ӧ��Ҫ��CRC��
            //OK_ASK
            if(RX_DATA_BUF[0]==0x58 && RX_DATA_BUF[1]==0x59 && RX_DATA_BUF[5]==0x00) {
                globle_Result=OK_ASK;
            }
            //OPEN_LOCK
            else if(RX_DATA_BUF[0]==0x00) {
                globle_Result=OPEN_LOCK;
                lock_state[0]=RX_DATA_BUF[0];
                //lock_state[1]=1
                //lock_state[1]=1
            }
        }
        //HAL_UART_Transmit(&UART_Config,&frame_2[uart_frame_id].buffer[count],frame_2[uart_frame_id].length-count,10);
        //HAL_UART_Transmit(&UART_Config,&RX_DATA_BUF[0],(frame_2[uart_frame_id].length-count)/2,10);  //
        frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
    }
}




uint8_t Get_Uart_Data_Processing_Result() {
    return globle_Result;
}
uint8_t Get_Task_State(Typedef_TASK_LIST TASK_LIST) {
    uint8_t temp;
    switch(TASK_LIST) {
    case START_LOCK_SEND :
        temp=	T0_enable;
        break;
    case OPEN_LOCK_SEND :
        temp=	T1_enable;
        break;
    case TICK_LOCK_SEND :
        temp=	T2_enable;
        break;
    case OPEN_LOCK_DATA_SEND :
        temp=	T3_enable;
        break;
    case GET_DATA :
        temp=	T4_enable;
        break;
    case SLEEP :
        temp=	T5_enable;
        break;
    }
    return temp;
}
void Set_Task_State(Typedef_TASK_LIST TASK_LIST,uint8_t state) {
    switch(TASK_LIST) {
    case START_LOCK_SEND :
        T0_enable=state;
        break;
    case OPEN_LOCK_SEND :
        T1_enable=state;
        break;
    case TICK_LOCK_SEND :
        T2_enable=state;
        break;
    case OPEN_LOCK_DATA_SEND :
        T3_enable=state;
        break;
    case GET_DATA :
        T4_enable=state;
        break;
    case SLEEP :
        T5_enable=state;
        break;
    }
}



uint16_t Start_Lock_Send_Task() {
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%100==0) {
        if(Get_Task_State(START_LOCK_SEND)) {
            if(Get_Uart_Data_Processing_Result()==OK_ASK) {
                send_count++;
                temp=OK_ASK;
                Set_Task_State(START_LOCK_SEND,STOP);
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(START_LOCK_SEND,START);

                Start_Lock_Send();
                test_delay();

                if(count%5==0) {
                    Set_Task_State(START_LOCK_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}

uint16_t Open_Lock_Send_Task() {
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%100==0) {
        if(Get_Task_State(OPEN_LOCK_SEND)) {
            if(Get_Uart_Data_Processing_Result()==OK_ASK) {
                send_count++;
                temp=OK_ASK;
                Set_Task_State(OPEN_LOCK_SEND,STOP);
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(OPEN_LOCK_SEND,START);

                Open_Lock_Send();
                test_delay();

                if(count%5==0) {
                    Set_Task_State(OPEN_LOCK_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}
uint16_t Tick_Lock_Send_Task() {
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%100==0) {
        if(Get_Task_State(TICK_LOCK_SEND)) {
            if(Get_Uart_Data_Processing_Result()==OK_ASK) {
                send_count++;
                temp=OK_ASK;
                Set_Task_State(TICK_LOCK_SEND,STOP);
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(TICK_LOCK_SEND,START);
                globle_Result=NO_ASK;

                Tick_Lock_Send();
                test_delay();

                if(count%5==0) {
                    Set_Task_State(TICK_LOCK_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}
uint16_t Open_Lock_Data_Send_Task() {
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%100==0) {
        if(Get_Task_State(OPEN_LOCK_DATA_SEND)) {
            if(Get_Uart_Data_Processing_Result()==OK_ASK) {
                send_count++;
                temp=OK_ASK;
                Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(OPEN_LOCK_DATA_SEND,START);

                Open_Lock_Data_Send(0,lock_state[0]);
                test_delay();

                if(count%5==0) {
                    Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}











