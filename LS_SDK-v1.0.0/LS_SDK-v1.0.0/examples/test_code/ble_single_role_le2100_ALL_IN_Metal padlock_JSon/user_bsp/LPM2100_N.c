#include "user_main.h"
#include "LPM2100_N.h"

#define LOCK_NUM 0x00   //从锁数量 0x01，0x02
#define START 0x01
#define STOP  0x00

#define VER_0  0xA0
#define VER_1  0xA0

uint8_t send_count;							 //发送计数
uint8_t lock_state[LOCK_NUM+1];  //锁状态存储
uint8_t C0_lock_state[LOCK_NUM+1];  //锁状态存储


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
void hex2string(uint8_t *IN_DATA,uint8_t *OUT_DATA,uint16_t len) {
    int i = 0;
    char newchar[10] = {0};
		len=len<<1;	
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
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CGATT?\r\n",sizeof("AT+CGATT?\r\n"),100);
        break;
		case CGATT:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CGATT=1",sizeof("AT+CGATT=1"),100);
    break;

    case CTM2MSETPM:
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CTM2MSETPM=117.60.157.137,5683,86400\r\n",sizeof("AT+CTM2MSETPM=117.60.157.137,5683,86400\r\n"),100);

        //HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CTM2MSETPM=221.229.214.202,5683,38400\r\n",sizeof("AT+CTM2MSETPM=221.229.214.202,5683,38400\r\n"),100);

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
		case CCID: 
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+ECICCID\r\n",sizeof("AT+ECICCID\r\n"),100);
     break;			 
		case CTM2MUPDATE: 
        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CTM2MUPDATE\r\n",sizeof("AT+CTM2MUPDATE\r\n"),100);
     break;				
				
//		case CPSMS_SET:
////        HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"AT+CPSMS=1,,,,\"00100010\"\r\n",sizeof("AT+CPSMS?\r\n"),50);
////     break;	
//		
		//AT+CGSN
	}
}

#define USER_DATE_BUF_LEN 4

const char software_ver[]="1.0";
const char hardware_ver[]="1.0";

//3.3 上报设备基础信息
void Start_Lock_Send() {
		uint8_t F_RX_BUF[300];
		uint8_t T_RX_BUF[255];
		uint8_t USER_DATE_BUF[USER_DATE_BUF_LEN];
		
    cJSON* cjson_test = NULL;
    cJSON* cjson_address = NULL;
    cJSON* cjson_skill = NULL;
    char* str = NULL;		
		static char buf1[16];
		static char buf2[21];
		memcpy(buf1,IMEI_DATA,15);
		memcpy(buf2,CCID_DATA,20);
		buf1[15]='\0';
		buf2[20]='\0';	
    /* 创建一个JSON数据对象(链表头结点) */
    cjson_test = cJSON_CreateObject();

    /* 添加一条整数类型的JSON数据(添加一个链表节点) */
    cJSON_AddNumberToObject(cjson_test, "decodeid", 3);
		
		/* 添加一条字符串类型的JSON数据(添加一个链表节点) */
    cJSON_AddStringToObject(cjson_test, "imei", buf1);
    cJSON_AddStringToObject(cjson_test, "ccid", buf2);
    cJSON_AddStringToObject(cjson_test, "software", &software_ver[0]);
    cJSON_AddStringToObject(cjson_test, "hardware", &hardware_ver[0]);

    /* 打印JSON对象(整条链表)的所有数据 */
    str = cJSON_Print(cjson_test);
 //		LOG_I("%s\n", str);
		memcpy(T_RX_BUF,str,strlen(str));

		uint16_t head_len=0;
		uint16_t data_len=4;
		uint16_t json_len=0;
		json_len=strlen(str);
		
		USER_DATE_BUF[0]=0x00;
		USER_DATE_BUF[1]=0x01;   //上报标记
		USER_DATE_BUF[2]=0x00;
		USER_DATE_BUF[3]=json_len;
		
//		LOG_I("josn_len:%d",json_len);
		head_len=strlen("AT+CTM2MSEND=");  		

//发送数据内容	
		hex2string(&USER_DATE_BUF[0],&F_RX_BUF[head_len],USER_DATE_BUF_LEN);
    hex2string(&T_RX_BUF[0],&F_RX_BUF[head_len+(USER_DATE_BUF_LEN*2)],json_len);
//发送数据头尾		
		memcpy(&F_RX_BUF[head_len+(json_len*2)+(USER_DATE_BUF_LEN*2)],",1\r\n",4);
    memcpy(F_RX_BUF,"AT+CTM2MSEND=",head_len);
		
		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],head_len+(USER_DATE_BUF_LEN*2)+(json_len*2)+4,400);	
		
//		LOG_I("%s\n",F_RX_BUF);
//		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],head_len+json_len+3,500);	
//		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)str,50,500);	
//		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)str,50,500);	
//		HAL_UART_Transmit_IT(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF));

			cJSON_Delete(cjson_test);
			LOG_I("03_SEND");
}
//按键按下向服务器请求消息更新
void Open_Lock_Send(){
			AT_Command_Send(CTM2MUPDATE);
			LOG_I("__CTM2MUPDATE__");
}

//3.1 上报设备当前状态 rtc task
void Tick_Lock_Send(){
		uint8_t F_RX_BUF[500];
		uint8_t T_RX_BUF[400];
		uint8_t USER_DATE_BUF[USER_DATE_BUF_LEN];
		
    cJSON* cjson_test = NULL;
    cJSON* cjson_address = NULL;
    cJSON* cjson_skill = NULL;
    char* str = NULL;		
		
		char lock_status[3]="000";
		 lock_status[2]='\0';
		if(lock_state[0]==0){
				lock_status[0]='0';
				lock_status[1]='0';
		}else{
				lock_status[0]='1';
				lock_status[1]='1';
		}
		
		

    /* 创建一个JSON数据对象(链表头结点) */
    cjson_test = cJSON_CreateObject();

    /* 添加一条整数类型的JSON数据(添加一个链表节点) */
    cJSON_AddNumberToObject(cjson_test, "decodeid", 1);
		cJSON_AddNumberToObject(cjson_test, "temperature", 0);
    cJSON_AddNumberToObject(cjson_test, "humidity", 0);
    cJSON_AddNumberToObject(cjson_test, "voltage",VBat_value_mV);
		/* 添加一条字符串类型的JSON数据(添加一个链表节点) */		
    cJSON_AddStringToObject(cjson_test, "status",&lock_status[0]);
		cJSON_AddNumberToObject(cjson_test, "shake",1);
		cJSON_AddNumberToObject(cjson_test, "water",1);		
		cJSON_AddStringToObject(cjson_test, "angle","0,0,0");		
		cJSON_AddStringToObject(cjson_test, "laglati","0.000000,0.000000");				

    /* 打印JSON对象(整条链表)的所有数据 */
    str = cJSON_Print(cjson_test);
//    LOG_I("%s\n", str);
		
		memcpy(T_RX_BUF,str,strlen(str));

		uint16_t head_len=0;
		uint16_t data_len=4;
		uint16_t json_len=0;
		json_len=strlen(str);
		
		USER_DATE_BUF[0]=0x00;
		USER_DATE_BUF[1]=0x01;   //上报标记
		USER_DATE_BUF[2]=0x00;
		USER_DATE_BUF[3]=json_len;
		
//LOG_I("josn_len:%d",json_len);
		head_len=strlen("AT+CTM2MSEND=");  		

//发送数据内容	
		hex2string(&USER_DATE_BUF[0],&F_RX_BUF[head_len],USER_DATE_BUF_LEN);
    hex2string(&T_RX_BUF[0],&F_RX_BUF[head_len+(USER_DATE_BUF_LEN*2)],json_len);
//发送数据头尾		
		memcpy(&F_RX_BUF[head_len+(json_len*2)+(USER_DATE_BUF_LEN*2)],",1\r\n",4);
    memcpy(F_RX_BUF,"AT+CTM2MSEND=",head_len);
		
		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],head_len+(USER_DATE_BUF_LEN*2)+(json_len*2)+4,400);	
		
//		LOG_I("%s\n",F_RX_BUF);
//		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],head_len+json_len+3,500);	
//		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)str,50,500);	
//		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)str,50,500);	
//		HAL_UART_Transmit_IT(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF));

			cJSON_Delete(cjson_test);
			LOG_I("01_SEND");
}

//3.5 开锁指令回应   在电机动作后发一包
void Reply_send(){
		uint8_t F_RX_BUF[500];
		uint8_t T_RX_BUF[400];
		uint8_t USER_DATE_BUF[USER_DATE_BUF_LEN];
		
    cJSON* cjson_test = NULL;
    cJSON* cjson_address = NULL;
    cJSON* cjson_skill = NULL;
    char* str = NULL;		
	
	/* 创建一个JSON数据对象(链表头结点) */
    cjson_test = cJSON_CreateObject();

    /* 添加一条整数类型的JSON数据(添加一个链表节点) */
    cJSON_AddNumberToObject(cjson_test, "decodeid", 5);
    cJSON_AddNumberToObject(cjson_test, "status",lock_state[0]);  //这里根据锁状态判断开锁成功开锁失败

    /* 打印JSON对象(整条链表)的所有数据 */
    str = cJSON_Print(cjson_test);
//    LOG_I("%s\n", str);
		
		memcpy(T_RX_BUF,str,strlen(str));

		uint16_t head_len=0;
		uint16_t data_len=4;
		uint16_t json_len=0;
		json_len=strlen(str);
		
		USER_DATE_BUF[0]=0x00;
		USER_DATE_BUF[1]=0x01;   //上报标记
		USER_DATE_BUF[2]=0x00;
		USER_DATE_BUF[3]=json_len;
		
//		LOG_I("josn_len:%d",json_len);
		head_len=strlen("AT+CTM2MSEND=");  		

//发送数据内容	
		hex2string(&USER_DATE_BUF[0],&F_RX_BUF[head_len],USER_DATE_BUF_LEN);
    hex2string(&T_RX_BUF[0],&F_RX_BUF[head_len+(USER_DATE_BUF_LEN*2)],json_len);
//发送数据头尾		
		memcpy(&F_RX_BUF[head_len+(json_len*2)+(USER_DATE_BUF_LEN*2)],",1\r\n",4);
    memcpy(F_RX_BUF,"AT+CTM2MSEND=",head_len);
		
		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],head_len+(USER_DATE_BUF_LEN*2)+(json_len*2)+4,400);	
		
//		LOG_I("%s\n",F_RX_BUF);
//		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],head_len+json_len+3,500);	
//		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)str,50,500);	
//		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)str,50,500);	
//		HAL_UART_Transmit_IT(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF));

			cJSON_Delete(cjson_test);
			LOG_I("05_SEND");
}

//3.2 上报设备当前状态
void Open_Lock_Data_Send(){
		auto uint8_t F_RX_BUF[300];
		auto uint8_t T_RX_BUF[255];
		auto uint8_t USER_DATE_BUF[USER_DATE_BUF_LEN];

		char lock_status_temp[3]="000";
		 lock_status_temp[2]='\0';
		if(lock_state[0]==0){
				lock_status_temp[0]='0';
				lock_status_temp[1]='0';
		}else{
				lock_status_temp[0]='1';
				lock_status_temp[1]='1';
		}
		  
    auto cJSON* cjson_test = NULL;
    auto cJSON* cjson_address = NULL;
    auto cJSON* cjson_skill = NULL;
    auto char* str = NULL;		

    /* 创建一个JSON数据对象(链表头结点) */
    cjson_test = cJSON_CreateObject();

    /* 添加一条整数类型的JSON数据(添加一个链表节点) */
		/* 添加一条字符串类型的JSON数据(添加一个链表节点) */
    cJSON_AddNumberToObject(cjson_test, "decodeid", 2);
		cJSON_AddStringToObject(cjson_test, "status", &lock_status_temp[0]);
    cJSON_AddNumberToObject(cjson_test, "shake", 1);
		cJSON_AddNumberToObject(cjson_test, "water",1);		
		cJSON_AddStringToObject(cjson_test, "angle","0,0,0");		
		cJSON_AddStringToObject(cjson_test, "laglati","0.000000,0.000000");				


    /* 打印JSON对象(整条链表)的所有数据 */
    str = cJSON_Print(cjson_test);
		cJSON_Delete(cjson_test);
    //LOG_I("%s\n", str);
    //LOG_HEX(str, strlen(str));

		memcpy(T_RX_BUF,str,strlen(str));
		uint16_t head_len=0;
		uint16_t data_len=4;
		uint16_t json_len=0;
		json_len=strlen(str);
		
		USER_DATE_BUF[0]=0x00;
		USER_DATE_BUF[1]=0x01;   //上报标记
		USER_DATE_BUF[2]=0x00;
		USER_DATE_BUF[3]=json_len;
		
//		LOG_I("josn_len:%d",json_len);
		head_len=strlen("AT+CTM2MSEND=");  		

//发送数据内容	
		hex2string(&USER_DATE_BUF[0],&F_RX_BUF[head_len],USER_DATE_BUF_LEN);
    hex2string(&T_RX_BUF[0],&F_RX_BUF[head_len+(USER_DATE_BUF_LEN*2)],json_len);
//发送数据头尾		
		memcpy(&F_RX_BUF[head_len+(json_len*2)+(USER_DATE_BUF_LEN*2)],",1\r\n",4);
    memcpy(F_RX_BUF,"AT+CTM2MSEND=",head_len);
		
		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],head_len+(USER_DATE_BUF_LEN*2)+(json_len*2)+4,400);	
		
//		LOG_I("%s\n",F_RX_BUF);
//		HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],head_len+json_len+3,500);	
//		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)str,50,500);	
//		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)str,50,500);	
//		HAL_UART_Transmit_IT(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF));

//			cJSON_Delete(cjson_test);

			LOG_I("02_SEND");
}


//01启动 锁发送
void Start_Lock_Send_() {
    uint8_t RX_BUF[100];
    uint8_t F_RX_BUF[255];
    uint8_t DATA_BUF[40+2+2];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X2C;		//长度
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
		
		DATA_BUF[17] = 0xC0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state
		
		
		DATA_BUF[17+2] = 0xA8;			//MAC_ADDR
		memcpy(&DATA_BUF[18+2],&MAC_ADDR[0],6);

		DATA_BUF[24+2] = 0xA7;			//MICI 卡号
		memcpy(&DATA_BUF[25+2],&CIMI_DATA[0],15);
		
		//MAC_ADDR[0]
		
    temp=CRC16_8005Modbus(&DATA_BUF[0],40+2);
    DATA_BUF[40+2]=(temp & 0xff00) >>8;
    DATA_BUF[41+2]= temp & 0xff;


    hex2string(DATA_BUF,RX_BUF,42+2);
    RX_BUF[84+4]='\0';
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("START_LOCK_SEND");
		send_time_delay+=100;
		//HAL_UART_Transmit(&UART_Config   ,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
}
//10请求开锁   携带数据同启动    请求开锁（如果20秒内没有收到服务器回复，则需要再次发送请求开锁指令，如果超过6次没有收到服务器开锁指令则放弃。）
void Open_Lock_Send_() {
    uint8_t RX_BUF[50];
    uint8_t F_RX_BUF[100];
    uint8_t DATA_BUF[17+2+2];
    //uint8_t DATA_BUF[17];
		uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X15;		//长度
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

		DATA_BUF[17] = 0xC0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state



    temp=CRC16_8005Modbus(&DATA_BUF[0],17+2);
    DATA_BUF[17+2]=(temp & 0xff00) >>8;
    DATA_BUF[18+2]= temp & 0xff;


    hex2string(DATA_BUF,RX_BUF,19+2);
    RX_BUF[38+4]='\0';

    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("Open_Lock_Send");
		send_time_delay+=100;
	}

//02 心跳 RTC定时唤醒发送数据 锁上传和平台回复数据同启动
void Tick_Lock_Send_() {
    uint8_t RX_BUF[50];
    uint8_t F_RX_BUF[100];
    uint8_t DATA_BUF[17+2+2];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X15;		//长度
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
		
		DATA_BUF[17] = 0xC0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state

    temp=CRC16_8005Modbus(&DATA_BUF[0],17+2);
    DATA_BUF[17+2]=(temp & 0xff00) >>8;
    DATA_BUF[18+2]= temp & 0xff;

    hex2string(DATA_BUF,RX_BUF,19+2);
    RX_BUF[38+4]='\0';
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		
		
		LOG_I("Tick_Lock_Send");
		send_time_delay+=100;
}

//20 电机动作 信息上报   21数据未处理改为20同锁数据上报
void Open_Lock_Data_Send_Moto() {
    uint8_t RX_BUF[50];
    uint8_t F_RX_BUF[100];
    uint8_t DATA_BUF[17+2+2];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X15;		//长度
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
		
		DATA_BUF[17] = 0xc0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state

    temp=CRC16_8005Modbus(&DATA_BUF[0],17+2);
    DATA_BUF[17+2]=(temp & 0xff00) >>8;
    DATA_BUF[18+2]= temp & 0xff;

    hex2string(DATA_BUF,RX_BUF,19+2);
    RX_BUF[38+4]='\0';
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("Open_Lock_Data_Send_Moto");
		send_time_delay+=100;
}


//20 信息上报输入 锁ID号，锁更新状态
void Open_Lock_Data_Send_() {
    uint8_t RX_BUF[50];
    uint8_t F_RX_BUF[100];
    uint8_t DATA_BUF[17+2+2];
    uint16_t temp;

    DATA_BUF[0] = Frame_header[0];//帧头
    DATA_BUF[1] = Frame_header[1];//帧头

    DATA_BUF[2] = 0X15;		//长度17
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
	
		DATA_BUF[17] = 0xC0; 	  //lock_ID
    DATA_BUF[18] = !C0_lock_state[0];	//lock_state
		
    temp=CRC16_8005Modbus(&DATA_BUF[0],17+2);
    DATA_BUF[17+2]=(temp & 0xff00) >>8;
    DATA_BUF[18+2]= temp & 0xff;

    hex2string(DATA_BUF,RX_BUF,19+2);
    RX_BUF[38+4]='\0';
    sprintf((char*)F_RX_BUF,"AT+CTM2MSEND=%s,1\r\n",(char*)RX_BUF);
    HAL_UART_Transmit(&UART_Config_AT,&F_RX_BUF[0],strlen((char*)F_RX_BUF)+1,100);
		
		LOG_I("Open_Lock_Data_Send");
		send_time_delay+=100;
}

