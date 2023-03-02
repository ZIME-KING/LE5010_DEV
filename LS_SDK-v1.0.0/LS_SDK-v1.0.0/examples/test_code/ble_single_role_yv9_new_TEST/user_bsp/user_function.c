#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"
#include <string.h>

//#define USB_CHECK PA03
#define USB_CHECK   PA00
//#define	USB_CHECK_B PB06

uint8_t look_status_send_count;

//定义重发时间 400是20s
//#define	USER_TIME 400
uint8_t user_time  =100;
uint8_t user_count =30;


tinyfs_dir_t ID_dir_1;
tinyfs_dir_t ID_dir_2;
tinyfs_dir_t ID_dir_3;

union Data
{
    uint16_t i;
    uint8_t  str[2];
} u16_to_u8;

uint8_t T0_enable=0;  //启动数据上报  			Start_Lock_Send_Task
uint8_t T1_enable=0;	//开锁数据请求				Open_Lock_Send
uint8_t T2_enable=0;	//心跳包发送					Tick_Lock_Send
uint8_t T3_enable=0;	//状态变更数据发送  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)
uint8_t T4_enable=0;  //获取信号强度				AT指令相关
uint8_t T5_enable=0;  //电机动作完成发送包

uint8_t T6_enable=0;  //
uint8_t T7_enable=0;  //
uint8_t T8_enable=0;  //


uint8_t open_lock_reply_Result=0;
uint8_t tick_reply_Result=0;
uint8_t open_lock_data_reply_Result=0;
uint8_t open_lock_data_moto_reply_Result=0;
uint8_t start_lock_reply_Result=0;
uint8_t send_mode_Result=0;//进入udp发送模式成功标记

uint32_t adc_value = 0;						//
uint8_t AT_RX_DATA_BUF[50];  			//保存接受到回复信息  +NNMI:2,A101 ->0xA1,0x01
uint8_t Db_val=0x00;

volatile uint8_t recv_flag = 0;
static ADC_HandleTypeDef hadc;
void Read_Last_Data(void);
static void lsadc_init(void);


uint8_t test_status;
uint8_t scan_card_flag;

#define LED_R     PA05
#define LED_G     PC00

#include "reg_lsgpio.h"

void User_Init() {
    lsadc_init();
    HAL_ADCEx_InjectedStart_IT(&hadc);
    Button_Gpio_Init();
    Lock_gpio_init();
    Basic_PWM_Output_Cfg();

    io_cfg_output(LED_R);   //LED_0
    io_write_pin(LED_R,0);	//LED_0

    io_cfg_output(LED_G);   //LED_1
    io_write_pin(LED_G,0);  //LED_1

    io_cfg_output(PA08);   //REST
    io_write_pin(PA08,0);  //REST

    io_cfg_output(PB07);   //充电控制
    io_write_pin(PB07,1);  //

    io_cfg_output(PC01);   //外置紫外灯
    io_write_pin(PC01,0);  //

    io_cfg_output(PA06);   //
    io_write_pin(PA06,1);	 //

		//HAL_IWDG_Init(32756*5);  	 //5s看门狗
    HAL_RTC_Init(2);    				 //RTC内部时钟源
  	RTC_wkuptime_set(12*60*60*1000); //单位ms  唤醒时间3h  休眠函数在sw.c 中
		//RTC_wkuptime_set(20*1000);	 //唤醒时间60s  休眠函数在sw.c 中
    Set_Sleep_Time(120);
		
    //WAKE_UP();                   //POWER_100MS下拉

    Set_Task_State(GET_DB_VAL,START);
}

extern uint8_t RTC_flag;
uint16_t uv_count;

void LED_TASK() {
    static uint8_t flag;
    static uint8_t count;

    if(uv_count>0) {
        //io_cfg_output(PC01);   //外置紫外灯
        io_write_pin(PC01,1);  //
        uv_count--;
    } else {
        //io_cfg_output(PC01);   //外置紫外灯
        io_write_pin(PC01,0);  //
    }
    //来自RTC的启动，不要亮灯
    if (RTC_flag==1) {
        io_write_pin(LED_G, 0);
        io_write_pin(LED_R, 0);
        io_write_pin(PC01, 0);
    }
    else {
        //io_write_pin(PC01, 0);
        count++;
        if(count%20==0) {
            if(flag==1)flag=0;
            else flag=1;
        }
        if(BLE_connected_flag==1) {
            io_write_pin(LED_G, 0);
            io_write_pin(LED_R, 1);
        }
        else {
            //5V接入
            io_cfg_input(USB_CHECK);   //PB09 config output
            if(io_read_pin(USB_CHECK)==1) {
                //LOG_I("usb_in");
                //20~90 绿灯闪
                if(VBat_value>20 && VBat_value<=90) {
                    io_write_pin(LED_G, flag);
                    io_write_pin(LED_R, 0);
                }
                //<20 红灯闪
                else if( VBat_value<=20) {
                    io_write_pin(LED_G, 0);
                    io_write_pin(LED_R, flag);
                }
                //20~90 绿灯常量
                else if(VBat_value>90) {
                    io_write_pin(LED_G, 1);
                    io_write_pin(LED_R, 0);
                }
            }
            else {
                if(VBat_value>20) {
                    io_write_pin(LED_G, 1);
                    io_write_pin(LED_R, 0);
                }
                //<20 红灯
                else if(VBat_value<=20) {
                    io_write_pin(LED_G, 0);
                    io_write_pin(LED_R, 1);
                }
            }
        }
    }
}
//冒泡排序
void _f(uint16_t a[],char len) {
    int n;  //存放数组a中元素的个数
    int i;  //比较的轮数
    int j;  //每轮比较的次数
    int buf;  //交换数据时用于存放中间数据

    n=len;
    for (i=0; i<n-1; ++i)  //比较n-1轮
    {
        for (j=0; j<n-1-i; ++j)  //每轮比较n-1-i次,
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

//为了让电量显示准确，加入校准100%时的电池电压功能
uint16_t global_vbat_max;
void Get_Vbat_Task() {
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

    //采集实时的电压
    if(lock_task_flag_1==0 && lock_task_flag_2==0 && buzzer_task_flag==0) {
        count++;
        //50MS 采集一次 20次 1s
        if(count>=10) {
            count=0;;
            temp_ADC_value[i]=Get_Vbat_val();
            if(i>19) {
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
    //在启动2s且未插电的时候记录一下电量，另外在休眠的时候也会记录一次 作为
    if(io_read_pin(USB_CHECK)!=1 ) {
        if(ADC_Count==2) {
            //once_flag=0;
            //LOG_I("1_Vbat:%d %",VBat_value);
            //tinyfs_write(ID_dir_3,RECORD_KEY7,&VBat_value,1);
            //tinyfs_write_through();
        }
        //完成10组更新一次 10ms 一次 20次一组 200ms  10次更新显示 2s
        //第一次更新在200ms的时候
        //为放电时电量显示
        //if(count%400==1  && ADC_Count>1){
        //LOG_I("Vbat:%d mV",true_VBat_value);
        static uint8_t last_ADC_count;
        if(ADC_Count != last_ADC_count) {
            last_ADC_count=ADC_Count;
            true_VBat_value=(true_VBat_value-3000)*100/(global_vbat_max-3000);
            true_VBat_value=(true_VBat_value)*100*0.01;
#ifdef USER_TEST
            LOG_I("Vbat:%d",true_VBat_value);
            LOG_I("ADC_Count:%d",ADC_Count);
#endif
            VBat_value=true_VBat_value;  //VBat_value 全局电量显示
        }
    }
    //2200mha 电池360ma充电电流预计6h小时充满 216s 加1% 这里5ms进一次
    //为冲电时电量显示
    if(io_read_pin(USB_CHECK)==1 ) {
        change_time++;
        sleep_time=0;
//			if(count%200==1  && ADC_Count>1 && true_VBat_value<=global_vbat_max-100){  //大于4.1v都算涓流算时间看电量
//									true_VBat_value=true_VBat_value-200;
//									if(true_VBat_value<3000)  true_VBat_value=3000;
//									//LOG_I("c_Vbat:%d mV",true_VBat_value);
//									true_VBat_value=(true_VBat_value-3000)*100/(global_vbat_max-3000);
//									true_VBat_value=(true_VBat_value)*100*0.01;
//									//LOG_I("c_Vbat:%d %",true_VBat_value);
//									VBat_value=true_VBat_value;  //VBat_value 全局电量显示
//			}
//			else{
        //300s电量+1
        if(change_time>=(300*200)) {
            change_time=0;
            VBat_value=VBat_value+1;
            // LOG_I("c_c_VBat_value:%d %",true_VBat_value);
            //LOG_I("c_c_VBat_value:%d %",VBat_value);
            if(VBat_value>=100) {
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



//锁信号强度获取
uint8_t Get_dBm() {
    return Db_val;
}

/////////////////////////////////////adc_value_num////////////////////////////////////////////////////
//电阻分压1/2，1/4  硬件上VREF为3.3V
uint16_t Get_Vbat_val() {
//	uint16_t adc_value_num;
    if(recv_flag == 1) {
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
void Read_Last_Data() {
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
    uint16_t length_11 = 15;
    uint16_t length_one = 1;

//uint8_t temp_val = 0xAA;

    tinyfs_mkdir(&ID_dir_1,ROOT_DIR,50);  //创建目录
    tinyfs_mkdir(&ID_dir_2,ROOT_DIR,51);  //创建目录
    tinyfs_mkdir(&ID_dir_3,ROOT_DIR,52);  //创建目录
    tinyfs_read(ID_dir_1,RECORD_KEY6,tmp,&length_3);//读到tmp中
    LOG_I("flash_init");
    LOG_I("%s",tmp);

    if(strncmp("ok",(char*)tmp,sizeof("ok"))!=0) {
        LOG_I("w_init");
        tinyfs_write(ID_dir_1,RECORD_KEY6,(uint8_t*)"ok",sizeof("ok"));
        tinyfs_write(ID_dir_1,RECORD_KEY11,(uint8_t*)MICI_DATA,sizeof(MICI_DATA));
        tinyfs_write(ID_dir_2,RECORD_KEY2,(uint8_t*)"NO_SET",sizeof("NO_SET"));
        tinyfs_write(ID_dir_2,RECORD_KEY1,(uint8_t*)SHORT_NAME,sizeof(SHORT_NAME));
        tinyfs_write(ID_dir_3,RECORD_KEY3,(uint8_t*)PASSWORD,sizeof(PASSWORD));
        tinyfs_write(ID_dir_3,RECORD_KEY4,(uint8_t*)&key[0],8);
        tinyfs_write(ID_dir_3,RECORD_KEY5,(uint8_t*)&key[8],8);

        //tinyfs_write(ID_dir_3,RECORD_KEY_T,(uint8_t*)&temp_val,1);	//给测试模式标记成0xBB（不开启）
        SYSCFG->BKD[7]=0;
#ifdef USER_TEST
        open_count=0;
        tinyfs_write(ID_dir_2,RECORD_KEY10,(uint8_t*)&open_count,1);
#endif
        u16_to_u8.i=4200;
        tinyfs_write(ID_dir_3,RECORD_KEY8,(uint8_t*)u16_to_u8.str,2);
        tinyfs_write_through();
    }

    tinyfs_read(ID_dir_1,RECORD_KEY11,tmp,&length_11);//读到tmp中
    LOG_I("read_MICI_DATA");
    LOG_I("%s",tmp);
    memcpy (&MICI_DATA[0], &tmp[0], 15);

    tinyfs_read(ID_dir_2,RECORD_KEY1,tmp,&length_10);//读到tmp中
    memcpy (&SHORT_NAME[0], &tmp[0], strlen((char*)&SHORT_NAME));
    memcpy (&NEW_SHORT_NAME[0], &tmp[0], strlen((char*)&SHORT_NAME));
    LOG_I("read_id");
    LOG_I("%s",SHORT_NAME);

    tinyfs_read(ID_dir_2,RECORD_KEY2,tmp,&length_7);//读到tmp中
    LOG_I("read_flag");
    LOG_I("%s",tmp);

    tinyfs_read(ID_dir_3,RECORD_KEY3,tmp,&length_6);//读到tmp中
    LOG_I("KEY3");
    LOG_HEX(tmp,6);
    memcpy (&PASSWORD[0], &tmp[0],6);

    tinyfs_read(ID_dir_3,RECORD_KEY4,tmp,&length_8);//读到tmp中
    LOG_I("KEY4");
    LOG_HEX(tmp,8);
    memcpy (&key[0], &tmp[0],8);

    tinyfs_read(ID_dir_3,RECORD_KEY5,tmp,&length_8);//读到tmp中
    LOG_I("KEY5");
    LOG_HEX(tmp,8);
    memcpy (&key[8], &tmp[0],8);

    tinyfs_read(ID_dir_3,RECORD_KEY7,tmp,&length_1);//读到tmp中
    LOG_I("KEY7_last_Vbat");
    LOG_I("Vbat,%d",tmp[0]);
    memcpy (&VBat_value, &tmp[0],1);


    tinyfs_read(ID_dir_3,RECORD_KEY8,tmp,&length_2);//读到tmp中
    LOG_I("KEY8_last_Vbat_max");
    memcpy (&global_vbat_max, &tmp[0],2);
    LOG_I("last_Vbat_max,%d",global_vbat_max);

    tinyfs_read(ID_dir_3,RECORD_KEY9,tmp,&length_15);//读到tmp中
    memcpy (&CIMI_DATA[0], &tmp[0],15);
    LOG_I("KEY9_CIMI");
    LOG_I("CIMI,%15s",CIMI_DATA);

    tinyfs_read(ID_dir_3,RECORD_KEY_T,tmp,&length_one);//读到tmp中
    memcpy (&test_mode_flag, &tmp[0],1);
    LOG_I("KEY_T_test_mode_flag");
    LOG_I("test_mode_flag,%d",test_mode_flag);



#ifdef USER_TEST
    tinyfs_read(ID_dir_2,RECORD_KEY10,tmp,&length_1_1);//读到tmp中
    memcpy (&open_count, &tmp[0],1);
    LOG_I("KEY10_open_count");
    LOG_I("open_count,%d",open_count);
    if(open_count>=200) {
        tinyfs_write(ID_dir_2,RECORD_KEY2,(uint8_t*)"SET_NO",sizeof("SET_NO"));
        tinyfs_write_through();
    }
#endif

}
//////////////////////////////////////////////////////////////////////////////////////
/********************
 **数据处理, 一帧接受完成 后跑一次
//在接收中断后面跑一次
******************/
void Uart_Data_Processing() {
    if(frame[uart_frame_id].status!=0) {    			//接收到数据后status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        LOG_HEX((uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length);
        //接收到的数据 到uart2  透传
        if( frame[uart_frame_id].buffer[0]==0x7f && frame[uart_frame_id].buffer[1]==0x09  && frame[uart_frame_id].length==11) {
            RFID_DATA_2[0]=frame[uart_frame_id].buffer[6];
            RFID_DATA_2[1]=frame[uart_frame_id].buffer[7];
            RFID_DATA_2[2]=frame[uart_frame_id].buffer[8];
            RFID_DATA_2[3]=frame[uart_frame_id].buffer[9];
            LOG_HEX(&RFID_DATA_2[0],4);

            Set_Task_State(OPEN_LOCK_DATA_SEND,START);   //数据上传服务器任务
            user_ble_send_flag=1;                        //蓝牙数据发送开启

            TX_DATA_BUF[0]=0x52;		// CMD
            TX_DATA_BUF[1]=TOKEN[0];
            TX_DATA_BUF[2]=TOKEN[1];
            TX_DATA_BUF[3]=TOKEN[2];
            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
            TX_DATA_BUF[5]=0x08;    	//LEN
            TX_DATA_BUF[6]=0x01;			//主锁无    ，关闭模式
            TX_DATA_BUF[7]=0x06;      //在线情况 1，2全在线，具体见协议文档
            TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
            TX_DATA_BUF[9]=0x02;
            TX_DATA_BUF[10]=RFID_DATA_2[0];
            TX_DATA_BUF[11]=RFID_DATA_2[1];
            TX_DATA_BUF[12]=RFID_DATA_2[2];
            TX_DATA_BUF[13]=RFID_DATA_2[3];
        }
        else if(frame[uart_frame_id].buffer[0]==0x7f && frame[uart_frame_id].buffer[1]==0x03  && frame[uart_frame_id].length==5) {
            if(RFID_DATA_2[0]+RFID_DATA_2[1]+RFID_DATA_2[2]+RFID_DATA_2[3] != 0x00 ) {
                RFID_DATA_2[0]=0x00;
                RFID_DATA_2[1]=0x00;
                RFID_DATA_2[2]=0x00;
                RFID_DATA_2[3]=0x00;
                LOG_HEX(&RFID_DATA[0],4);

                Set_Task_State(OPEN_LOCK_DATA_SEND,START);   //数据上传服务器任务
                user_ble_send_flag=1;                        //蓝牙数据发送开启
                TX_DATA_BUF[0]=0x52;		// CMD
                TX_DATA_BUF[1]=TOKEN[0];
                TX_DATA_BUF[2]=TOKEN[1];
                TX_DATA_BUF[3]=TOKEN[2];
                TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
                TX_DATA_BUF[5]=0x08;    	//LEN
                TX_DATA_BUF[6]=0x01;			//主锁无    ，关闭模式
                TX_DATA_BUF[7]=0x06;      //在线情况 1，2全在线，具体见协议文档
                TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
                TX_DATA_BUF[9]=0x02;
                TX_DATA_BUF[10]=RFID_DATA_2[0];
                TX_DATA_BUF[11]=RFID_DATA_2[1];
                TX_DATA_BUF[12]=RFID_DATA_2[2];
                TX_DATA_BUF[13]=RFID_DATA_2[3];
            }
        }
        frame[uart_frame_id].status=0;					//处理完数据后status 清0;
    }
}
void Uart_2_Data_Processing() {
    int count;
//		int len;
    if(frame_2[uart_2_frame_id].status!=0) {						//接收到数据后status=1;
        //HAL_UART_Transmit(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length,100);
        frame_2[uart_2_frame_id].buffer[frame_2[uart_2_frame_id].length]='\0';
        LOG_I("%s",frame_2[uart_2_frame_id].buffer);
        //返回接收到的数据到uart1上
        //收到NNMI卡号返回
        if( strncmp("AT+CGSN\r\r\n",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CGSN\r\r\n"))==0) {
            globle_Result=CGSN_OK;
//						 for(int i=0; i<frame_2[uart_2_frame_id].length; i++) {
//                if(frame_2[uart_2_frame_id].buffer[i]=='"') {
//                    count=i+1;
//                    break;
//                }
//            }
            count=strlen("AT+CGSN\r\r\n");
            for(int i=0; i<15; i++) {
                AT_RX_DATA_BUF[i]=frame_2[uart_2_frame_id].buffer[count+i];
            }
            LOG_I("%s",AT_RX_DATA_BUF);
        }
        //收到信号强度值
        else if( strncmp("AT+CSQ\r\r\n+CSQ: ",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CSQ\r\r\n+CSQ: "))==0)  {
            globle_Result=CSQ_OK;
            if(frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\r\n+CSQ: ")+1]==',') {
                Db_val=frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\n\r\n+CSQ: ")+0]-'0';
            }
            else {
                Db_val=(frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\r\n+CSQ: ")+0]-'0')*10;
                Db_val+=frame_2[uart_2_frame_id].buffer[strlen("AT+CSQ\r\r\n+CSQ: ")+1]-'0';
            }
        }
        //收到sim卡卡号
        else if( strncmp("AT+CIMI\r\r\n",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CIMI\r\r\n"))==0)  {

            count=strlen("AT+CIMI\r\r\n");
            if(frame_2[uart_2_frame_id].buffer[count]=='E') {
                globle_Result=CIMI_ERROR;
            }
            else {
                globle_Result=CIMI_OK;
                for(int i=0; i<15; i++) {
                    AT_RX_DATA_BUF[i]=frame_2[uart_2_frame_id].buffer[count+i];
                }
                LOG_I("%s",AT_RX_DATA_BUF);
            }
        }


        //收到服务器回复值
        else if(strncmp("\r\nRECV FROM:1",(char*)frame_2[uart_2_frame_id].buffer,strlen("\r\nRECV FROM:1"))==0) {
            //globle_Result=OK_ASK;
            for(int i=0; i<frame_2[uart_2_frame_id].length; i++) {
                if(frame_2[uart_2_frame_id].buffer[i]==0x58 && frame_2[uart_2_frame_id].buffer[i+1]==0x59) {
                    count=i;
                    break;
                }
            }
            for(int i=0; i<frame_2[uart_2_frame_id].length-count; i++) {
                AT_RX_DATA_BUF[i]=frame_2[uart_2_frame_id].buffer[count+i];
            }
            LOG_HEX(AT_RX_DATA_BUF,frame_2[uart_2_frame_id].length-count);


            //标记——————这里应该要加CRC的
            //OK_ASK 服务器有应答
            if(AT_RX_DATA_BUF[0]==0x58 && AT_RX_DATA_BUF[1]==0x59) {
                globle_Result=OK_ASK;
                //是OPEN_LOCK
                if(AT_RX_DATA_BUF[4]== 0x10 ) {


                    for(uint8_t i=5; i<AT_RX_DATA_BUF[2]-2; i+=2) {
                        if(AT_RX_DATA_BUF[i]==0x00 && AT_RX_DATA_BUF[i+1]==0x01) {
                            LOG_I("00_OPEN");
                        }
                        if(AT_RX_DATA_BUF[i]==0x01 && AT_RX_DATA_BUF[i+1]==0x01) {
                            lock_task_flag_1_temp=1;
                            open_lock_reply_Result=1;
                            LOG_I("01_OPEN");
                        }
                        if(AT_RX_DATA_BUF[i]==0x02 && AT_RX_DATA_BUF[i+1]==0x01) {
                            lock_task_flag_2_temp=1;
                            open_lock_reply_Result=1;
                            LOG_I("02_OPEN");
                        }
                    }
                }
                //心跳包上传成功服务器回复
                else if(AT_RX_DATA_BUF[4]==0x02) {
                    tick_reply_Result=1;
                }
                //数据包上传成功服务器回复
                else if(AT_RX_DATA_BUF[4]==0x20) {
                    open_lock_data_reply_Result=1;
                    open_lock_data_moto_reply_Result=1;
                }
                //数据包上传成功服务器回复
                else if(AT_RX_DATA_BUF[4]==0x21) {
                    //open_lock_data_moto_reply_Result=1;
                }
                //启动包上传成功服务器回复
                else if(AT_RX_DATA_BUF[4]==0x01) {
                    start_lock_reply_Result=1;
                }
            }
        }
        //收到OK消息 ,以上都不满足只用OK
        else if( strncmp("ATQ0\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("ATQ0\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+CTM2MREG\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CTM2MREG\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+ECPCFG=\"slpWaitTime\",15000\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ECPCFG=\"slpWaitTime\",15000\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ CTM2MSETPM =221.229.214.202,5683,38400\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+ECPMUCFG=1,4\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+ECPMUCFG=1,4\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+CPSMS=1\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CPSMS=1\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+CPSMS=1",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CPSMS=1"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+CEREG=0\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CEREG=0\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+CGATT=1\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+CGATT=1\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }


        else if( strncmp("AT+QIPSEND=1",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+QIPSEND=1"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
            send_mode_Result=1;
        }

        else if( strncmp("AT\r\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT\r\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+QIPCSGP=1,1,\"3GNET\"\r\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+QIPCSGP=1,1,\"3GNET\"\r\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+QIPOPEN=1,1,\"UDP\",\"139.224.136.93\",50513,0,1\r\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+QIPOPEN=1,1,\"UDP\",\"139.224.136.93\",50513,0,1\r\r\nOK"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
        else if( strncmp("AT+QIPACT?\r\r\n+QIPACT",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+QIPACT?\r\r\n+QIPACT"))==0) {
            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
            globle_Result=OK_AT;
        }
//				else if( strncmp("AT+QIPACT=1\r\r\nOK",(char*)frame_2[uart_2_frame_id].buffer,strlen("AT+QIPACT=1\r\r\nOK"))==0){
//            //HAL_UART_Transmit(&UART_Config,(uint8_t*)"error \r\n",sizeof("error \r\n"),10);
//            globle_Result=OK_AT;
//        }

        frame_2[uart_2_frame_id].status=0;					//处理完数据后status 清0;
    }
}


void Uart_3_Data_Processing() {
    if(frame_3[uart_3_frame_id].status!=0) {    			//接收到数据后status=1;
        LOG_HEX((uint8_t*)frame_3[uart_3_frame_id].buffer,frame_3[uart_3_frame_id].length);
        //接收到的数据打印透传
        if( frame_3[uart_3_frame_id].buffer[0]==0x7f && frame_3[uart_3_frame_id].buffer[1]==0x09  && frame_3[uart_3_frame_id].length==11) {

            if(scan_card_flag==1) {
                RFID_DATA[0]=frame_3[uart_3_frame_id].buffer[6];
                RFID_DATA[1]=frame_3[uart_3_frame_id].buffer[7];
                RFID_DATA[2]=frame_3[uart_3_frame_id].buffer[8];
                RFID_DATA[3]=frame_3[uart_3_frame_id].buffer[9];
								TX_DATA_BUF[9]=0x01;
                TX_DATA_BUF[10]=RFID_DATA[0];
                TX_DATA_BUF[11]=RFID_DATA[1];
                TX_DATA_BUF[12]=RFID_DATA[2];
                TX_DATA_BUF[13]=RFID_DATA[3];
                LOG_HEX(&RFID_DATA[0],4);
            }
            else {
                RFID_DATA_2[0]=frame_3[uart_3_frame_id].buffer[6];
                RFID_DATA_2[1]=frame_3[uart_3_frame_id].buffer[7];
                RFID_DATA_2[2]=frame_3[uart_3_frame_id].buffer[8];
                RFID_DATA_2[3]=frame_3[uart_3_frame_id].buffer[9];
								TX_DATA_BUF[9]=0x02;
                TX_DATA_BUF[10]=RFID_DATA_2[0];
                TX_DATA_BUF[11]=RFID_DATA_2[1];
                TX_DATA_BUF[12]=RFID_DATA_2[2];
                TX_DATA_BUF[13]=RFID_DATA_2[3];
                LOG_HEX(&RFID_DATA_2[0],4);
            }

            Set_Task_State(OPEN_LOCK_DATA_SEND,START);   //数据上传服务器任务
            user_ble_send_flag=1;                        //蓝牙数据发送开启
            TX_DATA_BUF[0]=0x52;		// CMD
            TX_DATA_BUF[1]=TOKEN[0];
            TX_DATA_BUF[2]=TOKEN[1];
            TX_DATA_BUF[3]=TOKEN[2];
            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
            TX_DATA_BUF[5]=0x08;    	//LEN
            TX_DATA_BUF[6]=0x01;			//主锁无    ，关闭模式
            TX_DATA_BUF[7]=0x06;      //在线情况 1，2全在线，具体见协议文档
            TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
            //TX_DATA_BUF[9]=0x01;
            //TX_DATA_BUF[10]=RFID_DATA[0];
            //TX_DATA_BUF[11]=RFID_DATA[1];
            //TX_DATA_BUF[12]=RFID_DATA[2];
            //TX_DATA_BUF[13]=RFID_DATA[3];
        }
        else if(frame_3[uart_3_frame_id].buffer[0]==0x7f && frame_3[uart_3_frame_id].buffer[1]==0x03  && frame_3[uart_3_frame_id].length==5) {
            if(scan_card_flag==1) {
                if(RFID_DATA[0]+RFID_DATA[1]+RFID_DATA[2]+RFID_DATA[3] != 0x00 ) {
                    RFID_DATA[0]=0x00;
                    RFID_DATA[1]=0x00;
                    RFID_DATA[2]=0x00;
                    RFID_DATA[3]=0x00;
                    LOG_HEX(&RFID_DATA[0],4);
                    Set_Task_State(OPEN_LOCK_DATA_SEND,START);   //数据上传服务器任务
                    user_ble_send_flag=1;                        //蓝牙数据发送开启
                    TX_DATA_BUF[0]=0x52;		// CMD
                    TX_DATA_BUF[1]=TOKEN[0];
                    TX_DATA_BUF[2]=TOKEN[1];
                    TX_DATA_BUF[3]=TOKEN[2];
                    TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
                    TX_DATA_BUF[5]=0x08;    	//LEN
                    TX_DATA_BUF[6]=0x01;			//主锁无    ，关闭模式
                    TX_DATA_BUF[7]=0x06;      //在线情况 1，2全在线，具体见协议文档
                    TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
                    TX_DATA_BUF[9]=0x01;
                    TX_DATA_BUF[10]=RFID_DATA[0];
                    TX_DATA_BUF[11]=RFID_DATA[1];
                    TX_DATA_BUF[12]=RFID_DATA[2];
                    TX_DATA_BUF[13]=RFID_DATA[3];
                }
            }
            else {
                if(RFID_DATA_2[0]+RFID_DATA_2[1]+RFID_DATA_2[2]+RFID_DATA_2[3] != 0x00) {
                    RFID_DATA_2[0]=0x00;
                    RFID_DATA_2[1]=0x00;
                    RFID_DATA_2[2]=0x00;
                    RFID_DATA_2[3]=0x00;
                    LOG_HEX(&RFID_DATA_2[0],4);
                    Set_Task_State(OPEN_LOCK_DATA_SEND,START);   //数据上传服务器任务
                    user_ble_send_flag=1;                        //蓝牙数据发送开启
                    TX_DATA_BUF[0]=0x52;		// CMD
                    TX_DATA_BUF[1]=TOKEN[0];
                    TX_DATA_BUF[2]=TOKEN[1];
                    TX_DATA_BUF[3]=TOKEN[2];
                    TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
                    TX_DATA_BUF[5]=0x08;    	//LEN
                    TX_DATA_BUF[6]=0x01;			//主锁无    ，关闭模式
                    TX_DATA_BUF[7]=0x06;      //在线情况 1，2全在线，具体见协议文档
                    TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
                    TX_DATA_BUF[9]=0x02;
                    TX_DATA_BUF[10]=RFID_DATA_2[0];
                    TX_DATA_BUF[11]=RFID_DATA_2[1];
                    TX_DATA_BUF[12]=RFID_DATA_2[2];
                    TX_DATA_BUF[13]=RFID_DATA_2[3];
                }
            }
        }
        frame_3[uart_3_frame_id].status=0;					//处理完数据后status 清0;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void RFID_INIT_1(void)
{
    scan_card_flag=1;
    //串口用uart1，
		HAL_UART_DeInit(&UART_Config_RFID);
    io_pull_write(PA14, IO_PULL_UP);  				//设置上拉
    io_pull_write(PA15, IO_PULL_UP);  				//设置上拉
    uart1_io_init(PA14, PA15);
    UART_Config_RFID.UARTX = UART1;
    UART_Config_RFID.Init.BaudRate = UART_BAUDRATE_9600;
    UART_Config_RFID.Init.MSBEN = 0;
    UART_Config_RFID.Init.Parity = UART_NOPARITY;
    UART_Config_RFID.Init.StopBits = UART_STOPBITS1;
    UART_Config_RFID.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Config_RFID);
}



void RFID_INIT_2(void)
{
    scan_card_flag=2;
    //串口用uart1，
		HAL_UART_DeInit(&UART_Config_RFID);
    io_pull_write(PA12, IO_PULL_UP);  				//设置上拉
    io_pull_write(PA13, IO_PULL_UP);  				//设置上拉
    uart1_io_init(PA12, PA13);
    UART_Config_RFID.UARTX = UART1;
    UART_Config_RFID.Init.BaudRate = UART_BAUDRATE_9600;
    UART_Config_RFID.Init.MSBEN = 0;
    UART_Config_RFID.Init.Parity = UART_NOPARITY;
    UART_Config_RFID.Init.StopBits = UART_STOPBITS1;
    UART_Config_RFID.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Config_RFID);
}

//1s切换一次
void Scan_RDIF_Task() {
    static uint8_t temp;
    static uint8_t i;
    static uint8_t once_flag ;
    uint8_t buf[]= {0x02,0x10,0x12,0x7F,0x02,0x10,0x12};
//    if(once_flag!=0xaa) {
//        once_flag=0xaa;
//        RFID_INIT_1();
//        HAL_UART_Receive_IT(&UART_Config_RFID,uart_3_buffer,1);
//    }
    i++;
    if(i%200==1) {
        temp=!temp;
        if(temp) {
						uart1_io_deinit();
            RFID_INIT_1();
            HAL_UART_Receive_IT(&UART_Config_RFID,uart_3_buffer,1);
        }
        else {
						uart1_io_deinit();
            RFID_INIT_2();
            HAL_UART_Receive_IT(&UART_Config_RFID,uart_3_buffer,1);
        }
        HAL_UART_Transmit(&UART_Config_RFID,&buf[0],7,100);
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
    case TEST_GET_IMEI_VAL:
        temp=	T6_enable;
        break;
    case TEST_GET_IMSI_VAL:
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
    case TEST_GET_IMEI_VAL:
        T6_enable=state;
        break;
    case TEST_GET_IMSI_VAL:
        T7_enable=state;
        break;
    case TEST_GET_DB_VAL:
        T8_enable=state;
        break;
    }
}
//开始启动数据包
uint16_t Start_Lock_Send_Task() {
    static uint16_t over_count=0;  // 超限计数
    static uint16_t count=0;  		 //
    static uint16_t busy=0;
    static uint16_t i=0;
    static uint8_t step=1;
    if(Get_Task_State(START_LOCK_SEND) /*&& AT_tset_flag==0 */) {
        busy=1;		//忙碌
        if(start_lock_reply_Result==1) {
            globle_Result=0xFF;
            start_lock_reply_Result=0;
            Set_Task_State(START_LOCK_SEND,STOP);
            busy=0;//空闲
            return busy;
        }
        switch (step) {
        case 1:
            if(send_mode_Result==1) {
                send_mode_Result=0;
                step++;
            }
            else {
                i++;
                if(i%400==10) {
                    globle_Result=NO_ASK;
                    over_count++;
                    if(over_count>5) {
                        Set_Task_State(START_LOCK_SEND,STOP);  //回复错误次数>5停发
                        busy=0;//空闲
                        break;
                    }
                    UDP_Data_Send(72);		//进入发送模式
                }
            }
            break;
        case 2:
            i++;
            if(i%400==20) {
                count++;
                globle_Result=NO_ASK;
                Set_Task_State(START_LOCK_SEND,START);
                Start_Lock_Send();
                if(count==1) {    //count=1发送数据，count=2为发送无回复跳转到第一步，
                    count=0;
                    step=1;
                }
            }
            break;
        }
    }
    else {
        count=0;
        over_count=0;
        i=0;
        step=1;
    }
    return busy;
}

//请求开锁
uint16_t Open_Lock_Send_Task() {
    static uint16_t over_count=0;  // 超限计数
    static uint16_t count=0;  		 //
    static uint16_t busy=0;
    static uint16_t i=0;
    static uint8_t step=1;
    if(Get_Task_State(OPEN_LOCK_SEND) /*&& AT_tset_flag==0 */) {
        busy=1;//忙碌
        if(open_lock_reply_Result==1) {
            globle_Result=0xFF;
            open_lock_reply_Result=0;
            Set_Task_State(OPEN_LOCK_SEND,STOP);
            busy=0;//空闲
            return busy;
        }
        switch (step) {
        case 1:
            if(send_mode_Result==1) {
                send_mode_Result=0;
                step++;
            }
            else {
                i++;
                if(i%100==1) {
                    globle_Result=NO_ASK;
                    over_count++;
                    if(over_count>5) {
                        Set_Task_State(OPEN_LOCK_SEND,STOP);  //回复错误次数>5停发
                        busy=0;//空闲
                        break;
                    }
                    UDP_Data_Send(37);		//进入发送模式
                }
            }
            break;
        case 2:
            i++;
            if(i%100==5) {
                count++;
                globle_Result=NO_ASK;
                Set_Task_State(OPEN_LOCK_SEND,START);
                Open_Lock_Send();
                if(count==1) {    //count=1发送数据，count=2为发送无回复跳转到第一步，
                    count=0;
                    step=1;
                }
            }
            break;
        }
    }
    else {
        count=0;
        over_count=0;
        i=0;
        step=1;
    }
    return busy;
}
//心跳包
uint16_t Tick_Lock_Send_Task() {
    static uint16_t over_count=0;  // 超限计数
    static uint16_t count=0;  		 //
    static uint16_t busy=0;
    static uint16_t i=0;
    static uint8_t step=1;
    if(Get_Task_State(TICK_LOCK_SEND) /*&& AT_tset_flag==0*/ ) {
        busy=1;  //忙碌
        if(tick_reply_Result==1) {
            globle_Result=0xFF;
            tick_reply_Result=0;
            Set_Task_State(TICK_LOCK_SEND,STOP);
            busy=0;//空闲
            return busy;
        }
        switch (step) {
        case 1:
            if(send_mode_Result==1) {
                send_mode_Result=0;
                step++;
            }
            else {
                i++;
                if(i%100==1) {
                    globle_Result=NO_ASK;
                    over_count++;
                    if(over_count>5) {
                        Set_Task_State(TICK_LOCK_SEND,STOP);  //回复错误次数>5停发
                        busy=0;//空闲
                        break;
                    }
                    UDP_Data_Send(37);		//进入发送模式
                }
            }
            break;
        case 2:
            i++;
            if(i%100==10) {
                count++;
                globle_Result=NO_ASK;
                Set_Task_State(TICK_LOCK_SEND,START);
                Tick_Lock_Send();
                if(count==1) {    //count=1发送数据，count=2为发送无回复跳转到第一步，
                    count=0;
                    step=1;
                }
            }
            break;
        }
    }
    else {
        count=0;
        over_count=0;
        i=0;
        step=1;
    }
    return busy;
}

//20信息上报  锁+状态
uint16_t Open_Lock_Data_Send_Task() {
    static uint16_t over_count=0;  // 超限计数
    static uint16_t count=0;  		 //
    static uint16_t busy=0;
    static uint16_t i=0;
    static uint8_t step=1;
    if(Get_Task_State(OPEN_LOCK_DATA_SEND) /*&& AT_tset_flag==0 && rfid_task_flag_1==0 && rfid_task_flag_2==0 */) {
        busy=1;  //忙碌
        if(open_lock_data_reply_Result==1) {
            globle_Result=0xFF;
            open_lock_data_reply_Result=0;
            //look_status_send_count--;
            //if( look_status_send_count==0)
            Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
            busy=0; //空闲
            return busy;
        }
        switch (step) {
        case 1:
            if(send_mode_Result==1) {
                send_mode_Result=0;
                step++;
            }
            else {
                i++;
                if(i%100==1) {
                    globle_Result=NO_ASK;
                    over_count++;
                    if(over_count>5) {
                        Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);  //回复错误次数>5停发
                        busy=0;
                        break;
                    }
                    UDP_Data_Send(47);		//进入发送模式
                }
            }
            break;
        case 2:
            i++;
            if(i%100==5) {
                count++;
                globle_Result=NO_ASK;
                Set_Task_State(OPEN_LOCK_DATA_SEND,START);
                Open_Lock_Data_Send();
                if(count==1) {    //count=1发送数据，count=2为发送无回复跳转到第一步，
                    count=0;
                    step=1;
                }
            }
            break;
        }
    }
    else {
        count=0;
        over_count=0;
        i=0;
        step=1;
    }
    return busy;
}

////5s 保持模块唤醒 查一下信号
//void NB_WAKE_Task() {
//    static uint8_t i;

//    if(Get_Task_State(GET_DB_VAL)==STOP) {
//        i++;
//        if(i%200==1) {
//            Set_Task_State(GET_DB_VAL,START);
//            i=1;
//        }
//    }
//}

uint8_t last_lock_state_0;
uint8_t last_lock_state_1;
//检测状态发生变化上报数据
void State_Change_Task() {
    static uint8_t sw1_count;
    static uint8_t sw1_flag;
    static uint8_t sw2_count;
    static uint8_t sw2_flag;
//		if(AT_tset_flag !=0 ){
//			return;
//		}

    if(Check_SW1()==1) {
        sw1_count++;
        if(sw1_count>10) sw1_count=10;
        if(sw1_count==3) {
            sw1_flag=1;
            lock_state[0]=0;
        }
    }
    else {
        lock_state[0]=1;
        sw1_count=0;
    }

    if(Check_SW2()==1) {
        //if( Check_SW1()==0){
        sw2_count++;
        if(sw2_count>10) sw2_count=10;
        if(sw2_count==3) {
            sw2_flag=1;
            lock_state[1]=0;
        }
    }
    else {
        lock_state[1]=1;
        sw2_count=0;
    }

    if(last_lock_state_0 != lock_state[0]  && lock_task_flag_1==0 && lock_task_flag_2==0) {  //开锁中任务中不触发
        sleep_time=0;
        LOG_I("State_Change");
        LOG_I("sw1:%d", lock_state[0]);
        open_lock_data_reply_Result=0;
        last_lock_state_0=lock_state[0];
        Set_Task_State(OPEN_LOCK_DATA_SEND,1); //状态改变数据上传

        look_status_send_count+=3;
        if(look_status_send_count>=3) {
            look_status_send_count=3;
        }
        RTC_flag=0;

        if(lock_state[0]==1) {
            buzzer_task_flag=1;
            uv_count=25000/50;
						//rfid_task_flag_1=1;   					 //开启一号卡扫卡
        }
        user_ble_send_flag=1;
        TX_DATA_BUF[0]=0x52;		// CMD
        TX_DATA_BUF[1]=TOKEN[0];
        TX_DATA_BUF[2]=TOKEN[1];
        TX_DATA_BUF[3]=TOKEN[2];
        TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
        TX_DATA_BUF[5]=0x08;    	//LEN
        TX_DATA_BUF[6]=0x01;			//主锁无，关闭模式
        TX_DATA_BUF[7]=0x06;    // 在线情况  1，2全在线，具体见协议文档
        TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
        TX_DATA_BUF[9]=0x01;
        TX_DATA_BUF[10]=RFID_DATA[0];
        TX_DATA_BUF[11]=RFID_DATA[1];
        TX_DATA_BUF[12]=RFID_DATA[2];
        TX_DATA_BUF[13]=RFID_DATA[3];
    }



    if(last_lock_state_1 != lock_state[1] && lock_task_flag_1==0 && lock_task_flag_2==0) {      //扫卡完成再上报数据

        //rfid_task_flag_2=1;    //开启二号卡扫卡

        if(lock_state[1]==1) {    		//关锁动作后需要读卡号
            buzzer_task_flag=1;
            uv_count=25000/50;
        }
        sleep_time=0;

        last_lock_state_1=lock_state[1];

        LOG_I("State_Change");
        LOG_I("sw2:%d", lock_state[1]);

        open_lock_data_reply_Result=0;
        Set_Task_State(OPEN_LOCK_DATA_SEND,1); //状态改变数据上传

        look_status_send_count+=3;
        if(look_status_send_count>=3) {
            look_status_send_count=3;
        }
        RTC_flag=0;

        user_ble_send_flag=1;
        TX_DATA_BUF[0]=0x52;		// CMD
        TX_DATA_BUF[1]=TOKEN[0];
        TX_DATA_BUF[2]=TOKEN[1];
        TX_DATA_BUF[3]=TOKEN[2];
        TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
        TX_DATA_BUF[5]=0x08;    	//LEN
        TX_DATA_BUF[6]=0x01;			//主锁无，关闭模式
        TX_DATA_BUF[7]=0x06;    // 在线情况  1，2全在线，具体见协议文档
        TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
        TX_DATA_BUF[9]=0x02;

        TX_DATA_BUF[10]=RFID_DATA_2[0];
        TX_DATA_BUF[11]=RFID_DATA_2[1];
        TX_DATA_BUF[12]=RFID_DATA_2[2];
        TX_DATA_BUF[13]=RFID_DATA_2[3];
    }
}
//获取信号值
uint16_t AT_GET_DB_TASK() {
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
            if(i%20==1) { //重发间隔
                count++;
                AT_Command_Send(CSQ);
                if(count<20) {
                    tinyfs_read(ID_dir_2,RECORD_KEY2,tmp,&length);//读到tmp中
                    if(strncmp("SET_OK",(char*)tmp,sizeof("SET_OK"))!=0) {
                        buzzer_task_flag=1;
                    }
                    if(reset_flag==1) {
                        buzzer_task_flag=1;
                    }
                }
                if(count>=20 && count<120) {
                    buzzer_task_flag=1;
                }
                else if(count>=120) {
                    Set_Task_State(GET_DB_VAL,STOP);
                    temp=TIME_OUT;
//                    RESET_NB();
                    count=0;
                }
            }
        }
    }
    else {
        count=0;
        //temp=0;
        i=0;
    }
    return temp;
}

//查询模块设置
uint16_t AT_User_Reply() {
    static uint8_t step=0;
    static uint8_t count=0;
    uint8_t temp =0xFF;
    count++;
    if(count%10==1) {
        switch(step) {
        case 0:    ///AT
            step++;
            AT_Command_Send(AT);
            //return 0xff;
            break;

        case 1:  //CGATT
            step++;
//					AT_Command_Send(CGATT_R);
            //return 0xff;
            break;

        case 2:  //CEREG
            step++;
//					AT_Command_Send(CEREG_R);
            temp=0xAA;
            //return 0xff;
            break;
        }
    }
    return temp;
}

//重置模块设置
uint16_t AT_User_Set() {
    static uint8_t step=0;
    static uint8_t count=0;
    uint8_t temp =0xFF;
    count++;
    if(count%20==1) {
        switch(step) {
        case 0:    ///AT
            step++;
            AT_Command_Send(AT);
            //buzzer_task_flag=1;
            temp=0xFF;
            break;

        case 1:  //CTM2MSETPM
            step++;
//				AT_Command_Send(CTM2MSETPM);
            //buzzer_task_flag=1;
            temp=0xFF;
            break;

        case 2:    ///AT
            step++;
//				AT_Command_Send(CTM2MREG);
            //buzzer_task_flag=1;
            temp=0xAA;
            break;
        }
    }
    return temp;
}


uint8_t AT_tset_flag=0x00;

void AT_User_Reply_Task() {
    if(AT_tset_flag==1) {
        if(AT_User_Reply()==0xAA)
            AT_tset_flag=0;
    }
}

void AT_User_Set_Task() {
    if(AT_tset_flag==2) {
        if(AT_User_Set()==0xAA)
            AT_tset_flag=0;
        if(reset_flag==1) {
            reset_flag=0;
            buzzer_task_flag_2=1;
        }

    }
}

//重置初始化跑一次
uint16_t AT_INIT() {
    static int step=0;
    static int count=0;
    static int count_out=0;
    static int set_flag=0;
//	static int once_flag=0;
    count++;
    static uint8_t tmp[10];
    uint16_t length = 10;
    const static uint8_t no_wirt_data[10]="3141592654";
    tinyfs_read(ID_dir_2,RECORD_KEY2,tmp,&length);//读到tmp中
    if(strncmp("SET_OK",(char*)tmp,sizeof("SET_OK"))==0) {
        return 0xff;
    }
    if(count%5==0) {
        switch(step) {

        case 0:
            //获取sim卡卡号数据
            if(Get_Uart_Data_Processing_Result()==CIMI_OK) {
                globle_Result=0xff;

                memcpy(&CIMI_DATA[0],&AT_RX_DATA_BUF[0],15);
                tinyfs_write(ID_dir_3,RECORD_KEY9,CIMI_DATA,15);
                tinyfs_write_through();
                LOG_I("CIMI_DATA %s",CIMI_DATA);

                step++;
                count_out=0;
            }
            else {
                set_flag++;
                count_out++;
                if(count_out>5) {
                    count_out=0	;
                    step++;
                }
                AT_Command_Send(CIMI);
                buzzer_task_flag=1;
            }
            break;


        case 1:
            if(Get_Uart_Data_Processing_Result()==CGSN_OK) {
                globle_Result=0xff;
                if(strncmp((char*)no_wirt_data,(char*)SHORT_NAME,sizeof(no_wirt_data))==0) {
                    memcpy(&SHORT_NAME[0],&AT_RX_DATA_BUF[5],10);
                    memcpy(&MICI_DATA[0],&AT_RX_DATA_BUF[0],15);
                    //后10位设备号写入，作为蓝牙广播名称
                    tinyfs_write(ID_dir_2,RECORD_KEY1,SHORT_NAME,sizeof(SHORT_NAME));
                    tinyfs_write(ID_dir_1,RECORD_KEY11,MICI_DATA,sizeof(MICI_DATA));
                    tinyfs_write_through();
                    LOG_I("SHORT_NAME %s",SHORT_NAME);
                    LOG_I("MICI_DATA %s",MICI_DATA);
                }
                step++;
                count_out=0;
            }
            else {
                set_flag++;
                count_out++;
                if(count_out>5) {
                    count_out=0	;
                    step++;
                }
                AT_Command_Send(CGSN);
                buzzer_task_flag=1;
            }
            break;

        case 2:
            //DELAY_US(200000); //50ms
            if(Get_Uart_Data_Processing_Result()==OK_AT) {
                globle_Result=0xff;
                step++;
                count_out=0;

                LOG_I("set_flag:%d",set_flag);
                if(set_flag>9) {
								
										AT_Command_Send(POWER_OFF);										
										DELAY_US(1000*100);
										//io_cfg_input(PB08);		//4G CAT1 电源		
                    io_cfg_output(PB08);  //
										io_write_pin(PB08,0);	//
                    DELAY_US(1000*1000*3);
										platform_reset(0); 						//初始化成功写入成功标记位，重启
                }
                else {
                    tinyfs_write(ID_dir_2,RECORD_KEY2,(uint8_t*)"SET_OK",sizeof("SET_OK"));
                    tinyfs_write_through();
	
										AT_Command_Send(POWER_OFF);										
										DELAY_US(1000*100);
										//io_cfg_input(PB08);		//4G CAT1 电源		
                    io_cfg_output(PB08);  //
										io_write_pin(PB08,0);	//
                    DELAY_US(1000*1000*3);
										platform_reset(0); 						//初始化成功写入成功标记位，重启
                }
            }
            else {
                set_flag++;
                count_out++;
                if(count_out>10) {
                    count_out=0	;
                    step++;
                }
                AT_Command_Send(AT);
                buzzer_task_flag=1;
            }
            break;
        default : /* 可选的 */
            break;
            //statement(s);
        }
    }
    return 0x88;
}
//重置
uint8_t UDP_INIT() {

    static int step=0;
    static int count=0;
    static int count_out=0;
    static int done=0; 			//完成udp初始化标记
    //static int set_flag=0;
    count++;

    if(count%5==1) {
        switch(step) {

        case 0:
            ////注册服务
            if(Get_Uart_Data_Processing_Result()==OK_AT) {
                globle_Result=0xff;
                step++;
                count_out=0;
            }
            else {
                count_out++;
                if(count_out>5) {
                    count_out=0	;
                    step++;
                }
                AT_Command_Send(QIPCSGP);
                //buzzer_task_flag=1;
            }
            break;

        case 1://查询激活服务
            if(Get_Uart_Data_Processing_Result()==OK_AT) {
                globle_Result=0xff;
                step+=2;
                count_out=0;
            }
            else {
                count_out++;
                if(count_out>5) {
                    count_out=0	;
                    step+=2;
                }
                step++;
                AT_Command_Send(QIPACT_ASK);
                //buzzer_task_flag=1;
            }
            break;

        //激活服务
        case 2:
            step--;
            AT_Command_Send(QIPACT);
            //buzzer_task_flag=1;
            break;

        //启动连接
        case 3:
            if(Get_Uart_Data_Processing_Result()==OK_AT) {
                globle_Result=0xff;
                step++;

                //AT_tset_flag=0;
                done=1;
                count_out=0;
            }
            else {

                count_out++;
                if(count_out>5) {
                    count_out=0	;
                    step++;
                    done=1;
                }
                AT_Command_Send(QIPOPEN);
                //buzzer_task_flag=1;
            }
            break;
        default : /* 可选的 */
            break;
            //statement(s);
        }
    }
    return  done;
}

static  uint16_t set_sleep_time;
//设置休眠时间
void Set_Sleep_Time(uint16_t time_s) {
    set_sleep_time=time_s;
}
//休眠任务
void Sleep_Task() {

    if(sleep_time*50>set_sleep_time*1000) {
        ls_sleep_enter_lp2();
    }
}
//extern uint8_t	psaaword_task_flag;  //改密码任务开始标记
//extern uint8_t	key_task_flag;			 //改密钥任务开始标记
void Password_Task() {
    static uint16_t count;
    static uint8_t buf_1[6];
    static uint8_t buf_2[6];
    if(psaaword_task_flag!=0) {
        count++;
        //10s
        if(count<200) {
            if(psaaword_task_flag==0xA1) {
                memcpy (&buf_1[0], &NEW_PASSWORD_BUF[0], 6);
            }
            else if(psaaword_task_flag==0xA2) {
                memcpy (&buf_2[0], &NEW_PASSWORD_BUF[0], 6);

                user_ble_send_flag=1;
                psaaword_task_flag=0;

                TX_DATA_BUF[0]=0xA1;		  // CMD
                TX_DATA_BUF[1]=TOKEN[0];
                TX_DATA_BUF[2]=TOKEN[1];
                TX_DATA_BUF[3]=TOKEN[2];
                TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
                TX_DATA_BUF[5]=0x01;      //LEN
                TX_DATA_BUF[6]=0x01;      //失败
                //通信帧1 PWD：为原密码。
                if((strncmp((char*)buf_1,(char*)PASSWORD,6)==0)) {
                    TX_DATA_BUF[6]=0x00;    //
                    //写入新密码重启有效
                    memcpy (&PASSWORD[0], &buf_2[0], 6);
                    tinyfs_write(ID_dir_3,RECORD_KEY3,(uint8_t*)buf_2,sizeof(PASSWORD));
                    tinyfs_write_through();
                    count=0;
                }
            }
        }
        else {
            count=0;
            psaaword_task_flag=0;
            user_ble_send_flag=1;
            TX_DATA_BUF[0]=0xA1;		// CMD
            TX_DATA_BUF[1]=TOKEN[0];
            TX_DATA_BUF[2]=TOKEN[1];
            TX_DATA_BUF[3]=TOKEN[2];
            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
            TX_DATA_BUF[5]=0x01;    //LEN
            TX_DATA_BUF[6]=0x01;    //失败
        }
    }
}
void Key_Task() {
    static uint16_t count;
    static uint8_t buf_L[8];
    static uint8_t buf_H[8];

    if(key_task_flag!=0) {
        count++;
        if(count<200) {
            if(key_task_flag==0xA5) {
                memcpy (&buf_L[0], &NEW_KEY_BUF[0], 8);
            }
            else if(key_task_flag==0xA6) {
                memcpy (&buf_H[0], &NEW_KEY_BUF[0], 8);
                user_ble_send_flag=1;
                TX_DATA_BUF[0]=0xA5;		// CMD
                TX_DATA_BUF[1]=TOKEN[0];
                TX_DATA_BUF[2]=TOKEN[1];
                TX_DATA_BUF[3]=TOKEN[2];
                TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
                TX_DATA_BUF[5]=0x01;    //LEN
                TX_DATA_BUF[6]=0x00;    //成功
                //通信帧1 低8位
                //通信帧2 高8位
                //写入新密码重启有效

                //memcpy (&key[0], &buf_L[0],8);
                //memcpy (&key[8], &buf_H[0],8);

                tinyfs_write(ID_dir_3,RECORD_KEY4,(uint8_t*)buf_L,8);
                tinyfs_write(ID_dir_3,RECORD_KEY5,(uint8_t*)buf_H,8);
                tinyfs_write_through();
                count=0;
                key_task_flag=0xff;
            }
        }
        else {
            count=0;
            key_task_flag=0;
            user_ble_send_flag=1;
            TX_DATA_BUF[0]=0xA5;		// CMD
            TX_DATA_BUF[1]=TOKEN[0];
            TX_DATA_BUF[2]=TOKEN[1];
            TX_DATA_BUF[3]=TOKEN[2];
            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
            TX_DATA_BUF[5]=0x01;    //LEN
            TX_DATA_BUF[6]=0x01;    //失败
        }
    }
}

void Once_Send() {
    static uint8_t once_flag=1;
    if(once_flag) {
        once_flag=0;
        uint8_t wkup_source = get_wakeup_source();   //获取唤醒源
        LOG_I("wkup_source:%x",wkup_source) ;
        if(Get_Task_State(OPEN_LOCK_DATA_SEND)) {
            //Open_Lock_Data_Send(0,lock_state[0]);
            //Set_Task_State(OPEN_LOCK_DATA_SEND,START);
        }
    }
}
