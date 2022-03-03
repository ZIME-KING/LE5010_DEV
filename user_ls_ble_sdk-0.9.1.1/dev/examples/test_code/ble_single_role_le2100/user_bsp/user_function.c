#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"
#include <string.h>

#define USB_CHECK PB06

uint8_t T0_enable;  //启动数据上报  			Start_Lock_Send_Task
uint8_t T1_enable;	//开锁数据请求				Open_Lock_Send
uint8_t T2_enable;	//心跳包发送					Tick_Lock_Send
uint8_t T3_enable;	//状态变更数据发送  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)
uint8_t T4_enable;  //设置休眠  					AT指令相关
uint8_t T5_enable;  //获取NB模块各项参数	AT指令相关

uint32_t adc_value = 0;						//
uint8_t AT_RX_DATA_BUF[50];  			//保存接受到回复信息  +NNMI:2,A101 ->0xA1,0x01
uint8_t Db_val;  

volatile uint8_t recv_flag = 0;
static ADC_HandleTypeDef hadc;
static void Read_Last_Data(void);
static void lsadc_init(void);

void User_Init() {
	  lsadc_init();
	  HAL_ADCEx_InjectedStart_IT(&hadc);
		Button_Gpio_Init();
	  moto_gpio_init();
		Basic_PWM_Output_Cfg();
		Read_Last_Data();
	
	  io_cfg_output(PC00);   //PB09 config output
    io_write_pin(PC01,0);  
    io_cfg_output(PC00);   //PB10 config output
    io_write_pin(PC00,0);  //PB10 write low power
		io_cfg_input(USB_CHECK);   //PB09 config output
	
		HAL_IWDG_Init(32756);  //1s看门狗
 		HAL_RTC_Init(2);    	 //RTC内部时钟源
		RTC_wkuptime_set(30);	 //唤醒时间30s  休眠函数在sw.c 中

	//RESET_NB();
		WAKE_UP();
}
void LED_TASK(){
	static uint8_t flag;
	static uint8_t count;
	count++;
	if(count%20==0){
		if(flag==1)flag=0;
		else flag=1;
	}
	if(BLE_connected_flag==1){
				io_write_pin(PC00, 0);
				io_write_pin(PC01, 1);
	}	
	else{
	//5V接入
	if(io_read_pin(USB_CHECK)==1){
		//20~90 绿灯闪
		if(VBat_value>20 && VBat_value<90){
				io_write_pin(PC00, flag);
				io_write_pin(PC01, 0);
		}
		//<20 红灯闪
		else if(VBat_value<20){
				io_write_pin(PC00, 0);
				io_write_pin(PC01, flag);
		}
		//20~90 绿灯常量
		else if(VBat_value>90){
				io_write_pin(PC00, 1);
				io_write_pin(PC01, 0);
		}
	}
	else{
		if(VBat_value>20){
				io_write_pin(PC00, 1);
				io_write_pin(PC01, 0);
		}
		//<20 红灯
		else if(VBat_value<=20){
				io_write_pin(PC00, 0);
				io_write_pin(PC01, 1);
		}
	}
}
}

void Get_Vbat_Task(){
		static uint16_t last_VBat_value;
		static uint16_t now_VBat_value;

		static uint16_t count;
		count++;
		now_VBat_value=Get_Vbat_val();
		last_VBat_value+=now_VBat_value;
		if(last_VBat_value!=0){
			last_VBat_value=last_VBat_value/2;
		}
		if(count%10==0){
					VBat_value=last_VBat_value;
		}
}



//锁信号强度获取
uint8_t Get_dBm() {
    //uint8_t Db_val;
    return Db_val;
}

/////////////////////////////////////adc_value_num////////////////////////////////////////////////////
//电阻分压1/4，硬件上VREF为1.4V
uint16_t Get_Vbat_val() {
	uint16_t adc_value_num;
		if(recv_flag == 1){
		        recv_flag = 0;
						adc_value_num=(4*1400*adc_value/4095);
						//adc_value_num=adc_value;
						//if(adc_value_num<3600) adc_value_num=3600;
						
						if(adc_value_num>4300){
							adc_value_num=4300;
						}
						if(adc_value_num<3000){
							adc_value_num=3000;
						}
						adc_value_num=(adc_value_num-3300)*100/(4200-3300);
						if(adc_value_num>100)adc_value_num=100;
						
						//adc_value_num=(adc_value_num)*100*0.01;
						HAL_ADCEx_InjectedStart_IT(&hadc);
						return adc_value_num;
		}
		return adc_value_num;
}
////////////////////////////////////////// ADC ///////////////////////////////////////////
static void lsadc_init(void)
{
	  adc12b_in6_io_init();
    hadc.Instance = LSADC;
    hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode          = ADC_SCAN_DISABLE;              /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
    hadc.Init.NbrOfConversion       = 1;                            /* Parameter discarded because sequencer is disabled */
    hadc.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
    hadc.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
    hadc.Init.ContinuousConvMode    = DISABLE;                        /* Continuous mode to have maximum conversion speed (no delay between conversions) */
    hadc.Init.TrigType      = ADC_INJECTED_SOFTWARE_TRIGT;            /* The reference voltage uses an internal reference */
    hadc.Init.Vref          = ADC_VREF_INSIDE;
    hadc.Init.AdcCkDiv = ADC_CLOCK_DIV2;

    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
        //Error_Handler();
    }
    ADC_InjectionConfTypeDef sConfigInjected = {0};
    sConfigInjected.InjectedChannel = ADC_CHANNEL_6;  //
    sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
    sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_15CYCLES;
    sConfigInjected.InjectedOffset = 0;
    sConfigInjected.InjectedNbrOfConversion = 1;
    sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
    sConfigInjected.AutoInjectedConv = DISABLE;

  if (HAL_ADCEx_InjectedConfigChannel(&hadc, &sConfigInjected) != HAL_OK)
  {
    /* Channel Configuration Error */
    //Error_Handler();
  }
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    recv_flag = 1;
    adc_value = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
}
/////////////////////////////////////////////////////////////////////////////////////
#define RECORD_KEY1 1	 //蓝牙名称
#define RECORD_KEY2 2  //完成模块初始化标记
#define RECORD_KEY3 3  //上一次启动的电量
tinyfs_dir_t ID_dir;

void Read_Last_Data(){
//static uint32_t count;
uint8_t  tmp[10];
uint16_t length = 10; 
uint16_t KEY3_length = 2; 

	tinyfs_mkdir(&ID_dir,ROOT_DIR,5);  //创建目录
	
  tinyfs_read(ID_dir,RECORD_KEY1,tmp,&length);//读到tmp中
	LOG_I("read_id");
	LOG_I("%s",tmp);
	memcpy ( &SHORT_NAME[0], &tmp[0], sizeof(tmp) );
	
	tinyfs_read(ID_dir,RECORD_KEY2,tmp,&length);//读到tmp中
	LOG_I("read_flag");
	LOG_I("%s",tmp);
	
	tinyfs_read(ID_dir,RECORD_KEY3,tmp,&KEY3_length);//读到tmp中
	LOG_I("read_flag");
	LOG_I("%s",tmp);
	if(tmp[0]==1){
		memcpy ( &VBat_value, &tmp[0], 1);
	}
	else{
			tmp[0]=1;
			VBat_value=Get_Vbat_val();
			tinyfs_write(ID_dir,RECORD_KEY3,&tmp[0],1);	
			tinyfs_write_through();
	}
	
//  LOG_HEX(tmp,sizeof(tmp));
//	if(tmp[0]!=ID_VAL_0 && tmp[1]!=ID_VAL_1 ){
//		while(io_read_pin(PA07)==1){
//			count++;	
////			for(char i=0;i<8;i++){
////				srand((unsigned)count+i);
////				id_num[i]= rand()%0xff;
////			}
//		}
//		SHORT_NAME[0]=ID_VAL_0;
//		SHORT_NAME[1]=ID_VAL_1;
//		tinyfs_write(ID_dir,RECORD_KEY1,SHORT_NAME,sizeof(SHORT_NAME));	
//		tinyfs_write_through();
//		LOG_I("SET_OK");
//		LOG_HEX(SHORT_NAME,sizeof(SHORT_NAME));
	//}
	//else{
	//}
//	memcpy ( &sent_buf[0], &id_num[0], sizeof(id_num) );
}


//////////////////////////////////////////////////////////////////////////////////////
/********************
 **数据处理, 一帧接受完成 后跑一次
//在接收中断后面跑一次
******************/
void Uart_Data_Processing() {

    if(frame[uart_frame_id].status!=0) {    			//接收到数据后status=1;
        HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        //接收到的数据 到uart2  透传
        frame[uart_frame_id].status=0;					//处理完数据后status 清0;
    }
}

void Uart_2_Data_Processing() {
    int count;
//		int len;
    if(frame_2[uart_2_frame_id].status!=0){						//接收到数据后status=1;
        //HAL_UART_Transmit(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length,100);
				frame_2[uart_2_frame_id].buffer[frame_2[uart_2_frame_id].length]='\0';
				LOG_I("%s",frame_2[uart_2_frame_id].buffer);
				//返回接收到的数据到uart1上
				//收到NNMI卡号返回
				//globle_Result=0XFF;
				if( strncmp("AT+CGSN=1\r\n+CGSN:",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CGSN=1\r\n+CGSN:"))==0) {
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
				}
				//收到信号强度值
        else if( strncmp("AT+CSQ\r\n+CSQ:",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CSQ\r\n+CSQ:"))==0)  {
					globle_Result=CSQ_OK;
						if(frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+1]==','){
								Db_val=frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+0]-'0';
						}
						else{
								Db_val=(frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+0]-'0')*10;
								Db_val+=frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+1]-'0';
						}
        }
				//收到服务器回复值		
				else if(strncmp("\r\n+NNMI:",(char*)frame_2[uart_2_frame_id].buffer,strlen("\r\n+NNMI:"))==0){
						//globle_Result=OK_ASK;
						 for(int i=0; i<frame_2[uart_2_frame_id].length; i++) {
                if(frame_2[uart_2_frame_id].buffer[i]==',') {
                    count=i+1;
                    break;
                }
            }
            //默认输入为大写
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
            //标记——————这里应该要加CRC的
            //OK_ASK
            if(AT_RX_DATA_BUF[0]==0x58 && AT_RX_DATA_BUF[1]==0x59) {
                globle_Result=OK_ASK;
            }
            //OPEN_LOCK
            else if(AT_RX_DATA_BUF[0]==0x5A && AT_RX_DATA_BUF[1]==0xA5) {
                globle_Result=OPEN_LOCK;
								KEY_FLAG=1;
            }
        }
				//收到OK消息 ,以上都不满足只用OK
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
				else if( strncmp("AT+ECPCFG=\"slpWaitTime\",10000\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ECPCFG=\"slpWaitTime\",10000\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				else if( strncmp("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				else if( strncmp("AT+ECPMUCFG=1,4\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ECPMUCFG=1,4\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				else if( strncmp("AT+CPSMS=1\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CPSMS=1\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				frame_2[uart_2_frame_id].status=0;					//处理完数据后status 清0;
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
//开始启动数据包
uint16_t Start_Lock_Send_Task(){
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%40==1) {
        if(Get_Task_State(START_LOCK_SEND)){
            if(Get_Uart_Data_Processing_Result()==OK_ASK) {
							 	globle_Result=0xFF;
                send_count++;
                temp=OK_ASK;
                Set_Task_State(START_LOCK_SEND,STOP);
								//Set_Task_State(OPEN_LOCK_SEND,START);  //
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(START_LOCK_SEND,START);

                Start_Lock_Send();
             

                if(count%3==0) {
                    Set_Task_State(START_LOCK_SEND,STOP);
										
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}
//请求开锁 ??现在不请求开锁直接上报数据了
uint16_t Open_Lock_Send_Task() {
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
   // static uint8_t once_flag=1;
		i++;
    if(i%40==1) {
        if(Get_Task_State(OPEN_LOCK_SEND)) {
            if(Get_Uart_Data_Processing_Result()==OPEN_LOCK) {
								globle_Result=0xFF;
                send_count++;
                temp=OPEN_LOCK;
                Set_Task_State(OPEN_LOCK_SEND,STOP);
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(OPEN_LOCK_SEND,START);

                Open_Lock_Send();

                if(count%3==0) {
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
    if(i%40==1) {
        if(Get_Task_State(TICK_LOCK_SEND)) {
            if(Get_Uart_Data_Processing_Result()==OK_ASK) {
								globle_Result=0xFF;
                send_count++;
                temp=OK_ASK;
                Set_Task_State(TICK_LOCK_SEND,STOP);
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(TICK_LOCK_SEND,START);
                Tick_Lock_Send();

                if(count%5==0) {
                    Set_Task_State(TICK_LOCK_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}
//20信息上报  锁+状态
uint16_t Open_Lock_Data_Send_Task(){
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%40==1) {
			  //LOG_I("%d",globle_Result);
        if(Get_Task_State(OPEN_LOCK_DATA_SEND)) {
            if(Get_Uart_Data_Processing_Result()==OK_ASK) {
								globle_Result=0xFF;
                send_count++;
                temp=OPEN_LOCK;
                Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(OPEN_LOCK_DATA_SEND,START);

							  Open_Lock_Data_Send(0,lock_state[0]);
							
                if(count%3==0) {
                    Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}

//void NB_WAKE_Task(){
//	 static uint8_t i;
//	i++;
//    if(i%100==1) {
//				AT_Command_Send(AT);
//    }
//}

//检测状态发生变化上报数据
void State_Change_Task(){
	static uint8_t last_lock_state;
		if(	Check_SW1()){
				lock_state[0]=0;
		}
		else{
				lock_state[0]=1;
		}
		//正常启动后进入zd
		if(last_lock_state != lock_state[0]){
				sleep_time=0;
				last_lock_state=lock_state[0];
		
				Set_Task_State(OPEN_LOCK_DATA_SEND,1); //状态改变数据上传
	
			if(moro_task_flag==1){
				}
				else{
			  user_ble_send_flag=1;
				TX_DATA_BUF[0]=0x52;		// CMD
				TX_DATA_BUF[1]=TOKEN[0];TX_DATA_BUF[2]=TOKEN[1];TX_DATA_BUF[3]=TOKEN[2];TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
				TX_DATA_BUF[5]=0x01;    //LEN
				TX_DATA_BUF[6]=lock_state[0];
				}
				
		}		
}
//开机初始化跑一次
void AT_GET_DATA(){
	static int step=1;
	static int count=0;
	static int count_out=0;
	static int set_flag=0;
	static int once_flag=0;
	count++;	
	static uint8_t tmp[10];
	uint16_t length = 10; 
	
	if(count%4==0){
	if(once_flag<20){
		tinyfs_read(ID_dir,RECORD_KEY2,tmp,&length);//读到tmp中
			if(Get_Uart_Data_Processing_Result()==CSQ_OK && Db_val!=99){
				globle_Result=0xff;
				LOG_I("DB:%d",Db_val);	
				
					once_flag=0xff;
					//Start_Lock_Send();
					Open_Lock_Data_Send(0,lock_state[0]);
					Set_Task_State(START_LOCK_SEND,1);
			}
			else{
				buzzer_task_flag=1;
				once_flag++;
				AT_Command_Send(CSQ);
		}
	}
	}
	if(strncmp("SET_OK",(char*)tmp,sizeof("SET_OK"))==0){
		return;
	}
	
	if(count%4==0){
		switch(step){
//			case 0:
//			if(Get_Uart_Data_Processing_Result()==CSQ_OK){
//				globle_Result=0xff;
//				Db_val=AT_RX_DATA_BUF[0];
//        //HAL_UART_Transmit(&UART_Config,"OK\r\n",5,20);  //
//				step++;
//			}
//			else{
//				AT_Command_Send(CSQ);
//			}
//			break;
			case 1:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==CGSN_OK){
				globle_Result=0xff;
				memcpy(&SHORT_NAME[0]   ,&AT_RX_DATA_BUF[8-3],7+3);
//				memcpy(&COMPLETE_NAME[0],&AT_RX_DATA_BUF[8],7);
				//后7位设备号写入，作为蓝牙广播名称
				tinyfs_write(ID_dir,RECORD_KEY1,SHORT_NAME,sizeof(SHORT_NAME));	
				tinyfs_write_through();
				start_adv();   //更新广播
				step++;
			}
			else{
				set_flag++;
				count_out++;
				if(count_out>5){
					count_out=0	;
					step++;
				}
				AT_Command_Send(CGSN);
				buzzer_task_flag=1;
			}
			break;
			
			case 2:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
			}
			else{
				set_flag++;
				count_out++;
				if(count_out>5){
					count_out=0	;
					step++;
				}
				AT_Command_Send(ATQ0);
				buzzer_task_flag=1;
			}
			break;
			
			case 3:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
			}
			else{
				set_flag++;
				count_out++;
				if(count_out>5){
					count_out=0	;
					step++;
				}
				AT_Command_Send(CTM2MSETPM);
				buzzer_task_flag=1;
			}
			break;
	
			case 4:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;	
				//Set_Task_State(START_LOCK_SEND,1);
			}
			else{
				set_flag++;
				count_out++;
				if(count_out>5){
					count_out=0	;
					step++;
				}
				AT_Command_Send(CTM2MREG);
				buzzer_task_flag=1;
			}
			break;
			
			case 5:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
				//Set_Task_State(START_LOCK_SEND,1);
			}
			else{
				set_flag++;
				count_out++;
				if(count_out>5){
					count_out=0	;
					step++;
				}
				AT_Command_Send(AT_SLEEP);
				buzzer_task_flag=1;
			}
			break;

			case 6:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
				//Set_Task_State(START_LOCK_SEND,1);
			}
			else{
				set_flag++;
				count_out++;
				if(count_out>5){
					count_out=0	;
					step++;
				}
				AT_Command_Send(CPSMS);
				buzzer_task_flag=1;
			}
			break;

			case 7:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
				if(set_flag>10){
						RESET_NB();
				}
				else{
					tinyfs_write(ID_dir,RECORD_KEY2,(uint8_t*)"SET_OK",sizeof("SET_OK"));	
					tinyfs_write_through();
					tinyfs_read(ID_dir,RECORD_KEY2,tmp,&length);//读到tmp中
					//Set_Task_State(START_LOCK_SEND,1);
					HAL_IWDG_Init(100);
					while(1);
				}
			}
			else{
				set_flag++;
				count_out++;
				if(count_out>50){
					count_out=0	;
					step++;
				}
				
				AT_Command_Send(ECPMUCFG);
				buzzer_task_flag=1;
			}
			break;					
			default : /* 可选的 */
      break;
			//statement(s);
		}
		}
}

//无操作120s休眠
void Sleep_Task(uint16_t time_s){
	uint8_t tmp[2];
	tmp[0]=0x01;
	tmp[1]=VBat_value;
	if(sleep_time*50>time_s*1000){
			tinyfs_write(ID_dir,RECORD_KEY3,&tmp[0],2);	
			tinyfs_write_through();
			ls_sleep_enter_lp2();
	}
}
