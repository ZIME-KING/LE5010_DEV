#include "user_adc.h"
#include "user_main.h"

static uint16_t adc_value;
static ADC_HandleTypeDef hadc;
static volatile uint8_t recv_flag = 0;


////////////////////////////////////////// ADC ///////////////////////////////////////////
static void lsadc_init(void)
{  
		adc12b_in6_io_init();   
		hadc.Instance = LSADC;
    hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode          = ADC_SCAN_DISABLE;              /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
    hadc.Init.NbrOfConversion       = 1;                            /* Parameter discarded because sequencer is disabled */
    hadc.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
    hadc.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
    hadc.Init.ContinuousConvMode    = DISABLE;                        /* Continuous mode to have maximum conversion speed (no delay between conversions) */
    hadc.Init.TrigType      = ADC_INJECTED_SOFTWARE_TRIGT;            /* The reference voltage uses an internal reference */
    hadc.Init.Vref          = ADC_VREF_VCC;//ADC_VREF_INSIDE ADC_VREF_VCC
    hadc.Init.AdcCkDiv = ADC_CLOCK_DIV32;

    hadc.Init.AdcDriveType=EINBUF_DRIVE_ADC;

    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
        //Error_Handler();
    }
    ADC_InjectionConfTypeDef sConfigInjected = {0};
    sConfigInjected.InjectedChannel = ADC_CHANNEL_6;  //
    sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
    sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_15CYCLES;
    sConfigInjected.InjectedOffset = 0;
    sConfigInjected.InjectedNbrOfConversion = 1;
    sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
    sConfigInjected.AutoInjectedConv = DISABLE;

    if (HAL_ADCEx_InjectedConfigChannel(&hadc, &sConfigInjected) != HAL_OK)
    {
        /* Channel Configuration Error */
        //Error_Handler();
    }
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    recv_flag = 1;
    adc_value = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
}



void  User_ADC_Init(){
		lsadc_init();
    HAL_ADCEx_InjectedStart_IT(&hadc);
    recv_flag = 0;

}


uint16_t Get_ADC_value(){
		static uint16_t mV;
		if(recv_flag == 1) 
		{
            //DELAY_US(200*1000); //delay 200ms
            recv_flag = 0;
						mV=4*3300*adc_value/4095;
            
						//LOG_I("Vbat_vol: %d mv",mV);  //By default, 1/8 of the power supply is used to collect
						
						
						HAL_ADCEx_InjectedStart_IT(&hadc);
						return  mV;
		}
		return  mV;
}



