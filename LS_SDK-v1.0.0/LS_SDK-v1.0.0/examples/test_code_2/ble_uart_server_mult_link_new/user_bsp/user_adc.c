#define LOG_TAG "ADC"
#include "user_adc.h"
#include "user_main.h"


static ADC_HandleTypeDef hadc;
static volatile uint8_t recv_flag = 0;

uint16_t aRxBuffer[2];
 
void ADC_Error_Handler(void)
{
	while(1);
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

static void adc_io_init(void)
{
    adc12b_in3_io_init();
    adc12b_in6_io_init();
}


////////////////////////////////////////// ADC ///////////////////////////////////////////
static void lsadc_init(void)
{  
		adc_io_init();
//		hadc.Instance = LSADC;
//    hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
//    hadc.Init.ScanConvMode          = ADC_SCAN_DISABLE;              /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
//    hadc.Init.NbrOfConversion       = 2;                            /* Parameter discarded because sequencer is disabled */
//    hadc.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
//    hadc.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
//   
//  	hadc.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode to have maximum conversion speed (no delay between conversions) */
//    hadc.Init.TrigType      = ADC_REGULAR_SOFTWARE_TRIGT;            /* The reference voltage uses an internal reference */
//   
//	  hadc.Init.Vref          = ADC_VREF_VCC;//ADC_VREF_INSIDE ADC_VREF_VCC
//    hadc.Init.AdcCkDiv = ADC_CLOCK_DIV32;
//    hadc.Init.AdcDriveType=EINBUF_DRIVE_ADC;

    hadc.Instance = LSADC;
    hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode          = ADC_SCAN_DISABLE;              /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
    hadc.Init.NbrOfConversion       = 2;                            /* Parameter discarded because sequencer is disabled */
    hadc.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
    hadc.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */

    hadc.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode to have maximum conversion speed (no delay between conversions) */
    hadc.Init.TrigType      = ADC_REGULAR_SOFTWARE_TRIGT;            /* Trig of conversion start done by which event */
    
	  hadc.Init.Vref          = ADC_VREF_VCC;//ADC_VREF_INSIDE ADC_VREF_VCC
    hadc.Init.AdcCkDiv = ADC_CLOCK_DIV32;
    hadc.Init.AdcDriveType=EINBUF_DRIVE_ADC;


    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
        ADC_Error_Handler();
    }
		ADC_ChannelConfTypeDef sConfig = {0};	
		sConfig.Channel      = ADC_CHANNEL_3;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        ADC_Error_Handler();
    }
		sConfig.Channel  = ADC_CHANNEL_6;
    sConfig.Rank         = ADC_REGULAR_RANK_2;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;//ADC_SAMPLETIME_15CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
       ADC_Error_Handler();
    }
		
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   if(recv_flag<2)
   {
       aRxBuffer[recv_flag++] = HAL_ADC_GetValue(hadc);
   }
}



void  User_ADC_Init(){
		lsadc_init();
    HAL_ADC_Start_IT(&hadc);
    recv_flag = 0;

}


uint16_t Get_ADC_value(){
		static uint16_t mV;
		static uint16_t temp_V;
		if(recv_flag >= 2) 
		{
            recv_flag = 0;
						mV=4*3300*aRxBuffer[1]/4095;
						temp_V=3300*aRxBuffer[0]/4095;
						
//						LOG_I("Vbat_vol=%d",mV);  //By default, 1/8 of the power supply is used to collect					
//						LOG_I("aRxBuffer[0]=%d",temp_V);
						
						HAL_ADC_Start_IT(&hadc);
						return  mV;
		}
		return  mV;
}



