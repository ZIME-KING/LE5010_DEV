#include "user_main.h"
#include "moto.h"

#include "lstimer.h"
#include <string.h>
#include "io_config.h"
#include "platform.h"
#include "log.h"


uint8_t moro_task_flag=0;;
///////////////////////////////////////////

uint8_t test_moro_task_flag=0;


//#define TEST_DEBUG

///////////////////////////////////////////


#define TIM_PRESCALER     (SDK_HCLK_MHZ-1)
#define TIM_PERIOD        (0xff - 1) /* Period Value  */
#define TIM_PULSE1        100       /* Capture Compare 1 Value  */
#define TIM_PULSE2        100       /* Capture Compare 2 Value  */
//#define TIM_PULSE3        100       /* Capture Compare 3 Value  */
//#define TIM_PULSE4      50        /* Capture Compare 4 Value  */

TIM_HandleTypeDef M_TimHandle;

reg_timer_t temp_time_val;

void save_timer_config(){
	memcpy(&temp_time_val,LSGPTIMC, sizeof(temp_time_val));
}

static void M_Basic_PWM_Output_Cfg(void)
{
		//save_timer_config();

    TIM_OC_InitTypeDef sConfig = {0};

    gptimc1_ch1_io_init(PB03, true, 0);
    gptimc1_ch2_io_init(PB04, true, 0);
//    gptimb1_ch3_io_init(PA09, true, 0);
//    gptimb1_ch4_io_init(PB15, true, 0);
    /*##-1- Configure the TIM peripheral #######################################*/
    M_TimHandle.Instance = LSGPTIMC;
    M_TimHandle.Init.Prescaler = TIM_PRESCALER;
    M_TimHandle.Init.Period = TIM_PERIOD;
    M_TimHandle.Init.ClockDivision = 0;
    M_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    M_TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Init(&M_TimHandle);

    /*##-2- Configure the PWM channels #########################################*/
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;

    sConfig.Pulse = TIM_PULSE1;
    HAL_TIM_PWM_ConfigChannel(&M_TimHandle, &sConfig, TIM_CHANNEL_1);

    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.Pulse = TIM_PULSE2;
    HAL_TIM_PWM_ConfigChannel(&M_TimHandle, &sConfig, TIM_CHANNEL_2);

//    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
//    sConfig.Pulse = TIM_PULSE3;
//    HAL_TIM_PWM_ConfigChannel(&M_TimHandle, &sConfig, TIM_CHANNEL_3);

//    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
//    sConfig.Pulse = TIM_PULSE4;
//    HAL_TIM_PWM_ConfigChannel(&M_TimHandle, &sConfig, TIM_CHANNEL_4);

    /*##-3- Start PWM signals generation #######################################*/
    HAL_TIM_PWM_Start(&M_TimHandle, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&M_TimHandle, TIM_CHANNEL_2);
//    HAL_TIM_PWM_Start(&M_TimHandle, TIM_CHANNEL_3);
//    HAL_TIM_PWM_Start(&M_TimHandle, TIM_CHANNEL_4);
}

void Moto_IO_Init()
{
//    io_cfg_output(PB03);   //PB09 config output
//    io_write_pin(PB03,0);
//    io_cfg_output(PB04);   //PB10 config output
//    io_write_pin(PB04,0);  //PB10 write low power

//    io_cfg_output(PC01);   //
//    io_write_pin(PC01,0);  // PC01 jw3651 NE使能 高有效
//	
//	
//	    io_cfg_output(PB13);   //PB09 config output
//    io_write_pin(PB13,1);

    M_Basic_PWM_Output_Cfg();

    M_TimHandle.Instance->CCR1=0;
    M_TimHandle.Instance->CCR2=0;
}

//主要是关闭定时器
void Moto_IO_DeInit()
{
    HAL_TIM_DeInit(&M_TimHandle);

	//memcpy(LSGPTIMC,&temp_time_val, sizeof(temp_time_val));   
}


void Moto_N() {

//    io_write_pin(PB03,1);
//    io_write_pin(PB04,0);	
	  M_TimHandle.Instance->CCR1=254;
    M_TimHandle.Instance->CCR2=0;

#ifdef TEST_DEBUG
    LOG_I("Moto_N");
#endif

}
void Moto_P() {
    //io_write_pin(PC01,1);
//    io_write_pin(PB03,0);
//    io_write_pin(PB04,1);
	  M_TimHandle.Instance->CCR1=0;
    M_TimHandle.Instance->CCR2=254;


#ifdef TEST_DEBUG
    LOG_I("Moto_P");
#endif


}
void Moto_S() {
//		io_write_pin(PC01,1);
//    io_write_pin(PB03,1);
//    io_write_pin(PB04,1);

	    M_TimHandle.Instance->CCR1=254;
      M_TimHandle.Instance->CCR2=254;
	

#ifdef TEST_DEBUG
    LOG_I("Moto_S");
#endif

}
void Moto_NULL() {

 //   io_write_pin(PB03,0);
 //   io_write_pin(PB04,0);
	
		  M_TimHandle.Instance->CCR1=0;
			M_TimHandle.Instance->CCR2=0;



#ifdef TEST_DEBUG
    LOG_I("Moto_NULL");
#endif

}

void Moto_Task() {
    static uint16_t time_out;
    static uint16_t count=0;
    static uint16_t stop_flag=0;

    static uint8_t offset_time=0;


    //check_sw();
    if(moro_task_flag==1) {
        time_out++;
        if(time_out>=400) {
            LOG_I("MOTO_TIME_OUT");
            Moto_S();
            Moto_NULL();
            moro_task_flag=0;
						time_out=0;
            return;
        }
        if(hw_lock_status!=tag_lock_status) {
            if(tag_lock_status==POS_0) {
                Moto_P();
            }
            else if(tag_lock_status==POS_90) {
                switch(hw_lock_status) {
                case  POS_0:
                    Moto_N();
										offset_time=0;
                    break;

                case  POS_0_90:
                    Moto_N();
                    offset_time=0;
										break;

                case  POS_90_180:
                    Moto_P();
										offset_time=0;
                    break;

                case  POS_OUT:
                    Moto_P();
										offset_time=0;
                    break;
                }
            }
        } else {

            if(hw_lock_status==POS_0) {
                Moto_S();
                Moto_NULL();
            } else {
                stop_flag=1;
            }
            //Moto_S();
            moro_task_flag=0;
        }
    }
    else {
        time_out=0;
    }

    if(stop_flag==1) {
        count++;
        if(count>offset_time) {
            count=0;
            stop_flag=0;
            Moto_S();
            Moto_NULL();
        }
    }

}


void Test_Moto_Task() {

    static uint16_t count;

    if(test_moro_task_flag!=0) {
        count++;
        if(test_moro_task_flag==1) {
            Moto_P();
        }
        else if(test_moro_task_flag==2) {
            Moto_N();
        }

        if(count>=5) {
            Moto_S();
            Moto_NULL();
            test_moro_task_flag=0;
        }
    }
    else {
        count=0;
    }
		
//		if(hw_lock_status==POS_90){
//					 Moto_S();
//		}              
//		else if(hw_lock_status==POS_0_90)    {
//					Moto_N();
//		}
//		else if(hw_lock_status==POS_90_180)    {
//					 Moto_P();
//		}		
		
}


