#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"
#include <string.h>

uint8_t T0_enable;  //���������ϱ�  			Start_Lock_Send_Task
uint8_t T1_enable;	//������������				Open_Lock_Send
uint8_t T2_enable;	//����������					Tick_Lock_Send
uint8_t T3_enable;	//״̬������ݷ���  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)
uint8_t T4_enable;  //��������  					ATָ�����
uint8_t T5_enable;  //��ȡNBģ��������	ATָ�����

uint32_t Get_ADC_Value(void);
static ADC_HandleTypeDef hadc;

uint8_t AT_RX_DATA_BUF[50];  			//������ܵ��ظ���Ϣ  +NNMI:2,A101 ->0xA1,0x01
uint8_t Db_val;  			
void Read_Last_Data();

uint32_t adc_value = 0;
volatile uint8_t recv_flag = 0;

void AT_GET_DATA(void);
void lsadc_init(void);

//uint8_t Open_Lock_Moto(){
//}
//uint8_t Close_Lock_Moto(){
//return 0;
//}

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
		Read_Last_Data();
		Buzzer_ON();
		test_delay();
		test_delay();
		Buzzer_OFF();
		
	  io_cfg_output(PC00);   //PB09 config output
    io_write_pin(PC01,0);  
    io_cfg_output(PC00);   //PB10 config output
    io_write_pin(PC01,0);  //PB10 write low power
	
		WAKE_UP();
		RESET_NB();
	
		//adc_value = (hadc.Init.AdcDriveType == INRES_ONETHIRD_EINBUF_DRIVE_ADC)?(Get_ADC_Value()*3):Get_ADC_Value();
	
 //     AT_GET_DATA();
//		int i=5;
}
void LED_TASK(){
		if(Get_Vbat_val()>40){
				io_write_pin(PC00, 1);
				io_write_pin(PC01, 0);
		}
		else{
				io_write_pin(PC00, 0);
				io_write_pin(PC01, 1);
		}
}



//���ź�ǿ�Ȼ�ȡ
uint8_t Get_dBm() {
    //uint8_t Db_val;
    return Db_val;
}

/////////////////////////////////////adc_value_num////////////////////////////////////////////////////
//�����ѹ1/4��Ӳ����VREFΪ1.4V
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
#define RECORD_KEY1 1	 //��������
#define RECORD_KEY2 2  //���ģ���ʼ�����
tinyfs_dir_t ID_dir;

void Read_Last_Data(){
static uint32_t count;
uint8_t  tmp[10];
uint16_t length = 10; 

	tinyfs_mkdir(&ID_dir,ROOT_DIR,5);  //����Ŀ¼
	
  tinyfs_read(ID_dir,RECORD_KEY1,tmp,&length);//����tmp��
	LOG_I("read_id");
	LOG_I("%s",tmp);
	memcpy ( &SHORT_NAME[0], &tmp[0], sizeof(tmp) );
	
	//LOG_HEX(tmp,sizeof(tmp));
	
	tinyfs_read(ID_dir,RECORD_KEY2,tmp,&length);//����tmp��
	LOG_I("read_flag");
	LOG_I("%s",tmp);

	//LOG_HEX(tmp,sizeof(tmp));

	
	
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
//		int len;
    if(frame_2[uart_2_frame_id].status!=0){						//���յ����ݺ�status=1;
        //HAL_UART_Transmit(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length,100);
				frame_2[uart_2_frame_id].buffer[frame_2[uart_2_frame_id].length]='\0';
				LOG_I("%s",frame_2[uart_2_frame_id].buffer);
				//���ؽ��յ������ݵ�uart1��
				//�յ�NNMI���ŷ���
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
				//�յ��ź�ǿ��ֵ
        else if( strncmp("AT+CSQ\r\n+CSQ:",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CSQ\r\n+CSQ:"))==0)  {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);          
						globle_Result=CSQ_OK;

						if(frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+1]==','){
								Db_val=frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+0]-'0';
						}
						else{
								Db_val=(frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+0]-'0')*10;
								Db_val+=frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")+1]-'0';
						}
//						HAL_UART_Transmit(&UART_Config,"/r/nA",3,20);
//						HAL_UART_Transmit(&UART_Config,&AT_RX_DATA_BUF[0],1,20);
//						HAL_UART_Transmit(&UART_Config,"/r/nA",3,20);
        }
				//�յ��������ظ�ֵ
				
				else if(strncmp("\r\n+NNMI:",(char*)frame_2[uart_2_frame_id].buffer,strlen("\r\n+NNMI:"))==0){
						//globle_Result=OK_ASK;
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
						// HAL_UART_Transmit(&UART_Config,&AT_RX_DATA_BUF[0],(frame_2[uart_2_frame_id].length-count)/2,10);

            //��ǡ���������������Ӧ��Ҫ��CRC��
            //OK_ASK
            if(AT_RX_DATA_BUF[0]==0x58 && AT_RX_DATA_BUF[1]==0x59 && AT_RX_DATA_BUF[5]==0x00) {
                globle_Result=OK_ASK;
								//HAL_UART_Transmit(&UART_Config,(uint8_t*)"aa",sizeof("aa"),10);
            }
            //OPEN_LOCK
            else if(AT_RX_DATA_BUF[0]==0x5A && AT_RX_DATA_BUF[1]==0xA5) {
                globle_Result=OPEN_LOCK;
								KEY_FLAG=1;

								//moro_task_flag=1;      
								//Set_Task_State(OPEN_LOCK_DATA_SEND,START);

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
				//		LOG_I("%d",globle_Result);
      
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
//��ʼ�������ݰ�
uint16_t Start_Lock_Send_Task(){
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%99==1) {
        if(Get_Task_State(START_LOCK_SEND)) {
            if(Get_Uart_Data_Processing_Result()==OK_ASK) {
							 	globle_Result=0xFF;
                send_count++;
                temp=OK_ASK;
                Set_Task_State(START_LOCK_SEND,STOP);
//								Set_Task_State(OPEN_LOCK_SEND,START);  //
            }
            else {
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(START_LOCK_SEND,START);

                Start_Lock_Send();
             

                if(count%5==0) {
                    Set_Task_State(START_LOCK_SEND,STOP);
										
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}
//������ 20s,6��
uint16_t Open_Lock_Send_Task() {
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
   // static uint8_t once_flag=1;

		i++;
    if(i%400==1) {
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

                if(count%6==0) {
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
    if(i%110==1) {
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
//20��Ϣ�ϱ�  ��+״̬
uint16_t Open_Lock_Data_Send_Task(){
    static uint8_t count;
    static uint16_t temp;
    static uint8_t i;
    i++;
    if(i%40==1) {
			  LOG_I("%d",globle_Result);
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
								//test_delay();

                if(count%3==0) {
                    Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}
//���״̬�����仯�ϱ�����
void State_Change_Task(){
	static uint8_t last_lock_state;
		if(	Check_SW1()){
				lock_state[0]=0;
		}
		else{
				lock_state[0]=1;
		}
		//�������������zd
		if(last_lock_state != lock_state[0]){
				last_lock_state=lock_state[0];
		
				//if(Get_Task_State(START_LOCK_SEND)==0 ) //&&
				//Get_Task_State(OPEN_LOCK_DATA_SEND)==0)
				//{
						LOG_I("%d",globle_Result);
						Set_Task_State(OPEN_LOCK_DATA_SEND,1); //״̬�ı������ϴ�
				
			
				//}
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
//������ʼ����һ��
void AT_GET_DATA(){
	static int step=1;
	static int count=0;
	static int once_flag=0;
	count++;	
	static uint8_t tmp[10];
	uint16_t length = 10; 
	
	if(once_flag<2){
		tinyfs_read(ID_dir,RECORD_KEY2,tmp,&length);//����tmp��
		LOG_I("read_flag");
		LOG_I("%s",tmp);	
			if(Get_Uart_Data_Processing_Result()==CSQ_OK){
				globle_Result=0xff;
				//Db_val=AT_RX_DATA_BUF[0];
				//LOG_I("%s",&frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")]);
				//&frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n+CSQ:")];
				LOG_I("DB:%d",Db_val);	
        //HAL_UART_Transmit(&UART_Config,"OK\r\n",5,20);  //
				//step++;
				once_flag++;
				  Start_Lock_Send();
					Set_Task_State(START_LOCK_SEND,1);
			}
			else{
				AT_Command_Send(CSQ);
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
				//��7λ�豸��д�룬��Ϊ�����㲥����
				tinyfs_write(ID_dir,RECORD_KEY1,SHORT_NAME,sizeof(SHORT_NAME));	
				tinyfs_write_through();
				
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
				tinyfs_write(ID_dir,RECORD_KEY2,(uint8_t*)"SET_OK",sizeof("SET_OK"));	
				tinyfs_write_through();
				tinyfs_read(ID_dir,RECORD_KEY2,tmp,&length);//����tmp��
				Set_Task_State(START_LOCK_SEND,1);
			}
			else{
				AT_Command_Send(ECPMUCFG);
			}
			break;					
			default : /* ��ѡ�� */
      break;
			//statement(s);
		}
		}
}

