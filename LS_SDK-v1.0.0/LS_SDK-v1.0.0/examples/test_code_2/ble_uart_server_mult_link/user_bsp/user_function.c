#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"

#define SW_EN_1  PA03    //红外1 开关
#define SW_EN_2  PA04    //红外2 开关

#define SW_IN_1  PB15    //红外1 输入
#define SW_IN_2  PA00    //红外2 输入
#define SW_IN_3  PA07    //霍尔  输入

typedef struct{
bool opt1;
bool opt2;
bool opt3;
}SW_TypeDef;

SW_TypeDef lock_sw;


void	check_sw(){ 
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
			io_write_pin(SW_EN_1,0);  
			io_write_pin(SW_EN_2,0);	
	}
}

void	power_io_init(){
		io_cfg_output(PA05);   //  超声波
    io_write_pin(PA05,1);  
		
		io_cfg_output(SW_EN_1);   //
    io_write_pin(SW_EN_1,1);  
		
		io_cfg_output(SW_EN_2);   //
    io_write_pin(SW_EN_2,1);  

		io_cfg_input(PA00);   //
		io_cfg_input(PB15);
		io_cfg_input(PA07);
}

void loop_task(){
	check_sw();

	LOG_I("OTP1:%d",io_read_pin(SW_IN_1));
	LOG_I("OTP2:%d",io_read_pin(SW_IN_2));	
	LOG_I("OTP3:%d",io_read_pin(SW_IN_3));	
}

//蓝牙启动成功跑一次
void User_BLE_Ready() {
    Buzzer_IO_Init();
    Moto_IO_Init();
    LED_Init();
		power_io_init();
    Moto_Task();
		
}



