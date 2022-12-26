#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"

UART_HandleTypeDef UART2_Config;
UART_HandleTypeDef UART1_Config;

uint32_t time_count=0x00;

uint8_t cmd_buf[64]; //接收到的命令缓存

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
						//if(crc16_ccitt(frame[uart_frame_id].buffer, frame[uart_frame_id].length-2)==0x00){
								p=CMD_Processing(&frame[uart_frame_id].buffer[3],(frame[uart_frame_id].length-6));     //第4位开始是应用数据
								//UART_Transmit_Str(&UART1_Config,p);
								HAL_UART_Transmit_IT(&UART1_Config,p,*p);
						
						//}
        }
        frame[uart_frame_id].status=0;					//处理完数据后status 清0;
    }
}



uint16_t DYP_distance;

//每接收成功一包处理一次数据
void Uart2_Data_Processing() {
    if(frame_2[uart_frame_id].status!=0) {    			//接收到数据后status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        LOG_HEX((uint8_t*)frame_2[uart_frame_id].buffer,frame_2[uart_frame_id].length);
				DYP_distance= (frame_2[uart_frame_id].buffer[1]<<8) +  (frame_2[uart_frame_id].buffer[2]);
        frame_2[uart_frame_id].status=0;					//处理完数据后status 清0;
		}
}





void	power_io_init(){
		io_cfg_output(PA05);   //  超声波
    io_write_pin(PA05,1);  
		
		io_cfg_output(SW_EN_1);   //
    io_write_pin(SW_EN_1,1);  
		
		io_cfg_output(SW_EN_2);   //
    io_write_pin(SW_EN_2,1);  

		io_cfg_input(PA00);   //
		io_cfg_input(PB15);
		io_cfg_input(PA07);
}

void loop_task(){
  Moto_Task();	
//moro_task_flag=1;
	
	
	static uint16_t count;
	
	
	 count++;
	 if(count%10==1){
			HAL_UART_Transmit(&UART2_Config,(uint8_t*)"ok",3,100);
	 }
	
	Uart2_Data_Processing();  //超声波数据
	Uart_Data_Processing();   //RS485数据处理
	//Get_ADC_value();
	Buzzer_Task_100();
		
	static uint8_t last_hw_lock_status;
	
	if(last_hw_lock_status!=hw_lock_status){
		last_hw_lock_status=hw_lock_status;
		LOG_I("OTP1:%d",lock_sw.opt1);
		LOG_I("OTP2:%d",lock_sw.opt2);	
		LOG_I("OTP3:%d",lock_sw.opt3);	
		LOG_I("status:%x",hw_lock_status);	
		LOG_I("DYP_distance:%d",DYP_distance);	
	}
	
}


static void ls_uart2_init(void)
{
    uart2_io_init(PA15, PA14);
    io_pull_write(PA14, IO_PULL_UP);

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


//蓝牙启动成功跑一次
void User_BLE_Ready() {
    Buzzer_IO_Init();
    Moto_IO_Init();
    LED_Init();
		
		User_ADC_Init();
		ls_uart2_init();
		ls_uart1_init();

		HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// 重新使能串口2接收中断

		HAL_UART_Receive_IT(&UART1_Config,uart_buffer,1);		// 重新使能串口1接收中断
		
		power_io_init();	
		tag_lock_status=POS_90;
		
////////////获取mac////////////////////		
		uint8_t addr[6];
    bool type;
    dev_manager_get_identity_bdaddr(addr,&type);
    LOG_I("type:%d,addr:",type);
    LOG_HEX(addr,sizeof(addr));
    memcpy(&lock_mac[0],&addr[0],6);
////////////////////////////////		
}







