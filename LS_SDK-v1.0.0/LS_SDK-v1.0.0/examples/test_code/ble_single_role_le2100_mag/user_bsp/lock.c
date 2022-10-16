#include "user_main.h"
#include "lock.h"

void Lock_gpio_init(void)
{
    io_cfg_output(PB03);   //PB09 config output
    io_write_pin(PB03,0);  
    io_cfg_output(PB04);   //PB10 config output
    io_write_pin(PB04,0);  //PB10 write low power
}
void Lock_1_on(){
	io_write_pin(PB03,1);   
}
void Lock_1_off(){
	io_write_pin(PB03,0);  
}
void Lock_2_on(){
	io_write_pin(PB04,1);   
}
void Lock_2_off(){
	io_write_pin(PB04,0);  
}

uint8_t lock_task_flag_1=0;
uint8_t lock_task_flag_2=0;

uint8_t lock_task_flag_1_temp=0;
uint8_t lock_task_flag_2_temp=0;


 //开锁任务  once 50ms
void Lock_task(){
		static uint16_t count_1;
		static uint16_t count_2;
		static uint8_t  temp=0x10;    //等待					 
		static uint8_t  busy_1=0;					 
		static uint8_t  busy_2=0;	
		
		if(lock_task_flag_1==1 && busy_2==0){
		
					if(lock_state[0]==0){
							 Lock_1_off();
							 lock_task_flag_1=0;
							 lock_task_flag_1_temp=0;
							 KEY_ONCE=0;
							 temp=0x00;         //成功
							 TX_DATA_BUF[6]=temp;
							 user_ble_send_flag=1;							 
							 LOG_I("LOCK1_OPEN");
							 busy_1=0;
					}
					else{
						count_1++;
						Lock_1_on();
						busy_1=1;						
							if(count_1>24){
									Lock_1_off();
									lock_task_flag_1=0;
									lock_task_flag_1_temp=0;
									KEY_ONCE=0;
									temp=0x03;       //失败
									TX_DATA_BUF[6]=temp;
									user_ble_send_flag=1;
									LOG_I("LOCK1_TIME_OUT");
									busy_1=0;
							}
					}
		}
		else{
			count_1=0;
		}
		
		
		if(lock_task_flag_2==1 && busy_1==0){

					if(lock_state[1]==0){
							 Lock_2_off();
							 lock_task_flag_2=0;
							 lock_task_flag_2_temp=0;
							 KEY_ONCE=0;
							 temp=0x00;         //成功
							 TX_DATA_BUF[6]=temp;
							 LOG_I("LOCK2_OPEN");
							 busy_2=0;
							 user_ble_send_flag=1;
					}
					else{
						count_2++;
						Lock_2_on();
						busy_2=1;						
							if(count_2>24){
									Lock_2_off();
									lock_task_flag_2=0;
									lock_task_flag_2_temp=0;
									KEY_ONCE=0;
									temp=0x03;       //失败
									TX_DATA_BUF[6]=temp;
									LOG_I("LOCK2_TIME_OUT");
									busy_2=0;
									user_ble_send_flag=1;
							}
					}
		}
		else{
			count_2=0;
		}
	
}
