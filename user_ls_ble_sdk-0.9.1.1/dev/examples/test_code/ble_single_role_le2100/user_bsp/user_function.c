#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"
#include <string.h>

#define USB_CHECK PB06

uint8_t T0_enable=0;  //启动数据上报  			Start_Lock_Send_Task
uint8_t T1_enable=0;	//开锁数据请求				Open_Lock_Send
uint8_t T2_enable=0;	//心跳包发送					Tick_Lock_Send
uint8_t T3_enable=0;	//状态变更数据发送  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)
uint8_t T4_enable=0;  //获取信号强度				AT指令相关
uint8_t T5_enable;  //										AT指令相关



uint8_t open_lock_reply_Result=0;
uint8_t tick_reply_Result=0;
uint8_t open_lock_data_reply_Result=0;
uint8_t start_lock_reply_Result=0;


uint32_t adc_value = 0;						//
uint8_t AT_RX_DATA_BUF[50];  			//保存接受到回复信息  +NNMI:2,A101 ->0xA1,0x01
uint8_t Db_val;  

volatile uint8_t recv_flag = 0;
static ADC_HandleTypeDef hadc;
 void Read_Last_Data(void);
static void lsadc_init(void);

void User_Init() {
	  lsadc_init();
	  HAL_ADCEx_InjectedStart_IT(&hadc);
		Button_Gpio_Init();
	  moto_gpio_init();
		Basic_PWM_Output_Cfg();
		//Read_Last_Data();
	
	  io_cfg_output(PC00);   //PB09 config output
    io_write_pin(PC01,0);  
    io_cfg_output(PC00);   //PB10 config output
    io_write_pin(PC00,0);  //PB10 write low power
		io_cfg_input(USB_CHECK);   //PB09 config output
						io_write_pin(PC00, 1);
				io_write_pin(PC01, 0);
	
		HAL_IWDG_Init(32756);  //1s看门狗
 		HAL_RTC_Init(2);    	 //RTC内部时钟源
		//RTC_wkuptime_set(48*60*60);	 //唤醒时间30s  休眠函数在sw.c 中
		RTC_wkuptime_set(60);	 //唤醒时间30s  休眠函数在sw.c 中
		WAKE_UP();
		Set_Task_State(GET_DB_VAL,START);
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
		else if(VBat_value>=0 && VBat_value<20){
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
		else if(VBat_value>=0 && VBat_value<=20){
				io_write_pin(PC00, 0);
				io_write_pin(PC01, 1);
		}
	}
}
}

void Get_Vbat_Task(){
		static uint16_t temp_ADC_value[20];
		 uint32_t true_ADC_value=0;
		static uint16_t true_VBat_value=0;
		static uint16_t count=0;
	
		count++;
		count=count%20;
		temp_ADC_value[count]=Get_Vbat_val();
		
		if(count%20==0){
				for(int i=0;i<20;i++){
						true_ADC_value+=temp_ADC_value[i];
					//LOG_I("VBat_value%d:%d",i,temp_ADC_value[i]);
				}
				true_ADC_value=true_ADC_value/20;
					//LOG_I("true_VBat_value:%d",true_ADC_value);
				true_VBat_value=(4*1400*true_ADC_value/4095);
				
				//LOG_I("true_VBat_value:%d",true_VBat_value);
				if(true_VBat_value>4300){
							true_VBat_value=4300;
				}
				if(true_VBat_value<3000){
							true_VBat_value=3000;
				}
						true_VBat_value=(true_VBat_value-3300)*100/(4200-3300);
						if(true_VBat_value>100)true_VBat_value=100;
						true_VBat_value=(true_VBat_value)*100*0.01;
						//VBat_value=true_VBat_value;
						//LOG_I("VBAT%:%d",VBat_value);
		}
		
		if(count%200==0){
				VBat_value=true_VBat_value;
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
						HAL_ADCEx_InjectedStart_IT(&hadc);
						return adc_value;
		}
		return adc_value;
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
#define RECORD_KEY3 3  //上电启动包，一次标记
tinyfs_dir_t ID_dir;

void Read_Last_Data(){
//static uint32_t count;
uint8_t  tmp[10];
uint16_t length = 10; 
//uint16_t length2 = 10; 
//uint16_t KEY3_length = 1; 

	tinyfs_mkdir(&ID_dir,ROOT_DIR,5);  //创建目录
	
  tinyfs_read(ID_dir,RECORD_KEY1,tmp,&length);//读到tmp中
	LOG_I("read_id");
	LOG_I("%s",tmp);
	memcpy ( &SHORT_NAME[0], &tmp[0], sizeof(tmp) );
	
	tinyfs_read(ID_dir,RECORD_KEY2,tmp,&length);//读到tmp中
	LOG_I("read_flag");
	LOG_I("%s",tmp);
	
	tinyfs_read(ID_dir,RECORD_KEY3,&tmp[0],&length);//读到tmp中
	LOG_I("start:");
	LOG_I("%s",tmp);
////	if(tmp[0]==1){
//		memcpy ( &VBat_value, &tmp[0], 1);
////	}
//	else{
//			tmp[0]=1;
//			VBat_value=Get_Vbat_val();
//			tinyfs_write(ID_dir,RECORD_KEY3,&tmp[0],1);	
//			tinyfs_write_through();
//	}
	
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
            //OK_ASK 服务器有应答
            if(AT_RX_DATA_BUF[0]==0x58 && AT_RX_DATA_BUF[1]==0x59) {
                globle_Result=OK_ASK;
								   //是OPEN_LOCK
									if(AT_RX_DATA_BUF[4]== 0x10) {
										KEY_FLAG=1;
										open_lock_reply_Result=1;
									}
									//心跳包上传成功服务器回复
									else if(AT_RX_DATA_BUF[4]==0x02) {
											tick_reply_Result=1;
									}
									//数据包上传成功服务器回复
									else if(AT_RX_DATA_BUF[4]==0x20) {
											open_lock_data_reply_Result=1;
									}
									//启动包上传成功服务器回复
									else if(AT_RX_DATA_BUF[4]==0x01) {
										start_lock_reply_Result=1;
									}
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
		case GET_DB_VAL :
        temp=	T4_enable;
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
		case GET_DB_VAL :
        T4_enable=state;
        break;
    }
}
//开始启动数据包
uint16_t Start_Lock_Send_Task(){
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
     if(Get_Task_State(START_LOCK_SEND)){
					i++;
					if(i%200==1){
            if(Get_Uart_Data_Processing_Result()==OK_ASK  && start_lock_reply_Result==1) {
							 	globle_Result=0xFF;
								start_lock_reply_Result=0;
                send_count++;
                temp=OK_ASK;
                Set_Task_State(START_LOCK_SEND,STOP);
								i=0;
								//Set_Task_State(OPEN_LOCK_SEND,START);  //
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(START_LOCK_SEND,START);
                Start_Lock_Send();
             
                if(count%6==0) {
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
	
			if(Get_Task_State(OPEN_LOCK_SEND)) {
						i++;
						if(i%100==1) {
            if(Get_Uart_Data_Processing_Result()==OPEN_LOCK   && open_lock_reply_Result==1) {
								globle_Result=0xFF;
								open_lock_reply_Result=0;
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
       if(Get_Task_State(TICK_LOCK_SEND)) {
				i++;
				if(i%100==1) {
						if(Get_Uart_Data_Processing_Result()==OK_ASK && tick_reply_Result==1) {
								globle_Result=0xFF;
								tick_reply_Result=0;
                send_count++;
                temp=OK_ASK;
                Set_Task_State(TICK_LOCK_SEND,STOP);
								i=0;
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
    
   if(Get_Task_State(OPEN_LOCK_DATA_SEND)) {
			i++;			
			if(i%100==1) {
   		      if(Get_Uart_Data_Processing_Result()==OK_ASK && open_lock_data_reply_Result==1) {
								globle_Result=0xFF;
								open_lock_data_reply_Result=0;
                send_count++;
                temp=OPEN_LOCK;
                Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
								i=0;
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
				LOG_I("State_Change");
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
//获取信号值
uint16_t AT_GET_DB_TASK(){
	  static uint8_t count=0;
    static uint16_t temp;
    static uint8_t i;
    
   if(Get_Task_State(GET_DB_VAL)) {
			i++;			
			if(i%10==1) {
   		      if(Get_Uart_Data_Processing_Result()==CSQ_OK && Db_val!=99) {
								globle_Result=0xFF;
                Set_Task_State(GET_DB_VAL,STOP);
								i=0;
								count=0;
								temp=0xff;
            }
            else {
                count++;
							  AT_Command_Send(CSQ);
                if(count>20 && count<40) {
										buzzer_task_flag=1;
                }
								else if(count>=40){
									Set_Task_State(GET_DB_VAL,STOP);
									buzzer_task_flag=1;
                  temp=TIME_OUT;
									count=0;
								}
            }
        }
    }
    return temp;
//		static int count=0;
//		static int count_out=0;
//	
//	if(count%10==1){
//		if(count_out<5){
//			if(Get_Uart_Data_Processing_Result()==CSQ_OK && Db_val!=99){
//				count_out=10;
//				globle_Result=0xff;
//				LOG_I("DB:%d",Db_val);	
//			}
//			else{
//				//buzzer_task_flag=1;
//				count_out++;
//				AT_Command_Send(CSQ);
//			}
//		}
//		else if(count_out!=10){
//			buzzer_task_flag=1;
//			count_out++;
//		}
//	}
}


//重置初始化跑一次
uint16_t AT_INIT(){
	static int step=1;
	static int count=0;
	static int count_out=0;
	static int set_flag=0;
	static int once_flag=0;
	count++;	
	static uint8_t tmp[10];
	uint16_t length = 10; 
	
	
	tinyfs_read(ID_dir,RECORD_KEY2,tmp,&length);//读到tmp中
	if(strncmp("SET_OK",(char*)tmp,sizeof("SET_OK"))==0){
		return 0xff;
	}
	
	if(count%10==0){
		switch(step){
			case 1:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==CGSN_OK){
				globle_Result=0xff;
				memcpy(&SHORT_NAME[0]   ,&AT_RX_DATA_BUF[8-3],7+3);
//				memcpy(&COMPLETE_NAME[0],&AT_RX_DATA_BUF[8],7);
				//后7位设备号写入，作为蓝牙广播名称
				tinyfs_write(ID_dir,RECORD_KEY1,SHORT_NAME,sizeof(SHORT_NAME));	
				tinyfs_write_through();
				//start_adv();   //更新广播
				step++;
				count_out=0;
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
				count_out=0;
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
				count_out=0;
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
				count_out=0;
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
				count_out=0;
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
				count_out=0;
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
				count_out=0;
				
				LOG_I("set_flag:%d",set_flag);
				if(set_flag>7){
						RESET_NB();//
						DELAY_US(1000*1000*2);
						platform_reset(0); 					//初始化成功，重启
				}
				else{
					tinyfs_write(ID_dir,RECORD_KEY2,(uint8_t*)"SET_OK",sizeof("SET_OK"));	
					tinyfs_write_through();
					platform_reset(0); 					//初始化成功，重启
					//tinyfs_read(ID_dir,RECORD_KEY2,tmp,&length);//读到tmp中
					//Set_Task_State(START_LOCK_SEND,1);
					//HAL_IWDG_Init(100);
				}
			}
			else{
				set_flag++;
				count_out++;
				if(count_out>10){
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
static  uint16_t set_sleep_time;
//设置休眠时间
void Set_Sleep_Time(uint16_t time_s){
	set_sleep_time=time_s;
}

//休眠任务
void Sleep_Task(){
	
	uint8_t tmp[2];
	tmp[0]=0x01;
	tmp[1]=VBat_value;
	if(sleep_time*50>set_sleep_time*1000){
//			tinyfs_write(ID_dir,RECORD_KEY3,&tmp[0],2);	
//			tinyfs_write_through();
			ls_sleep_enter_lp2();
	}
}
