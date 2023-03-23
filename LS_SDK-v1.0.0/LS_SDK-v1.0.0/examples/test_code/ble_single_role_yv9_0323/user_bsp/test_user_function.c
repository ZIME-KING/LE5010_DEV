#define LOG_TAG "USER"
#include "user_main.h"
#include "test_user_function.h"
#include <string.h>

#define TUSB_CHECK   PA00
//#define	TUSB_CHECK_B PB06
#define LED_R     PA05
#define LED_G     PC00

void TEST_LED_TASK(){
	static uint8_t flag;
	static uint8_t flag1;
	static uint16_t count;
	count++;
	if(count%10==0){
		if(flag1==1)flag1=0;
		else flag1=1;
	}
	if(count%20==0){
		if(flag==1)flag=0;
		else flag=1;
	}

	//前5s闪烁,后面亮红灯
	if(count<50){
			io_write_pin(LED_G, flag1);
			io_write_pin(LED_R, !flag1);
	}
	else if(count==51){
			io_write_pin(LED_G, 0);
			io_write_pin(LED_R, 1);
	}
	else{
			if(BLE_connected_flag==1){
				io_write_pin(LED_G, 0);
				io_write_pin(LED_R, 1);
			}
			else{
				if(Get_Task_State(TEST_GET_IMEI_VAL)==START){
						io_write_pin(LED_G, 0);
						io_write_pin(LED_R, 1);
				}
				else if(Get_Task_State(TEST_GET_IMSI_VAL)==START){
						io_write_pin(LED_G, 0);
						io_write_pin(LED_R, flag1);
				}
				else if(Get_Task_State(TEST_GET_DB_VAL)==START){
						io_write_pin(LED_G, 0);
						io_write_pin(LED_R, flag);
				}
				else{
					
					io_cfg_input(TUSB_CHECK);   //PB09 config output
//				io_cfg_input(TUSB_CHECK_B);   //PB09 config output										
//				LOG_I("usb_in1%d",io_read_pin(TUSB_CHECK));
//				LOG_I("usb_in2%d",io_read_pin(TUSB_CHECK_B));		
					if(io_read_pin(TUSB_CHECK)==1 ){
						
//						io_cfg_output(TUSB_CHECK);   //PB09 config output
//						io_cfg_output(TUSB_CHECK_B);   //PB09 config output
//						io_write_pin(TUSB_CHECK, 0);
//						io_write_pin(TUSB_CHECK_B, 0);
												
						LOG_I("usb_in");
						//20~90 绿灯闪
						//if(VBat_value>20 && VBat_value<=90){
						io_write_pin(LED_G, flag);
						io_write_pin(LED_R, 0);
						//}
						}
					else{
						if(VBat_value>20){
							io_write_pin(LED_G, 1);
							io_write_pin(LED_R, 0);
						}
						//<20 红灯
						else if(VBat_value>0 && VBat_value<=20){
							io_write_pin(LED_G, 0);
							io_write_pin(LED_R, 1);
						}
					}
				}
			}
	}
}
//获取信号值
uint16_t TEST_AT_GET_DB_TASK(){
		static uint8_t count=0;
    static uint16_t temp;
    static uint16_t i;
   //static uint8_t tmp[10];
	 //uint16_t length = 10; 
   if(Get_Task_State(TEST_GET_DB_VAL)) {
   		      if(Get_Uart_Data_Processing_Result()==CSQ_OK && Db_val!=99   && Db_val!=0) {
								globle_Result=0xFF;
                Set_Task_State(TEST_GET_DB_VAL,STOP);
								i=0;
								count=0;
								temp=0xff;								
            }
            else{							
								i++;
								temp=0;
                if(i%20==1){  //重发间隔
										count++;
										AT_Command_Send(CSQ);
												buzzer_task_flag=1;
										 //if(count>=20){
											//Set_Task_State(GET_DB_VAL,START);
											temp=TIME_OUT;
										//}
								}
            }
     }
    else{
			count=0;
			i=0;
		}
    return temp;
}



//获取IMEI设备识别号
uint16_t TEST_AT_GET_IMEI_TASK(){
		static uint8_t count=0;
    static uint16_t temp;
    static uint16_t i;
//    static uint8_t tmp[10];
//		uint16_t length = 10; 
   if(Get_Task_State(TEST_GET_IMEI_VAL)) {
   		      if(Get_Uart_Data_Processing_Result()==CGSN_OK) {
								globle_Result=0xFF;
                Set_Task_State(TEST_GET_IMEI_VAL,STOP);
								Set_Task_State(TEST_GET_IMSI_VAL,START);
								i=0;
								count=0;
								temp=0xff;				
								LOG_I("MICI_DATA %s",MICI_DATA);		 										
            }
            else{							
								i++;
								temp=0;
                if(i%20==1){  //重发间隔
										count++;
										AT_Command_Send(CGSN);
												buzzer_task_flag=1;
										// if(count>=20){
										//	Set_Task_State(GET_MODE_VAL,START);
										//}
								}
            }
     }
    else{
			count=0;
			i=0;
		}
    return temp;
}

//获取IMSI
uint16_t TEST_AT_GET_IMSI_TASK(){
		static uint8_t count=0;
    static uint16_t temp;
    static uint16_t i;
//    static uint8_t tmp[10];
//		uint16_t length = 10; 
		if(Get_Task_State(TEST_GET_IMSI_VAL)) {
			if(Get_Uart_Data_Processing_Result()==CIMI_OK){
				globle_Result=0xff;
				//memcpy(&CIMI_DATA[0],&AT_RX_DATA_BUF[0],15);
				//tinyfs_write(ID_dir_3,RECORD_KEY9,CIMI_DATA,sizeof(CIMI_DATA));	
				//tinyfs_write_through();
				LOG_I("CIMI_DATA %s",CIMI_DATA);		 									
				i=0;
				count=0;
				temp=0xff;								
				Set_Task_State(TEST_GET_IMSI_VAL,STOP);
				Set_Task_State(TEST_GET_DB_VAL,START);
			}
			else{
					i++;
					temp=0;
          if(i%20==1){  //重发间隔
						count++;
						AT_Command_Send(CIMI);
						buzzer_task_flag=1;
						//if(count>=20){
						//	Set_Task_State(GET_EMIC_VAL,START);
						//}
				  }
			}
		}
    else{
			count=0;
			i=0;
		}
    return temp;
}
