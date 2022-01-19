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
#include "sleep.h"
#include "lsadc.h"

#define UART_SVC_ADV_NAME "LS UART"
#define UART_SERVER_MAX_MTU  247
#define UART_SERVER_MTU_DFT  23
#define UART_SERVER_MAX_DATA_LEN (uart_server_mtu - 3)
#define UART_SVC_RX_MAX_LEN (UART_SERVER_MAX_MTU - 3)
#define UART_SVC_TX_MAX_LEN (UART_SERVER_MAX_MTU - 3)
#define UART_SVC_BUFFER_SIZE (1024)

#define UART_SERVER_TIMEOUT 50 // timer units: ms
//////////////////////////////////

char global_flag=0;
char global_count=0;

ADC_HandleTypeDef hadc;

//////////////////////////////
static const uint8_t ls_uart_svc_uuid_128[] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x01,0x00,0x40,0x6e};
static const uint8_t ls_uart_rx_char_uuid_128[] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x02,0x00,0x40,0x6e};
static const uint8_t ls_uart_tx_char_uuid_128[] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x03,0x00,0x40,0x6e};
static const uint8_t att_decl_char_array[] = {0x03,0x28};
static const uint8_t att_desc_client_char_cfg_array[] = {0x02,0x29};
enum uart_svc_att_db_handles
{
    UART_SVC_IDX_RX_CHAR,
    UART_SVC_IDX_RX_VAL,
    UART_SVC_IDX_TX_CHAR,
    UART_SVC_IDX_TX_VAL,
    UART_SVC_IDX_TX_NTF_CFG,
    UART_SVC_ATT_NUM
};

static const struct att_decl ls_uart_server_att_decl[UART_SVC_ATT_NUM] =
{
    [UART_SVC_IDX_RX_CHAR] = {
        .uuid = att_decl_char_array,
        .s.max_len = 0,
        .s.uuid_len = UUID_LEN_16BIT,
        .s.read_indication = 1,   
        .char_prop.rd_en = 1,
    },
    [UART_SVC_IDX_RX_VAL] = {
        .uuid = ls_uart_rx_char_uuid_128,
        .s.max_len = UART_SVC_RX_MAX_LEN,
        .s.uuid_len = UUID_LEN_128BIT,
        .s.read_indication = 1,
        .char_prop.wr_cmd = 1,
        .char_prop.wr_req = 1,
    },
    [UART_SVC_IDX_TX_CHAR] = {
        .uuid = att_decl_char_array,
        .s.max_len = 0,
        .s.uuid_len = UUID_LEN_16BIT,
        .s.read_indication = 1,
        .char_prop.rd_en = 1, 
    },
    [UART_SVC_IDX_TX_VAL] = {
        .uuid = ls_uart_tx_char_uuid_128,
        .s.max_len = UART_SVC_TX_MAX_LEN,
        .s.uuid_len = UUID_LEN_128BIT,
        .s.read_indication = 1,
        .char_prop.ntf_en = 1,
    },
    [UART_SVC_IDX_TX_NTF_CFG] = {
        .uuid = att_desc_client_char_cfg_array,
        .s.max_len = 0,
        .s.uuid_len = UUID_LEN_16BIT,
        .s.read_indication = 1,
        .char_prop.rd_en = 1,
        .char_prop.wr_req = 1,
    },
};
static const struct svc_decl ls_uart_server_svc =
{	
    .uuid = ls_uart_svc_uuid_128,
    .att = (struct att_decl*)ls_uart_server_att_decl,
    .nb_att = UART_SVC_ATT_NUM,
    .uuid_len = UUID_LEN_128BIT,
};
static struct gatt_svc_env ls_uart_server_svc_env;
static uint8_t connect_id = 0xff; 
static uint8_t uart_server_rx_byte;
static uint8_t uart_server_buf[UART_SVC_BUFFER_SIZE];
static uint8_t uart_server_tx_buf[UART_SVC_BUFFER_SIZE];
static uint16_t uart_server_rx_index = 0;

UART_HandleTypeDef UART_Server_Config; 

static bool uart_server_tx_busy;
static bool uart_server_ntf_done = true;
static uint16_t uart_server_mtu = UART_SERVER_MTU_DFT;
static struct builtin_timer *uart_server_timer_inst = NULL;
static bool update_adv_intv_flag = false;
static uint16_t cccd_config = 0;


enum adv_status
{
    ADV_IDLE,
    ADV_BUSY,
};
static uint8_t adv_obj_hdl;
static uint8_t advertising_data[28] = {0x05, 0x09, 'u', 'a', 'r', 't'};
static uint8_t scan_response_data[31];
static uint8_t adv_status = ADV_IDLE;


static void ls_uart_server_init(void);
static void ls_uart_server_timer_cb(void *param);
static void ls_uart_init(void);
static void ls_uart_server_read_req_ind(uint8_t att_idx, uint8_t con_idx);
static void ls_uart_server_write_req_ind(uint8_t att_idx, uint8_t con_idx, uint16_t length, uint8_t const *value);
static void ls_uart_server_send_notification(void);
static void start_adv(void);
static void ls_uart_server_data_length_update(uint8_t con_idx);

void stop_adv(void)
{
    if(adv_status == ADV_BUSY)
		{
        dev_manager_stop_adv(adv_obj_hdl);
				adv_status = ADV_IDLE;
		}
}

void start_adv(void)
{
    uint8_t adv_data_length = 0;

    LS_ASSERT(adv_obj_hdl != 0xff);
//    adv_data_length = ADV_DATA_PACK(advertising_data, 2, GAP_ADV_TYPE_SHORTENED_NAME, UART_SVC_ADV_NAME, strlen((const char *)UART_SVC_ADV_NAME),
//                                                         GAP_ADV_TYPE_MANU_SPECIFIC_DATA, sent_buf, sizeof(sent_buf));
	
	    adv_data_length = ADV_DATA_PACK(advertising_data, 1,GAP_ADV_TYPE_MANU_SPECIFIC_DATA, sent_buf, sizeof(sent_buf));
		
    if(adv_status != ADV_IDLE)
    {
        dev_manager_update_adv_data(adv_obj_hdl, advertising_data, adv_data_length, scan_response_data, 0);
    }
    else
    {
        dev_manager_start_adv(adv_obj_hdl, advertising_data, adv_data_length, scan_response_data, 0);
    }
    adv_status = ADV_BUSY;
}


#define USER_EVENT_PERIOD_0 5			 //按键扫描
#define USER_EVENT_PERIOD_1 50     //按键处理
#define USER_EVENT_PERIOD_2 200		 //更新广播

static void ls_user_event_timer_init(void);

static void ls_user_event_timer_cb_0(void *param);
static void ls_user_event_timer_cb_1(void *param);
static void ls_user_event_timer_cb_2(void *param);

static struct builtin_timer_0 *user_event_timer_inst_0 = NULL;
static struct builtin_timer_1 *user_event_timer_inst_1 = NULL;
static struct builtin_timer_2 *user_event_timer_inst_2 = NULL;

static void ls_user_event_timer_init(void)
{
	user_event_timer_inst_0 =builtin_timer_create(ls_user_event_timer_cb_0);
	builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);
	
	user_event_timer_inst_1 =builtin_timer_create(ls_user_event_timer_cb_1);
	builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);

	user_event_timer_inst_2 =builtin_timer_create(ls_user_event_timer_cb_2);
	builtin_timer_start(user_event_timer_inst_2, USER_EVENT_PERIOD_2, NULL);
}

//5ms   做按键扫描
static void ls_user_event_timer_cb_0(void *param)
{
/**
user_code	
*/	
	scan_touch_key();
	scan_key();
	globle_count++;
	no_act_count++;
	
	if(led_open_flag){
		led_open_count++;
	}	
	//LOG_I("no_act_count_5ms:%d",no_act_count);		
	if(adv_status == ADV_IDLE){
				if((no_act_count>100 && led_open_flag==0) ||
				(no_act_count>100 && led_open_count>=12000)){
						
				LOG_I("no_act_count%d",no_act_count);		
				LOG_I("ls_sleep_enter_LP3");	
					ls_sleep_enter_LP3();
		}
	}
	//////////////////////////////////////////
	builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);
}

//20ms
static void ls_user_event_timer_cb_1(void *param)
{
/**
user_code	
*/
	if(adv_status == ADV_BUSY){
		io_toggle_pin(PB09);
	
	}
	 if(led_open_flag){
			io_write_pin(PB08,1);	
	 }
	 else{
			io_write_pin(PB08,0);	
		  led_open_count=0;
	 }
	
	
	if(touch_key_staus==SHORT && touch_key_busy){
		LOG_I("touch_key_staus=%x",touch_key_staus);
		led_open_count=0;
		//sprintf((char*)buf,"KEY=%d",touch_key_staus);
		//HAL_UART_Transmit_IT(&UART_Server_Config, (uint8_t*)buf, sizeof(buf));
		touch_key_busy=0;
	}
	if(touch_key_staus==DOUBLE && touch_key_busy){
		LOG_I("touch_key_staus=%x",touch_key_staus);
		led_open_flag=!led_open_flag;
		led_open_count=0;
		//sprintf((char*)buf,"KEY=%d",touch_key_staus);
		//HAL_UART_Transmit_IT(&UART_Server_Config, (uint8_t*)buf, sizeof(buf));
		touch_key_busy=0;
	}
	if(touch_key_staus==LONG && touch_key_busy){
		LOG_I("touch_key_staus=%x",touch_key_staus);
		//sprintf((char*)buf,"KEY=%d",touch_key_staus);
		//HAL_UART_Transmit_IT(&UART_Server_Config, (uint8_t*)buf, sizeof(buf));
		touch_key_busy=0;
	}
	builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);
}
//200ms
static void ls_user_event_timer_cb_2(void *param)
{
	static uint32_t temp_time;
/**
user_code	
*/
	
	//memcpy ( &sent_buf[0], &id_num[0], sizeof(id_num) );
	
	//LOG_HEX(sent_buf,sizeof(sent_buf));
		
	if(key_busy && adv_status == ADV_IDLE){
		serial_num++; 
		sent_buf[8]=(serial_num<<4) + (uint8_t)adc_value_num;
		if(key_status==1){
			sent_buf[9]=0xAF;
		}
		else if(key_status==2 || key_status==4){
			sent_buf[9]=0xBF;
		}
		LOG_I("key_staus=%x",key_status);
		LOG_I("adc_value_num=%d",adc_value_num);
		uint16_t crc_val=crc16_check(sent_buf,sizeof(sent_buf)-2);
//		sent_buf[10]=(uint8_t)((crc_val>>8)&0x00ff);
//		sent_buf[11]=(uint8_t)(crc_val&0x00ff);
		sent_buf[10]=0xB7;
		sent_buf[11]=0x8E;
		
		LOG_HEX(sent_buf,sizeof(sent_buf));
		
		start_adv();   //更新adv 和开始adv，包含判断不用停止adv
		key_busy=0;
		temp_time=globle_count;
	}
	 // 5ms globle_count++ 发送1s 关闭 极端下 800ms到1200ms发送时间
	 if(globle_count-temp_time>200){
			stop_adv();
		 	io_write_pin(PB09,0);	
		}
	 
	Get_vbat_val();   //电池电量更新
	
	builtin_timer_start(user_event_timer_inst_2, USER_EVENT_PERIOD_2, NULL);
}

static void ls_uart_server_init(void)
{
    uart_server_timer_inst = builtin_timer_create(ls_uart_server_timer_cb);
    builtin_timer_start(uart_server_timer_inst, UART_SERVER_TIMEOUT, NULL);
}

static void ls_uart_server_update_adv_interval(uint8_t input_intv)
{
    LOG_I("input_char: %d",input_intv);
    uint32_t new_intv = (input_intv - '0')*160;
    dev_manager_update_adv_interval(adv_obj_hdl, new_intv, new_intv);
    dev_manager_stop_adv(adv_obj_hdl);
    update_adv_intv_flag = true;
}

static void ls_uart_server_timer_cb(void *param)
{
    if(connect_id != 0xff)
    {
        uint32_t cpu_stat = enter_critical();
        // LOG_I("uart timer out, length=%d", uart_server_rx_index);
        ls_uart_server_send_notification();
        exit_critical(cpu_stat);
    }
    uint8_t input_char = (uint8_t)SEGGER_RTT_GetKey();
    if(connect_id == 0xff && input_char != 0xff && input_char > '0' && input_char <= '9')
    {
        ls_uart_server_update_adv_interval(input_char);
    }
    if(uart_server_timer_inst)
    {
        builtin_timer_start(uart_server_timer_inst, UART_SERVER_TIMEOUT, NULL); 
    }
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_server_tx_busy = false;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(uart_server_rx_index < UART_SVC_BUFFER_SIZE)
    {
        uart_server_buf[uart_server_rx_index++] = uart_server_rx_byte;
    }
    else
    {   
        LOG_I("uart server rx buffer overflow!");
    }
    HAL_UART_Receive_IT(&UART_Server_Config, &uart_server_rx_byte, 1);
}
static void ls_uart_init(void)
{
    uart1_io_init(PA13, PA14);
    UART_Server_Config.UARTX = UART1;
    UART_Server_Config.Init.BaudRate = UART_BAUDRATE_115200;
    UART_Server_Config.Init.MSBEN = 0;
    UART_Server_Config.Init.Parity = UART_NOPARITY;
    UART_Server_Config.Init.StopBits = UART_STOPBITS1;
    UART_Server_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Server_Config);
}
static void ls_uart_server_read_req_ind(uint8_t att_idx, uint8_t con_idx)
{
    uint16_t handle = 0;
    if(att_idx == UART_SVC_IDX_TX_NTF_CFG)
    {
        handle = gatt_manager_get_svc_att_handle(&ls_uart_server_svc_env, att_idx);
        gatt_manager_server_read_req_reply(con_idx, handle, 0, (void*)&cccd_config, 2);
    }
}
static void ls_uart_server_write_req_ind(uint8_t att_idx, uint8_t con_idx, uint16_t length, uint8_t const *value)
{
    if(att_idx == UART_SVC_IDX_RX_VAL)
    { 
        if(uart_server_tx_busy)
        {
            LOG_I("Uart tx busy, data discard!");
        }
        else
        {
            uart_server_tx_busy = true;
            LS_ASSERT(length <= UART_SVC_BUFFER_SIZE);
            memcpy(uart_server_tx_buf, (uint8_t*)value, length);
            HAL_UART_Transmit_IT(&UART_Server_Config, (uint8_t*)uart_server_tx_buf, length);
        } 
    }
    else if (att_idx == UART_SVC_IDX_TX_NTF_CFG)
    {
        LS_ASSERT(length == 2);
        memcpy(&cccd_config, value, length);
    }
    
}
static void ls_uart_server_send_notification(void)
{
    if(uart_server_rx_index > 0 && uart_server_ntf_done)
    {
        uart_server_ntf_done = false;
        uint16_t handle = gatt_manager_get_svc_att_handle(&ls_uart_server_svc_env, UART_SVC_IDX_TX_VAL);
        uint16_t tx_len = uart_server_rx_index > co_min(UART_SERVER_MAX_DATA_LEN, UART_SVC_TX_MAX_LEN) ? co_min(UART_SERVER_MAX_DATA_LEN, UART_SVC_TX_MAX_LEN) : uart_server_rx_index;
        uart_server_rx_index -= tx_len;
        gatt_manager_server_send_notification(connect_id, handle, uart_server_buf, tx_len, NULL);
        memcpy((void*)&uart_server_buf[0], (void*)&uart_server_buf[tx_len], uart_server_rx_index);
    }    
}

static void ls_uart_server_data_length_update(uint8_t con_idx)
{
    struct gap_set_pkt_size dlu_param = 
    {
        .pkt_size = 251,
    };
    gap_manager_set_pkt_size(con_idx, &dlu_param);
}

static void gap_manager_callback(enum gap_evt_type type,union gap_evt_u *evt,uint8_t con_idx)
{
    switch(type)
    {
    case CONNECTED:
        connect_id = con_idx;
        LOG_I("connected!");
    break;
    case DISCONNECTED:
        connect_id = 0xff;
        uart_server_mtu = UART_SERVER_MTU_DFT;
        LOG_I("disconnected!");
        start_adv();
    break;
    case CONN_PARAM_REQ:
        //LOG_I
    break;
    case CONN_PARAM_UPDATED:

    break;
    default:

    break;
    }
}

static void gatt_manager_callback(enum gatt_evt_type type,union gatt_evt_u *evt,uint8_t con_idx)
{
    switch (type)
    {
    case SERVER_READ_REQ:
        LOG_I("read req");
        ls_uart_server_read_req_ind(evt->server_read_req.att_idx, con_idx);
    break;
    case SERVER_WRITE_REQ:
        LOG_I("write req");
        ls_uart_server_write_req_ind(evt->server_write_req.att_idx, con_idx, evt->server_write_req.length, evt->server_write_req.value);
    break;
    case SERVER_NOTIFICATION_DONE:
        uart_server_ntf_done = true;
        LOG_I("ntf done");
    break;
    case MTU_CHANGED_INDICATION:
        uart_server_mtu = evt->mtu_changed_ind.mtu;
        LOG_I("mtu: %d", uart_server_mtu);
        ls_uart_server_data_length_update(con_idx);
    break;
    default:
        LOG_I("Event not handled!");
        break;
    }
}

static void create_adv_obj()
{
    struct legacy_adv_obj_param adv_param = {
        .adv_intv_min = 0x20,
        .adv_intv_max = 0x20,
        .own_addr_type = PUBLIC_OR_RANDOM_STATIC_ADDR,
        .filter_policy = 0,
        .ch_map = 0x7,
        .disc_mode = ADV_MODE_GEN_DISC,
        .prop = {
						.connectable = 1,   //0不可连接 ，1可连接
            .scannable = 1,
            .directed = 0,
            .high_duty_cycle = 0,
        },
    };
    dev_manager_create_legacy_adv_object(&adv_param);
}

/*
static void create_highduty_adv_obj(void)
{
    struct legacy_adv_obj_param adv_param = {
        .peer_addr = (struct dev_addr*)peer_addr_1,
        .peer_addr_type = RANDOM_ADDR,
        .adv_intv_min = 0x20,
        .adv_intv_max = 0x20,
        .own_addr_type = PUBLIC_OR_RANDOM_STATIC_ADDR,
        .filter_policy = 0,
        .ch_map = 0x7,
        .disc_mode = ADV_MODE_NON_DISC,
        .prop = {
            .connectable = 1,
            .scannable = 0,
            .directed = 1,
            .high_duty_cycle = 1,
        },
    };
    dev_manager_create_legacy_adv_object(&adv_param);
}*/

static void dev_manager_callback(enum dev_evt_type type,union dev_evt_u *evt)
{
    switch(type)
    {
    case STACK_INIT:
    {
        struct ble_stack_cfg cfg = {
            .private_addr = false,
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
        dev_manager_add_service((struct svc_decl *)&ls_uart_server_svc);
        ls_uart_init(); 
        HAL_UART_Receive_IT(&UART_Server_Config, &uart_server_rx_byte, 1); 
        ls_uart_server_init();   
			
				ls_user_event_timer_init();// 定时器初始化
    }
    break;
    case SERVICE_ADDED:
        gatt_manager_svc_register(evt->service_added.start_hdl, UART_SVC_ATT_NUM, &ls_uart_server_svc_env);
        create_adv_obj();
    break;
    case ADV_OBJ_CREATED:
        LS_ASSERT(evt->obj_created.status == 0);
        adv_obj_hdl = evt->obj_created.handle;

				//start_adv();
		break;
    case ADV_STOPPED:
        if (update_adv_intv_flag)
        {
            update_adv_intv_flag = false;
            //start_adv();
        }
    break;
    case SCAN_STOPPED:
    
    break;
    default:

    break;
    }
    
}
const uint8_t dev_addr[6]={0x75,0x67,0x79,0x76,0x75,0x80};  //设置MAC地址固定

int main()
{
    sys_init_app();
    ble_init();
		user_init(); 
		dev_manager_set_mac_addr((uint8_t*)dev_addr);
	
    dev_manager_init(dev_manager_callback);
    gap_manager_init(gap_manager_callback);
    gatt_manager_init(gatt_manager_callback);
    ble_loop();
}




