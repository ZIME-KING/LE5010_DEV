#include "user_main.h"
#include "LPM2100_N.h"

#define LOCK_NUM 0x00   //�������� 0x01��0x02
#define START 0x01
#define STOP  0x00


#define VER_0  0xA0          //Ӳ���汾

#ifdef DEF                   //Ĭ��������
	 #define VER_1  0xA0
#endif

#ifdef MAG                   //�ְ���
		#define VER_1  0xA3      //A1->A3
#endif
#ifdef METAL_PADLOCK         //��������
		#define VER_1  0xA2
#endif

#ifdef WHITE         //����
			#define VER_1  0xA4
#endif


//#define VER_0  0xA0
//#define VER_1  0xB0

uint8_t send_count;							 //���ͼ���
uint8_t lock_state[LOCK_NUM+1];  //��״̬�洢
uint8_t C0_lock_state[LOCK_NUM+1];  //��״̬�洢


const uint8_t	Frame_header[2]= {0x58,0x59};

void WAKE_UP(){
		io_cfg_output(PA13);               //���ģʽ                     
    io_pull_write(PA13, IO_PULL_UP);   //��������  
		io_write_pin(PA13,0);  
		DELAY_US(10*1000);
		io_write_pin(PA13,1);
		DELAY_US(10*1000);
		io_write_pin(PA13,0);
}

void RESET_NB(){
		io_cfg_output(PA05);               //���ģʽ                     
    io_pull_write(PA05, IO_PULL_UP);   //��������  
		io_write_pin(PA05,0);
		DELAY_US(10*1000);
		io_write_pin(PA05,1);             
		DELAY_US(10*1000);
		io_write_pin(PA05,0);
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

//AT�����
void AT_Command_Send(Typedef_AT AT_COM) {
    switch(AT_COM) {
		
		case ATE1:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"ATE1\r\n",sizeof("ATE1\r\n"),100);
    break;
		
		case AT:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT\r\n",sizeof("AT\r\n"),100);
        break;
    case ATQ0:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"ATQ0\r\n",sizeof("ATQ0\r\n"),100);
        break;
		case CFUN_R: //��������
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CFUN?\r\n",sizeof("AT+CFUN?\r\n"),100);
        break;
			
		case CFUN:		//��������
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CFUN=1",sizeof("AT+CFUN=1"),100);
        break;
		
		case CGATT_R:  //���Ų�ѯ
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
    case CPSMS_RESET:
        //HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CPSMS=1\r\n",sizeof("AT+CPSMS=1\r\n"),50);
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CPSMS=2\r\n",sizeof("AT+CPSMS=2\r\n"),100);
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
		case AT_ENTER_SLEEP:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+ECSLEEP=0\r\n",sizeof("AT+ECSLEEP=0"),100);
        break;	
		case CIMI:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CIMI\r\n",sizeof("AT+CIMI\r\n"),100);
        break;	
		case CEREG_R:  //��ע��״̬ 
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CEREG?\r\n",sizeof("AT+CEREG?\r\n"),100);
     break;	
				case CPSMS_R: //��psm����
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CPSMS?\r\n",sizeof("AT+CPSMS?\r\n"),100);
     break;	
		
		 case CEREG: //����psm�����ϱ�
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CEREG=0\r\n",sizeof("AT+CEREG=0\r\n"),100);
     break;	
		 
		 case SKTCREATE: //����SKTCREATE            //ѡ��IPV4  UDP  IPPROTO_UDPͨѶ��ʽ       
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+SKTCREATE=1,2,17\r\n",sizeof("AT+SKTCREATE=1,2,17\r\n"),100);
     break;	
		 
		 case SKTCONNECT: //����Զ�̷������Ͷ˿�
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+SKTCONNECT=1,139.224.136.93,50513\r\n",sizeof("AT+SKTCONNECT=1,139.224.136.93,50513\r\n"),100);
     break;	
		 
		 case SKTDELETE: //ɾ��SKTCREATE
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+SKTDELETE=1\r\n",sizeof("AT+SKTDELETE=1\r\n"),100);
     break;	
		 
		 
				
//		case CPSMS_SET:
////        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CPSMS=1,,,,\"00100010\"\r\n",sizeof("AT+CPSMS?\r\n"),50);
////     break;	
//		
		//AT+CGSN
	}
}
//01���� ������
void Start_Lock_Send() {
    uint8_t RX_BUF[255];
    uint8_t F_RX_BUF[255];
    uint8_t DATA_BUF[60];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//֡ͷ
    DATA_BUF[1] = Frame_header[1];//֡ͷ

    DATA_BUF[2] = 0X3c;		//����
    DATA_BUF[3] = send_count++;   //send_count;     //����ID  ����һ��++ 0~255
    DATA_BUF[4] = 0X01;   //������  ����

    DATA_BUF[5] = 0XA0;   //��Ӳ���汾��
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//���̼��汾��
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//֧�ֵĴ�������
    DATA_BUF[12] = 0X00;

    DATA_BUF[13] = 0XB0; 	//���� 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

    DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state
		
		DATA_BUF[17] = 0xC0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state
		
		
		DATA_BUF[19] = 0xA8;			//MAC_ADDR
		memcpy(&DATA_BUF[20],&MAC_ADDR[0],6);

		DATA_BUF[26] = 0xA7;			//MICI ����
		memcpy(&DATA_BUF[27],&CIMI_DATA[0],15);
		
		DATA_BUF[42] = 0xA9;			//MICI �豸��
		memcpy(&DATA_BUF[43],&MICI_DATA[0],15);
		
		
		
		//MAC_ADDR[0]
		
    temp=CRC16_8005Modbus(&DATA_BUF[0],58);
    DATA_BUF[59]=(temp & 0xff00) >>8;
    DATA_BUF[58]= temp & 0xff;


    hex2string(DATA_BUF,RX_BUF,60);
    RX_BUF[120]='\0';
    //sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
		sprintf((char*)F_RX_BUF,"AT+SKTSEND=1,%d,%s\r\n",strlen((char*)RX_BUF)>>1,(char*)RX_BUF);	
		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,500);
		
		LOG_I("START_LOCK_SEND");
		send_time_delay+=100;
		//HAL_UART_Transmit(&UART_Config   ,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
}
//10������   Я������ͬ����    �����������20����û���յ��������ظ�������Ҫ�ٴη���������ָ��������6��û���յ�����������ָ�����������
void Open_Lock_Send() {
    uint8_t RX_BUF[100];
    uint8_t F_RX_BUF[255];
    uint8_t DATA_BUF[37];
    //uint8_t DATA_BUF[17];
		uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//֡ͷ
    DATA_BUF[1] = Frame_header[1];//֡ͷ

    DATA_BUF[2] = 0X25;		//����
    DATA_BUF[3] = send_count++;   //����ID  ����һ��++ 0~255
    DATA_BUF[4] = 0X10;   //������  ������

    DATA_BUF[5] = 0XA0;   //��Ӳ���汾��
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//���̼��汾��
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//֧�ֵĴ�������
    DATA_BUF[12] = 0X00;

    DATA_BUF[13] = 0XB0; 	//���� 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

    DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state

		DATA_BUF[17] = 0xC0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state


		DATA_BUF[19] = 0xA9;			//MICI ����
		memcpy(&DATA_BUF[20],&MICI_DATA[0],15);


    temp=CRC16_8005Modbus(&DATA_BUF[0],35);
    DATA_BUF[36]=(temp & 0xff00) >>8;
    DATA_BUF[35]= temp & 0xff;


    hex2string(DATA_BUF,RX_BUF,37);
    RX_BUF[74]='\0';

    //sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    sprintf((char*)F_RX_BUF,"AT+SKTSEND=1,%d,%s\r\n",strlen((char*)RX_BUF)>>1,(char*)RX_BUF);	
		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("Open_Lock_Send");
		send_time_delay+=100;
	}

//02 ���� RTC��ʱ���ѷ������� ���ϴ���ƽ̨�ظ�����ͬ����
void Tick_Lock_Send() {
    uint8_t RX_BUF[100];
    uint8_t F_RX_BUF[255];
    uint8_t DATA_BUF[37];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//֡ͷ
    DATA_BUF[1] = Frame_header[1];//֡ͷ

    DATA_BUF[2] = 0X25;		//����
    DATA_BUF[3] = send_count;   //����ID  ����һ��++ 0~255
    DATA_BUF[4] = 0X02;   //������  ����

    DATA_BUF[5] = 0XA0;   //��Ӳ���汾��
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//���̼��汾��
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//֧�ֵĴ�������
    DATA_BUF[12] = 0X00;

    DATA_BUF[13] = 0XB0; 	//���� 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

		DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state
		
		DATA_BUF[17] = 0xC0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state

		DATA_BUF[19] = 0xA9;			//MICI ����
		memcpy(&DATA_BUF[20],&MICI_DATA[0],15);


    temp=CRC16_8005Modbus(&DATA_BUF[0],35);
    DATA_BUF[36]=(temp & 0xff00) >>8;
    DATA_BUF[35]= temp & 0xff;


    hex2string(DATA_BUF,RX_BUF,37);
    RX_BUF[74]='\0';

    //sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    sprintf((char*)F_RX_BUF,"AT+SKTSEND=1,%d,%s\r\n",strlen((char*)RX_BUF)>>1,(char*)RX_BUF);	
		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		
		
		LOG_I("Tick_Lock_Send");
		send_time_delay+=100;
}

//20 ������� ��Ϣ�ϱ�   21����δ�����Ϊ20ͬ�������ϱ�
void Open_Lock_Data_Send_Moto() {
    uint8_t RX_BUF[100];
    uint8_t F_RX_BUF[255];
    uint8_t DATA_BUF[37];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//֡ͷ
    DATA_BUF[1] = Frame_header[1];//֡ͷ

    DATA_BUF[2] = 0X24;		//����
    DATA_BUF[3] = send_count++;     //����ID  ����һ��++ 0~255
    DATA_BUF[4] = 0X20;   //������  ����

    DATA_BUF[5] = 0XA0;   //��Ӳ���汾��
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//���̼��汾��
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//֧�ֵĴ�������
    DATA_BUF[12] = LOCK_NUM;

    DATA_BUF[13] = 0XB0; 	//���� 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

		DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state
		
		DATA_BUF[17] = 0xc0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state

		DATA_BUF[19] = 0xA9;			//MICI ����
		memcpy(&DATA_BUF[20],&MICI_DATA[0],15);


    temp=CRC16_8005Modbus(&DATA_BUF[0],35);
    DATA_BUF[36]=(temp & 0xff00) >>8;
    DATA_BUF[35]= temp & 0xff;


    hex2string(DATA_BUF,RX_BUF,37);
    RX_BUF[74]='\0';

    //sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    sprintf((char*)F_RX_BUF,"AT+SKTSEND=1,%d,%s\r\n",strlen((char*)RX_BUF)>>1,(char*)RX_BUF);	
		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("Open_Lock_Data_Send_Moto");
		send_time_delay+=100;
}


//20 ��Ϣ�ϱ����� ��ID�ţ�������״̬
void Open_Lock_Data_Send() {
    uint8_t RX_BUF[100];
    uint8_t F_RX_BUF[255];
    uint8_t DATA_BUF[37];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//֡ͷ
    DATA_BUF[1] = Frame_header[1];//֡ͷ

    DATA_BUF[2] = 0X25;		//����
    DATA_BUF[3] = send_count++;     //����ID  ����һ��++ 0~255
    DATA_BUF[4] = 0X20;   //������  ����

    DATA_BUF[5] = 0XA0;   //��Ӳ���汾��
    DATA_BUF[6] = VER_0;

    DATA_BUF[7] = 0XA1;		//���̼��汾��
    DATA_BUF[8] = VER_1;

    DATA_BUF[9]  = 0XA2;		//���ź�ǿ�ȣ�0-31��99��
    DATA_BUF[10] = Get_dBm();

    DATA_BUF[11] = 0XA6;		//֧�ֵĴ�������
    DATA_BUF[12] = LOCK_NUM;

    DATA_BUF[13] = 0XB0; 	//���� 0~100%;
    DATA_BUF[14] = VBat_value;	//50%

		DATA_BUF[15] = 0x00; 	  //lock_ID
    DATA_BUF[16] = !lock_state[0];	//lock_state
	
		DATA_BUF[17] = 0xC0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state
		
		DATA_BUF[19] = 0xA9;			//MICI ����
		memcpy(&DATA_BUF[20],&MICI_DATA[0],15);


    temp=CRC16_8005Modbus(&DATA_BUF[0],35);
    DATA_BUF[36]=(temp & 0xff00) >>8;
    DATA_BUF[35]= temp & 0xff;


    hex2string(DATA_BUF,RX_BUF,37);
    RX_BUF[74]='\0';

    
		//sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
		sprintf((char*)F_RX_BUF,"AT+SKTSEND=1,%d,%s\r\n",strlen((char*)RX_BUF)>>1,(char*)RX_BUF);	
		
		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("Open_Lock_Data_Send");
		send_time_delay+=100;
}

