#include "user_main.h"
#include "scan_sw.h"


//配置任意开关中断模式可以唤醒启动

#define SW1 PA00
#define SW2 PA01
#define KEY PB15

//io下降沿唤醒函数
static void exitpb15_iowkup_init(void)
{	
	  io_cfg_input(KEY);               				//输入模式                     
    io_pull_write(KEY, IO_PULL_UP);  				//设置上拉    
    io_exti_config(KEY,INT_EDGE_FALLING);   //下降沿触发中断 
    io_exti_enable(KEY,true);            		//启动中断使能    
}
static void exitpa00_iowkup_init(void)
{	
	  io_cfg_input(SW1);                       
    io_pull_write(SW1, IO_PULL_UP);         
    io_exti_config(SW1,INT_EDGE_FALLING);   
    io_exti_enable(SW1,true);                
}
static void SW2_init(){
	  io_cfg_input(SW2);                       
    io_pull_write(SW2, IO_PULL_UP);
}

void Button_Gpio_Init(){
	exitpb15_iowkup_init();
	exitpa00_iowkup_init();
	SW2_init();
}
//5ms 跑一次
void Scan_Key(){
	static uint8_t count;
	static uint8_t edge_flag;
	
	if(io_read_pin(KEY)==0){
			count++;
			if(count==2){
					buzzer_task_flag=1;
					Set_Task_State(OPEN_LOCK_SEND,1);//开锁数据请求
			}
	}
	else{
			count=0;
			//buzzer_task_flag=0;
	}
		edge_flag=edge_flag<<1;
		edge_flag+=io_read_pin(SW2);
		
		if(edge_flag == 0x0F){	//上升延
				buzzer_task_flag=1;
				if(//Get_Task_State(OPEN_LOCK_SEND)==0 &&
					 Get_Task_State(START_LOCK_SEND)==0 &&
					 Get_Task_State(TICK_LOCK_SEND)==0 )
				{
						Set_Task_State(OPEN_LOCK_DATA_SEND,1); //状态改变数据上传
				}
				
			
			  user_ble_send_flag=1;
				TX_DATA_BUF[0]=0x52;		// CMD
				TX_DATA_BUF[1]=TOKEN[0];TX_DATA_BUF[2]=TOKEN[1];TX_DATA_BUF[3]=TOKEN[2];TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
				TX_DATA_BUF[5]=0x01;    //LEN
				TX_DATA_BUF[6]=lock_state[0];
		}
		else if(edge_flag == 0xF0){
				buzzer_task_flag=1;
				if(//Get_Task_State(OPEN_LOCK_SEND)==0 &&
					 Get_Task_State(START_LOCK_SEND)==0 &&
					 Get_Task_State(TICK_LOCK_SEND)==0 )
				{
						Set_Task_State(OPEN_LOCK_DATA_SEND,1); //状态改变数据上传
				}
				user_ble_send_flag=1;    
				TX_DATA_BUF[0]=0x52;		// CMD
				TX_DATA_BUF[1]=TOKEN[0];TX_DATA_BUF[2]=TOKEN[1];TX_DATA_BUF[3]=TOKEN[2];TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
				TX_DATA_BUF[5]=0x01;    //LEN
				TX_DATA_BUF[6]=lock_state[0];
		}
}


uint8_t Check_SW1(){
	return io_read_pin(SW1);
} 
uint8_t Check_SW2(){
	return io_read_pin(SW2);
}


void  ls_sleep_enter_lp2(void)
{
	io_write_pin(PC00, 0);
	io_write_pin(PC01, 0);
	struct deep_sleep_wakeup wakeup;
	memset(&wakeup,0,sizeof(wakeup));
	wakeup.pb15 = 1 ;									//选择PB15作为唤醒io
	wakeup.pb15_rising_edge = 0;			//选择边沿唤醒
	wakeup.pa00 = 1 ;									//选择PA00作为唤醒io
	wakeup.pa00_rising_edge = 0;			//选择边沿唤醒
	
	wakeup.rtc  = 1 ;
  enter_deep_sleep_mode_lvl2_lvl3(&wakeup);//调用唤醒函数
}