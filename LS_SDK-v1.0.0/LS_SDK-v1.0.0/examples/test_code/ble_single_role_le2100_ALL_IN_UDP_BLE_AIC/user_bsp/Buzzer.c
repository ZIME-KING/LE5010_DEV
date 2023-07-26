#include "user_main.h"
#include "Buzzer.h"

#define TIM_PRESCALER     (SDK_HCLK_MHZ-1)
#define TIM_PERIOD        (500 - 1) /* Period Value  */
#define TIM_PULSE1        	250       /* Capture Compare 1 Value  */
//#define TIM_PULSE2        200       /* Capture Compare 2 Value  */
//#define TIM_PULSE3        100       /* Capture Compare 3 Value  */
//#define TIM_PULSE4        50        /* Capture Compare 4 Value  */

TIM_HandleTypeDef TimHandle;
uint8_t buzzer_task_flag;
uint8_t buzzer_task_flag_2;


reg_timer_t temp_time_val;

void save_timer_config(){
	memcpy(&temp_time_val,LSGPTIMC, sizeof(temp_time_val));
}

void reset_timer_config(){
	memcpy(&temp_time_val,LSGPTIMC, sizeof(temp_time_val));
}

//50ms 跑一次
void Buzzer_Task(){
//	static uint8_t lask_state=0xff;
	static uint8_t count;
	static uint8_t count_2;
	static uint8_t temp;
	if(buzzer_task_flag==1){
  		Buzzer_ON();
		if(count>=2){
			buzzer_task_flag=0;
			Buzzer_OFF();
		}
		count++;
	}
	else{
		count=0;
	}
	
	if(buzzer_task_flag_2==1){
			count_2++;
			count_2=count_2%4;
		if(count_2<2){
  		Buzzer_ON();
		}
		else{
			Buzzer_OFF();
			temp++;
		}
		
		if(temp>=4){
			Buzzer_OFF();
			buzzer_task_flag_2=0;
		}
	}
	else{
			temp=0;
			count_2=0;
	}
}

void Basic_PWM_Output_Cfg(void)
{
		static uint8_t once_flag=0;
		if(once_flag!=0xAA){
					once_flag=0xAA;
					save_timer_config();					//保存定时器初始值
		}	
    TIM_OC_InitTypeDef sConfig = {0};

    gptimc1_ch1_io_init(PA06, true, 0);
   // gptimb1_ch2_io_init(PA01, true, 0);
   // gptimb1_ch3_io_init(PB14, true, 0);
   // gptimb1_ch4_io_init(PB15, true, 0);
    /*##-1- Configure the TIM peripheral #######################################*/
    TimHandle.Instance = LSGPTIMC;
    TimHandle.Init.Prescaler = TIM_PRESCALER; 
    TimHandle.Init.Period = TIM_PERIOD;
    TimHandle.Init.ClockDivision = 0;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Init(&TimHandle);

    /*##-2- Configure the PWM channels #########################################*/
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;

    sConfig.Pulse = TIM_PULSE1;
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);

//    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
//    sConfig.Pulse = TIM_PULSE2;
//    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);

//    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
//    sConfig.Pulse = TIM_PULSE3;
//    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);

//    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
//    sConfig.Pulse = TIM_PULSE4;
//    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);

    /*##-3- Start PWM signals generation #######################################*/
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
//    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
//    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
//    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
}

void Buzzer_ON(){
	 TimHandle.Instance->CCR1=TIM_PULSE1;
}
void Buzzer_OFF(){
		TimHandle.Instance->CCR1=0;
}
void Buzzer_Init(){
	Basic_PWM_Output_Cfg();
	TimHandle.Instance->CCR1=0;
}
void Buzzer_DeInit(){
			reset_timer_config();
	//	gptimc1_ch1_io_deinit();
    HAL_TIM_DeInit(&TimHandle);


	
	//		io_pull_write(PA06,IO_PULL_DOWN);		
//		io_cfg_input(PA06);   // config output
}




//HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
//int main(void)
//{
//    sys_init_none();
//    Basic_PWM_Output_Cfg();
//    while (1);
//}

