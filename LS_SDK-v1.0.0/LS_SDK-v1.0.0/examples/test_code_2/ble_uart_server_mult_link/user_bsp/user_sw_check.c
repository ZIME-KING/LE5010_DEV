 #include "user_main.h"
 #include "user_sw_check.h"
 
 
SW_TypeDef lock_sw;

 
void	check_sw(){ 
static uint8_t count;

if(moro_task_flag){
		count++;
	if(count%2==0){
				io_write_pin(SW_EN_1,1);  
				io_write_pin(SW_EN_2,1);  
	}
	else{
			lock_sw.opt1= io_read_pin(SW_IN_1);
			lock_sw.opt2 =io_read_pin(SW_IN_2) ;
			lock_sw.opt3 =io_read_pin(SW_IN_3);
		
			if(lock_sw.opt3==0){
					hw_lock_status=0xFF;
			}
			else{
					hw_lock_status=(lock_sw.opt1<<4) + lock_sw.opt2;
			}
			io_write_pin(SW_EN_1,0);  
			io_write_pin(SW_EN_2,0);	
	}
}

}
void	check_sw_(){ 
static uint8_t count;

		count++;
	if(count%2==0){
				io_write_pin(SW_EN_1,1);  
				io_write_pin(SW_EN_2,1);  
	}
	else{
			lock_sw.opt1= io_read_pin(SW_IN_1);
			lock_sw.opt2 =io_read_pin(SW_IN_2) ;
			lock_sw.opt3 =io_read_pin(SW_IN_3);
		
			if(lock_sw.opt3==0){
					hw_lock_status=0xFF;
			}
			else{
					hw_lock_status=(lock_sw.opt1<<4) + lock_sw.opt2;
			}
			io_write_pin(SW_EN_1,0);  
			io_write_pin(SW_EN_2,0);	
	}
}