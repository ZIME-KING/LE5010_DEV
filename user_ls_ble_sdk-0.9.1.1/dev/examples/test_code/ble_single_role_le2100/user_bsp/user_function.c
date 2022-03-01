#include "user_main.h"
#include "user_function.h"
#include <string.h>

uint8_t T0_enable;  //启动数据上报  			Start_Lock_Send_Task
uint8_t T1_enable;	//开锁数据请求				Open_Lock_Send
uint8_t T2_enable;	//心跳包发送					Tick_Lock_Send
uint8_t T3_enable;	//状态变更数据发送  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)
uint8_t T4_enable;  //设置休眠  					AT指令相关
uint8_t T5_enable;  //获取NB模块各项参数	AT指令相关

uint32_t Get_ADC_Value(void);
static ADC_HandleTypeDef hadc;

uint8_t AT_RX_DATA_BUF[50];  			//保存接受到回复信息  +NNMI:2,A101 ->0xA1,0x01
uint8_t Db_val;  			

uint32_t adc_value = 0;
volatile uint8_t recv_flag = 0;

void AT_GET_DATA(void);
void lsadc_init(void);

uint8_t Open_Lock_Moto(){
}
uint8_t Close_Lock_Moto(){
return 0;
}

static void test_delay() {
    int i=65535;
    while(i--);
}

void User_Init() {
	  lsadc_init();
	  HAL_ADCEx_InjectedStart_IT(&hadc);
		Button_Gpio_Init();
	  moto_gpio_init();
		Basic_PWM_Output_Cfg();
		Buzzer_ON();
		test_delay();
		test_delay();
		Buzzer_OFF();
		
	  io_cfg_output(PC00);   //PB09 config output
    io_write_pin(PC01,0);  
    io_cfg_output(PC00);   //PB10 config output
    io_write_pin(PC01,0);  //PB10 write low power
	
		WAKE_UP();
		if(Get_Vbat_val()>40){
				io_write_pin(PC00, 0);
				io_write_pin(PC01, 1);
		}
		else{
				io_write_pin(PC00, 1);
				io_write_pin(PC01, 0);
		}

		//adc_value = (hadc.Init.AdcDriveType == INRES_ONETHIRD_EINBUF_DRIVE_ADC)?(Get_ADC_Value()*3):Get_ADC_Value();
	
//		AT_GET_DATA();
//		int i=5;
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
						adc_value_num=(adc_value_num-3000)*100/(4300-3000);
						if(adc_value_num>100)adc_value_num=100;
						
						//adc_value_num=(adc_value_num)*100*0.01;
						HAL_ADCEx_InjectedStart_IT(&hadc);
						return adc_value_num;
		}
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
        HAL_UART_Transmit(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length,100);
        //返回接收到的数据到uart1上
				//收到NNMI卡号返回
				globle_Result=0XFF;
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
				//收到信号强度值
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
						HAL_UART_Transmit(&UART_Config,"/r/nA",3,20);
						HAL_UART_Transmit(&UART_Config,&AT_RX_DATA_BUF[0],1,20);
						HAL_UART_Transmit(&UART_Config,"/r/nA",3,20);
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
						// HAL_UART_Transmit(&UART_Config,&AT_RX_DATA_BUF[0],(frame_2[uart_2_frame_id].length-count)/2,10);

            //标记——————这里应该要加CRC的
            //OK_ASK
            if(AT_RX_DATA_BUF[0]==0x58 && AT_RX_DATA_BUF[1]==0x59 && AT_RX_DATA_BUF[5]==0x00) {
                globle_Result=OK_ASK;
								//HAL_UART_Transmit(&UART_Config,(uint8_t*)"aa",sizeof("aa"),10);
            }
            //OPEN_LOCK
            else if(AT_RX_DATA_BUF[0]==0x5A && AT_RX_DATA_BUF[1]==0xA5) {
                globle_Result=OPEN_LOCK;
								moro_task_flag=1;      
								Set_Task_State(OPEN_LOCK_DATA_SEND,START);
                //lock_state[0]=AT_RX_DATA_BUF[0];								
								//HAL_UART_Transmit(&UART_Config,(uint8_t*)"BB",sizeof("BB"),10);
                //lock_state[1]=1
                //lock_state[1]=1
            }
        }
				//收到错误消息
				//else if(strpbrk((char*)frame_2[uart_2_frame_id].buffer,"ERROR")==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
         //   globle_Result=ERROE_AT;
        //}
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
				else if( strncmp("AT+ECPCFG=\"slpWaitTime\",5000\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ECPCFG=\"slpWaitTime\",5000\r\nOK"))==0){
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
        //HAL_UART_Transmit(&UART_Config,&RX_DATA_BUF[0],(frame_2[uart_frame_id].length-count)/2,10);  //
				//if(globle_Result==OK_ASK)
				//HAL_UART_Transmit(&UART_Config,(uint8_t*)"FREAM_OK",sizeof("FREAM_OK"),10);
      
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
    if(i%100==1) {
        if(Get_Task_State(START_LOCK_SEND)) {
            if(Get_Uart_Data_Processing_Result()==OK_ASK) {
							 	globle_Result=0xFF;
                send_count++;
                temp=OK_ASK;
                Set_Task_State(START_LOCK_SEND,STOP);
								Set_Task_State(OPEN_LOCK_SEND,START);
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
//请求开锁
uint16_t Open_Lock_Send_Task() {
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%100==1) {
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
                //test_delay();

                if(count%5==0) {
                    Set_Task_State(OPEN_LOCK_SEND,STOP);
                    temp=TIME_OUT;
										ls_sleep_enter_lp2();
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
    if(i%100==1) {
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
//20信息上报  锁+状态
uint16_t Open_Lock_Data_Send_Task(){
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%100==1) {
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
               // test_delay();

                if(count%5==0) {
                    Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}


//开机初始化跑一次
void AT_GET_DATA(){
	static int step=0;
	static int count=0;
	count++;	
	if(count%2==0){
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
				//Set_Task_State(START_LOCK_SEND,1);
			}
			else{
				AT_Command_Send(CTM2MREG);
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
				AT_Command_Send(AT_SLEEP);
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
				AT_Command_Send(CPSMS);
			}
			break;

			case 7:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
				Set_Task_State(START_LOCK_SEND,1);
			}
			else{
				AT_Command_Send(AECPMUCFG);
			}
			break;					
		}
		}
}

