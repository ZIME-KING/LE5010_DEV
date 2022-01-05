#include "user_function.h"
#include "key.h"


unsigned char key_status=0;	
unsigned char key_busy=0; 		//按键事件忙标记，	

unsigned char touch_key_busy=0;
unsigned char touch_key_staus=0;
  
#define SHORT_TIME 50/5
#define LONG_TIME  800/5
#define DOUBLE_RELEASE_TIME_MAX 300/5
#define DOUBLE_RELEASE_TIME_MIN 20/5	

//5MS进一次中断，检测按键
void scan_key(){
	static int release_count_double;
	static int release_count ;
	static int perss_count ;
	static int perss_count_long;
  static unsigned char edge_flag;
  static unsigned char edge_status;
	
	if(io_read_pin(PA07)){
		io_write_pin(PA08,0) ;
	}
	else{
		io_write_pin(PA08,1) ;
		no_act_count=0;
	}
		  edge_flag=edge_flag<<1;
			edge_flag+=io_read_pin(PA07);
			
			//edge_flag=(edge_flag|0x03);
			if(edge_flag == 0x0F){
				edge_status=1;				//上升延
				release_count_double=release_count;
				release_count=0;
			}
			else if(edge_flag == 0xF0){
				edge_status=0;				//下降延
			}
			if(edge_status){
					release_count++;
					if(release_count>65530)release_count=65530;
			}
			else{
				perss_count_long++;
					perss_count++;
			}
			if(key_busy==1){
			}
			else{
			
			//按键释放 大于双击最大时间后进行处理
			if(release_count==(DOUBLE_RELEASE_TIME_MAX)){
				if( release_count_double>DOUBLE_RELEASE_TIME_MIN  &&  release_count_double<DOUBLE_RELEASE_TIME_MAX ){
					key_status=DOUBLE;
					key_busy=1;
				}
			  else if(SHORT_TIME < perss_count &&  perss_count<LONG_TIME ){
					key_status=SHORT;
					key_busy=1;
				}
				else if( perss_count>LONG_TIME && perss_count<LONG_TIME*1.5){
					key_status=LONG;
					key_busy=1;
				}
//				LOG_I("touch_key_staus=%d",touch_key_staus);
//				LOG_I("perss_count=%d",perss_count);
//				LOG_I("release_count_double=%d",release_count_double);
				 release_count_double=0;
				 perss_count=0;
				 perss_count_long=0;
			}
			else if(perss_count_long>LONG_TIME*1.5){
					key_status=LONG_NO_RELEASE;
					key_busy=1;
					perss_count_long=0;
			}
	}
}

//5MS进一次中断，检测触摸按键

void scan_touch_key(){
	static int release_count_double;
	static int release_count ;
	static int perss_count ;
  static unsigned char edge_flag;
  static unsigned char edge_status;
	
		if(io_read_pin(PA00)==0){
			no_act_count=0;
		}
	
		  edge_flag=edge_flag<<1;
			edge_flag+=io_read_pin(PA00);
			
			//edge_flag=(edge_flag|0x03);
			if(edge_flag == 0x0F){
				edge_status=1;				//上升延
				release_count_double=release_count;
				release_count=0;
			}
			else if(edge_flag == 0xF0){
				edge_status=0;				//下降延
			}
			if(edge_status){
					release_count++;
					if(release_count>65530)release_count=65530;
			}
			else{
					perss_count++;
			}
			if(touch_key_busy==1){
			}
			else{
			
			//按键释放 大于双击最大时间后进行处理
			if(release_count==(DOUBLE_RELEASE_TIME_MAX)){
				if( release_count_double>DOUBLE_RELEASE_TIME_MIN  &&  release_count_double<DOUBLE_RELEASE_TIME_MAX ){
					touch_key_staus=DOUBLE;
					touch_key_busy=1;
				}
			  else if(SHORT_TIME < perss_count &&  perss_count<LONG_TIME ){
					touch_key_staus=SHORT;
					touch_key_busy=1;
				}
				else if( perss_count>LONG_TIME ){
					touch_key_staus=LONG;
					touch_key_busy=1;
				}
//				LOG_I("touch_key_staus=%d",touch_key_staus);
//				LOG_I("perss_count=%d",perss_count);
//				LOG_I("release_count_double=%d",release_count_double);
				 release_count_double=0;
				 perss_count=0;
			}
	}
}
