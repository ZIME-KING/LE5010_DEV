#include "LED_RGB.h"

#include "user_main.h"
#include "lstimer.h"
#include <string.h>
#include "io_config.h"
#include "platform.h"
#include "log.h"

#define TIM_PRESCALER     (SDK_HCLK_MHZ-1)
#define TIM_PERIOD        (0xff - 1) /* Period Value  */
#define TIM_PULSE1        100       /* Capture Compare 1 Value  */
#define TIM_PULSE2        100       /* Capture Compare 2 Value  */
#define TIM_PULSE3        100       /* Capture Compare 3 Value  */
//#define TIM_PULSE4      50        /* Capture Compare 4 Value  */

TIM_HandleTypeDef TimHandle;

static void Basic_PWM_Output_Cfg(void)
{
    TIM_OC_InitTypeDef sConfig = {0};

    gptimb1_ch1_io_init(PA10, true, 0);
    gptimb1_ch2_io_init(PA11, true, 0);
    gptimb1_ch3_io_init(PA09, true, 0);
//    gptimb1_ch4_io_init(PB15, true, 0);
    /*##-1- Configure the TIM peripheral #######################################*/
    TimHandle.Instance = LSGPTIMB;
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

    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.Pulse = TIM_PULSE2;
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);

    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.Pulse = TIM_PULSE3;
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);

//    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
//    sConfig.Pulse = TIM_PULSE4;
//    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);

    /*##-3- Start PWM signals generation #######################################*/
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
//    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
}

void LED_Init(void)
{

    io_cfg_output(PB13);   //PB09 config output
    io_write_pin(PB13,1);

    Basic_PWM_Output_Cfg();

    TimHandle.Instance->CCR1=10;
    TimHandle.Instance->CCR2=20;
    TimHandle.Instance->CCR3=30;
}

uint8_t now_r;
uint8_t now_g;
uint8_t now_b;

uint8_t set_r;
uint8_t set_g;
uint8_t set_b;

uint8_t set_t0=10;
uint8_t set_t1=0;
uint8_t set_t2=10;
uint8_t set_t3=0;

uint8_t LED_status=1;

uint8_t buf[3]= {0xff,0x00,0x00};

uint8_t max_brightness=0xff;
uint8_t min_brightness=0x00;

void Set_LED_Function_val(uint8_t status,
													uint8_t r,uint8_t g,uint8_t b,
													uint8_t max,uint8_t min,
													uint8_t T0,uint8_t T1,uint8_t T2,uint8_t T3)
{
	LED_status=status;
	if(LED_status==1){
				buf[0]=r; buf[1]=g;buf[2]=b;
	}
	else{
			 buf[0]=0; buf[1]=0;buf[2]=0;                        
	}
	
	max_brightness = max;
	min_brightness=  min;
	set_t0=T0;
	set_t1=T1;
	set_t2=T2;
	set_t3=T3;
}



//1ms  跑一次
void LED_Functon() {
    static uint8_t mode_flag=0;
    static uint16_t count=0;
    static uint16_t count_T1=0;
    static uint16_t count_T3=0;
    static uint16_t time=1;
    static uint16_t temp=1;
    count++;
    TimHandle.Instance->CCR1=now_g;
    TimHandle.Instance->CCR2=now_r;
    TimHandle.Instance->CCR3=now_b;

    switch(mode_flag) {
    case 0:
        set_r= buf[0]*max_brightness/256;
        set_g= buf[1]*max_brightness/256;
        set_b= buf[2]*max_brightness/256;
        time=set_t0*100/(max_brightness-min_brightness);     //T0为协议输入值 单位100ms
        if(count%time==0) {
            temp=0;
            if(now_r<set_r) {
                now_r++;
                temp++;
            }
            if(now_g<set_g) {
                now_g++;
                temp++;
            }
            if(now_b<set_b) {
                now_b++;
                temp++;
            }
            if(temp==0) {
                mode_flag=1;
            }
        }
        break;
    case 1:
        set_r= buf[0]*max_brightness/256;
        set_g= buf[1]*max_brightness/256;
        set_b= buf[2]*max_brightness/256;
        count_T1++;
        if(count_T1>=set_t1*100) {
            mode_flag=2;
            count_T1=0;
        }
        break;
    case 2:
        set_r= buf[0]*min_brightness/256;
        set_g= buf[1]*min_brightness/256;
        set_b= buf[2]*min_brightness/256;
        time=set_t2*100/(max_brightness-min_brightness);     //T0为协议输入值 单位100ms

        if(count%time==0) {
            temp=0;
            if(now_r>set_r) {
                now_r--;
                temp++;
            }
            if(now_g>set_g) {
                now_g--;
                temp++;
            }
            if(now_b>set_b) {
                now_b--;
                temp++;
            }
            if(temp==0) {
                mode_flag=3;
            }
        }
        break;

    case 3:
        set_r= 0 ;
        set_g= 0 ;
        set_b= 0 ;
        count_T3++;
        if(count_T3>=set_t3*100) {
            mode_flag=0;
            count_T3=0;

        }
        break;
    }
}
