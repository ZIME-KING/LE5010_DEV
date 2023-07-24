#include "user_main.h"
#include "Buzzer.h"

#define BUZZER_PIN PA06
#define BUZZER_EN  PA06

uint8_t buzzer_task_flag=0;         //一声
//uint8_t buzzer_task_flag_2=0;        //两声

void Buzzer_IO_Init(){
		io_cfg_output(BUZZER_PIN);   //PB09 config output
    io_write_pin(BUZZER_PIN,0);  
		
		io_cfg_output(BUZZER_EN);   //
    io_write_pin(BUZZER_EN,0);  
		
		io_pull_write(BUZZER_PIN,IO_PULL_DISABLE);
		io_pull_write(BUZZER_EN,IO_PULL_DISABLE);
}

void Buzzer_IO_DeInit(){
		io_cfg_disable (BUZZER_PIN);
		io_pull_write(BUZZER_PIN,IO_PULL_DOWN);

 		io_cfg_disable (BUZZER_EN);
		io_pull_write(BUZZER_EN,IO_PULL_DOWN);
}



////50ms 跑一次
//void Buzzer_Task(){

//	static uint8_t count;
//	static uint8_t count_2;
//	static uint8_t temp;
//	
//	if(buzzer_task_flag==1){
//  		Buzzer_ON();
//		if(count>=2){
//			buzzer_task_flag=0;
//			Buzzer_OFF();
//		}
//		count++;
//	}
//	else{
//		count=0;
//	}
//	
//	if(buzzer_task_flag_2==1){
//			count_2++;
//			count_2=count_2%4;
//		if(count_2<2){
//  		Buzzer_ON();
//		}
//		else{
//			Buzzer_OFF();
//			temp++;
//		}
//		
//		if(temp>=4){
//			Buzzer_OFF();
//			buzzer_task_flag_2=0;
//		}
//	}
//	else{
//			temp=0;
//			count_2=0;
//	}
//}



uint8_t buzzer_open_val;
uint8_t buzzer_close_val;
uint16_t buzzer_count_val;


void Set_buzzer_Task_val(uint16_t a,uint8_t b,uint8_t c){
 
 buzzer_count_val=a;
 buzzer_open_val=b;
 buzzer_close_val=c;
}

void Buzzer_Task_100(){

	static uint16_t i;
	static uint16_t count;	

	static uint16_t temp=0;
	
	if(buzzer_task_flag==0) return;	
	
	if(i<=buzzer_open_val){
		 Buzzer_ON();
	}
	else if(i>buzzer_open_val && i<=(buzzer_close_val+buzzer_open_val)){
		 Buzzer_OFF();
	}
	else {
		 i=0;
		 count++;
		 if(count>=buzzer_count_val)  {
		  count=0;
			buzzer_task_flag=0;
		 }
		 Buzzer_OFF();		 
	}	
	temp++;
	if(temp%100==1) 	i++;
}



void Buzzer_ON(){
	 	io_write_pin(BUZZER_PIN,1);   
}
void Buzzer_OFF(){
	 	io_write_pin(BUZZER_PIN,0);   
}


