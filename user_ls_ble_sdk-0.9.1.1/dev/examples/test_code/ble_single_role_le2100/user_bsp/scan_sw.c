#include "user_main.h"
#include "scan_sw.h"


//�������⿪���ж�ģʽ���Ի�������

#define SW1 PA00
#define SW2 PA01
#define KEY PB15

//io�½��ػ��Ѻ���
static void exitpb15_iowkup_init(void)
{	
	  io_cfg_input(KEY);               //����ģʽ                     
    io_pull_write(KEY, IO_PULL_UP);  //��������    
    io_exti_config(KEY,INT_EDGE_FALLING);  //�½��ش����ж� 
    io_exti_enable(KEY,true);            //�����ж�ʹ��    
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
//5ms ��һ��
void Scan_Key(){
	static uint8_t count;
	if(io_read_pin(KEY)==0){
			count++;
			if(count==2){
					buzzer_task_flag=1;
			}
	}
	else{
			count=0;
			buzzer_task_flag=0;
	}
}

void  ls_sleep_enter_lp2(void)
{
  struct deep_sleep_wakeup wakeup;
	memset(&wakeup,0,sizeof(wakeup));
	wakeup.pb15 = 1 ;									//ѡ��PB15��Ϊ����io
	wakeup.pb15_rising_edge = 0;			//ѡ����ػ���
	wakeup.pa00 = 1 ;									//ѡ��PA00��Ϊ����io
	wakeup.pa00_rising_edge = 0;			//ѡ����ػ���
	
	wakeup.rtc  = 1 ;
  enter_deep_sleep_mode_lvl2_lvl3(&wakeup);//���û��Ѻ���
}