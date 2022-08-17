#define LOG_TAG "MAIN"
#include "ls_ble.h"
#include "platform.h"
#include "prf_diss.h"
#include "log.h"
#include "ls_dbg.h"
#include "cpu.h"
#include "lsuart.h"
#include "builtin_timer.h"
#include <string.h>
#include "co_math.h"
#include "io_config.h"
#include "SEGGER_RTT.h"
#include "user_function.h"
#include "stdio.h"
#include "io_config.h"
#include "tinyfs.h"
#include "sleep.h"
#include "lsadc.h"

static void set_id_num(void);
static void lsadc_init(void);

uint16_t adc_value;
volatile uint8_t recv_flag = 0;

uint32_t globle_count=0;

uint16_t adc_value_num;
uint8_t key_status_num;
uint8_t serial_num;
uint8_t id_num[8]={0x0B,0xA4,0x01,0x00,0x7C,0xA0,0x1F,0x14};
uint8_t sent_buf[12];

uint32_t no_act_count=0;
uint32_t led_open_count=0;
uint8_t led_open_flag=0;

void gpio_init(void)
{
    io_cfg_output(PA01);   //PB09 config output
    io_write_pin(PA01,0);  //PB09 write low power
    io_cfg_output(PB08);   //PB09 config output
    io_write_pin(PB08,0);  //PB09 write low power
	  io_cfg_output(PB09);   //PB09 config output
    io_write_pin(PB09,0);  //PB09 write low power
	
	  io_cfg_input(PA07);
		io_cfg_input(PA00);
	  io_pull_write(PA07, IO_PULL_UP);    //PB11 config pullup
	  io_pull_write(PA00, IO_PULL_UP);    //PB11 config pullup
}

const uint16_t adv_int_arr[7] = {80, 160, 320, 800, 1600, 3200 ,40};
//广播间隔50MS  100ms   200ms 500ms  1000ms   2000ms 25ms

static uint8_t adv_obj_hdl;
void update_adv_intv(uint32_t new_adv_intv)
{
    LOG_I("adv_intv:%d",new_adv_intv);
    dev_manager_update_adv_interval(adv_obj_hdl,new_adv_intv,new_adv_intv);
//    if(adv_status)
//    {
//        dev_manager_stop_adv(adv_obj_hdl);
//        update_adv_intv_flag = true;
//    }
}
  

void user_init(void){
		gpio_init();
		set_id_num();
		lsadc_init();
	  HAL_ADCEx_InjectedStart_IT(&hadc);	
		Get_vbat_val();
		while(recv_flag==0);
		Get_vbat_val();
		LOG_I("Vbat_vol: %d mv",(3*1400*adc_value/4095));  //By default, 1/8 of the power supply is used to collect
		update_adv_intv(adv_int_arr[6]);
} 
/////////////////////////////////////adc_value_num////////////////////////////////////////////////////
void Get_vbat_val(){
		if(recv_flag == 1){
		        recv_flag = 0;
						adc_value_num=(3*1400*adc_value/4095);
						if(adc_value_num<2700) adc_value_num=2700;
						adc_value_num=(adc_value_num-2700)*100/(4200-2700);
						adc_value_num=(adc_value_num)*15*0.01;
						HAL_ADCEx_InjectedStart_IT(&hadc); 
		}
}
 
///////////////////////////////////////// ADC ///////////////////////////////////////////

static void lsadc_init(void)
{
//    io_clr_pin(PA00);
//    io_cfg_output(PA00);
		adc12b_in1_io_init();
	
    hadc.Instance = LSADC;
    hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode          = ADC_SCAN_DISABLE;              /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
    hadc.Init.NbrOfConversion       = 1;                            /* Parameter discarded because sequencer is disabled */
    hadc.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
    hadc.Init.NbrOfDiscConversion   = 1;                             /* Parameter discarded because sequencer is disabled */
    hadc.Init.ContinuousConvMode    = DISABLE;                        /* Continuous mode to have maximum conversion speed (no delay between conversions) */
    hadc.Init.TrigType      = ADC_INJECTED_SOFTWARE_TRIGT;            /* The reference voltage uses an internal reference */
    hadc.Init.Vref          = ADC_VREF_INSIDE;
    hadc.Init.AdcCkDiv = ADC_CLOCK_DIV2;

    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
        //Error_Handler();
    }
    ADC_InjectionConfTypeDef sConfigInjected = {0};
    sConfigInjected.InjectedChannel = ADC_CHANNEL_1;
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



///////////////////////////////sleep///////////////////////////////////////////////

static void exitpa07_iowkup_init(void)
{	
		io_cfg_input(PA07);                       
    io_pull_write(PA07, IO_PULL_UP);         
    io_exti_config(PA07,INT_EDGE_FALLING);   
    io_exti_enable(PA07,true);               

		io_cfg_input(PA00);                       
    io_pull_write(PA00, IO_PULL_UP);         
    io_exti_config(PA00,INT_EDGE_FALLING);   
    io_exti_enable(PA00,true);       
}

 void ls_sleep_enter_LP3(void)
{
  struct deep_sleep_wakeup  wakeup;
	gpio_init();
	exitpa07_iowkup_init();
	
	memset (&wakeup,0,sizeof(wakeup));
	wakeup.pa07 = 1;
	wakeup.pa07_rising_edge = 0;
	wakeup.pa00 = 1;
	wakeup.pa00_rising_edge = 0;
	
	enter_deep_sleep_mode_lvl2_lvl3(&wakeup);//调用睡眠函数
}


////////////////////////////// Set_ID_NUM ///////////////////////////////////////////////
#define ID_VAL_0 0xA0
#define ID_VAL_1 0xA1
#define RECORD_KEY1 1
//#define ID_SET
tinyfs_dir_t ID_dir;


void set_id_num(){
#ifdef ID_SET
static uint32_t count;
uint8_t tmp[8];
	tinyfs_mkdir(&ID_dir,ROOT_DIR,5);  //创建目录
	uint16_t length = 8; 
  tinyfs_read(ID_dir,RECORD_KEY1,tmp,&length);//读到tmp中
	LOG_I("read_id");
	LOG_HEX(tmp,sizeof(tmp));
	if(tmp[0]!=ID_VAL_0 && tmp[1]!=ID_VAL_1 ){
		while(io_read_pin(PA07)==1){
			count++;	
			for(char i=0;i<8;i++){
				srand((unsigned)count+i);
				id_num[i]= rand()%0xff;
			}
		}
		id_num[0]=ID_VAL_0;
		id_num[1]=ID_VAL_1;
		tinyfs_write(ID_dir,RECORD_KEY1,id_num,sizeof(id_num));	
		tinyfs_write_through();
		LOG_I("SET_OK");
		LOG_HEX(id_num,sizeof(id_num));
	}
	else{
		memcpy ( &id_num[0], &tmp[0], sizeof(tmp) );
		LOG_I("NO_wirt_OK");
	}
	#endif
	memcpy ( &sent_buf[0], &id_num[0], sizeof(id_num) );
}
//////////////////////// CRC ////////////////////////////////////////////
static const uint16_t crc16_table[] = {
	0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF, 0x8C48, 0x9DC1,
	0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7, 0x1081, 0x0108, 0x3393, 0x221A,
	0x56A5, 0x472C, 0x75B7, 0x643E, 0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64,
	0xF9FF, 0xE876, 0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
	0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5, 0x3183, 0x200A,
	0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C, 0xBDCB, 0xAC42, 0x9ED9, 0x8F50,
	0xFBEF, 0xEA66, 0xD8FD, 0xC974, 0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9,
	0x2732, 0x36BB, 0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
	0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A, 0xDECD, 0xCF44,
	0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72, 0x6306, 0x728F, 0x4014, 0x519D,
	0x2522, 0x34AB, 0x0630, 0x17B9, 0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3,
	0x8A78, 0x9BF1, 0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
	0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70, 0x8408, 0x9581,
	0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7, 0x0840, 0x19C9, 0x2B52, 0x3ADB,
	0x4E64, 0x5FED, 0x6D76, 0x7CFF, 0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324,
	0xF1BF, 0xE036, 0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
	0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5, 0x2942, 0x38CB,
	0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD, 0xB58B, 0xA402, 0x9699, 0x8710,
	0xF3AF, 0xE226, 0xD0BD, 0xC134, 0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E,
	0x5CF5, 0x4D7C, 0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
	0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB, 0xD68D, 0xC704,
	0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232, 0x5AC5, 0x4B4C, 0x79D7, 0x685E,
	0x1CE1, 0x0D68, 0x3FF3, 0x2E7A, 0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3,
	0x8238, 0x93B1, 0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
	0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330, 0x7BC7, 0x6A4E,
	0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

static uint16_t crc16_init(void)
{
	return 0x8075;
}

static uint16_t crc16_finish(uint16_t crc)
{
	return crc;
}

static uint16_t crc16_update_byte(uint16_t crc, uint8_t value)
{
	return crc16_table[(crc ^ value) & 0xFF] ^ (crc >> 8);
}
 
static uint16_t crc16_update_bytes(uint16_t crc, const uint8_t *buff, uint16_t length)
{
	const uint8_t *buff_end = buff + length;

	while (buff < buff_end) {
		crc = crc16_update_byte(crc, *buff++);
	}

	return crc;
}

uint16_t crc16_check(const uint8_t *buff, uint16_t size)
{
	uint16_t crc = crc16_init();
	crc = crc16_update_bytes(crc, buff, size);
	return crc16_finish(crc);
}


