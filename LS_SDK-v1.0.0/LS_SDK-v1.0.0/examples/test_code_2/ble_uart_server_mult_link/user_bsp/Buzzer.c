#include "user_main.h"
#include "Buzzer.h"

#define BUZZER_PIN PB02
#define BUZZER_EN  PA06

uint8_t buzzer_task_flag=0;         //一声
uint8_t buzzer_task_flag_2=0;        //两声

void Buzzer_IO_Init(){
		io_cfg_output(BUZZER_PIN);   //PB09 config output
    io_write_pin(BUZZER_PIN,0);  
		
		io_cfg_output(BUZZER_EN);   //
    io_write_pin(BUZZER_EN,0);  
}


//50ms 跑一次
void Buzzer_Task(){

	static uint8_t count;
	static uint8_t count_2;
	static uint8_t temp;
	
	if(buzzer_task_flag==1){
  		Buzzer_ON();
		if(count>=2){
			buzzer_task_flag=0;
			Buzzer_OFF();
		}
		count++;
	}
	else{
		count=0;
	}
	
	if(buzzer_task_flag_2==1){
			count_2++;
			count_2=count_2%4;
		if(count_2<2){
  		Buzzer_ON();
		}
		else{
			Buzzer_OFF();
			temp++;
		}
		
		if(temp>=4){
			Buzzer_OFF();
			buzzer_task_flag_2=0;
		}
	}
	else{
			temp=0;
			count_2=0;
	}
}
void Buzzer_ON(){
	 	io_write_pin(BUZZER_PIN,1);   
}
void Buzzer_OFF(){
	 	io_write_pin(BUZZER_PIN,0);   
}


