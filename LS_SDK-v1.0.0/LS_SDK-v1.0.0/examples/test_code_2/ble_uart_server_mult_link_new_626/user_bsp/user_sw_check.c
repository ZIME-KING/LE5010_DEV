 #include "user_main.h"
 #include "user_sw_check.h"
 
 
SW_TypeDef lock_sw;

__attribute__((constructor)) void init_lock_sw(){
		
		lock_sw.opt1=0;
	  lock_sw.opt2=1;
}

void	check_sw(){ 
static uint8_t count;

	count++;
	if(moro_task_flag==1){	
				if(count%2==0){
				io_write_pin(SW_EN_1,1);  
				io_write_pin(SW_EN_2,1);  
		}
		else if(count%2==1){
			lock_sw.opt1= io_read_pin(SW_IN_1);
			lock_sw.opt2 =io_read_pin(SW_IN_2) ;
			lock_sw.opt3 =0xFF;//io_read_pin(SW_IN_3);
			hw_lock_status=(lock_sw.opt1<<4) + lock_sw.opt2;		
			io_write_pin(SW_EN_1,0);  
			io_write_pin(SW_EN_2,0);	
		}
	}
	else{
		if(count%500==0){
				io_write_pin(SW_EN_1,1);  
				io_write_pin(SW_EN_2,1);  
		}
		else if(count%500==1){
			lock_sw.opt1= io_read_pin(SW_IN_1);
			lock_sw.opt2 =io_read_pin(SW_IN_2) ;
			lock_sw.opt3 =0xFF;//io_read_pin(SW_IN_3);
			hw_lock_status=(lock_sw.opt1<<4) + lock_sw.opt2;		
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
			lock_sw.opt3 =0xFF;//io_read_pin(SW_IN_3);
		
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






//阻塞方式测量红外状态
void	check_sw_wait(){ 
//static uint8_t count;
//			count++;
		io_cfg_output(SW_EN_1);   	//
    io_write_pin(SW_EN_1,1);

    io_cfg_output(SW_EN_2);   	//
    io_write_pin(SW_EN_2,1);
		
		io_cfg_input(SW_IN_1);   			//
    io_cfg_input(SW_IN_2);
		
			
			io_write_pin(SW_EN_1,1);  
			io_write_pin(SW_EN_2,1);  
			DELAY_US(5000);
			lock_sw.opt1= io_read_pin(SW_IN_1);
			lock_sw.opt2 =io_read_pin(SW_IN_2) ;
			lock_sw.opt3 =0xFF;//io_read_pin(SW_IN_3);
			hw_lock_status=(lock_sw.opt1<<4) + lock_sw.opt2;		
			io_write_pin(SW_EN_1,0);  
			io_write_pin(SW_EN_2,0);		
}


void	SW_IO_Init(){
		io_cfg_output(SW_EN_1);   	//
    io_write_pin(SW_EN_1,1);

    io_cfg_output(SW_EN_2);   	//
    io_write_pin(SW_EN_2,1);
		
		io_cfg_input(SW_IN_1);   			//
    io_cfg_input(SW_IN_2);		
}

void	SW_IO_Deinit(){
		io_cfg_output(SW_EN_1);   	//
    io_write_pin(SW_EN_1,0);

    io_cfg_output(SW_EN_2);   	//
    io_write_pin(SW_EN_2,0);
//		
		io_cfg_input(SW_IN_1);   			//
    io_cfg_input(SW_IN_2);			
}











