#define LOG_TAG "MAIN"

#include "user_main.h"

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


#define SLAVE_SERVER_ROLE 1
#define MASTER_CLIENT_ROLE 0

//#define UART_SVC_ADV_NAME "LS Single Role"
#define UART_SERVER_MAX_MTU  517
#define UART_SERVER_MTU_DFT  23
#define UART_SVC_RX_MAX_LEN (UART_SERVER_MAX_MTU - 3)
#define UART_SVC_TX_MAX_LEN (UART_SERVER_MAX_MTU - 3)
#define UART_SERVER_MAX_DATA_LEN (uart_server_mtu - 3)
#define UART_CLIENT_MAX_DATA_LEN (uart_client_mtu - 3)

#define UART_SERVER_MASTER_NUM 1
#define UART_CLIENT_NUM (SDK_MAX_CONN_NUM - UART_SERVER_MASTER_NUM)
#define UART_SERVER_TIMEOUT 50 // timer units: ms

#define CON_IDX_INVALID_VAL 0xff
#if SLAVE_SERVER_ROLE == 1
#define CONNECTION_IS_SERVER(idx) ((idx) != CON_IDX_INVALID_VAL && (idx) == con_idx_server)
#else
#define CONNECTION_IS_SERVER(idx) 0
#endif
#if MASTER_CLIENT_ROLE == 1
#define CONNECTION_IS_CLIENT(idx) ((idx) != CON_IDX_INVALID_VAL && (idx) == con_idx_client)
#else
#define CONNECTION_IS_CLIENT(idx) 0
#endif
#define UART_SYNC_BYTE  0xA5
#define UART_SYNC_BYTE_LEN 1
#define UART_LEN_LEN  2
#define UART_LINK_ID_LEN 1
#define UART_HEADER_LEN (UART_SYNC_BYTE_LEN + UART_LEN_LEN + UART_LINK_ID_LEN)
#define UART_SVC_BUFFER_SIZE (UART_SERVER_MAX_MTU + UART_HEADER_LEN)
#define UART_RX_PAYLOAD_LEN_MAX (UART_SVC_BUFFER_SIZE - UART_HEADER_LEN)
#define UART_TX_PAYLOAD_LEN_MAX (UART_SVC_BUFFER_SIZE - UART_HEADER_LEN)

#define CONNECTED_MSG_PATTERN 0x5ce5
#define CONNECTED_MSG_PATTERN_LEN 2
#define DISCONNECTED_MSG_PATTERN 0xdead
#define DISCONNECTED_MSG_PATTERN_LEN 2


uint8_t user_ble_send_flag;
uint8_t BLE_connected_flag;
uint8_t user_conid;



enum uart_rx_status
{
    UART_IDLE,
    UART_SYNC,
    UART_LEN_REV,
    UART_LINK_ID,
    UART_RECEIVING,
};
static uint8_t uart_state = UART_IDLE;
static bool uart_tx_busy;
static uint8_t uart_rx_buf[UART_SVC_BUFFER_SIZE];
static UART_HandleTypeDef UART_Config; 
static uint8_t current_uart_tx_idx; // bit7 = 1 : client, bit7 = 0 : server


static const uint8_t ls_uart_svc_uuid_128[] = {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xCA,0xFA,0x00,0x00};
static const uint8_t ls_uart_rx_char_uuid_128[] = {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xCB,0xFA,0x00,0x00};
static const uint8_t ls_uart_tx_char_uuid_128[] = {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xCC,0xFA,0x00,0x00};





//static const uint8_t ls_uart_svc_uuid_128_test[] = {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xFF,0xFF,0x00,0x00};
//static const uint8_t ls_uart_rx_char_uuid_128_test[] = {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xFF,0xAA,0x00,0x00};
//static const uint8_t ls_uart_tx_char_uuid_128_test[] = {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xFF,0xBB,0x00,0x00};






//static const uint8_t CD_uuid_16[] = {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xCD,0xFA,0x00,0x00};
//static const uint8_t ls_uart_svc_uuid_128[] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x01,0x00,0x40,0x6e};
//static const uint8_t ls_uart_rx_char_uuid_128[] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x02,0x00,0x40,0x6e};
//static const uint8_t ls_uart_tx_char_uuid_128[] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x03,0x00,0x40,0x6e};
static const uint8_t att_desc_client_char_cfg_array[] = {0x02,0x29};

#if SLAVE_SERVER_ROLE == 1
/************************************************data for Server*****************************************************/
static const uint8_t att_decl_char_array[] = {0x03,0x28};
enum uart_svc_att_db_handles
{
    UART_SVC_IDX_RX_CHAR,
    UART_SVC_IDX_RX_VAL,
    UART_SVC_IDX_TX_CHAR,
    UART_SVC_IDX_TX_VAL,
    UART_SVC_IDX_TX_NTF_CFG,
		
//		UART_SVC_IDX_RX_CHAR_TEST,
//    UART_SVC_IDX_RX_VAL_TEST,
//    UART_SVC_IDX_TX_CHAR_TEST,
//    UART_SVC_IDX_TX_VAL_TEST,
//    UART_SVC_IDX_TX_NTF_CFG_TEST,
		
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
static uint8_t uart_server_tx_buf[UART_SVC_BUFFER_SIZE];
static uint8_t uart_server_ble_buf[UART_SVC_BUFFER_SIZE];
static uint16_t uart_server_recv_data_length;
static bool uart_server_ntf_done;
static uint16_t uart_server_mtu;
static uint8_t con_idx_server;
static uint16_t cccd_config;

 uint8_t adv_obj_hdl;
static uint8_t advertising_data[28];
static uint8_t scan_response_data[31];

static void ls_uart_server_init(void);
static void ls_uart_init(void);
static void ls_uart_server_client_uart_tx(void);
static void ls_uart_server_read_req_ind(uint8_t att_idx, uint8_t con_idx);
static void ls_uart_server_write_req_ind(uint8_t att_idx, uint8_t con_idx, uint16_t length, uint8_t const *value);
static void ls_uart_server_send_notification(void);
 void start_adv(void);
#endif
/************************************************data for client*****************************************************/
#if MASTER_CLIENT_ROLE == 1
enum initiator_status
{
    INIT_IDLE,
    INIT_BUSY,
};
enum scan_status
{
    SCAN_IDLE,
    SCAN_BUSY,
};

static struct dev_addr specified_slave_addr;
// static const uint8_t peer_slave_addr0[BLE_ADDR_LEN] = {0x01,0xcc,0xcc,0xcc,0xcc,0xcc};
static uint8_t con_idx_client;
static bool uart_client_wr_cmd_done;
static uint16_t uart_client_mtu;
static uint8_t uart_client_tx_buf[UART_SVC_BUFFER_SIZE];
static uint8_t uart_client_ble_buf[UART_SVC_BUFFER_SIZE];
static uint16_t uart_client_recv_data_length;
static uint16_t uart_client_svc_attribute_handle; // handle for primary service attribute handle
static uint16_t uart_client_svc_end_handle;
static uint16_t uart_client_tx_attribute_handle;
static uint16_t uart_client_tx_pointer_handle;
static uint16_t uart_client_rx_attribute_handle;
static uint16_t uart_client_rx_pointer_handle;
static uint16_t uart_client_cccd_handle;
static uint8_t *next_connect_addr;

static uint8_t scan_obj_hdl = 0xff;
static uint8_t init_obj_hdl = 0xff;
static uint8_t init_status = INIT_IDLE; 
static uint8_t dev_addr_type = 0;          /*  0:Public, 1:Private */

static void ls_uart_client_init(void);
static void ls_uart_client_service_dis(uint8_t con_idx);
static void ls_uart_client_char_tx_dis(uint8_t con_idx);
static void ls_uart_client_char_rx_dis(uint8_t con_idx);
static void ls_uart_client_char_desc_dis(uint8_t con_idx);
static void ls_uart_client_send_write_req(void);
static void start_scan(void);
#endif




 void ls_user_event_timer_cb_0(void *param);
 void ls_user_event_timer_cb_1(void *param);
 void ls_user_event_timer_cb_2(void *param);


 struct builtin_timer *user_event_timer_inst_0 = NULL;
 struct builtin_timer *user_event_timer_inst_1 = NULL;
 struct builtin_timer *user_event_timer_inst_2 = NULL;


//	builtin_timer_0  1ms			正常模式运行
//	builtin_timer_1  20ms     正常模式运行
//	builtin_timer_2  500ms    低功耗模式
//	开启低功耗模式后按定时器周期唤醒
void ls_app_timer_init(void)
{
    user_event_timer_inst_0 =builtin_timer_create(ls_user_event_timer_cb_0);
    //builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);

    user_event_timer_inst_1 =builtin_timer_create(ls_user_event_timer_cb_1);
    //builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);
		
    user_event_timer_inst_2 =builtin_timer_create(ls_user_event_timer_cb_2);
    builtin_timer_start(user_event_timer_inst_2, USER_EVENT_PERIOD_2, NULL);
}

//1ms
void ls_user_event_timer_cb_0(void *param) {
			
		builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);
	
	  check_sw();  
		LED_Functon();
		Uart_2_Time_Even();
		Uart_Time_Even();
		
		time_count++;

		if(time_count%5000==0){
					LOG_I("1ms");
		}
}

//20ms
void ls_user_event_timer_cb_1(void *param) {
		builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);
		
		loop_task_normal_power();	
		ls_uart_server_send_notification();   
}

//500ms
void ls_user_event_timer_cb_2(void *param) {

		builtin_timer_start(user_event_timer_inst_2, USER_EVENT_PERIOD_2, NULL);
		
		loop_task_lower_power();	
		ls_uart_server_send_notification();   		
}

static void ls_uart_server_client_uart_tx(void);
static void ls_single_role_timer_cb(void *param);
static struct builtin_timer *ble_app_timer_inst = NULL;
//static void ls_app_timer_init(void)
//{
//    ble_app_timer_inst = builtin_timer_create(ls_single_role_timer_cb);
//    builtin_timer_start(ble_app_timer_inst, UART_SERVER_TIMEOUT, NULL);
//}
static void ls_single_role_timer_cb(void *param)
{
#if SLAVE_SERVER_ROLE == 1    
    ls_uart_server_send_notification();
#endif    
    ls_uart_server_client_uart_tx();
#if MASTER_CLIENT_ROLE == 1    
    ls_uart_client_send_write_req();
#endif    
    if(ble_app_timer_inst)
    {
        builtin_timer_start(ble_app_timer_inst, UART_SERVER_TIMEOUT, NULL); 
    }
}
static void ls_uart_init(void)
{
    uart1_io_init(PB00, PB01);
    io_pull_write(PB01, IO_PULL_UP);
    UART_Config.UARTX = UART1;
    UART_Config.Init.BaudRate = UART_BAUDRATE_115200;
    UART_Config.Init.MSBEN = 0;
    UART_Config.Init.Parity = UART_NOPARITY;
    UART_Config.Init.StopBits = UART_STOPBITS1;
    UART_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Config);
}
#if SLAVE_SERVER_ROLE == 1
static void ls_uart_server_init(void)
{
    con_idx_server = CON_IDX_INVALID_VAL;
    uart_server_ntf_done = true;
    uart_server_mtu = UART_SERVER_MTU_DFT;
    uart_server_recv_data_length = 0;   
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
    if(att_idx == UART_SVC_IDX_RX_VAL && uart_server_tx_buf[0] != UART_SYNC_BYTE)
    { 
        LS_ASSERT(length <= UART_TX_PAYLOAD_LEN_MAX);
        uart_server_tx_buf[0] = UART_SYNC_BYTE;
        uart_server_tx_buf[1] = length & 0xff;
        uart_server_tx_buf[2] = (length >> 8) & 0xff;
        uart_server_tx_buf[3] = con_idx; // what uart will receive should be the real connection index. array_idx is internal.
        memcpy((void*)&uart_server_tx_buf[UART_HEADER_LEN], value, length);
        uint32_t cpu_stat = enter_critical();
        if(!uart_tx_busy)
        {
            uart_tx_busy = true;
            current_uart_tx_idx = (0 << 7);
            HAL_UART_Transmit_IT(&UART_Config, &uart_server_tx_buf[0], length + UART_HEADER_LEN);
        } 
        exit_critical(cpu_stat);
    }
    else if (att_idx == UART_SVC_IDX_TX_NTF_CFG)
    {
        LS_ASSERT(length == 2);
        memcpy(&cccd_config, value, length);
    }
}

static void ls_uart_server_send_notification(void)
{
		uint8_t *p;

    //for(uint8_t idx = 0; idx < UART_SERVER_MASTER_NUM; idx++)
    //{
    //    uint8_t con_idx = con_idx_array[idx];
        uint32_t cpu_stat = enter_critical();
      //if(con_idx != CON_IDX_INVALID_VAL && uart_server_recv_data_length_array[idx] != 0 && uart_server_ntf_done_array[idx])
       
			if(user_ble_send_flag==1 && BLE_connected_flag==1)
			 {
            // LS_ASSERT(con_idx < UART_SERVER_MASTER_NUM);
            //uart_server_ntf_done_array[idx] = false;
						user_ble_send_flag=0;
            uint16_t handle = gatt_manager_get_svc_att_handle(&ls_uart_server_svc_env, UART_SVC_IDX_TX_VAL);

					//uint16_t tx_len = uart_server_recv_data_length_array[idx] > co_min(UART_SERVER_MAX_DATA_LEN(idx), UART_SVC_TX_MAX_LEN) ?
					//co_min(UART_SERVER_MAX_DATA_LEN(idx), UART_SVC_TX_MAX_LEN) : uart_server_recv_data_length_array[idx];
					//uart_server_recv_data_length_array[idx] -= tx_len;
					//memcpy((void*)&uart_server_b	le_buf_array[idx][0], (void*)&uart_server_ble_buf_array[idx][tx_len], uart_server_recv_data_length_array[idx]);       

						p=CMD_Processing(0x00,1);		
						gatt_manager_server_send_notification(con_idx_server, handle, p+1, *p, NULL);
						//LOG_I("con_idx:%d",con_idx);

						LOG_I("发送的");
						LOG_HEX(p+1,*p);		
				}
        exit_critical(cpu_stat);
    //}
}

//蓝牙数据写入，数据处理
static void user_write_req_ind(uint8_t att_idx, uint8_t con_idx, uint16_t length, uint8_t const *value)
{

    uint8_t RX_DATA_BUF[100];
    if(att_idx == UART_SVC_IDX_RX_VAL)// && uart_server_tx_buf[0] != UART_SYNC_BYTE)
    {
        LS_ASSERT(length <= UART_TX_PAYLOAD_LEN_MAX);
        memcpy(&RX_DATA_BUF[0],value,length);				
        LOG_I("接收到的");
        LOG_HEX(&RX_DATA_BUF[0],length);			
				CMD_Processing(&RX_DATA_BUF[0],length);
				user_ble_send_flag=1;				
    }
    else if (att_idx == UART_SVC_IDX_TX_NTF_CFG)
    {
        LS_ASSERT(length == 2);
        memcpy(&cccd_config, value, length);
    }
}

//const uint16_t adv_int_arr[6] = {80, 160, 320, 800, 1600, 3200};

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

void start_adv(void)
{
		uint8_t TEMP[2]={0xCA,0XFA}; 
		//uint8_t TEMP1[2]={0xFF,0XFF}; 

    LS_ASSERT(adv_obj_hdl != 0xff);
    uint8_t adv_data_length = ADV_DATA_PACK(advertising_data, 3,GAP_ADV_TYPE_COMPLETE_LIST_16_BIT_UUID, &TEMP[0], 2, 
																																GAP_ADV_TYPE_COMPLETE_NAME, &lockid[0], 18,
																																GAP_ADV_TYPE_APPEARANCE,&ble_adv_data[0],2);
    dev_manager_start_adv(adv_obj_hdl, advertising_data, adv_data_length, scan_response_data, 0);
    LOG_I("adv start");
}

#endif
#if MASTER_CLIENT_ROLE == 1
static void ls_uart_client_init(void)
{
    con_idx_client = CON_IDX_INVALID_VAL;
    uart_client_wr_cmd_done = true;
    uart_client_mtu = UART_SERVER_MTU_DFT;
    uart_client_recv_data_length = 0;
    uart_client_svc_attribute_handle = 0x1;
    uart_client_svc_end_handle = 0xffff;  
}
static void ls_uart_client_recv_ntf_ind(uint8_t handle, uint8_t con_idx, uint16_t length, uint8_t const *value) 
{
    LOG_I("ls_uart_client_recv_ntf_ind");
    if(uart_client_tx_buf[0] != UART_SYNC_BYTE)
    { 
        LS_ASSERT(length <= UART_TX_PAYLOAD_LEN_MAX);
        uart_client_tx_buf[0] = UART_SYNC_BYTE;
        uart_client_tx_buf[1] = length & 0xff;
        uart_client_tx_buf[2] = (length >> 8) & 0xff;
        uart_client_tx_buf[3] = con_idx; // what uart will receive should be the real connection index. array_idx is internal.
        memcpy((void*)&uart_client_tx_buf[UART_HEADER_LEN], value, length);
        uint32_t cpu_stat = enter_critical();
        if(!uart_tx_busy)
        {
            uart_tx_busy = true;
            current_uart_tx_idx = (1 << 7);
            HAL_UART_Transmit_IT(&UART_Config, &uart_client_tx_buf[0], length + UART_HEADER_LEN);
        } 
        exit_critical(cpu_stat);
    }    
}
static void ls_uart_client_send_write_req(void)
{
    uint32_t cpu_stat = enter_critical();
    if(con_idx_client != CON_IDX_INVALID_VAL && uart_client_recv_data_length != 0 && uart_client_wr_cmd_done) 
    {
        uart_client_wr_cmd_done = false;
        uint16_t tx_len = uart_client_recv_data_length > co_min(UART_CLIENT_MAX_DATA_LEN, UART_SVC_TX_MAX_LEN) ? 
                        co_min(UART_CLIENT_MAX_DATA_LEN, UART_SVC_TX_MAX_LEN) : uart_client_recv_data_length;
        uart_client_recv_data_length -= tx_len;
        LOG_I("ls_uart_client_send_write_req, tx_len = %d", tx_len);
        gatt_manager_client_write_no_rsp(con_idx_client, uart_client_rx_pointer_handle, &uart_client_ble_buf[0], tx_len);         
        memcpy((void*)&uart_client_ble_buf[0], (void*)&uart_client_ble_buf[tx_len], uart_client_recv_data_length);
    }
    exit_critical(cpu_stat);
}
static void ls_uart_client_service_dis(uint8_t con_idx)
{
    gatt_manager_client_svc_discover_by_uuid(con_idx, (uint8_t*)&ls_uart_svc_uuid_128[0], UUID_LEN_128BIT, 1, 0xffff);
}

static void ls_uart_client_char_tx_dis(uint8_t con_idx)
{
    gatt_manager_client_char_discover_by_uuid(con_idx, (uint8_t*)&ls_uart_tx_char_uuid_128[0], UUID_LEN_128BIT, uart_client_svc_attribute_handle, uart_client_svc_end_handle);
}

static void ls_uart_client_char_rx_dis(uint8_t con_idx)
{
    gatt_manager_client_char_discover_by_uuid(con_idx, (uint8_t*)&ls_uart_rx_char_uuid_128[0], UUID_LEN_128BIT, uart_client_svc_attribute_handle, uart_client_svc_end_handle);
}

static void ls_uart_client_char_desc_dis(uint8_t con_idx)
{
    gatt_manager_client_desc_char_discover(con_idx, uart_client_svc_attribute_handle, uart_client_svc_end_handle);
}
static void create_scan_obj(void)
{
    dev_manager_create_scan_object(PUBLIC_OR_RANDOM_STATIC_ADDR);
}
static void create_init_obj(void)
{
    dev_manager_create_init_object(PUBLIC_OR_RANDOM_STATIC_ADDR);
}
static void start_init(uint8_t *peer_addr)
{
    struct dev_addr peer_dev_addr_str;
    memcpy(peer_dev_addr_str.addr, peer_addr, BLE_ADDR_LEN);
    struct start_init_param init_param = {
        .scan_intv = 64,
        .scan_window = 48,
        .conn_to = 0,
        .conn_intv_min = 16,
        .conn_intv_max = 16,
        .conn_latency = 0,
        .supervision_to = 200,

        .peer_addr = &peer_dev_addr_str,
        .peer_addr_type = dev_addr_type,
        .type = DIRECT_CONNECTION,
    };
    dev_manager_start_init(init_obj_hdl,&init_param);
}
static void start_scan(void)
{
    LS_ASSERT(scan_obj_hdl != 0xff);
    struct start_scan_param scan_param = {
        .scan_intv = 0x4000,
        .scan_window = 0x4000,
        .duration = 0,
        .period = 0,
        .type = OBSERVER,
        .active = 0,
        .filter_duplicates = 0,
    };
    dev_manager_start_scan(scan_obj_hdl, &scan_param);
    LOG_I("start scan");
}
#endif
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    LOG_I("tx cplt, current_uart_tx_idx = %d", current_uart_tx_idx);
    uart_tx_busy = false;
#if SLAVE_SERVER_ROLE == 1  
    if ((current_uart_tx_idx & (1 << 7)) == 0)
    {
        uart_server_tx_buf[0] = 0; // clear server buffer sync byte
    }
#endif      
#if MASTER_CLIENT_ROLE == 1    
    if ((current_uart_tx_idx & (1 << 7)) != 0)
    {
        uart_client_tx_buf[0] = 0; // clear client buffer sync byte
    }
#endif    
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->UARTX==UART1) {
        Receive_Interrupt();
        HAL_UART_Receive_IT(&UART1_Config,uart_buffer,1);		// 重新使能串口1接收中断
    }
		
    else if(huart->UARTX==UART2) {
        Uart_2_Receive_Interrupt();
        HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// 重新使能串口2接收中断
    }
    else if(huart->UARTX==UART3) {
        //Uart_3_Receive_Interrupt();
        //HAL_UART_Receive_IT(&UART_Config_RFID,uart_3_buffer,1);		// 重新使能串口3接收中断
    }
}


static void ls_uart_server_client_uart_tx(void)
{
#if SLAVE_SERVER_ROLE == 1    
    if (uart_server_tx_buf[0] == UART_SYNC_BYTE)
    {
        uint32_t cpu_stat = enter_critical();
        if (!uart_tx_busy)
        {
            uint16_t length = (uart_server_tx_buf[2] << 8) | uart_server_tx_buf[1];
            uart_tx_busy = true;
            current_uart_tx_idx = (0 << 7);
            HAL_UART_Transmit_IT(&UART_Config, &uart_server_tx_buf[0], length + UART_HEADER_LEN);
        }
        exit_critical(cpu_stat);
    }
#endif
#if MASTER_CLIENT_ROLE == 1
    if (uart_client_tx_buf[0] == UART_SYNC_BYTE)
    {
        uint32_t cpu_stat = enter_critical();
        if (!uart_tx_busy)
        {
            uint16_t length = (uart_client_tx_buf[2] << 8) | uart_client_tx_buf[1];
            uart_tx_busy = true;
            current_uart_tx_idx = (1 << 7);
            HAL_UART_Transmit_IT(&UART_Config, &uart_client_tx_buf[0], length + UART_HEADER_LEN);
        }
        exit_critical(cpu_stat);
    }
#endif    
}

static void connect_pattern_send_prepare(uint8_t con_idx)
{   
    if (CONNECTION_IS_SERVER(con_idx))
    {
#if SLAVE_SERVER_ROLE == 1           
        uart_server_tx_buf[0] = UART_SYNC_BYTE;
        uart_server_tx_buf[1] = CONNECTED_MSG_PATTERN_LEN & 0xff;
        uart_server_tx_buf[2] = (CONNECTED_MSG_PATTERN_LEN >> 8) & 0xff;
        uart_server_tx_buf[3] = con_idx;
        uart_server_tx_buf[4] = CONNECTED_MSG_PATTERN & 0xff;
        uart_server_tx_buf[5] = (CONNECTED_MSG_PATTERN >> 8) & 0xff;
#endif          
    }  
    else if (CONNECTION_IS_CLIENT(con_idx))
    {
#if MASTER_CLIENT_ROLE == 1        
        uart_client_tx_buf[0] = UART_SYNC_BYTE;
        uart_client_tx_buf[1] = CONNECTED_MSG_PATTERN_LEN & 0xff;
        uart_client_tx_buf[2] = (CONNECTED_MSG_PATTERN_LEN >> 8) & 0xff;
        uart_client_tx_buf[3] = con_idx | (1 << 7);
        uart_client_tx_buf[4] = CONNECTED_MSG_PATTERN & 0xff;
        uart_client_tx_buf[5] = (CONNECTED_MSG_PATTERN >> 8) & 0xff;
#endif        
    }       
}
static void disconnect_pattern_send_prepare(uint8_t con_idx, uint8_t role)
{   
    if (role == LS_BLE_ROLE_SLAVE)
    {
#if SLAVE_SERVER_ROLE == 1          
        uart_server_tx_buf[0] = UART_SYNC_BYTE;
        uart_server_tx_buf[1] = DISCONNECTED_MSG_PATTERN_LEN & 0xff;
        uart_server_tx_buf[2] = (DISCONNECTED_MSG_PATTERN_LEN >> 8) & 0xff;
        uart_server_tx_buf[3] = con_idx;
        uart_server_tx_buf[4] = DISCONNECTED_MSG_PATTERN & 0xff;
        uart_server_tx_buf[5] = (DISCONNECTED_MSG_PATTERN >> 8) & 0xff;
#endif          
    }
    else if (role == LS_BLE_ROLE_MASTER)
    {
#if MASTER_CLIENT_ROLE == 1        
        uart_client_tx_buf[0] = UART_SYNC_BYTE;
        uart_client_tx_buf[1] = DISCONNECTED_MSG_PATTERN_LEN & 0xff;
        uart_client_tx_buf[2] = (DISCONNECTED_MSG_PATTERN_LEN >> 8) & 0xff;
        uart_client_tx_buf[3] = con_idx | (1 << 7);
        uart_client_tx_buf[4] = DISCONNECTED_MSG_PATTERN & 0xff;
        uart_client_tx_buf[5] = (DISCONNECTED_MSG_PATTERN >> 8) & 0xff;
#endif        
    }   
}
static void gap_manager_callback(enum gap_evt_type type,union gap_evt_u *evt,uint8_t con_idx)
{
    switch(type)
    {
    case CONNECTED:
#if SLAVE_SERVER_ROLE == 1     
        if (gap_manager_get_role(con_idx) == LS_BLE_ROLE_SLAVE)
        {
            LS_ASSERT(con_idx_server == 0xff);
            con_idx_server = con_idx;   
            connect_pattern_send_prepare(con_idx); 
						BLE_connected_flag=1;
        }
#endif
#if MASTER_CLIENT_ROLE == 1        
        if (gap_manager_get_role(con_idx) == LS_BLE_ROLE_MASTER)
        {
            LS_ASSERT(con_idx_client == 0xff);
            con_idx_client = con_idx;   
            connect_pattern_send_prepare(con_idx); 

            gatt_manager_client_mtu_exch_send(con_idx);
        }    
#endif           
        LOG_I("connected! new con_idx = %d", con_idx);
				
				user_conid=con_idx;
				
				if(tag_lock_status==POS_0){
							Enter_Power_Mode_NL();
				}
				else if(tag_lock_status==POS_90){
							Enter_Power_Mode_NH();
				}
				
		break;
    case DISCONNECTED:
        LOG_I("disconnected! delete con_idx = %d", con_idx);       
        if (CONNECTION_IS_SERVER(con_idx))
        {
#if SLAVE_SERVER_ROLE == 1               
            disconnect_pattern_send_prepare(con_idx, LS_BLE_ROLE_SLAVE);
            con_idx_server = CON_IDX_INVALID_VAL;        
            uart_server_mtu = UART_SERVER_MTU_DFT;
            start_adv();
						BLE_connected_flag=0;
						lock_success_flag=0;
#endif            
        }      
        else if (CONNECTION_IS_CLIENT(con_idx))
        {
#if MASTER_CLIENT_ROLE == 1            
            disconnect_pattern_send_prepare(con_idx, LS_BLE_ROLE_MASTER);
            con_idx_client = CON_IDX_INVALID_VAL;        
            uart_client_mtu = UART_SERVER_MTU_DFT;
            start_scan();
            init_status = INIT_IDLE;   
#endif                
        }
    break;
    case CONN_PARAM_REQ:

    break;
    case CONN_PARAM_UPDATED:

    break;
    default:

    break;
    }
}

static void gatt_manager_callback(enum gatt_evt_type type,union gatt_evt_u *evt,uint8_t con_idx)
{
    bool disconnected = false;
#if SLAVE_SERVER_ROLE == 1       
    if (gap_manager_get_role(con_idx) == LS_BLE_ROLE_SLAVE)
    {
        disconnected = con_idx_server == CON_IDX_INVALID_VAL;
    }
#endif
#if MASTER_CLIENT_ROLE == 1    
    if (gap_manager_get_role(con_idx) == LS_BLE_ROLE_MASTER)
    {
        disconnected = con_idx_client == CON_IDX_INVALID_VAL;
    }   
#endif    
    if (!disconnected)
    {
        switch (type)
        {
#if SLAVE_SERVER_ROLE == 1              
        case SERVER_READ_REQ:
            LOG_I("read req");
            ls_uart_server_read_req_ind(evt->server_read_req.att_idx, con_idx);
        break;
        case SERVER_WRITE_REQ:
            LOG_I("write req");
						user_write_req_ind (evt->server_write_req.att_idx, con_idx, evt->server_write_req.length, evt->server_write_req.value);
            //ls_uart_server_write_req_ind(evt->server_write_req.att_idx, con_idx, evt->server_write_req.length, evt->server_write_req.value);
        break;
        case SERVER_NOTIFICATION_DONE:
            uart_server_ntf_done = true;
            LOG_I("ntf done");
        break;
#endif        
        case MTU_CHANGED_INDICATION:
            if (CONNECTION_IS_SERVER(con_idx))
            {
#if SLAVE_SERVER_ROLE == 1                    
                uart_server_mtu = evt->mtu_changed_ind.mtu;
#endif                
            }
            else if (CONNECTION_IS_CLIENT(con_idx))
            {
#if MASTER_CLIENT_ROLE == 1                 
                uart_client_mtu = evt->mtu_changed_ind.mtu;
                ls_uart_client_service_dis(con_idx);
#endif                
            }                
            LOG_I("mtu exch ind, mtu = %d", evt->mtu_changed_ind.mtu);
        break;
#if MASTER_CLIENT_ROLE == 1          
        case CLIENT_RECV_NOTIFICATION:
            ls_uart_client_recv_ntf_ind(evt->client_recv_notify_indicate.handle, con_idx, evt->client_recv_notify_indicate.length, evt->client_recv_notify_indicate.value);
            LOG_I("svc dis notification, length = %d", evt->client_recv_notify_indicate.length);
        break;
        case CLIENT_PRIMARY_SVC_DIS_IND:
            if (!memcmp(evt->client_svc_disc_indicate.uuid, ls_uart_svc_uuid_128, sizeof(ls_uart_svc_uuid_128)))
            {
                uart_client_svc_attribute_handle = evt->client_svc_disc_indicate.handle_range.begin_handle;
                uart_client_svc_end_handle = evt->client_svc_disc_indicate.handle_range.end_handle;
                ls_uart_client_char_tx_dis(con_idx);
                LOG_I("svc dis success, attribute_handle = %d, end_handle = %d", uart_client_svc_attribute_handle, uart_client_svc_end_handle);
            }    
            else
            {
                LOG_I("unexpected svc uuid");
            }    
        break;
        case CLIENT_CHAR_DIS_BY_UUID_IND:
            if (!memcmp(evt->client_disc_char_indicate.uuid, ls_uart_tx_char_uuid_128, sizeof(ls_uart_tx_char_uuid_128)))
            {
                uart_client_tx_attribute_handle = evt->client_disc_char_indicate.attr_handle;
                uart_client_tx_pointer_handle = evt->client_disc_char_indicate.pointer_handle;
                ls_uart_client_char_rx_dis(con_idx);
                LOG_I("tx dis success, attribute handle = %d, pointer handler = %d", uart_client_tx_attribute_handle, uart_client_tx_pointer_handle);
            }
            else if (!memcmp(evt->client_disc_char_indicate.uuid, ls_uart_rx_char_uuid_128, sizeof(ls_uart_rx_char_uuid_128)))
            {
                uart_client_rx_attribute_handle = evt->client_disc_char_indicate.attr_handle;
                uart_client_rx_pointer_handle = evt->client_disc_char_indicate.pointer_handle;
                ls_uart_client_char_desc_dis(con_idx);
                LOG_I("rx dis success, attribute handle = %d, pointer handler = %d", uart_client_rx_attribute_handle, uart_client_rx_pointer_handle);
            }
            else
            {
                LOG_I("unexpected char uuid");
            }   
        break;
        case CLIENT_CHAR_DESC_DIS_BY_UUID_IND:
            if (!memcmp(evt->client_disc_char_desc_indicate.uuid, att_desc_client_char_cfg_array, sizeof(att_desc_client_char_cfg_array)))
            {
                uart_client_cccd_handle = evt->client_disc_char_desc_indicate.attr_handle;
                LOG_I("cccd dis success, cccd handle = %d", uart_client_cccd_handle);
                gatt_manager_client_cccd_enable(con_idx, uart_client_cccd_handle, 1, 0);
            }
            else
            {
                LOG_I("unexpected desc uuid");
            }      
        break;
        case CLIENT_WRITE_WITH_RSP_DONE:
            if(evt->client_write_rsp.status == 0)
            {
                LOG_I("write success");
            }
            else
            {
                LOG_I("write fail, status = %d", evt->client_write_rsp.status);
            }       
        break;
        case CLIENT_WRITE_NO_RSP_DONE:
            if(evt->client_write_no_rsp.status == 0)
            {
                LS_ASSERT(gap_manager_get_role(con_idx) == LS_BLE_ROLE_MASTER);
                uart_client_wr_cmd_done = true;
                LOG_I("write no rsp success");
            }
            else
            {
                LOG_I("write fail, status = %d", evt->client_write_rsp.status);
            }    
        break;
#endif        
        default:
            LOG_I("Event not handled!");
            break;
        }
    }
    else
    {
        LOG_I("receive gatt msg when disconnected!");
    }
}
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
//        uint8_t addr[6];
//        bool type;
//        dev_manager_get_identity_bdaddr(addr,&type);
//        LOG_I("type:%d,addr:",type);
//        LOG_HEX(addr,sizeof(addr));
#if SLAVE_SERVER_ROLE == 1
        dev_manager_add_service((struct svc_decl *)&ls_uart_server_svc);
        ls_uart_server_init();

#endif     

#if MASTER_CLIENT_ROLE == 1
        ls_uart_client_init(); 
        if (scan_obj_hdl == 0xff)
        {
            create_scan_obj();
        }
#endif       
         
				 
				 ls_app_timer_init();
//				 builtin_timer_stop(user_event_timer_inst_0);
//				 builtin_timer_stop(user_event_timer_inst_1);
//				 builtin_timer_stop(user_event_timer_inst_2);
				 
				 
				 User_BLE_Ready();
    }
    break;
#if SLAVE_SERVER_ROLE == 1    
    case SERVICE_ADDED:
        gatt_manager_svc_register(evt->service_added.start_hdl, UART_SVC_ATT_NUM, &ls_uart_server_svc_env);
        create_adv_obj();
    break;
    case ADV_OBJ_CREATED:
        LS_ASSERT(evt->obj_created.status == 0);
        adv_obj_hdl = evt->obj_created.handle;
       
				
				start_adv();
        
				LOG_I("adv start");
    break;
    case ADV_STOPPED:
        LOG_I("adv stopped");
    break;
#endif  
#if MASTER_CLIENT_ROLE == 1  
    case SCAN_OBJ_CREATED:
        LOG_I("scan obj created");
        LS_ASSERT(evt->obj_created.status == 0);
        scan_obj_hdl = evt->obj_created.handle;
        create_init_obj();
    break;
    case SCAN_STOPPED:
        LOG_I("scan stopped, next_connect_addr=%d", next_connect_addr);
        if (next_connect_addr)
        {
            start_init(next_connect_addr);
            next_connect_addr = 0;
            init_status = INIT_BUSY;
        }
    break;
    case ADV_REPORT:
    {
        #if 0
        LOG_I("adv received, addr: %2x:%2x:%2x:%2x:%2x:%2x", evt->adv_report.adv_addr->addr[5],
                                                       evt->adv_report.adv_addr->addr[4],
                                                       evt->adv_report.adv_addr->addr[3],
                                                       evt->adv_report.adv_addr->addr[2],
                                                       evt->adv_report.adv_addr->addr[1],
                                                       evt->adv_report.adv_addr->addr[0]);
        #endif
        // if (!memcmp(peer_slave_addr0, evt->adv_report.adv_addr->addr, BLE_ADDR_LEN))
        struct adv_payload_struct adv_data_name;
        if (dev_manager_adv_report_parse(GAP_ADV_TYPE_SHORTENED_NAME, &evt->adv_report, &adv_data_name))
        {
            if (adv_data_name.size == sizeof(UART_SVC_ADV_NAME) &&
                0 == memcmp((void*)adv_data_name.p_data, UART_SVC_ADV_NAME, sizeof(UART_SVC_ADV_NAME)))
            {
                if (init_obj_hdl != 0xff && init_status == INIT_IDLE)
                {
                    dev_addr_type = evt->adv_report.adv_addr_type;
                    memcpy((void*)&specified_slave_addr.addr[0], (void*)&evt->adv_report.adv_addr->addr[0], BLE_ADDR_LEN);
                    next_connect_addr = &specified_slave_addr.addr[0];
                    dev_manager_stop_scan(scan_obj_hdl);
                }  
            }
        }
    }
    break;
    case INIT_OBJ_CREATED:
        LOG_I("init obj created");
        LS_ASSERT(evt->obj_created.status == 0);
        init_obj_hdl = evt->obj_created.handle;
        start_scan();
    break;
    case INIT_STOPPED:
        init_status = INIT_IDLE;
        LOG_I("init stopped");      
    break;
#endif    
    default:

    break;
    }   
}






int main()
{
    sys_init_app();
    ble_init();
    dev_manager_init(dev_manager_callback);
    gap_manager_init(gap_manager_callback);
    gatt_manager_init(gatt_manager_callback);
    ble_loop();
}
