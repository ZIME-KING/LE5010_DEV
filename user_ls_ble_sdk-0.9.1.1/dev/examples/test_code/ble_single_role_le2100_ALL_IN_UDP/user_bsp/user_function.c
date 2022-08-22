#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"
#include <string.h>

//#define USB_CHECK PA03
#define USB_CHECK   PA03
//#define	USB_CHECK_B PB06

uint8_t look_status_send_count;

//�����ط�ʱ�� 400��20s
//#define	USER_TIME 400
uint8_t user_time  =100;
uint8_t user_count =30;

//#define	USER_TIME  100
//#define	USER_COUNT 25


//#define RECORD_KEY1  1	//��������
//#define RECORD_KEY2  2  //���ģ���ʼ�����
//#define RECORD_KEY3  3  //������������
//#define RECORD_KEY4  4  //����ͨ����Կ ��8λ
//#define RECORD_KEY5  5  //����ͨ����Կ ��8λ
//#define RECORD_KEY6  6  //��flash����д���ʼֵ���
//#define RECORD_KEY7  7  //last_Vbat
//#define RECORD_KEY8  8  //last_Vbat_max
//#define RECORD_KEY9  9  //CIMI

//#ifdef USER_TEST 
//#define RECORD_KEY10 10 
//#endif

tinyfs_dir_t ID_dir_1;
tinyfs_dir_t ID_dir_2;
tinyfs_dir_t ID_dir_3;

union Data
{
   uint16_t i;
   uint8_t  str[2];
} u16_to_u8;

uint8_t T0_enable=0;  //���������ϱ�  			Start_Lock_Send_Task
uint8_t T1_enable=0;	//������������				Open_Lock_Send
uint8_t T2_enable=0;	//����������					Tick_Lock_Send
uint8_t T3_enable=0;	//״̬������ݷ���  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)
uint8_t T4_enable=0;  //��ȡ�ź�ǿ��				ATָ�����
uint8_t T5_enable=0;  //���������ɷ��Ͱ�

uint8_t T6_enable=0;  //
uint8_t T7_enable=0;  //
uint8_t T8_enable=0;  //


uint8_t open_lock_reply_Result=0;
uint8_t tick_reply_Result=0;
uint8_t open_lock_data_reply_Result=0;
uint8_t open_lock_data_moto_reply_Result=0;
uint8_t start_lock_reply_Result=0;


uint32_t adc_value = 0;						//
uint8_t AT_RX_DATA_BUF[50];  			//������ܵ��ظ���Ϣ  +NNMI:2,A101 ->0xA1,0x01
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
	  io_cfg_output(PC01);   //PB09 config output
    io_write_pin(PC01,0);  
    io_cfg_output(PC00);   //PB10 config output
    io_write_pin(PC00,0);  //PB10 write low power
		
//		io_cfg_input(USB_CHECK);   //PB09 config output
//		io_cfg_input(USB_CHECK_B);   //PB09 config output
//	
////		io_cfg_output(USB_CHECK);   
////    io_write_pin(USB_CHECK,0);  
////    io_cfg_output(USB_CHECK_B);   
////    io_write_pin(USB_CHECK_B,0);  
	
		//io_pull_write(USB_CHECK,IO_PULL_DOWN); //����PA04�ڲ�����
		//io_pull_write(USB_CHECK_B,IO_PULL_DOWN); //����PA04�ڲ�����
	
		//io_write_pin(PC00, 1);
		//io_write_pin(PC01, 0);
	
		HAL_IWDG_Init(32756);  //1s���Ź�
 		HAL_RTC_Init(2);    	 //RTC�ڲ�ʱ��Դ
		RTC_wkuptime_set(3*60*60);	 //����ʱ��48h  ���ߺ�����sw.c ��
		//RTC_wkuptime_set(60);	 //����ʱ��30s  ���ߺ�����sw.c ��
		WAKE_UP();
	
		Set_Task_State(GET_DB_VAL,START);
}
extern uint8_t RTC_flag;
void LED_TASK(){
	static uint8_t flag;
	static uint8_t count;
	
	//uint8_t wkup_source = get_wakeup_source();   //��ȡ����Դ
	//����RTC������������������
  if (RTC_flag==1) {
				io_write_pin(PC00, 0);
				io_write_pin(PC01, 0);
	}
	else{
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
	//5V����
		io_cfg_input(USB_CHECK);   //PB09 config output
//  	io_cfg_input(USB_CHECK_B);   //PB09 config output
		
	if(io_read_pin(USB_CHECK)==1){
		//LOG_I("usb_in");
		//20~90 �̵���
		if(VBat_value>20 && VBat_value<=90){
				io_write_pin(PC00, flag);
				io_write_pin(PC01, 0);
		}
		//<20 �����
		else if( VBat_value<=20){
				io_write_pin(PC00, 0);
				io_write_pin(PC01, flag);
		}
		//20~90 �̵Ƴ���
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
		//<20 ���
		else if(VBat_value<=20){
				io_write_pin(PC00, 0);
				io_write_pin(PC01, 1);
		}
	}
//		io_cfg_output(USB_CHECK);   
//    io_write_pin(USB_CHECK,0);  
//    io_cfg_output(USB_CHECK_B);   
//    io_write_pin(USB_CHECK_B,0);  
	}
	}
}
//ð������
void _f(uint16_t a[],char len){
		int n;  //�������a��Ԫ�صĸ���
    int i;  //�Ƚϵ�����
    int j;  //ÿ�ֱȽϵĴ���
    int buf;  //��������ʱ���ڴ���м�����

	n=len;
	for (i=0; i<n-1; ++i)  //�Ƚ�n-1��
    {
        for (j=0; j<n-1-i; ++j)  //ÿ�ֱȽ�n-1-i��,
        {
            if (a[j] < a[j+1])
            {
                buf = a[j];
                a[j] = a[j+1];
                a[j+1] = buf;
            }
        }
    }
}


//Ϊ���õ�����ʾ׼ȷ������У׼100%ʱ�ĵ�ص�ѹ����
//��

uint16_t global_vbat_max;
void Get_Vbat_Task(){
		uint32_t true_ADC_value=0;
		static uint16_t temp_ADC_value[20];
		static uint16_t true_VBat_value=0;

		static uint16_t count=0;
		//static uint16_t count_2=0;
		
		static uint16_t i=0;
		static uint8_t ADC_Count=0;
	
		static uint16_t once_flag=0;
		static uint16_t change_time=0;
		static uint16_t start_time=0;
		if(start_time<0xffff)start_time++;
		if(start_time==2*200)once_flag=1;
	
	
		io_cfg_input(USB_CHECK);   //PB09 config output
//  	io_cfg_input(USB_CHECK_B);   //PB09 config output
	
	
		//�ɼ�ʵʱ�ĵ�ѹ
		if(moro_task_flag==0 && buzzer_task_flag==0){
			count++;
			//50MS �ɼ�һ�� 20�� 1s
			if(count>=10){
						count=0;;
						temp_ADC_value[i]=Get_Vbat_val();
						if(i>19){
							i=0;
							ADC_Count++;						
							_f(temp_ADC_value,20);
							true_ADC_value=(temp_ADC_value[1]+temp_ADC_value[2])/2;
							true_VBat_value=(4*1400*true_ADC_value/4095);
							if(true_VBat_value>4200)	true_VBat_value=global_vbat_max;
							if(true_VBat_value<3000)	true_VBat_value=3000;
							//LOG_I("Vbat:%d %",true_VBat_value);
						}
						i++;
			}
		}
		//������2s��δ����ʱ���¼һ�µ��������������ߵ�ʱ��Ҳ���¼һ�� ��Ϊ
		if(io_read_pin(USB_CHECK)!=1 ){
								if(ADC_Count==2){
									//once_flag=0;			
									//LOG_I("1_Vbat:%d %",VBat_value);
									//tinyfs_write(ID_dir_3,RECORD_KEY7,&VBat_value,1);	
									//tinyfs_write_through();	
								}									
								//���10�����һ�� 10ms һ�� 20��һ�� 200ms  10�θ�����ʾ 2s
								//��һ�θ�����200ms��ʱ��
								//Ϊ�ŵ�ʱ������ʾ								
								//if(count%400==1  && ADC_Count>1){
								//LOG_I("Vbat:%d mV",true_VBat_value);			
								static uint8_t last_ADC_count;
								if(ADC_Count != last_ADC_count){
									last_ADC_count=ADC_Count;
									true_VBat_value=(true_VBat_value-3000)*100/(global_vbat_max-3000);
									true_VBat_value=(true_VBat_value)*100*0.01;
									#ifdef USER_TEST
									LOG_I("Vbat:%d",true_VBat_value);
									LOG_I("ADC_Count:%d",ADC_Count);
									#endif
									VBat_value=true_VBat_value;  //VBat_value ȫ�ֵ�����ʾ
								}
								
								
		}
		
	
		//2200mha ���360ma������Ԥ��6hСʱ���� 216s ��1% ����5ms��һ��
		//Ϊ���ʱ������ʾ
		if(io_read_pin(USB_CHECK)==1 ){
			change_time++;
			sleep_time=0;
//			if(count%200==1  && ADC_Count>1 && true_VBat_value<=global_vbat_max-100){  //����4.1v���������ʱ�俴����
//									true_VBat_value=true_VBat_value-200;
//									if(true_VBat_value<3000)  true_VBat_value=3000;
//									//LOG_I("c_Vbat:%d mV",true_VBat_value);					
//									true_VBat_value=(true_VBat_value-3000)*100/(global_vbat_max-3000);
//									true_VBat_value=(true_VBat_value)*100*0.01;
//									//LOG_I("c_Vbat:%d %",true_VBat_value);
//									VBat_value=true_VBat_value;  //VBat_value ȫ�ֵ�����ʾ
//			}
//			else{
			//300s����+1
			if(change_time>=(300*200)){
				change_time=0;
				VBat_value=VBat_value+1;
			 // LOG_I("c_c_VBat_value:%d %",true_VBat_value);	
				//LOG_I("c_c_VBat_value:%d %",VBat_value);	
				if(VBat_value>=100){
						global_vbat_max=true_VBat_value;   //
						LOG_I("vbat_max:%d %",global_vbat_max);	
						u16_to_u8.i=global_vbat_max;	
						tinyfs_write(ID_dir_3,RECORD_KEY8,(uint8_t*)u16_to_u8.str,2);	
						tinyfs_write_through();
						VBat_value=100;
					}
				}
//		}
		}
//		io_cfg_output(USB_CHECK);   
//    io_write_pin(USB_CHECK,0);  
//    io_cfg_output(USB_CHECK_B);   
//    io_write_pin(USB_CHECK_B,0);  
}



//���ź�ǿ�Ȼ�ȡ
uint8_t Get_dBm() {
    //uint8_t Db_val;
    return Db_val;
}

/////////////////////////////////////adc_value_num////////////////////////////////////////////////////
//�����ѹ1/2��1/4  Ӳ����VREFΪ3.3V
uint16_t Get_Vbat_val() {
//	uint16_t adc_value_num;	
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
    hadc.Init.Vref          = ADC_VREF_INSIDE;//ADC_VREF_INSIDE ADC_VREF_VCC
    hadc.Init.AdcCkDiv = ADC_CLOCK_DIV2;
		
		hadc.Init.AdcDriveType=EINBUF_DRIVE_ADC;
    
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
void Read_Last_Data(){
//static uint32_t count;
//const static uint8_t no_wirt_data[10]={0x17,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0xF8,0x07};

static uint8_t tmp[10];
uint16_t length_1  = 1; 
#ifdef USER_TEST
	uint16_t length_1_1  = 1; 
#endif
uint16_t length_2  = 2; 
uint16_t length_3  = 3; 
uint16_t length_10 = 10; 
uint16_t length_7  = 10; 
uint16_t length_6  = 6; 
uint16_t length_8  = 8; 
uint16_t length_15 = 15;
uint16_t length_one = 1;

//uint8_t temp_val = 0xAA;

	tinyfs_mkdir(&ID_dir_1,ROOT_DIR,50);  //����Ŀ¼
	tinyfs_mkdir(&ID_dir_2,ROOT_DIR,51);  //����Ŀ¼
	tinyfs_mkdir(&ID_dir_3,ROOT_DIR,52);  //����Ŀ¼
	tinyfs_read(ID_dir_1,RECORD_KEY6,tmp,&length_3);//����tmp��
	LOG_I("flash_init");
	LOG_I("%s",tmp);
	LOG_HEX(tmp,10);
	
	if(strncmp("ok",(char*)tmp,sizeof("ok"))!=0){
		LOG_I("w_init");
		tinyfs_write(ID_dir_1,RECORD_KEY6,(uint8_t*)"ok",sizeof("ok"));	
		tinyfs_write(ID_dir_2,RECORD_KEY2,(uint8_t*)"NO_SET",sizeof("NO_SET"));	
		tinyfs_write(ID_dir_2,RECORD_KEY1,(uint8_t*)SHORT_NAME,sizeof(SHORT_NAME));	
		tinyfs_write(ID_dir_3,RECORD_KEY3,(uint8_t*)PASSWORD,sizeof(PASSWORD));	
		tinyfs_write(ID_dir_3,RECORD_KEY4,(uint8_t*)&key[0],8);			
		tinyfs_write(ID_dir_3,RECORD_KEY5,(uint8_t*)&key[8],8);	
		
//		tinyfs_write(ID_dir_3,RECORD_KEY_T,(uint8_t*)&temp_val,1);	//������ģʽ��ǳ�0xBB����������
		
		SYSCFG->BKD[7]=0;
		
		#ifdef USER_TEST 
		open_count=0;
		tinyfs_write(ID_dir_2,RECORD_KEY10,(uint8_t*)&open_count,1);	
		#endif
		
		u16_to_u8.i=4200;
		tinyfs_write(ID_dir_3,RECORD_KEY8,(uint8_t*)u16_to_u8.str,2);	
		tinyfs_write_through();
	}
	
	tinyfs_read(ID_dir_2,RECORD_KEY1,tmp,&length_10);//����tmp��
	LOG_I("read_id");
	LOG_I("%s",tmp);
	memcpy (&SHORT_NAME[0], &tmp[0], strlen((char*)&SHORT_NAME));
	memcpy (&NEW_SHORT_NAME[0], &tmp[0], strlen((char*)&SHORT_NAME));

	tinyfs_read(ID_dir_2,RECORD_KEY2,tmp,&length_7);//����tmp��
	LOG_I("read_flag");
	LOG_I("%s",tmp);
		
	tinyfs_read(ID_dir_3,RECORD_KEY3,tmp,&length_6);//����tmp��
	LOG_I("KEY3");
	LOG_HEX(tmp,6);
	memcpy (&PASSWORD[0], &tmp[0],6);

	tinyfs_read(ID_dir_3,RECORD_KEY4,tmp,&length_8);//����tmp��
	LOG_I("KEY4");
	LOG_HEX(tmp,8);
	memcpy (&key[0], &tmp[0],8);

	tinyfs_read(ID_dir_3,RECORD_KEY5,tmp,&length_8);//����tmp��
	LOG_I("KEY5");
	LOG_HEX(tmp,8);
	memcpy (&key[8], &tmp[0],8);
	
	tinyfs_read(ID_dir_3,RECORD_KEY7,tmp,&length_1);//����tmp��
	LOG_I("KEY7_last_Vbat");
	LOG_I("Vbat,%d",tmp[0]);
	memcpy (&VBat_value, &tmp[0],1);
	
	
	tinyfs_read(ID_dir_3,RECORD_KEY8,tmp,&length_2);//����tmp��
	LOG_I("KEY8_last_Vbat_max");
	memcpy (&global_vbat_max, &tmp[0],2);
	LOG_I("last_Vbat_max,%d",global_vbat_max);
	
	tinyfs_read(ID_dir_3,RECORD_KEY9,tmp,&length_15);//����tmp��
	memcpy (&CIMI_DATA[0], &tmp[0],15);
	LOG_I("KEY9_CIMI");
	LOG_I("CIMI,%s",&CIMI_DATA[0]);
	
	tinyfs_read(ID_dir_3,RECORD_KEY_T,tmp,&length_one);//����tmp��
	memcpy (&test_mode_flag, &tmp[0],1);
	LOG_I("KEY_T_test_mode_flag");
	LOG_I("test_mode_flag,%d",test_mode_flag);
	
	

#ifdef USER_TEST 
	tinyfs_read(ID_dir_2,RECORD_KEY10,tmp,&length_1_1);//����tmp��
	memcpy (&open_count, &tmp[0],1);
	LOG_I("KEY10_open_count");
	LOG_I("open_count,%d",open_count);
	if(open_count>=200){
					tinyfs_write(ID_dir_2,RECORD_KEY2,(uint8_t*)"SET_NO",sizeof("SET_NO"));	
					tinyfs_write_through();
	}
#endif

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
				HAL_UART_Transmit(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length,100);
							
				frame_2[uart_2_frame_id].buffer[frame_2[uart_2_frame_id].length]='\0';
				LOG_I("%s",frame_2[uart_2_frame_id].buffer);
				//���ؽ��յ������ݵ�uart1��
				//�յ�NNMI���ŷ���
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
				//�յ��ź�ǿ��ֵ
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
				//�յ�sim������
        else if( strncmp("AT+CIMI\r\n",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CIMI\r\n"))==0)  {
					
					count=strlen("AT+CIMI\r\n");
          if(frame_2[uart_2_frame_id].buffer[count]=='E'){
						globle_Result=CIMI_ERROR;
					}
					else{
						globle_Result=CIMI_OK;
						for(int i=0; i<count+15; i++) {
                AT_RX_DATA_BUF[i]=frame_2[uart_2_frame_id].buffer[count+i];
						}
					}
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
            //��ǡ���������������Ӧ��Ҫ��CRC��
            //OK_ASK ��������Ӧ��
            if(AT_RX_DATA_BUF[0]==0x58 && AT_RX_DATA_BUF[1]==0x59) {
                globle_Result=OK_ASK;
								   //��OPEN_LOCK
									if(AT_RX_DATA_BUF[4]== 0x10  && AT_RX_DATA_BUF[5]== 0x00 && AT_RX_DATA_BUF[6]== 0x01 ) {
										KEY_FLAG=1;
										open_lock_reply_Result=1;
										#ifdef USER_TEST
												open_count++;
										#endif
									}
									//�������ϴ��ɹ��������ظ�
									else if(AT_RX_DATA_BUF[4]==0x02) {
											tick_reply_Result=1;
									}
									//���ݰ��ϴ��ɹ��������ظ�
									else if(AT_RX_DATA_BUF[4]==0x20) {
											open_lock_data_reply_Result=1;
											open_lock_data_moto_reply_Result=1;
									}
									//���ݰ��ϴ��ɹ��������ظ�
									else if(AT_RX_DATA_BUF[4]==0x21) {
											//open_lock_data_moto_reply_Result=1;
									}
									//�������ϴ��ɹ��������ظ�
									else if(AT_RX_DATA_BUF[4]==0x01) {
										start_lock_reply_Result=1;
									}
            }
        }
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
				else if( strncmp("AT+ECPCFG=\"slpWaitTime\",15000\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ECPCFG=\"slpWaitTime\",15000\r\nOK"))==0){
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
				else if( strncmp("AT+CPSMS=1",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CPSMS=1"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				else if( strncmp("AT+CEREG=0\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CEREG=0\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				else if( strncmp("AT+CGATT=1\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CGATT=1\r\nOK"))==0){
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
				
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
		case GET_DB_VAL :
        temp=	T4_enable;
        break;
		case OPEN_LOCK_DATA_SEND_MOTO:
        temp=	T5_enable;
        break;
		case GET_MODE_VAL:
        temp=	T6_enable;
        break;
		case GET_EMIC_VAL:
        temp=	T7_enable;
        break;
		case TEST_GET_DB_VAL:
        temp=	T8_enable;
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
		case OPEN_LOCK_DATA_SEND_MOTO:
        T5_enable=state;
        break;
		case GET_MODE_VAL:
        T6_enable=state;
        break;
		case GET_EMIC_VAL:
        T7_enable=state;
        break;
		case TEST_GET_DB_VAL:
        T8_enable=state;
        break;
    }
}
//��ʼ�������ݰ�
uint16_t Start_Lock_Send_Task(){
    static uint8_t count;
    static uint16_t temp;
    static uint16_t i;
     if(Get_Task_State(START_LOCK_SEND)  && AT_tset_flag==0 ){
            if(start_lock_reply_Result==1) {
							 	globle_Result=0xFF;
								start_lock_reply_Result=0;

                temp=OK_ASK;
                Set_Task_State(START_LOCK_SEND,STOP);
								i=0;
								//Set_Task_State(OPEN_LOCK_SEND,START);  //
            }
            else {
							i++;
							if(i%400==10){
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(START_LOCK_SEND,START);
                Start_Lock_Send();
             
                if(count==6) {
									count=0;
                    Set_Task_State(START_LOCK_SEND,STOP);
										
                    temp=TIME_OUT;
                }
            }
        }
    }
    return temp;
}
//������
uint16_t Open_Lock_Send_Task() {
    static uint8_t count;
    static uint16_t temp;
    static uint16_t i;
			if(Get_Task_State(OPEN_LOCK_SEND) && AT_tset_flag==0){
            if( open_lock_reply_Result==1) {
								globle_Result=0xFF;
								open_lock_reply_Result=0;
								//send_count++;
                temp=OPEN_LOCK;
                Set_Task_State(OPEN_LOCK_SEND,STOP);
								i=0;
            }
            else {
						i++;
						if(i%user_time==1) {
								count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(OPEN_LOCK_SEND,START);

                Open_Lock_Send();

                if(count==user_count) {
									count=0;
                    Set_Task_State(OPEN_LOCK_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
		else{
			count=0;
			temp=0;
			i=0;
		}
    return temp;
}
//������
uint16_t Tick_Lock_Send_Task() {
    static uint8_t count;
    static uint16_t temp;
    static uint16_t i;
       if(Get_Task_State(TICK_LOCK_SEND)) {

						if(tick_reply_Result==1) {
								globle_Result=0xFF;
								tick_reply_Result=0;
//                send_count++;
                temp=OK_ASK;
                Set_Task_State(TICK_LOCK_SEND,STOP);
								i=0;
            }
            else {
							i++;
							if(i%400==100) {
               count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(TICK_LOCK_SEND,START);
                Tick_Lock_Send();

                if(count==6) {
									count=0;
                    Set_Task_State(TICK_LOCK_SEND,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
		else{
			count=0;
			temp=0;
			i=0;
		}
    return temp;
}
//20��Ϣ�ϱ�  ��+״̬
uint16_t Open_Lock_Data_Send_Task(){
    static uint8_t count;
    static uint16_t temp;
    static uint16_t i;
		//static uint16_t send_count_20=2;
	
		//static uint8_t once_flag;  
   if(Get_Task_State(OPEN_LOCK_DATA_SEND)){
		 if(open_lock_data_reply_Result==1  && look_status_send_count==0) {
								open_lock_data_reply_Result=0;
								LOG_I("20_REPLY_OK");
								globle_Result=0xFF;	
								Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
								temp=OPEN_LOCK;
								i=0;
				}
				else {
					i++;		
					if(i%user_time==1){
								if(look_status_send_count)
								look_status_send_count--;
								LOG_I("send_count_20=%d",look_status_send_count);
                count++;
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(OPEN_LOCK_DATA_SEND,START);
							  Open_Lock_Data_Send();
                if(count==user_count){
										count=0;
                    Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
                    temp=TIME_OUT;
                }
           }
        }
    }
	 	else{
			count=0;
			temp=0;
			i=0;
		}
    return temp;
}


//21
uint16_t Open_Lock_Data_Send_Moto_Task(){
    static uint8_t count;
    static uint16_t temp;
    static uint16_t i;
//		static uint8_t once_flag;
    
   if(Get_Task_State(OPEN_LOCK_DATA_SEND_MOTO)){
		 
		 if(open_lock_data_moto_reply_Result==1) {
							  LOG_I("21_REPLY_OK");
								globle_Result=0xFF;
								open_lock_data_moto_reply_Result=0;
//                send_count++;
                temp=OPEN_LOCK;
                Set_Task_State(OPEN_LOCK_DATA_SEND_MOTO,STOP);
								i=0;
				}
				else {
					i++;
					//LOG_I("I:%d",i);
					if(i%user_time==10){
						count++;
						//LOG_I("Ii:%d",i);
                temp=NO_ASK;
                globle_Result=NO_ASK;
                Set_Task_State(OPEN_LOCK_DATA_SEND_MOTO,START);
							  Open_Lock_Data_Send_Moto();
                if(count==user_count) {
									count=0;
                    Set_Task_State(OPEN_LOCK_DATA_SEND_MOTO,STOP);
                    temp=TIME_OUT;
                }
            }
        }
    }
	 	else{
			count=0;
			temp=0;
			i=0;
		}
    return temp;
}





//5s ����ģ�黽�� ��һ���ź�
void NB_WAKE_Task(){
	 static uint8_t i;

		if(Get_Task_State(GET_DB_VAL)==STOP){
			i++;
			if(i%200==1) {
				Set_Task_State(GET_DB_VAL,START);		
				i=1;
			}
		}
}
uint8_t last_lock_state;
//���״̬�����仯�ϱ�����
void State_Change_Task(){
	static uint8_t count;
			count++;
			if(count>20)count=20;
	
			static uint8_t sw_count;
			static uint8_t sw_flag;			

			static uint8_t sw_count_2;
			static uint8_t sw_flag_2;			

	
//			static uint8_t sw2_count;
//			static uint8_t sw1_count;
//	
//			static uint8_t sw2_flag;
//			static uint8_t sw1_flag;

//			if(Check_SW2()==1){
//					sw2_count++;
//					if(sw2_count>10) sw2_count=10;
//					if(sw2_count==3){
//						sw2_flag=1;
//					}
//			}
//			else sw2_count=0;
//			
//			if(Check_SW1()==0) {
//				sw1_count++;
//				if(sw1_count>10) sw1_count=10;
//					if(sw1_count==3){
//						sw1_flag=1;
//					}
//			}
//			else sw1_count=0;

			
			if(Check_SW2()==1 && Check_SW1()==0){
			//if( Check_SW1()==0){
					sw_count++;
					if(sw_count>10) sw_count=10;
					if(sw_count==3){
					sw_flag=1;
						lock_state[0]=1;
				}
			}
			else{
				lock_state[0]=0;
				sw_count=0;
			}
			
			if( Check_SW2()==0){
					sw_count_2++;
					if(sw_count_2>10) sw_count_2=10;
					if(sw_count_2==3){
					sw_flag_2=1;
						C0_lock_state[0]=1;
				}
			}
			else{
				C0_lock_state[0]=0;
				sw_count_2=0;
			}
			
		
			
			
//			if(sw_flag==1/*Check_SW2()==1 && Check_SW1()==0 */){
//				lock_state[0]=1;
//				sw_flag=0;
//			}
//			else{
//				lock_state[0]=0;
//			}
			if(last_lock_state != lock_state[0]){
					if(moro_task_flag==1){
					}
					else{
						if(lock_state[0]==1) buzzer_task_flag=1;
							sleep_time=0;
							last_lock_state=lock_state[0];
							LOG_I("State_Change");		
							//�������ظ���Ҫ����ms���ڼ�״̬�б���Ҫֱ�ӷ��ͣ�����ǰ��ģ����ַ�������״̬����
							
							//Set_Task_State(OPEN_LOCK_DATA_SEND,1); //״̬�ı������ϴ�				
							
							//if(count>10){
									//Open_Lock_Data_Send();  		
								//LOG_I("State_Change111111111");	
							//}
							open_lock_data_reply_Result=0;
						
							Set_Task_State(OPEN_LOCK_DATA_SEND,1); //״̬�ı������ϴ�				
							look_status_send_count+=3;
						  if(look_status_send_count>=3){
									look_status_send_count=3;
							}
							user_ble_send_flag=1;
							TX_DATA_BUF[0]=0x52;		// CMD
							TX_DATA_BUF[1]=TOKEN[0];TX_DATA_BUF[2]=TOKEN[1];TX_DATA_BUF[3]=TOKEN[2];TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
							TX_DATA_BUF[5]=0x01;    //LEN
							TX_DATA_BUF[6]=lock_state[0];
				}
		}
}
//��ȡ�ź�ֵ
uint16_t AT_GET_DB_TASK(){
		static uint8_t count=0;
    static uint16_t temp;
    static uint16_t i;
    static uint8_t tmp[10];
		uint16_t length = 10; 
   if(Get_Task_State(GET_DB_VAL)) {
   		      if(Get_Uart_Data_Processing_Result()==CSQ_OK && Db_val!=99   && Db_val!=0) {
								globle_Result=0xFF;
                Set_Task_State(GET_DB_VAL,STOP);
								i=0;
								count=0;
								temp=0xff;								
            }
            else {
								i++;
								temp=0;
                if(i%20==1){  //�ط����
										count++;
										AT_Command_Send(CSQ);
										if(count<20){
										tinyfs_read(ID_dir_2,RECORD_KEY2,tmp,&length);//����tmp��
											if(strncmp("SET_OK",(char*)tmp,sizeof("SET_OK"))!=0){
												buzzer_task_flag=1;
											}
											if(reset_flag==1){
												buzzer_task_flag=1;
											}
										}
										if(count>=20 && count<40) {
											buzzer_task_flag=1;
										}
										else if(count>=40){
											Set_Task_State(GET_DB_VAL,STOP);
											temp=TIME_OUT;
											RESET_NB();
											count=0;
										}
								}
            }
     }
    else{
			count=0;
			//temp=0;
			i=0;
		}
    return temp;
}

//��ѯģ������
uint16_t AT_User_Reply(){
static uint8_t step=0;
static uint8_t count=0;
uint8_t temp =0xFF;
		count++;
		if(count%10==1){
		switch(step){
				case 0:    ///AT
					step++;				
					AT_Command_Send(AT);  
					//return 0xff;
				break;
			
				case 1:  //CGATT
					step++;
					AT_Command_Send(CGATT_R);
					//return 0xff;
				break;
									
				case 2:  //CEREG
					step++;
					AT_Command_Send(CEREG_R);
					temp=0xAA;
					//return 0xff;
				break;
		}	
	}
	return temp;
}

//����ģ������
uint16_t AT_User_Set(){
static uint8_t step=0;
static uint8_t count=0;
 uint8_t temp =0xFF;
	count++;
	if(count%20==1){
		switch(step){
			case 0:    ///AT
				step++;				
				AT_Command_Send(AT);
				//buzzer_task_flag=1;			
				temp=0xFF;
			break;
			
			case 1:  //CTM2MSETPM
				step++;
				AT_Command_Send(SKTCREATE);
				//buzzer_task_flag=1;
				temp=0xFF;
			break;
		
			case 2:    ///AT
				step++;				
				AT_Command_Send(SKTCONNECT);
				//buzzer_task_flag=1;			
				temp=0xAA;
			break;		
		}
	}
	return temp;
}


uint8_t AT_tset_flag=0;

void AT_User_Reply_Task(){
		if(AT_tset_flag==1){
			if(AT_User_Reply()==0xAA)
			AT_tset_flag=0;
		}
}

void AT_User_Set_Task(){
		if(AT_tset_flag==2){
			if(AT_User_Set()==0xAA)
			AT_tset_flag=0;
			if(reset_flag==1){
				reset_flag=0;
				buzzer_task_flag_2=1;
			}
			
		}
}

//���ó�ʼ����һ��
uint16_t AT_INIT(){
	static int step=0;
	static int count=0;
	static int count_out=0;
	static int set_flag=0;
//	static int once_flag=0;
	count++;	
	static uint8_t tmp[10];
	uint16_t length = 10; 
	const static uint8_t no_wirt_data[10]="3141592654";

	
	tinyfs_read(ID_dir_2,RECORD_KEY2,tmp,&length);//����tmp��
	if(strncmp("SET_OK",(char*)tmp,sizeof("SET_OK"))==0){
		return 0xff;
	}
	
	if(count%5==0){
		switch(step){
			
			case 0:
			//��ȡsim����������
			if(Get_Uart_Data_Processing_Result()==CIMI_OK){
				globle_Result=0xff;				

					memcpy(&CIMI_DATA[0],&AT_RX_DATA_BUF[0],15);
					tinyfs_write(ID_dir_3,RECORD_KEY9,CIMI_DATA,sizeof(CIMI_DATA));	
					tinyfs_write_through();
					LOG_I("WIRT_OK %s",CIMI_DATA);		 					
				
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
				AT_Command_Send(CIMI);
				buzzer_task_flag=1;
			}
			break;
			
			
			case 1:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==CGSN_OK){
				globle_Result=0xff;
				
				if(strncmp((char*)no_wirt_data,(char*)SHORT_NAME,sizeof(no_wirt_data))==0){			
					//LOG_I("WIRT_OK");		
					
					memcpy(&SHORT_NAME[0],&AT_RX_DATA_BUF[8-3],7+3);
					//��7λ�豸��д�룬��Ϊ�����㲥����
					tinyfs_write(ID_dir_2,RECORD_KEY1,SHORT_NAME,sizeof(SHORT_NAME));	
					
					tinyfs_write_through();
					LOG_I("WIRT_OK %s",SHORT_NAME);		 
					
					//start_adv();   //���¹㲥
				}
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
				AT_Command_Send(CEREG);
				buzzer_task_flag=1;
			}
			break;
			

			case 8:
			//DELAY_US(200000); //50ms
			if(Get_Uart_Data_Processing_Result()==OK_AT){
				globle_Result=0xff;
				step++;
				count_out=0;
				
				LOG_I("set_flag:%d",set_flag);
				if(set_flag>9){
						RESET_NB();//
						DELAY_US(1000*1000*2);
						platform_reset(0); 					//��ʼ���ɹ�������
				}
				else{
					tinyfs_write(ID_dir_2,RECORD_KEY2,(uint8_t*)"SET_OK",sizeof("SET_OK"));	
					tinyfs_write_through();
					platform_reset(0); 					//��ʼ���ɹ�������
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
			default : /* ��ѡ�� */
      break;
			//statement(s);
		}
		}
	return 0x88;
}
static  uint16_t set_sleep_time;
//��������ʱ��
void Set_Sleep_Time(uint16_t time_s){
	set_sleep_time=time_s;
}
//��������
void Sleep_Task(){
	if(sleep_time*50>set_sleep_time*1000){
			ls_sleep_enter_lp2();
	}
}
//extern uint8_t	psaaword_task_flag;  //����������ʼ���
//extern uint8_t	key_task_flag;			 //����Կ����ʼ���
void Password_Task(){
	static uint16_t count;
	static uint8_t buf_1[6];
	static uint8_t buf_2[6];
	if(psaaword_task_flag!=0){
		count++;
		//10s
		if(count<200){
					if(psaaword_task_flag==0xA1){
						memcpy (&buf_1[0], &NEW_PASSWORD_BUF[0], 6);
					}
					else if(psaaword_task_flag==0xA2){
						memcpy (&buf_2[0], &NEW_PASSWORD_BUF[0], 6);
						
						user_ble_send_flag=1;
						psaaword_task_flag=0;
						
						TX_DATA_BUF[0]=0xA1;		  // CMD
						TX_DATA_BUF[1]=TOKEN[0];
						TX_DATA_BUF[2]=TOKEN[1];
						TX_DATA_BUF[3]=TOKEN[2];
						TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
						TX_DATA_BUF[5]=0x01;      //LEN
						TX_DATA_BUF[6]=0x01;      //ʧ��
						//ͨ��֡1 PWD��Ϊԭ���롣
						if((strncmp((char*)buf_1,(char*)PASSWORD,6)==0)){
							TX_DATA_BUF[6]=0x00;    //
							//д��������������Ч
							memcpy (&PASSWORD[0], &buf_2[0], 6);
							tinyfs_write(ID_dir_3,RECORD_KEY3,(uint8_t*)buf_2,sizeof(PASSWORD));	
							tinyfs_write_through();
							count=0;
						}
					}
		}
		else{
				count=0;
				psaaword_task_flag=0;
				user_ble_send_flag=1;
				TX_DATA_BUF[0]=0xA1;		// CMD
				TX_DATA_BUF[1]=TOKEN[0];
				TX_DATA_BUF[2]=TOKEN[1];
				TX_DATA_BUF[3]=TOKEN[2];
				TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
				TX_DATA_BUF[5]=0x01;    //LEN
				TX_DATA_BUF[6]=0x01;    //ʧ��
		}
	}
}
void Key_Task(){
	static uint16_t count;
	static uint8_t buf_L[8];
	static uint8_t buf_H[8];

	if(key_task_flag!=0){
			count++;
		if(count<200){
					if(key_task_flag==0xA5){
						memcpy (&buf_L[0], &NEW_KEY_BUF[0], 8);
					}
					else if(key_task_flag==0xA6){
						memcpy (&buf_H[0], &NEW_KEY_BUF[0], 8);
						user_ble_send_flag=1;
						TX_DATA_BUF[0]=0xA5;		// CMD
						TX_DATA_BUF[1]=TOKEN[0];
						TX_DATA_BUF[2]=TOKEN[1];
						TX_DATA_BUF[3]=TOKEN[2];
						TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
						TX_DATA_BUF[5]=0x01;    //LEN
						TX_DATA_BUF[6]=0x00;    //�ɹ�
						//ͨ��֡1 ��8λ
						//ͨ��֡2 ��8λ
						//д��������������Ч

						//memcpy (&key[0], &buf_L[0],8);
						//memcpy (&key[8], &buf_H[0],8);
						
						tinyfs_write(ID_dir_3,RECORD_KEY4,(uint8_t*)buf_L,8);	
						tinyfs_write(ID_dir_3,RECORD_KEY5,(uint8_t*)buf_H,8);	
						tinyfs_write_through();
						count=0;
						key_task_flag=0xff;
					}
		}
		else{
				count=0;
				key_task_flag=0;
				user_ble_send_flag=1;
				TX_DATA_BUF[0]=0xA5;		// CMD
				TX_DATA_BUF[1]=TOKEN[0];
				TX_DATA_BUF[2]=TOKEN[1];
				TX_DATA_BUF[3]=TOKEN[2];
				TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
				TX_DATA_BUF[5]=0x01;    //LEN
				TX_DATA_BUF[6]=0x01;    //ʧ��
		}
	}
}

void Once_Send(){
	static uint8_t once_flag=1;
	if(once_flag){
		once_flag=0;
		uint8_t wkup_source = get_wakeup_source();   //��ȡ����Դ
    LOG_I("wkup_source:%x",wkup_source) ;
		if(Get_Task_State(OPEN_LOCK_DATA_SEND)){
					//Open_Lock_Data_Send(0,lock_state[0]);  		
          //Set_Task_State(OPEN_LOCK_DATA_SEND,START);
		}
	}
}
