#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"

uint8_t address=0x00;  //485通信地址
uint8_t lockid[16]={0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}; //锁16位id号


uint8_t cmd_buf[64]; //接收到的命令缓存


//每接收成功一包处理一次数据
void Uart_Data_Processing() {
    if(frame[uart_frame_id].status!=0) {    			//接收到数据后status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        LOG_HEX((uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length);
        //接收到的数据 到uart2  透传
        if (frame[uart_frame_id].buffer[0]==0xF5 && 
						frame[uart_frame_id].buffer[1]==address  && 
						frame[uart_frame_id].buffer[frame[uart_frame_id].length-1]==0xF6)
				{				
						if(crc16_ccitt(frame[uart_frame_id].buffer, frame[uart_frame_id].length-2)==0x00){
								CMD_Processing(&frame[uart_frame_id].buffer[3]);     //第4位开始是应用数据
						}
        }
        frame[uart_frame_id].status=0;					//处理完数据后status 清0;
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
	LOG_I("OTP1:%d",lock_sw.opt1);
	LOG_I("OTP2:%d",lock_sw.opt2);	
	LOG_I("OTP3:%d",lock_sw.opt3);	
	LOG_I("status:%x",hw_lock_status);	
}

//蓝牙启动成功跑一次
void User_BLE_Ready() {
    Buzzer_IO_Init();
    Moto_IO_Init();
    LED_Init();
		power_io_init();	
		tag_lock_status=POS_90;
}





