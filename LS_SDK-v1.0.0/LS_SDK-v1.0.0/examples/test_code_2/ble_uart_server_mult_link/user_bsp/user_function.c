#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"

#define VER 0x01

UART_HandleTypeDef UART2_Config;
UART_HandleTypeDef UART1_Config;

uint32_t time_count=0x00;
uint8_t ble_adv_data[2]= {VER,0xFF};

//uint8_t cmd_buf[64]; //接收到的命令缓存

uint16_t check_DYP_time_out=0;

#define RISE_TIME 300


//每接收成功一包处理一次数据
void Uart_Data_Processing() {
    uint8_t *p;

    if(frame[uart_frame_id].status!=0) {    			//接收到数据后status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        LOG_HEX((uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length);
        //接收到的数据 到uart2  透传
        if (frame[uart_frame_id].buffer[0]==0xF5 &&
                frame[uart_frame_id].buffer[1]==address  &&
                frame[uart_frame_id].buffer[frame[uart_frame_id].length-1]==0xF6)
        {
            if(crc16_ccitt(frame[uart_frame_id].buffer, frame[uart_frame_id].length-2)==0x00) {
                p=CMD_Processing(&frame[uart_frame_id].buffer[3],(frame[uart_frame_id].length-6));     //第4位开始是应用数据
                //UART_Transmit_Str(&UART1_Config,p);
                HAL_UART_Transmit_IT(&UART1_Config,p,*p);
            }
        }
        frame[uart_frame_id].status=0;					//处理完数据后status 清0;
    }
}



uint16_t DYP_distance;

//每接收成功一包处理一次数据  超声波测距
void Uart2_Data_Processing() {
    if(frame_2[uart_frame_id].status!=0) {   			//接收到数据后status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        if(frame_2[uart_frame_id].buffer[0] != 0xff) {
            return;
        }

        check_DYP_time_out=0;
        //LOG_HEX((uint8_t*)frame_2[uart_frame_id].buffer,frame_2[uart_frame_id].length);
        DYP_distance= (frame_2[uart_frame_id].buffer[1]<<8) +  (frame_2[uart_frame_id].buffer[2]);
        //LOG_I("DYP_distance:%d",DYP_distance);
        if(DYP_distance<=500) {
            car_val=0x01;
        }
        else {
            car_val=0xff;
        }
        //LOG_I("car_val:%d",car_val);

        frame_2[uart_frame_id].status=0;					//处理完数据后status 清0;
    }
}

void Check_DYP_distance() {
    static uint16_t count;
    count++;
    count=count%10;

    if(count==0) {
        HAL_UART_Transmit(&UART2_Config,(uint8_t*)"123456789",10,10);
    }
    if(count==1) {
        check_DYP_time_out++;
        //LOG_I("check_DYP_time_out:%d",check_DYP_time_out);
        if(check_DYP_time_out>2) {
            car_val=0x02;   //模块没有找到
            //LOG_I("car_val:%d",car_val);
        }
    }
}


void LED_Auto_close(){
		if(LED_status==0x01){
			user_time_cont++;
			if(user_time_cont>=led_sleep_time*10){
				 LED_status=0x02;
			}
		}else{
			 user_time_cont=0;
		}
}


//03命令自动升锁流程
void auto_mode_function(uint8_t mode) {

    static uint16_t no_car_time;
    static uint16_t find_car_time;
    static uint16_t rise_time;

    static uint8_t once_flag=0xff;
		
		if(once_flag==0xff){
				once_flag=0xAA;
				rise_time= second_rise_time*10;
		}
    
		if(mode==USER_SET) {
        rise_time=first_rise_time*10;
    }

    else if(mode==USER_RUN && lock_mode==0x01) {

        LOG_I("find_car_time:%d",find_car_time);
        LOG_I("no_car_time:%d",no_car_time);
        LOG_I("rise_time:%d",rise_time);

        //无车
        if(car_val==0xff) {
            find_car_time=0;
            no_car_time++;
						

        }
        //有车
        else if(car_val==0x01) {
            find_car_time++;
						 no_car_time=0;

						if(find_car_time>=100){
								rise_time=	second_rise_time*10;

						}
        }

        if(no_car_time>=rise_time) {
            tag_lock_status=POS_90;
            moro_task_flag=1;
            lock_mode=0x00;
						
						LOG_I("no cat_time_out");
						
        }
    } else {
        //rise_time=0;
				no_car_time=0;
				find_car_time=0;
    }
}

//外力按压时的情况  
void err_mode_function() {

    static uint16_t count;
    static uint16_t time_out;

//	static uint8_t last_lock_mode=0x01;
    count++;

		//1s
    if(count%2==1) {
				if(count>=65535){
					 count=51;
				}
		
        //电机未动作
        if(moro_task_flag==0) {
            if(hw_lock_status!=tag_lock_status ) {
                // last_lock_mode=lock_mode;
                lock_mode=0x02;  //err
            }
            else {
                // lock_mode=last_lock_mode ;
            }
        }
    }

    if(lock_mode==0x02) {

				LOG_I("time_out:%d",time_out);
				if(hw_lock_status==tag_lock_status){						
						    if(time_out>=hw_err_out_time*10  && count>=100) {                 
										lock_mode=0x01;       
                }
								else{
										lock_mode=0x00;
								}
								time_out=0;
				}	
				if(time_out>=(hw_err_out_time*10)+1  && count>=100) {
             tag_lock_status=POS_0;                   
         }
				
        //开始报警
        Set_buzzer_Task_val( hw_err_buzzer_all_time,hw_err_buzzer_on_time,hw_err_buzzer_off_time);
        buzzer_task_flag=1;   //启动蜂鸣器


        if(tag_lock_status ==POS_90) {

            
           // LOG_I("time_out:%d",time_out);

            switch(hw_lock_status) {

            case POS_0_90:
								time_out++;
								//LOG_I("time_out:%d",time_out);
                moro_task_flag=1;
                break;

            case POS_0:
								time_out++;
                moro_task_flag=1;

                break;
						
						case POS_90_180:
								time_out=0;
                moro_task_flag=1;

                break;
								

            case POS_OUT:
								time_out=0;
                moro_task_flag=1;

                break;
            }
        }

        else if(tag_lock_status==POS_0) {
            switch(hw_lock_status) {
            case POS_90:
                moro_task_flag=1;

                break;

            case POS_0_90:
                moro_task_flag=1;

                break;

            case POS_0:
                moro_task_flag=1;

                break;

            case POS_90_180:
                moro_task_flag=1;

                break;

            case POS_OUT:
                moro_task_flag=1;

                break;

            }
        }
				else{
					time_out=0;
				}
    }
}



void power_io_init() {
    io_cfg_output(PA05);   //  超声波
    io_write_pin(PA05,1);

    io_cfg_output(SW_EN_1);   	//
    io_write_pin(SW_EN_1,1);

    io_cfg_output(SW_EN_2);   	//
    io_write_pin(SW_EN_2,1);

    io_cfg_input(PA00);   			//
    io_cfg_input(PB15);
    io_cfg_input(PA07);
}

void User_Print_Log() {
    static uint8_t last_hw_lock_status;

    if(last_hw_lock_status!=hw_lock_status) {
        last_hw_lock_status=hw_lock_status;
        LOG_I("OTP1:%d",lock_sw.opt1);
        LOG_I("OTP2:%d",lock_sw.opt2);
        LOG_I("OTP3:%d",lock_sw.opt3);

        LOG_I("hw_lock_status:%d",hw_lock_status);
        LOG_I("tag_lock_status:%d",tag_lock_status);
    }

    static uint8_t last_lock_mode;

    if(last_lock_mode!=lock_mode) {
        last_lock_mode=lock_mode;
        LOG_I("lock_mode:%d",lock_mode);
    }



    //LOG_I("vbat_val: %d vbat_val",vbat_val);
    //LOG_I("lock_mode:%d",lock_mode);


}
void loop_task() {

    User_Print_Log();
		LED_Auto_close();

    Moto_Task();              //电机任务
    Test_Moto_Task();

    Uart2_Data_Processing();  //超声波数据
    Uart_Data_Processing();   //RS485数据处理
    Buzzer_Task_100();
    Check_DYP_distance();    //超声波发送

    auto_mode_function(USER_RUN);
    err_mode_function();

    if(reset_flag==0)HAL_IWDG_Refresh();	 	 //喂狗

    vbat_val=Get_ADC_value()*20/1000;
}

static void ls_uart2_init(void)
{
    uart2_io_init(PA14, PA15);
    io_pull_write(PA15, IO_PULL_UP);

    UART2_Config.UARTX = UART2;
    UART2_Config.Init.BaudRate = UART_BAUDRATE_9600;
    UART2_Config.Init.MSBEN = 0;
    UART2_Config.Init.Parity = UART_NOPARITY;
    UART2_Config.Init.StopBits = UART_STOPBITS1;
    UART2_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART2_Config);
}


static void ls_uart1_init(void)
{
    uart1_io_init(PA12, PA13);
    io_pull_write(PA13, IO_PULL_UP);

    UART1_Config.UARTX = UART1;
    UART1_Config.Init.BaudRate = UART_BAUDRATE_115200;
    UART1_Config.Init.MSBEN = 0;
    UART1_Config.Init.Parity = UART_NOPARITY;
    UART1_Config.Init.StopBits = UART_STOPBITS1;
    UART1_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART1_Config);
}

//检测密码，传入密码
bool Check_Password(uint8_t *password)
{
    static struct cavan_sha_context context;
    u8 digest[MD5_DIGEST_SIZE];
    cavan_md5_init_context(&context);
    cavan_sha_init(&context);
    cavan_sha_update(&context, def_password, 16);
    cavan_sha_update(&context, rand_password, 16);
    cavan_sha_update(&context, &lockid[2], 16);
    cavan_sha_finish(&context, digest);

    LOG_I("password:");
    LOG_HEX(digest,16);

    return memcmp(digest, password, MD5_DIGEST_SIZE) == 0;
}



tinyfs_dir_t ID_dir_1;
void Read_Last_Data() {

    uint8_t *p;
    uint8_t tmp[18];
    uint16_t length_0 = 1;
    uint16_t length_1 = 18;
    uint16_t length_2 = 16;
    uint16_t length_3 = 16;
    uint16_t length_4 = 16;
    uint16_t length_5 = 3;

    uint16_t length_6 = 2;
    uint16_t length_7 = 2;
    uint16_t length_8 = 1;

    uint16_t length_9 = 1;
    uint16_t length_10 = 1;
    uint16_t length_11 = 1;
    uint16_t length_12 = 1;
		
		uint16_t length_13 = 1;

    tinyfs_mkdir(&ID_dir_1,ROOT_DIR,53);  //创建目录

    tinyfs_read(ID_dir_1,RECORD_KEY5,tmp,&length_5);//读到tmp中
    LOG_I("flash_init");
    LOG_I("%s",tmp);

    if(strncmp("ok",(char*)tmp,sizeof("ok"))!=0) {
        LOG_I("write_init_ok");
        tinyfs_write(ID_dir_1,RECORD_KEY5,(uint8_t*)"ok",sizeof("ok"));

        tinyfs_write(ID_dir_1,RECORD_KEY0,&address,1);
        tinyfs_write(ID_dir_1,RECORD_KEY1,(uint8_t*)lockid,sizeof(lockid));
        tinyfs_write(ID_dir_1,RECORD_KEY2,&def_password[0],16);
        tinyfs_write(ID_dir_1,RECORD_KEY3,&rand_password[0],16);
        tinyfs_write(ID_dir_1,RECORD_KEY4,(uint8_t*)lock_sn,sizeof(lock_sn));

        //p=(uint8_t*)&first_rise_time;
        tinyfs_write(ID_dir_1,RECORD_KEY6,(uint8_t*)&first_rise_time,2);
        //p=(uint8_t*)&second_rise_time;
        tinyfs_write(ID_dir_1,RECORD_KEY7,(uint8_t*)&second_rise_time,2);
        //p=(uint8_t*)&low_vbat_val;
        tinyfs_write(ID_dir_1,RECORD_KEY8,&low_vbat_val,1);

        //p=(uint8_t*)&hw_err_out_time;
        tinyfs_write(ID_dir_1,RECORD_KEY9,&hw_err_out_time,1);
        //p=(uint8_t*)&hw_err_buzzer_all_time;
        tinyfs_write(ID_dir_1,RECORD_KEY10,&hw_err_buzzer_all_time,1);
        //p=(uint8_t*)&hw_err_buzzer_on_time;
        tinyfs_write(ID_dir_1,RECORD_KEY11,&hw_err_buzzer_on_time,1);
        //p=(uint8_t*)&hw_err_buzzer_off_time;
        tinyfs_write(ID_dir_1,RECORD_KEY12,&hw_err_buzzer_off_time,1);

				tinyfs_write(ID_dir_1,RECORD_KEY13,&led_sleep_time,1);

        tinyfs_write_through();
    }

    tinyfs_read(ID_dir_1,RECORD_KEY0,tmp,&length_0);//读到tmp中
    address= tmp[0];
    LOG_I("485_address:%x",address);

    tinyfs_read(ID_dir_1,RECORD_KEY1,tmp,&length_1);//读到tmp中
    memcpy (&lockid[0], &tmp[0], 18);
    LOG_I("ID:%s",lockid);

    tinyfs_read(ID_dir_1,RECORD_KEY2,tmp,&length_2);//读到tmp中
    memcpy (&def_password[0], &tmp[0], 16);
    LOG_I("def_password:");
    LOG_HEX(&def_password[0],16);

    tinyfs_read(ID_dir_1,RECORD_KEY3,tmp,&length_3);//读到tmp中
    memcpy (&rand_password[0], &tmp[0], 16);
    LOG_I("rand_password:");
    LOG_HEX(&rand_password[0],16);

    tinyfs_read(ID_dir_1,RECORD_KEY4,tmp,&length_4);//读到tmp中
    memcpy (&lock_sn, &tmp[0],16);
    LOG_I("lock_sn:");
    LOG_HEX(&lock_sn[0],16);

    tinyfs_read(ID_dir_1,RECORD_KEY6,tmp,&length_6);//读到tmp中
    memcpy (&first_rise_time, &tmp[0], 2);
    LOG_I("first_rise_time:");
    LOG_I("%d",first_rise_time);

    tinyfs_read(ID_dir_1,RECORD_KEY7,tmp,&length_7);//读到tmp中
    memcpy (&second_rise_time, &tmp[0], 2);
    LOG_I("second_rise_time:");
    LOG_I("%d",second_rise_time);

    tinyfs_read(ID_dir_1,RECORD_KEY8,tmp,&length_8);//读到tmp中
    memcpy (&low_vbat_val, &tmp[0], 1);
    LOG_I("low_vbat_val:");
    LOG_I("%d",low_vbat_val);

    tinyfs_read(ID_dir_1,RECORD_KEY9,tmp,&length_9);//读到tmp中
    memcpy (&hw_err_out_time, &tmp[0], 1);
    LOG_I("hw_err_out_time:");
    LOG_I("%d",hw_err_out_time);

    tinyfs_read(ID_dir_1,RECORD_KEY10,tmp,&length_10);//读到tmp中
    memcpy (&hw_err_buzzer_all_time, &tmp[0], 1);
    LOG_I("hw_err_buzzer_all_time:");
    LOG_I("%d",hw_err_buzzer_all_time);

    tinyfs_read(ID_dir_1,RECORD_KEY11,tmp,&length_11);//读到tmp中
    memcpy (&hw_err_buzzer_on_time, &tmp[0], 1);
    LOG_I("hw_err_buzzer_on_time:");
    LOG_I("%d",hw_err_buzzer_on_time);

    tinyfs_read(ID_dir_1,RECORD_KEY12,tmp,&length_12);//读到tmp中
    memcpy (&hw_err_buzzer_off_time, &tmp[0], 1);
    LOG_I("hw_err_buzzer_off_time:");
    LOG_I("%d",hw_err_buzzer_off_time);
		
		tinyfs_read(ID_dir_1,RECORD_KEY13,tmp,&length_12);//读到tmp中
    memcpy (&led_sleep_time, &tmp[0], 1);
    LOG_I("led_sleep_time:");
    LOG_I("%d",led_sleep_time);
		
		
		
}

//蓝牙启动成功跑一次
void User_BLE_Ready() {

    Read_Last_Data();


    Buzzer_IO_Init();
    Moto_IO_Init();
    LED_Init();

    User_ADC_Init();
    ls_uart2_init();
    ls_uart1_init();

    HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// 重新使能串口2接收中断

    HAL_UART_Receive_IT(&UART1_Config,uart_buffer,1);		// 重新使能串口1接收中断
    power_io_init();


    HAL_IWDG_Init(32756*1);  	 //5s看门狗


//    tag_lock_status=POS_90;

////////////获取mac////////////////////
    uint8_t addr[6];
    bool type;
    dev_manager_get_identity_bdaddr(addr,&type);
    LOG_I("type:%d,addr:",type);
    LOG_HEX(addr,sizeof(addr));
    memcpy(&lock_mac[0],&addr[0],6);

////////////////////////////////
}







