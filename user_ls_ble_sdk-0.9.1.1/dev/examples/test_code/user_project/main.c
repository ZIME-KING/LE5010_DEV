#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lstimer.h"
#include "lsuart.h"
#include "lsdmac.h"

#include "io_config.h"
#include "platform.h"
#include "log.h"
#include "user_uart.h"

#include "user_main.h"

#define TIM_PRESCALER     (SDK_HCLK_MHZ-1)
#define TIM_PERIOD        (1000 - 1) /* Period Value  */
#define TIM_PULSE1        500       /* Capture Compare 1 Value  */  
#define TIM_PULSE2        200       /* Capture Compare 2 Value  */  

TIM_HandleTypeDef TimHandle_TIM; 	//TMIER: LSBSTIM
TIM_HandleTypeDef TimHandle_PWM;  //TMIER: LSGPTIMB
UART_HandleTypeDef UART_Config; 

uint32_t uwTick;  //1ms定时时基

static void Basic_PWM_Output_Cfg(void)
{
    TIM_OC_InitTypeDef sConfig = {0};

    gptimb1_ch1_io_init(PA01, true, 0);
    gptimb1_ch2_io_init(PA02, true, 0);

    /*##-1- Configure the TIM peripheral #######################################*/
    TimHandle_PWM.Instance = LSGPTIMB;
    TimHandle_PWM.Init.Prescaler = TIM_PRESCALER; 
    TimHandle_PWM.Init.Period = TIM_PERIOD;
    TimHandle_PWM.Init.ClockDivision = 0;
    TimHandle_PWM.Init.CounterMode = TIM_COUNTERMODE_UP;
    TimHandle_PWM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Init(&TimHandle_PWM);

    /*##-2- Configure the PWM channels #########################################*/
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;

    sConfig.Pulse = TIM_PULSE1;
    HAL_TIM_PWM_ConfigChannel(&TimHandle_PWM, &sConfig, TIM_CHANNEL_1);

    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.Pulse = TIM_PULSE2;
    HAL_TIM_PWM_ConfigChannel(&TimHandle_PWM, &sConfig, TIM_CHANNEL_2);

    /*##-3- Start PWM signals generation #######################################*/
    HAL_TIM_PWM_Start(&TimHandle_PWM, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TimHandle_PWM, TIM_CHANNEL_2);
}

void User_PWM_Output_Cfg(uint16_t PULSE1,uint16_t PULSE2)
{
    TIM_OC_InitTypeDef sConfig = {0};

    gptimb1_ch1_io_init(PA01, true, 0);
    gptimb1_ch2_io_init(PA02, true, 0);

    /*##-1- Configure the TIM peripheral #######################################*/
    TimHandle_PWM.Instance = LSGPTIMB;
    TimHandle_PWM.Init.Prescaler = TIM_PRESCALER; 
    TimHandle_PWM.Init.Period = TIM_PERIOD;
    TimHandle_PWM.Init.ClockDivision = 0;
    TimHandle_PWM.Init.CounterMode = TIM_COUNTERMODE_UP;
    TimHandle_PWM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Init(&TimHandle_PWM);

    /*##-2- Configure the PWM channels #########################################*/
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;

    sConfig.Pulse = PULSE1;
    HAL_TIM_PWM_ConfigChannel(&TimHandle_PWM, &sConfig, TIM_CHANNEL_1);

    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.Pulse = PULSE2;
    HAL_TIM_PWM_ConfigChannel(&TimHandle_PWM, &sConfig, TIM_CHANNEL_2);

    /*##-3- Start PWM signals generation #######################################*/
    HAL_TIM_PWM_Start(&TimHandle_PWM, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TimHandle_PWM, TIM_CHANNEL_2);
}

void Basic_Timer_Cfg(void)
{
    TimHandle_TIM.Instance           = LSBSTIM;
    TimHandle_TIM.Init.Prescaler     = TIM_PRESCALER;
    TimHandle_TIM.Init.Period        = TIM_PERIOD;
    TimHandle_TIM.Init.ClockDivision = 0;
    TimHandle_TIM.Init.CounterMode   = TIM_COUNTERMODE_UP;
    HAL_TIM_Init(&TimHandle_TIM);
    HAL_TIM_Base_Start_IT(&TimHandle_TIM);
}

void gpio_init(void)
{
//    io_cfg_output(PB09);   //PB09 config output
//    io_write_pin(PB09,0);  //PB09 write low power
//    io_cfg_output(PB10);   //PB10 config output
//    io_write_pin(PB10,1);  //PB10 write low power
//		io_cfg_output(PA01);   //PA01 config output
//    io_write_pin(PA01,0);  //PA01 write low power
	  io_cfg_output(PB12);
    io_write_pin(PB12,1);
}
 
void exti_test(void)
{
    io_cfg_input(PB11);    //PB11 config input
    io_pull_write(PB11, IO_PULL_UP);    //PB11 config pullup
    io_exti_config(PB11,INT_EDGE_FALLING);    //PB11 interrupt falling edge
    io_exti_enable(PB11,true);    //PB11 interrupt enable
}




static void uart_init(void)
{
    UART_Config.UARTX = UART1;
    UART_Config.Init.BaudRate = UART_BAUDRATE_115200;
    UART_Config.Init.MSBEN = 0;
    UART_Config.Init.Parity = UART_NOPARITY; 
    UART_Config.Init.StopBits = UART_STOPBITS1;
    UART_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Config);
}

void user_delay(int time){
	time=time*1000;
	while(time--);
}

int main(void)
{
//	uint8_t user_buf[50];  
	
    sys_init_none();
    gpio_init();

		Basic_PWM_Output_Cfg();
		Basic_Timer_Cfg();
	  
	  uart1_io_init(PB00,PB01);
    uart_init();
    HAL_UART_Receive_IT(&UART_Config,uart_buffer,1);  //启动接收中断 重新使能串口1接收中断
	
		UART_Transmit_Str(&UART_Config,(uint8_t*)"uart_ok/r/n");
	//exti_test();
	 while (1)
   {
		 
		if(CompareTime(&Task_5)){
		  GetTime(&Task_5);
			io_toggle_pin(PB12);    //pb12 反转
	  }
		
	  if(CompareTime(&Task_50)){
		  GetTime(&Task_50);
	  }

	  if(CompareTime(&Task_1000)){
		  GetTime(&Task_1000);
			user_delay(100);
			HAL_UART_Transmit_IT(&UART_Config,uart_buffer,20);  //把\0发出去
			user_delay(100);
			UART_Transmit_Str(&UART_Config,(uint8_t*)"\r\n");
	  }
		
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == LSBSTIM)
    {
			uwTick++;
			Uart_Time_Even();
      //io_toggle_pin(PB12);
    }
}

void io_exti_callback(uint8_t pin) 
{
    switch (pin)
    {
    case PB11:
        // do something
        io_toggle_pin(PB10);  
        break;
    default:
        break;
    }
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->UARTX==UART1){

		HAL_UART_Receive_IT(&UART_Config,uart_buffer,1);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->UARTX==UART1){

		Receive_Interrupt();  
	  HAL_UART_Receive_IT(&UART_Config,uart_buffer,1);
	}
}

 




