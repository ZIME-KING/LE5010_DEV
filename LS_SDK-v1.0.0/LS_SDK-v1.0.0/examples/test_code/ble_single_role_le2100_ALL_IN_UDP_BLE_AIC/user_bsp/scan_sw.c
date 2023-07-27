#include "user_main.h"
#include "scan_sw.h"


//配置任意开关中断模式可以唤醒启动

#define SW1 PA00
#define SW2 PA01
#define KEY PB15





//IO中断测试
void Button_io_init_exti(void)
{	
		io_cfg_input(SW1);    //PB11 config input
    io_pull_write(SW1, IO_PULL_UP);    //PB11 config pullup
    io_exti_config(SW1,INT_EDGE_FALLING);    //PB11 interrupt falling edge
    io_exti_enable(SW1,true);    //PB11 interrupt enable
		
		
		io_cfg_input(SW2);    //PB11 config input
    io_pull_write(SW2, IO_PULL_UP);    //PB11 config pullup
    io_exti_config(SW2,INT_EDGE_FALLING);    //PB11 interrupt falling edge
    io_exti_enable(SW2,true);    //PB11 interrupt enable
		
		io_cfg_input(KEY);    //PB11 config input
    io_pull_write(KEY, IO_PULL_UP);    //PB11 config pullup
    io_exti_config(KEY,INT_EDGE_FALLING);    //PB11 interrupt falling edge
    io_exti_enable(KEY,true);    //PB11 interrupt enable
}

void io_exti_callback(uint8_t pin) 
{
		uint8_t i=0;
    switch (pin)
    {
    case SW1:
				LOG_I("SW1_tig %d",io_read_pin(SW1));
				sleep_time=0;   //休眠时间清0
				i++;
        break;
		
		case SW2:
				LOG_I("SW2_tig %d",io_read_pin(SW2));
				sleep_time=0;   //休眠时间清0
				i++;
        break;	

		case KEY:
				LOG_I("KEY_tig %d",io_read_pin(KEY));
				sleep_time=0;   //休眠时间清0
				i++;
        break;					
				
    default:
        break;
    }
	
		if(i!=0){
//				LOG_I("ENTER_POWER_H");
//				for(uint8_t i=0;i<100;i++){
//					LOG_I("SW1_tig %d",io_read_pin(SW1));
//					LOG_I("SW2_tig %d",io_read_pin(SW2));
//					LOG_I("KEY_tig %d",io_read_pin(KEY));
//				}
        power_mode=POWER_H;
				user_event_period=5;
				builtin_timer_start(user_event_timer_inst_0, user_event_period, NULL);
				AT_User_Set(0xFF);   //重新设置socket启动标记
		}
		
}





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

//static void SW2_init(){
//	  io_cfg_input(SW2);                       
//    io_pull_write(SW2, IO_PULL_UP);
//}

void Button_Gpio_Init(){
	
	
		io_cfg_input(SW1);    //PB11 config input
    io_pull_write(SW1, IO_PULL_UP);    //PB11 config pullup
    io_exti_config(SW1,INT_EDGE_FALLING);    //PB11 interrupt falling edge
    io_exti_enable(SW1,false);    //PB11 interrupt enable
		
		
		io_cfg_input(SW2);    //PB11 config input
    io_pull_write(SW2, IO_PULL_UP);    //PB11 config pullup
    io_exti_config(SW2,INT_EDGE_FALLING);    //PB11 interrupt falling edge
    io_exti_enable(SW2,false);    //PB11 interrupt enable
		
		io_cfg_input(KEY);    //PB11 config input
    io_pull_write(KEY, IO_PULL_UP);    //PB11 config pullup
    io_exti_config(KEY,INT_EDGE_FALLING);    //PB11 interrupt falling edge
    io_exti_enable(KEY,false);    //PB11 interrupt enable
	
}

void Button_Gpio_DeInit(){
	 	io_cfg_input(KEY);               				//                     
  io_pull_write(KEY, IO_PULL_UP);  				//    
	
	io_cfg_input(SW1);                       
  io_pull_write(SW1, IO_PULL_UP);     
	
	io_cfg_input(SW2);                       
  io_pull_write(SW2, IO_PULL_UP);

}




//#define RECORD_KEY1 1	 //蓝牙名称
#define RECORD_KEY2 2  //完成模块初始化标记
uint8_t wd_FLAG=0;
uint8_t KEY_ONCE;      //按键按下一次标记
//5ms 跑一次
void Scan_Key(){
	static uint16_t count=0;
//	static uint8_t edge_flag;
//	static uint8_t edge_flag_1;
	
	if(KEY_FLAG==1 && KEY_ONCE==1){
			//KEY_FLAG=0;
			moro_task_flag=1; 
			//globle_Result=0xff;
	}
	
	
	if(io_read_pin(KEY)==0){
			count++;
			if(count==3){
					user_count=30;
					sleep_time=0;
					SYSCFG->BKD[7]=0;
					KEY_ONCE=1;
					
				  if(test_mode_flag!=0xAA && moro_task_flag!=1){
						 moro_task_flag=1; 
					}
					LOG_I("Vbat:%d",VBat_value);				
					//if(Get_Task_State(OPEN_LOCK_SEND)){
					//		Open_Lock_Send();
					//		buzzer_task_flag=1;
					//}
					//else{
							Set_Task_State(OPEN_LOCK_SEND,START); //开锁数据请求
							buzzer_task_flag=1;
					//}
			}
			if(count==100 && test_mode_flag!=0xAA){
						uint8_t temp_val = 0xAA;
						
						LOG_I("reset_count%d",count);			
						//uint16_t length_one = 1;
						tinyfs_write(ID_dir_3,RECORD_KEY_T,(uint8_t*)&temp_val,1);	//给测试模式标记成0xBB（不开启）
						tinyfs_write_through();
						RESET_NB();
						platform_reset(0);
			}
			
			if(count==1500){
							buzzer_task_flag=1;
							//模块重新配置服务器
							tinyfs_write(ID_dir_2,RECORD_KEY2,(uint8_t*)"SET_NO",sizeof("SET_NO"));	
							tinyfs_write_through();
							RESET_NB();
//							wd_FLAG=1;
							platform_reset(0);
			}
			
	}
	else{
			count=0;
			//buzzer_task_flag=0;
	}
	
//	
//		edge_flag=edge_flag<<1;
//		edge_flag+=io_read_pin(SW2);
//		
//		if(edge_flag == 0x0F){	//上升延
//				buzzer_task_flag=1;

//		}
//		else if(edge_flag == 0xF0){
//				buzzer_task_flag=1;
//		}
//		
//		
//		
//		
//		
//		edge_flag_1=edge_flag_1<<1;
//		edge_flag_1+=io_read_pin(SW2);
//		
//		if(edge_flag_1 == 0x0F){	//上升延
//				buzzer_task_flag=1;
//				if(Get_Task_State(OPEN_LOCK_SEND)==1 )
//				{
//						Set_Task_State(OPEN_LOCK_DATA_SEND,1); //状态改变数据上传
//				}
//				//TX_DATA_BUF[6]=lock_state[0];
//		}
//		else if(edge_flag_1 == 0xF0){
//				buzzer_task_flag=1;
//				if(Get_Task_State(OPEN_LOCK_SEND)==1 )
//				{
//						Set_Task_State(OPEN_LOCK_DATA_SEND,1); //状态改变数据上传
//				}
//		}
}


uint8_t Check_SW1(){
	return io_read_pin(SW1);
} 
uint8_t Check_SW2(){
	return io_read_pin(SW2);
}
#define RECORD_KEY7 7
void  ls_sleep_enter_lp2(void)
{
	RESET_NB();
	
	tinyfs_write(ID_dir_3,RECORD_KEY7,&VBat_value,1);	
	#ifdef USER_TEST 
	tinyfs_write(ID_dir_2,RECORD_KEY10,(uint8_t*)&open_count,1);	
	#endif
	tinyfs_write_through();
	
	io_init();
	io_write_pin(PC00, 0);
	io_write_pin(PC01, 0);
	exitpb15_iowkup_init();
	exitpa00_iowkup_init();

	
	
	struct deep_sleep_wakeup wakeup;
	memset(&wakeup,0,sizeof(wakeup));
	wakeup.pb15 = 1 ;									//选择PB15作为唤醒io
	wakeup.pb15_rising_edge = 0;			//选择边沿唤醒
	wakeup.pa00 = 1 ;									//选择PA00作为唤醒io
	wakeup.pa00_rising_edge = 0;			//选择边沿唤醒
	
	wakeup.rtc  = 1 ;
  enter_deep_sleep_mode_lvl2_lvl3(&wakeup);//调用唤醒函数
}
