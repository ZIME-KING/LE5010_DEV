#include "user_main.h"
#include "moto.h"

void moto_gpio_init(void)
{
    io_cfg_output(PB03);   //PB09 config output
    io_write_pin(PB03,0);  
    io_cfg_output(PB04);   //PB10 config output
    io_write_pin(PB04,0);  //PB10 write low power
}
void Moto_P(){
	io_write_pin(PB03,1);   
	io_write_pin(PB04,0);   
}
void Moto_N(){
	io_write_pin(PB03,0);  
	io_write_pin(PB04,1);  
}
void Moto_S(){
	io_write_pin(PB03,1);  
	io_write_pin(PB04,1);  
}
void Moto_NULL(){
	io_write_pin(PB03,0);  
	io_write_pin(PB04,0);  
}


uint8_t Check_SW1(){
	return 0;
} 

uint8_t moro_task_flag;
uint8_t Moto_Task(){
	uint8_t temp=0x01;
	static int count=0;
	static int step=0;
	if(moro_task_flag==1){
		count++;
		switch(step){
			case 0:
					Moto_P();
					if(count>2000/50){
							count=0;
							step=1;
					}
					if(Check_SW1()==0){
					  	count=0;
							step=1;
					}
			break;
					
			case 1:			
					Moto_S();
					if(count>200/50){
							count=0;
							step=2;
							temp=Check_SW1();  		//00 打开成功  0x03 失败
							user_ble_send_flag=1;	//蓝牙发送数据
					}
			break;
			case 2:			
					Moto_N();
					if(count>2000/50){
							count=0;
							step=3;
					}
			break;
			case 3:			
					Moto_S();
					if(count>200/50){
							count=0;
							step=4;
					}
			break;
			case 4:			
					Moto_P();
					if(count>500/50){
							count=0;
							step=5;
					}
			break;
			case 5:			
					Moto_NULL();
					step=0;
					count=0;
					moro_task_flag=0;
			break;
	}
	}
	return temp;
}