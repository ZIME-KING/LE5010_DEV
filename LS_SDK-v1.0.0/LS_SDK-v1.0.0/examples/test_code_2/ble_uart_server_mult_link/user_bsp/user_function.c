#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"

uint8_t address=0x00;  //485ͨ�ŵ�ַ
uint8_t lockid[16]={0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}; //��16λid��


uint8_t cmd_buf[64]; //���յ��������


//ÿ���ճɹ�һ������һ������
void Uart_Data_Processing() {
    if(frame[uart_frame_id].status!=0) {    			//���յ����ݺ�status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        LOG_HEX((uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length);
        //���յ������� ��uart2  ͸��
        if (frame[uart_frame_id].buffer[0]==0xF5 && 
						frame[uart_frame_id].buffer[1]==address  && 
						frame[uart_frame_id].buffer[frame[uart_frame_id].length-1]==0xF6)
				{				
						if(crc16_ccitt(frame[uart_frame_id].buffer, frame[uart_frame_id].length-2)==0x00){
								CMD_Processing(&frame[uart_frame_id].buffer[3]);     //��4λ��ʼ��Ӧ������
						}
        }
        frame[uart_frame_id].status=0;					//���������ݺ�status ��0;
    }
}


void	power_io_init(){
		io_cfg_output(PA05);   //  ������
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

//���������ɹ���һ��
void User_BLE_Ready() {
    Buzzer_IO_Init();
    Moto_IO_Init();
    LED_Init();
		power_io_init();	
		tag_lock_status=POS_90;
}





