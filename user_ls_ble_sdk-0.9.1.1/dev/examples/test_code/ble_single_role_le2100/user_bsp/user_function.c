#include "user_main.h"
#include "user_function.h"
#include <string.h>

uint8_t T0_enable;  //���������ϱ�  			Start_Lock_Send_Task
uint8_t T1_enable;	//������������				Open_Lock_Send
uint8_t T2_enable;	//����������					Tick_Lock_Send
uint8_t T3_enable;	//״̬������ݷ���  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)
uint8_t T4_enable;  //��������  					ATָ�����
uint8_t T5_enable;  //��ȡNBģ��������	ATָ�����


uint8_t AT_RX_DATA_BUF[50];  			//������ܵ��ظ���Ϣ  +NNMI:2,A101 ->0xA1,0x01
uint8_t Db_val;  									//	


uint8_t Open_Lock_Moto(){
}
uint8_t Close_Lock_Moto(){
return 0;
}

static void test_delay() {
    int i=65535;
    while(i--);
}
void AT_GET_DATA();

void User_Init() {
		Button_Gpio_Init();
	  moto_gpio_init();
		Basic_PWM_Output_Cfg();
		Buzzer_ON();
		test_delay();
		test_delay();
		Buzzer_OFF();
		WAKE_UP();
//		AT_GET_DATA();
//		int i=5;
}
//���ź�ǿ�Ȼ�ȡ
uint8_t Get_dBm() {
    //uint8_t Db_val;
    return Db_val;
}
//��ȡ��ص��� 0~100
uint8_t Get_Vbat_val() {
    uint8_t x;
    return x;
}


/********************
 **���ݴ���, һ֡������� ����һ��
//�ڽ����жϺ�����һ��
******************/
void Uart_Data_Processing() {

    if(frame[uart_frame_id].status!=0) {    			//���յ����ݺ�status=1;
        HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        //���յ������� ��uart2  ͸��
        frame[uart_frame_id].status=0;					//���������ݺ�status ��0;
    }
}

void Uart_2_Data_Processing() {
    int count;
		int len;
    if(frame_2[uart_2_frame_id].status!=0){						//���յ����ݺ�status=1;
        HAL_UART_Transmit(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length,100);
        //���ؽ��յ������ݵ�uart1��
				//�յ�NNMI���ŷ���
				if( strncmp("AT+CGSN=1\r\n+CGSN:",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CGSN=1\r\n+CGSN:"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"ok \r\n",sizeof("ok \r\n"),10);
            globle_Result=CGSN_OK;
						 for(int i=0; i<frame_2[uart_2_frame_id].length; i++) {
                if(frame_2[uart_2_frame_id].buffer[i]=='"') {
                    count=i+1;
                    break;
                }
            }
            for(int i=0; i<count+15; i++) {
                AT_RX_DATA_BUF[i]=frame_2[uart_2_frame_id].buffer[count+i];
						}
						//HAL_UART_Transmit(&UART_Config,AT_RX_DATA_BUF,15,20);
				}
				//�յ��ź�ǿ��ֵ
        else if( strncmp("AT+CSQ\r\n+CSQ:",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CSQ\r\n+CSQ:"))==0)  {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);          
						globle_Result=CSQ_OK;

						if(frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+2]==','){
								AT_RX_DATA_BUF[0]=frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+1]-'0';
						}
						else{
								AT_RX_DATA_BUF[0]=frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+1]-'0'*10;
								AT_RX_DATA_BUF[0]+=frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+2]-'0';
						}
						//HAL_UART_Transmit(&UART_Config,"AAA",4,20);
        }
				//�յ��������ظ�ֵ
				
				else if( strncmp("+NNMI:",(char*)frame_2[uart_2_frame_id].buffer,strlen("+NNMI:"))==0){
						 for(int i=0; i<frame_2[uart_2_frame_id].length; i++) {
                if(frame_2[uart_2_frame_id].buffer[i]==',') {
                    count=i+1;
                    break;
                }
            }
            //Ĭ������Ϊ��д
            for(int i=0; i<frame_2[uart_2_frame_id].length-count; i++) {
                AT_RX_DATA_BUF[i]=0;
                if( frame_2[uart_2_frame_id].buffer[count+i*2]<='9')
                    AT_RX_DATA_BUF[i]+= (frame_2[uart_2_frame_id].buffer[count+i*2]-'0')<<4;
                else
                    AT_RX_DATA_BUF[i]+=(frame_2[uart_2_frame_id].buffer[count+i*2]-'A'+10)<<4;

                if(frame_2[uart_2_frame_id].buffer[count+1+i*2]<='9')
                    AT_RX_DATA_BUF[i]+=(frame_2[uart_2_frame_id].buffer[count+1+i*2]-'0');
                else
                    AT_RX_DATA_BUF[i]+=(frame_2[uart_2_frame_id].buffer[count+1+i*2]-'A'+10);
            }
						 HAL_UART_Transmit(&UART_Config,&AT_RX_DATA_BUF[0],(frame_2[uart_2_frame_id].length-count)/2,10);

            //��ǡ���������������Ӧ��Ҫ��CRC��
            //OK_ASK
            if(AT_RX_DATA_BUF[0]==0x58 && AT_RX_DATA_BUF[1]==0x59 && AT_RX_DATA_BUF[5]==0x00) {
                globle_Result=OK_ASK;
            }
            //OPEN_LOCK
            else if(AT_RX_DATA_BUF[0]==0x00) {
                globle_Result=OPEN_LOCK;
                lock_state[0]=AT_RX_DATA_BUF[0];
                //lock_state[1]=1
                //lock_state[1]=1
            }
        }
				//�յ�������Ϣ
				//else if(strpbrk((char*)frame_2[uart_2_frame_id].buffer,"ERROR")==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
         //   globle_Result=ERROE_AT;
        //}
				//�յ�OK��Ϣ ,���϶�������ֻ��OK
				else if( strncmp("ATQ0\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("ATQ0\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				else if( strncmp("ATQ0\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("ATQ0\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				else if( strncmp("AT+CTM2MREG\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CTM2MREG\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				else if( strncmp("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
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
uint16_t Start_Lock_Send_Task(){
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


//������ʼ����һ��
void AT_GET_DATA(){
	static int step=0;
	static int count=0;
	count++;	
	if(count%10==0){
		switch(step){
			case 0:
			if(Get_Uart_Data_Processing_Result()==CSQ_OK){
				globle_Result=0xff;
				Db_val=AT_RX_DATA_BUF[0];
        //HAL_UART_Transmit(&UART_Config,"OK\r\n",5,20);  //
				step++;
			}
			else{
				AT_Command_Send(CSQ);
			}
			break;
			
			case 1:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==CGSN_OK){
				globle_Result=0xff;
				memcpy(&SHORT_NAME[0]   ,&AT_RX_DATA_BUF[8],7);
				memcpy(&COMPLETE_NAME[0],&AT_RX_DATA_BUF[8],7);
				step++;
			}
			else{
				AT_Command_Send(CGSN);
			}
			break;
			
			case 2:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
			}
			else{
				AT_Command_Send(ATQ0);
			}
			break;
			
			case 3:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
			}
			else{
				AT_Command_Send(CTM2MSETPM);
			}
			break;
	
			case 4:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
				Set_Task_State(START_LOCK_SEND,1);
			}
			else{
				AT_Command_Send(CTM2MREG);
			}
			break;
		}
		}
}

