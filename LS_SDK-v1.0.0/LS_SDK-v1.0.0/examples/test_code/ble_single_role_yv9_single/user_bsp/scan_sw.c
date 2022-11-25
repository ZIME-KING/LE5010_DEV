#include "user_main.h"
#include "scan_sw.h"


//�������⿪���ж�ģʽ���Ի�������

#define SW1 PA00
#define SW2 PA01
#define KEY PB15

static void exitpb15_iowkup_init(void)
{
	  io_cfg_input(KEY);               				//����ģʽ                     
    io_pull_write(KEY, IO_PULL_UP);  				//��������    
    io_exti_config(KEY,INT_EDGE_FALLING);   //�½��ش����ж� 
    io_exti_enable(KEY,true);            		//�����ж�ʹ��    
}

//static void exitpa07_iowkup_init(void)
//{
//	  io_cfg_input(SW1);                 
//    io_pull_write(SW1, IO_PULL_UP);         
//    io_exti_config(SW1,INT_EDGE_FALLING);   
//    io_exti_enable(SW1,true);                
//}


static void exitpa00_iowkup_init(void)
{
	  io_cfg_input(SW1);                       
    io_pull_write(SW1, IO_PULL_UP);         
    io_exti_config(SW1,INT_EDGE_FALLING);   
    io_exti_enable(SW1,true);                
}

//static void exitpb11_iowkup_init(void)
//{
//	  io_cfg_input(SW2);                       
//    io_pull_write(SW2, IO_PULL_UP);         
//    io_exti_config(SW2,INT_EDGE_FALLING);   
//    io_exti_enable(SW2,true);                
//}

void Button_Gpio_Init(){
	io_cfg_input(KEY);               				//����ģʽ                     
  io_pull_write(KEY, IO_PULL_UP);  				//��������    
	
	io_cfg_input(SW1);                       
  io_pull_write(SW1, IO_PULL_UP);     
	
	io_cfg_input(SW2);                       
  io_pull_write(SW2, IO_PULL_UP);
//	SW2_init();
}
//#define RECORD_KEY1 1	 //��������
#define RECORD_KEY2 2  //���ģ���ʼ�����
uint8_t wd_FLAG=0;
uint8_t KEY_ONCE;      //��������һ�α��
//5ms ��һ��
void Scan_Key(){
	static uint16_t count;
//	static uint8_t edge_flag;
//	static uint8_t edge_flag_1;
	
	if(KEY_FLAG==1 && KEY_ONCE==1){
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
//					rfid_task_flag_1=1; 
//					//����ģʽ���а������¾�������������
				  
				  if(test_mode_flag!=0xAA && moro_task_flag!=1){
						 moro_task_flag=1; 
					}
					
					
					LOG_I("Vbat:%d",VBat_value);		
					LOG_I("Db_val:%d",Db_val);
					LOG_I("L11:%d",lock_state[0]);						
//					LOG_I("L2:%d",lock_state[1]);	
					LOG_I("sw1:%d",Check_SW1());						
					LOG_I("sw2:%d",Check_SW2());					

					LOG_I("Open_Lock_Send:%d",T0_enable);		
					LOG_I("Open_Lock_Send:%d",T1_enable);
					LOG_I("Tick_Lock_Send:%d",T2_enable);						
					LOG_I("Data_Send:%d",T3_enable);	
					LOG_I("sw1:%d",Check_SW1());						
					LOG_I("sw2:%d",Check_SW2());		

//uint8_t T0_enable=0;  //���������ϱ�  			c
//uint8_t T1_enable=0;	//������������				Open_Lock_Send
//uint8_t T2_enable=0;	//����������					Tick_Lock_Send
//uint8_t T3_enable=0;	//״̬������ݷ���  	Open_Lock_Data_Send(uint8_t lock_ID,uint8_t lock_state)


					Read_Status();				
					Set_Task_State(OPEN_LOCK_SEND,START); //������������
					buzzer_task_flag=1;
			}
			if(count==100 && test_mode_flag!=0xAA){
						uint8_t temp_val = 0xAA;
						//uint16_t length_one = 1;
						tinyfs_write(ID_dir_3,RECORD_KEY_T,(uint8_t*)&temp_val,1);	//������ģʽ��ǳ�0xBB����������
						tinyfs_write_through();
						
							AT_Command_Send(POWER_OFF);										
							DELAY_US(1000*100);
							io_cfg_input(PB08);		//4G CAT1 ��Դ		
              DELAY_US(1000*1000*3);
							platform_reset(0); 	
			}			
			if(count==1500){
							buzzer_task_flag=1;
							//ģ���������÷�����
							tinyfs_write(ID_dir_2,RECORD_KEY2,(uint8_t*)"SET_NO",sizeof("SET_NO"));	
							tinyfs_write_through();
							
							
							AT_Command_Send(POWER_OFF);										
							DELAY_US(1000*100);
							io_cfg_input(PB08);		//4G CAT1 ��Դ		
              DELAY_US(1000*1000*3);
							platform_reset(0); 						
			}
			
	}
	else{
			count=0;
	}
	
//	
//		edge_flag=edge_flag<<1;
//		edge_flag+=io_read_pin(SW2);
//		
//		if(edge_flag == 0x0F){	//������
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
//		if(edge_flag_1 == 0x0F){	//������
//				buzzer_task_flag=1;
//				if(Get_Task_State(OPEN_LOCK_SEND)==1 )
//				{
//						Set_Task_State(OPEN_LOCK_DATA_SEND,1); //״̬�ı������ϴ�
//				}
//				//TX_DATA_BUF[6]=lock_state[0];
//		}
//		else if(edge_flag_1 == 0xF0){
//				buzzer_task_flag=1;
//				if(Get_Task_State(OPEN_LOCK_SEND)==1 )
//				{
//						Set_Task_State(OPEN_LOCK_DATA_SEND,1); //״̬�ı������ϴ�
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
	tinyfs_write(ID_dir_3,RECORD_KEY7,&VBat_value,1);	
	#ifdef USER_TEST 
	tinyfs_write(ID_dir_2,RECORD_KEY10,(uint8_t*)&open_count,1);	
	#endif
	tinyfs_write_through();

	AT_Command_Send(POWER_OFF);
	DELAY_US(3*1000*1000);


		io_cfg_output(PA03);     //
		io_write_pin(PA03, 0);	 //
	
		io_cfg_input(PA04);			 //

		io_cfg_output(PA05);		 //
		io_write_pin(PA05, 0);		
		
		
		io_cfg_output(PC00);
		io_cfg_output(PC01);
		io_cfg_output(PA06);
		
		io_write_pin(PC00, 0);	
		io_write_pin(PC01,0);	
		io_write_pin(PA06,0);	
		
		io_cfg_input(PB03);	
		io_cfg_input(PB04);	
		
		
		io_cfg_output(PB08);		//4G CAT1 ��Դ				
		io_write_pin(PB08,0);	
	
	exitpb15_iowkup_init();
	exitpa00_iowkup_init();
//	exitpa07_iowkup_init();
//	exitpb11_iowkup_init();
	
	
	struct deep_sleep_wakeup wakeup;
	memset(&wakeup,0,sizeof(wakeup));
	wakeup.pb15 = 1 ;									//ѡ��PB15��Ϊ����io
	wakeup.pb15_rising_edge = 0;			//ѡ����ػ���
	wakeup.pa00 = 1 ;									//ѡ��PA00��Ϊ����io
	wakeup.pa00_rising_edge = 0;			//ѡ����ػ���
//	wakeup.pa07 = 1 ;									//ѡ��PB15��Ϊ����io
//	wakeup.pa07_rising_edge = 0;			//ѡ����ػ���
//	wakeup.pb11 = 1 ;									//ѡ��PA00��Ϊ����io
//	wakeup.pb11_rising_edge = 0;			//ѡ����ػ���
	
	wakeup.rtc  = 1 ;
  enter_deep_sleep_mode_lvl2_lvl3(&wakeup);//���û��Ѻ���         //�иĶ�
}
