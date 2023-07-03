#define LOG_TAG "USER"
#include "main.h"
#include "user_function.h"
#include "hw_keyboard.h"

uint8_t key[8]= {0};


//int16_t USer_KeyPressed(KeyCode_t _key){


//	//key[]
//	//return 
//}


UART_HandleTypeDef UART2_Config;
UART_HandleTypeDef UART1_Config;

//ÿ���ճɹ�һ������һ������
void Uart_Data_Processing() {
    if(frame[uart_frame_id].status!=0) {    			//���յ����ݺ�status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        LOG_HEX((uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length);
        
				
				key[0] =0x01;				
				key[4] =0x0B;
        app_hid_send_keyboard_report(1,key,8,0);
        
				key[0] =0x00;
				key[4] =0x00;
        app_hid_send_keyboard_report(1,key,8,0);
				
				
				
			
				key[2] =0x0B;
        app_hid_send_keyboard_report(0,key,3,0);
				key[2] =0x00;
	      app_hid_send_keyboard_report(0,key,3,0);
				
				
//				key[3] =0x01;
//        app_hid_send_keyboard_report(2,key,3,0);
//        key[3] =0x00;
//        app_hid_send_keyboard_report(2,key,3,0);
				
				
				
				
				
				
//				key[2] =0x0A;
//        app_hid_send_keyboard_report(0,key,8,0);
//        key[2] =0x00;
//        app_hid_send_keyboard_report(0,key,8,0);
//				
//				key[2] =0x0A;
//        app_hid_send_keyboard_report(0,key,8,0);
//        key[2] =0x00;
//        app_hid_send_keyboard_report(0,key,8,0);
				
				
				
				
				//���յ������� ��uart2  ͸��
        frame[uart_frame_id].status=0;					//���������ݺ�status ��0;
    }
}

//ÿ���ճɹ�һ������һ������  ���������
void Uart2_Data_Processing() {
    if(frame_2[uart_frame_id].status!=0) { 			//���յ����ݺ�status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        frame_2[uart_frame_id].status=0;					//���������ݺ�status ��0;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->UARTX==UART1) {
        Receive_Interrupt();
        HAL_UART_Receive_IT(&UART1_Config,uart_buffer,1);		// ����ʹ�ܴ���1�����ж�
    }
		
    else if(huart->UARTX==UART2) {
        Uart_2_Receive_Interrupt();
        HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// ����ʹ�ܴ���2�����ж�
    }
    else if(huart->UARTX==UART3) {
        //Uart_3_Receive_Interrupt();
        //HAL_UART_Receive_IT(&UART_Config_RFID,uart_3_buffer,1);		// ����ʹ�ܴ���3�����ж�
    }
}




void User_Print_Log() {
		//LOG_I("OTP1");
}
void loop_task() {
    User_Print_Log();
    Uart2_Data_Processing();  //����������
    Uart_Data_Processing();   //RS485���ݴ���
  //  if(reset_flag==0)HAL_IWDG_Refresh();	 	 //ι��	
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
    uart1_io_init(PA13, PA14);
    io_pull_write(PA14, IO_PULL_UP);

    UART1_Config.UARTX = UART1;
    UART1_Config.Init.BaudRate = UART_BAUDRATE_115200;
    UART1_Config.Init.MSBEN = 0;
    UART1_Config.Init.Parity = UART_NOPARITY;
    UART1_Config.Init.StopBits = UART_STOPBITS1;
    UART1_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART1_Config);
}

//���������ɹ���һ��
void User_BLE_Ready() {

    ls_uart2_init();
    ls_uart1_init();
		
    HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// ����ʹ�ܴ���2�����ж�
    HAL_UART_Receive_IT(&UART1_Config,uart_buffer,1);		// ����ʹ�ܴ���1�����ж�


    //HAL_IWDG_Init(32756*1);  	 //5s���Ź�


//    tag_lock_status=POS_90;

////////////��ȡmac////////////////////
    uint8_t addr[6];
    bool type;
    dev_manager_get_identity_bdaddr(addr,&type);
    LOG_I("type:%d,addr:",type);
    LOG_HEX(addr,sizeof(addr));
//    memcpy(&lock_mac[0],&addr[0],6);

////////////////////////////////



}







