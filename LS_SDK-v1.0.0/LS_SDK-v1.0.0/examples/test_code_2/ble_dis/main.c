#define LOG_TAG "MAIN"
#include "ls_ble.h"
#include "platform.h"
#include "prf_diss.h"
#include "log.h"
#include "ls_dbg.h"
#include <string.h>
#include "builtin_timer.h"
#include "io_config.h"
#include "reg_lsgpio.h"


#define APP_DIS_DEV_NAME                ("LS Dis Server")
#define APP_DIS_DEV_NAME_LEN            (sizeof(APP_DIS_DEV_NAME))

/// Manufacturer Name Value
#define APP_DIS_MANUFACTURER_NAME       ("LinkedSemi")
#define APP_DIS_MANUFACTURER_NAME_LEN   (sizeof(APP_DIS_MANUFACTURER_NAME))

/// ADV Data
#define APP_DIS_ADV_DATA                ("LS Dis Server")

/// Model Number String Value
#define APP_DIS_MODEL_NB_STR            ("LS-BLE-1.0")
#define APP_DIS_MODEL_NB_STR_LEN        (10)

/// Serial Number
#define APP_DIS_SERIAL_NB_STR           ("1.0.0.0-LE")
#define APP_DIS_SERIAL_NB_STR_LEN       (10)

/// Firmware Revision
#define APP_DIS_FIRM_REV_STR            ("1.0.0")
#define APP_DIS_FIRM_REV_STR_LEN        (5)

/// System ID Value - LSB -> MSB
#define APP_DIS_SYSTEM_ID               ("\x12\x34\x56\xFF\xFE\x9A\xBC\xDE")
#define APP_DIS_SYSTEM_ID_LEN           (8)

/// Hardware Revision String
#define APP_DIS_HARD_REV_STR           ("1.0.0")
#define APP_DIS_HARD_REV_STR_LEN       (5)

/// Software Revision String
#define APP_DIS_SW_REV_STR              ("1.0.0")
#define APP_DIS_SW_REV_STR_LEN          (5)

/// IEEE
#define APP_DIS_IEEE                    ("\xFF\xEE\xDD\xCC\xBB\xAA")
#define APP_DIS_IEEE_LEN                (6)

/// PNP ID Value
#define APP_DIS_PNP_ID                  ("\x02\x5E\x04\x40\x00\x00\x03")
#define APP_DIS_PNP_ID_LEN              (7)





 void ls_user_event_timer_cb_0(void *param);
 void ls_user_event_timer_cb_1(void *param);

struct builtin_timer *user_event_timer_inst_0 = NULL;
struct builtin_timer *user_event_timer_inst_1 = NULL;

#define USER_EVENT_PERIOD_0 2000
#define USER_EVENT_PERIOD_1 100


void ls_app_timer_init(void)
{
    user_event_timer_inst_0 =builtin_timer_create(ls_user_event_timer_cb_0);
    builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);
		
		user_event_timer_inst_1 =builtin_timer_create(ls_user_event_timer_cb_1);
    builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);
}

//loop_task_normal_power
//loop_task_lower_power

//2000ms
static void ls_user_event_timer_cb_0(void *param) {
	builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);
	static int count=0;
	count++;
	
	
	DELAY_US(100*1000);



LOG_I("count=%d",count);
	 //30s关闭
	if(count==5){
			 //	builtin_timer_stop(user_event_timer_inst_1);       //关闭定时器0
				builtin_timer_delete(user_event_timer_inst_1);
	}
	if(count==10){
				user_event_timer_inst_1 =builtin_timer_create(ls_user_event_timer_cb_1);
    builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);
	}
	
	
}
static void ls_user_event_timer_cb_1(void *param) {
	builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);
	static int count;
	count++;
	
//	if(count%2==0){
//		io_cfg_output(PA01);				//  
//		io_write_pin(PA01,1);
//	}
//	else if(count%2==1){
//		io_cfg_output(PA01);				//  
//		io_write_pin(PA01,0);	
//	}
}










static uint8_t adv_obj_hdl;
static uint8_t advertising_data[28] = {11,0x08,'L','i','n','k','e','d','s','e','m','i'};;
static uint8_t scan_response_data[31];

static void dis_server_get_dev_name(struct gap_dev_info_dev_name *dev_name_ptr, uint8_t con_idx);
static void dis_server_get_appearance(struct gap_dev_info_appearance *dev_appearance_ptr, uint8_t con_idx);
static void dis_server_get_slv_pref_param(struct gap_dev_info_slave_pref_param *dev_slv_pref_param_ptr, uint8_t con_idx);

static void gap_manager_callback(enum gap_evt_type type,union gap_evt_u *evt,uint8_t con_idx)
{
    switch(type)
    {
    case CONNECTED:

    break;
    case DISCONNECTED:
        dev_manager_start_adv(adv_obj_hdl,advertising_data,sizeof(advertising_data),scan_response_data,sizeof(scan_response_data));
    break;
    case CONN_PARAM_REQ:
        //LOG_I
    break;
    case CONN_PARAM_UPDATED:

    break;
    case GET_DEV_INFO_DEV_NAME:
        dis_server_get_dev_name((struct gap_dev_info_dev_name*)evt, con_idx);
    break;
    case GET_DEV_INFO_APPEARANCE:
        dis_server_get_appearance((struct gap_dev_info_appearance*)evt, con_idx);
    break;
    case GET_DEV_INFO_SLV_PRE_PARAM:
        dis_server_get_slv_pref_param((struct gap_dev_info_slave_pref_param*)evt, con_idx);
    break;
    default:

    break;
    }
}

static void gatt_manager_callback(enum gatt_evt_type type,union gatt_evt_u *evt,uint8_t con_idx)
{
    
}
static void dis_server_get_dev_name(struct gap_dev_info_dev_name *dev_name_ptr, uint8_t con_idx)
{
    LS_ASSERT(dev_name_ptr);
    dev_name_ptr->value = (uint8_t*)APP_DIS_DEV_NAME;
    dev_name_ptr->length = APP_DIS_DEV_NAME_LEN;
}
static void dis_server_get_appearance(struct gap_dev_info_appearance *dev_appearance_ptr, uint8_t con_idx)
{
    LS_ASSERT(dev_appearance_ptr);
    dev_appearance_ptr->appearance = 0;
}
static void dis_server_get_slv_pref_param(struct gap_dev_info_slave_pref_param *dev_slv_pref_param_ptr, uint8_t con_idx)
{
    LS_ASSERT(dev_slv_pref_param_ptr);
    dev_slv_pref_param_ptr->con_intv_min  = 8;
    dev_slv_pref_param_ptr->con_intv_max  = 20;
    dev_slv_pref_param_ptr->slave_latency =  0;
    dev_slv_pref_param_ptr->conn_timeout  = 200;
}

static void dis_server_get_value(uint8_t value)
{
    // Initialize length
    uint8_t len = 0;
    // Pointer to the data
    uint8_t *data = NULL;
    switch (value)
    {
    case DIS_SERVER_MANUFACTURER_NAME_CHAR:
        len = APP_DIS_MANUFACTURER_NAME_LEN;
        data = (uint8_t *)APP_DIS_MANUFACTURER_NAME;
        break;
    case DIS_SERVER_MODEL_NB_STR_CHAR:
        len = APP_DIS_MODEL_NB_STR_LEN;
        data = (uint8_t *)APP_DIS_MODEL_NB_STR;
        break;
    case DIS_SERVER_SERIAL_NB_STR_CHAR:
        len = APP_DIS_SERIAL_NB_STR_LEN;
        data = (uint8_t *)APP_DIS_SERIAL_NB_STR;
        break;
    case DIS_SERVER_HARD_REV_STR_CHAR:
        len = APP_DIS_HARD_REV_STR_LEN;
        data = (uint8_t *)APP_DIS_HARD_REV_STR;
        break;
    case DIS_SERVER_FIRM_REV_STR_CHAR:
        len = APP_DIS_FIRM_REV_STR_LEN;
        data = (uint8_t *)APP_DIS_FIRM_REV_STR;
        break;
    case DIS_SERVER_SW_REV_STR_CHAR:
        len = APP_DIS_SW_REV_STR_LEN;
        data = (uint8_t *)APP_DIS_SW_REV_STR;
        break;
    case DIS_SERVER_SYSTEM_ID_CHAR:
        len = APP_DIS_SYSTEM_ID_LEN;
        data = (uint8_t *)APP_DIS_SYSTEM_ID;
        break;
    case DIS_SERVER_IEEE_CHAR:
        len = APP_DIS_IEEE_LEN;
        data = (uint8_t *)APP_DIS_IEEE;
        break;
    case DIS_SERVER_PNP_ID_CHAR:
        len = APP_DIS_PNP_ID_LEN;
        data = (uint8_t *)APP_DIS_PNP_ID;
        break;
    default:
        LOG_I("Unsupported dis value!");
        break;
    }
    if(data)
    {
        prf_diss_value_confirm(value, len, data);
    }   
}

static void prf_dis_server_callback(enum diss_evt_type type,union diss_evt_u *evt)
{
    switch(type)
    {
    case DIS_SERVER_SET_VALUE_RSP:

    break;
    case DIS_SERVER_GET_VALUE_IND:
        dis_server_get_value(evt->get_value_ind.value);
    break;
    default:
        LS_ASSERT(0);
    break;
    }
}

static void create_adv_obj()
{
    struct legacy_adv_obj_param adv_param = {
        .adv_intv_min = 1600,
        .adv_intv_max = 1600,
        .own_addr_type = PUBLIC_OR_RANDOM_STATIC_ADDR,
        .filter_policy = 0,
        .ch_map = 0x7,
        .disc_mode = ADV_MODE_GEN_DISC,
        .prop = {
            .connectable = 1,
            .scannable = 1,
            .directed = 0,
            .high_duty_cycle = 0,
        },
    };
    dev_manager_create_legacy_adv_object(&adv_param);
}

static void prf_added_handler(struct profile_added_evt *evt)
{
    LOG_I("profile:%d, start handle:0x%x\n",evt->id,evt->start_hdl);
    switch(evt->id)
    {
    case PRF_DIS_SERVER:
        prf_dis_server_callback_init(prf_dis_server_callback);
        create_adv_obj();
    break;
    default:

    break;
    }
}


void User_io_Init() {
    LSGPIOA->MODE = 0;
    LSGPIOA->IE = 0;
    LSGPIOA->OE = 0;
    LSGPIOA->OT = 0;
    LSGPIOA->PUPD = 0xAAAAAA88;  //pa00,PA02不接上下拉，其余全部下拉
    LSGPIOB->MODE &= 0x3c00;  //3C00
    LSGPIOB->IE = 0;
    LSGPIOB->OE = 0;
    LSGPIOB->OT = 0;
    // LSGPIOB->PUPD = 0x2800;
    LSGPIOB->PUPD =  0x2AA96AAA;	//	PB15 浮空	 AAA9 6AAA
		
		
//		LSGPIOA->MODE = 0;
//    LSGPIOA->IE = 0;
//    LSGPIOA->OE = 0;
//		
//		//LSGPIOA->OT = 0X20;
//    
//		LSGPIOA->PUPD = 0;
//    LSGPIOB->MODE &= 0x3c00;  //3C00
//    LSGPIOB->IE = 0;
//    LSGPIOB->OE = 0;
//    LSGPIOB->PUPD = 0x2800;
//		
		
   //io_set_pin(PC01);
//    io_pull_write(PC01,IO_PULL_DISABLE);
//    io_cfg_input(PC01);
}


//void io_init(void)
//{
//    RCC->AHBEN |= RCC_GPIOA_MASK | RCC_GPIOB_MASK | RCC_GPIOC_MASK;
//    LSGPIOA->MODE = 0;
//    LSGPIOA->IE = 0;
//    LSGPIOA->OE = 0;
//		
//		//LSGPIOA->OT = 0X20;
//    
//		LSGPIOA->PUPD = 0;
//    LSGPIOB->MODE &= 0x3c00;  //3C00
//    LSGPIOB->IE = 0;
//    LSGPIOB->OE = 0;
//    LSGPIOB->PUPD = 0x2800;
//    arm_cm_set_int_isr(EXTI_IRQn,EXTI_Handler);
//    __NVIC_EnableIRQ(EXTI_IRQn);
//}

static void dev_manager_callback(enum dev_evt_type type,union dev_evt_u *evt)
{
    switch(type)
    {
    case STACK_INIT:
    {
        struct ble_stack_cfg cfg = {
            .private_addr = true,
            .controller_privacy = false,
        };
        dev_manager_stack_init(&cfg);
    }
    break;
    case STACK_READY:
    {
        uint8_t addr[6];
        bool type;
        dev_manager_get_identity_bdaddr(addr,&type);
        LOG_I("type:%d,addr:",type);
        LOG_HEX(addr,sizeof(addr));
        dev_manager_prf_dis_server_add(NO_SEC,0xffff);
				
				ls_app_timer_init();
				
				User_io_Init();
				
				
				
				
    }break;
    case SERVICE_ADDED:

    break;
    case PROFILE_ADDED:
        prf_added_handler(&evt->profile_added);
    break;
    case ADV_OBJ_CREATED:
        LS_ASSERT(evt->obj_created.status == 0);
        adv_obj_hdl = evt->obj_created.handle;
        dev_manager_start_adv(adv_obj_hdl,advertising_data,sizeof(advertising_data),scan_response_data,sizeof(scan_response_data));
    break;
    case ADV_STOPPED:
    
    break;
    case SCAN_STOPPED:
    
    break;
    default:

    break;
    }
    
}

extern void dcdc_off(void);
int main()
{
    sys_init_app();
		dcdc_off();
    ble_init();
    dev_manager_init(dev_manager_callback);
    gap_manager_init(gap_manager_callback);
    gatt_manager_init(gatt_manager_callback);
    ble_loop();
}
