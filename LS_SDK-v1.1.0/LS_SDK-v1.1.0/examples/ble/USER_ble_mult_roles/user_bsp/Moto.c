#include "user_main.h"
#include "moto.h"

uint8_t moro_task_flag;

void Moto_IO_Init()
{
    io_cfg_output(PB03);   //PB09 config output
    io_write_pin(PB03,0);  
    io_cfg_output(PB04);   //PB10 config output
    io_write_pin(PB04,0);  //PB10 write low power
}
void Moto_N(){
	io_write_pin(PB03,1);   
	io_write_pin(PB04,0);   
}
void Moto_P(){
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

void Moto_Task(){
	if(moro_task_flag){
			__nop();
	}	
}
