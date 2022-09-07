#include "user_main.h"
#include "scan_sw.h"


//�������⿪���ж�ģʽ���Ի�������

#define SW1 PB11
#define SW2 PA07
#define KEY PB15

//io�½��ػ��Ѻ���
static void exitpb15_iowkup_init(void)
{
	  io_cfg_input(KEY);               				//����ģʽ                     
    io_pull_write(KEY, IO_PULL_UP);  				//��������    
    io_exti_config(KEY,INT_EDGE_FALLING);   //�½��ش����ж� 
    io_exti_enable(KEY,true);            		//�����ж�ʹ��    
}

static void exitpa07_iowkup_init(void)
{
	  io_cfg_input(SW1);                 
    io_pull_write(SW1, IO_PULL_UP);         
    io_exti_config(SW1,INT_EDGE_FALLING);   
    io_exti_enable(SW1,true);                
}

static void exitpb11_iowkup_init(void)
{
	  io_cfg_input(SW2);                       
    io_pull_write(SW2, IO_PULL_UP);         
    io_exti_config(SW2,INT_EDGE_FALLING);   
    io_exti_enable(SW2,true);                
}

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
	
	if(KEY_ONCE==1){
			if(lock_task_flag_1_temp==1){
							lock_task_flag_1=1; 
			}
			if(lock_task_flag_2_temp==1){
							lock_task_flag_2=1;	
			}
	}
		
	if(io_read_pin(KEY)==0){
			count++;
			if(count==3){
					user_count=30;
					sleep_time=0;
					SYSCFG->BKD[7]=0;
					KEY_ONCE=1;
					
					//����ģʽ���а������¾�������������
				  if(test_mode_flag!=0xAA){
						 lock_task_flag_1=1; 
						 lock_task_flag_2=1; 
					}
					LOG_I("Vbat:%d",VBat_value);		
					LOG_I("Db_val:%d",Db_val);
					LOG_I("L11:%d",lock_state[0]);						
					LOG_I("L2:%d",lock_state[1]);	
					LOG_I("sw1:%d",Check_SW1());						
					LOG_I("sw2:%d",Check_SW2());	
					
					Read_Status();
					Set_Task_State(OPEN_LOCK_SEND,START); //������������
					buzzer_task_flag=1;
			}
			if(count==100 && test_mode_flag!=0xAA){
						uint8_t temp_val = 0xAA;
						//uint16_t length_one = 1;
						tinyfs_write(ID_dir_3,RECORD_KEY_T,(uint8_t*)&temp_val,1);	//������ģʽ��ǳ�0xBB����������
						tinyfs_write_through();
						RESET_NB();
						platform_reset(0);
			}			
			if(count==1500){
							buzzer_task_flag=1;
							//ģ���������÷�����
							tinyfs_write(ID_dir_2,RECORD_KEY2,(uint8_t*)"SET_NO",sizeof("SET_NO"));	
							tinyfs_write_through();
							RESET_NB();
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
	exitpa07_iowkup_init();
	exitpb11_iowkup_init();

	
	
	struct deep_sleep_wakeup wakeup;
	memset(&wakeup,0,sizeof(wakeup));
	wakeup.pb15 = 1 ;									//ѡ��PB15��Ϊ����io
	wakeup.pb15_rising_edge = 0;			//ѡ����ػ���
	wakeup.pa00 = 1 ;									//ѡ��PA00��Ϊ����io
	wakeup.pa00_rising_edge = 0;			//ѡ����ػ���
	
	wakeup.rtc  = 1 ;
  enter_deep_sleep_mode_lvl2_lvl3(&wakeup);//���û��Ѻ���
}
