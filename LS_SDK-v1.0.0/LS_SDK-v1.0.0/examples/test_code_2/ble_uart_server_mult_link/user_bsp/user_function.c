#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"

UART_HandleTypeDef UART2_Config;
UART_HandleTypeDef UART1_Config;



uint8_t address=0x00;  //485通信地址
uint8_t lockid[18]={'I','N',0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31}; //锁16位id号


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
								p=CMD_Processing(&frame[uart_frame_id].buffer[3]);     //第4位开始是应用数据
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
	moro_task_flag=1;
	
	
	static uint16_t count;
	
	
	 count++;
	 if(count%10==1){
			HAL_UART_Transmit(&UART2_Config,(uint8_t*)"ok",3,100);
	 }
	
	Uart2_Data_Processing();  //超声波数据
	Uart_Data_Processing();   //RS485数据处理
	//Get_ADC_value();
	Buzzer_Task_100();
	
	
//	LOG_I("OTP1:%d",lock_sw.opt1);
//	LOG_I("OTP2:%d",lock_sw.opt2);	
//	LOG_I("OTP3:%d",lock_sw.opt3);	
//	LOG_I("status:%x",hw_lock_status);	
//	LOG_I("DYP_distance:%d",DYP_distance);	
	
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

}





