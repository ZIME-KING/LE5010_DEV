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

#include "user_main.h"


#define SLAVE_SERVER_ROLE 1
#define MASTER_CLIENT_ROLE 0

#ifdef USER_TEST
uint8_t open_count;
#endif
uint8_t MAC_ADDR[6];
uint8_t CIMI_DATA[15] =	"460000000000000";
uint8_t MICI_DATA[15] =	"860000000000000";
uint8_t RFID_DATA[4]	= {0x00,0x00,0x00,0x00};
uint8_t RFID_DATA_2[4]= {0X00,0x00,0x00,0x00};
uint8_t SHORT_NAME[10]=	"3141592654";

uint8_t NEW_SHORT_NAME[10];
uint8_t SHORT_NAME_LEN=10;
uint16_t send_time_delay=0; //说明nb模块收到数据后，发送过程中功率与信号强度相关，造成电压骤降，每次发送后延时一段时间再去采集电压


//uint8_t COMPLETE_NAME[7]= "0123456";
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
#define UART_SYNC_BYTE  '+'
#define UART_SYNC_BYTE_LEN 1
#define UART_LEN_LEN  3
#define UART_LINK_ID_LEN 5  //NNMI:
#define UART_HEADER_LEN (UART_SYNC_BYTE_LEN + UART_LEN_LEN + UART_LINK_ID_LEN)
#define UART_SVC_BUFFER_SIZE (UART_SERVER_MAX_MTU + UART_HEADER_LEN)
#define UART_RX_PAYLOAD_LEN_MAX (UART_SVC_BUFFER_SIZE - UART_HEADER_LEN)
#define UART_TX_PAYLOAD_LEN_MAX (UART_SVC_BUFFER_SIZE - UART_HEADER_LEN)

#define CONNECTED_MSG_PATTERN 0x5ce5
#define CONNECTED_MSG_PATTERN_LEN 2
#define DISCONNECTED_MSG_PATTERN 0xdead
#define DISCONNECTED_MSG_PATTERN_LEN 2


uint8_t globle_Result;  //接受数据处理结果
uint8_t user_ble_send_flag=0;

uint8_t TX_DATA_BUF[16]; //BEL
uint8_t RX_DATA_BUF[16];
uint8_t DATA_BUF[16];
uint8_t TOKEN[4]= {0xf1,0xf2,0xf3,0xf4};
uint8_t PASSWORD[6]= {0x30,0x30,0x30,0x30,0x30,0x30};

uint8_t NEW_PASSWORD_BUF[6]; //暂存改password的数据
uint8_t NEW_KEY_BUF[8];      //暂存改key的数据，分两次接收

uint8_t	psaaword_task_flag=0;  //改密码任务开始标记
uint8_t	key_task_flag=0;			 //改密钥任务开始标记


uint8_t	reset_flag=0;			 //掉电启动标记


uint8_t BLE_connected_flag=0;
uint8_t VBat_value=0;

//enum uart_rx_status
//{
//    UART_IDLE,
//    UART_SYNC,
//    UART_LEN_REV,
//    UART_LINK_ID,
//    UART_RECEIVING,
//		UART_AT_NAME,
//};
//static uint8_t uart_state = UART_IDLE;
static bool uart_tx_busy;
//static uint8_t uart_rx_buf[UART_SVC_BUFFER_SIZE];
UART_HandleTypeDef UART_Config;
UART_HandleTypeDef UART_Config_AT;
UART_HandleTypeDef UART_Config_RFID;
//static uint8_t current_uart_tx_idx; // bit7 = 1 : client, bit7 = 0 : server
static const uint8_t ls_uart_svc_uuid_128[] =     {0xFB,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xF0,0xFF,0x00,0x00};
static const uint8_t ls_uart_rx_char_uuid_128[] = {0xFB,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xF2,0xFF,0x00,0x00};
static const uint8_t ls_uart_tx_char_uuid_128[] = {0xFB,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xF1,0xFF,0x00,0x00};

//static const uint8_t ls_uart_svc_uuid_128[] =     {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB0,0x00,0x40,0x51,0x04,0xC0,0xFF,0x00,0xF0};
//static const uint8_t ls_uart_rx_char_uuid_128[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB0,0x00,0x40,0x51,0x04,0xC1,0xFF,0x00,0xF0};
//static const uint8_t ls_uart_tx_char_uuid_128[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB0,0x00,0x40,0x51,0x04,0xC2,0xFF,0x00,0xF0};
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
static uint8_t uart_server_tx_buf[UART_SVC_BUFFER_SIZE];     //BLE设备接受到的数据
static uint8_t uart_server_ble_buf[UART_SVC_BUFFER_SIZE];		 //BLE设备需发送的数据缓存
static uint16_t uart_server_recv_data_length;
static bool uart_server_ntf_done;
static uint16_t uart_server_mtu;
static uint8_t con_idx_server;
static uint16_t cccd_config;

static uint8_t adv_obj_hdl;
static uint8_t advertising_data[28];
static uint8_t scan_response_data[31];

static void ls_uart_server_init(void);
static void ls_uart_init(void);
static void ls_uart_server_client_uart_tx(void);
static void ls_uart_server_read_req_ind(uint8_t att_idx, uint8_t con_idx);
//static void ls_uart_server_write_req_ind(uint8_t att_idx, uint8_t con_idx, uint16_t length, uint8_t const *value);
static void ls_uart_server_send_notification(void);
//static void start_adv(void);
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

static const uint8_t peer_slave_addr0[BLE_ADDR_LEN] = {0x01,0xcc,0xcc,0xcc,0xcc,0xcc};
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
static void ls_uart_server_client_uart_tx(void);

static void ls_user_event_timer_cb_0(void *param);
static void ls_user_event_timer_cb_1(void *param);
static void ls_single_role_timer_cb(void *param);

static struct builtin_timer_0 *user_event_timer_inst_0 = NULL;
static struct builtin_timer_1 *user_event_timer_inst_1 = NULL;
static struct builtin_timer 	*ble_app_timer_inst 		 = NULL;

#define USER_EVENT_PERIOD_0 5			 //按键扫描
#define USER_EVENT_PERIOD_1 50     //按键处理

static void ls_app_timer_init(void)
{
    user_event_timer_inst_0 =builtin_timer_create(ls_user_event_timer_cb_0);
    builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);

    user_event_timer_inst_1 =builtin_timer_create(ls_user_event_timer_cb_1);
    builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);

    ble_app_timer_inst = builtin_timer_create(ls_single_role_timer_cb);
    builtin_timer_start(ble_app_timer_inst, UART_SERVER_TIMEOUT, NULL);

    builtin_timer_start(ble_app_timer_inst, UART_SERVER_TIMEOUT, NULL);
}
static void ls_single_role_timer_cb(void *param)
{
#if SLAVE_SERVER_ROLE == 1
    ls_uart_server_send_notification();
#endif
    // ls_uart_server_client_uart_tx();
#if MASTER_CLIENT_ROLE == 1
    ls_uart_client_send_write_req();
#endif
    if(ble_app_timer_inst)
    {
        builtin_timer_start(ble_app_timer_inst, UART_SERVER_TIMEOUT, NULL);
    }
}

//5ms   做按键扫描
static void ls_user_event_timer_cb_0(void *param)
{

    Uart_3_Time_Even();  //串口接收数据
    Uart_3_Data_Processing();  //串口接收数据
				
    Uart_2_Time_Even();  //串口接收数据
    Uart_2_Data_Processing();

//		Uart_Time_Even();
//		Uart_Data_Processing();

    Scan_Key();					 //扫描按键
    Get_Vbat_Task();		 //获取电池电量 0~100

    builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);
}
//uint16_t temp_count=0;
uint16_t sleep_time=0;
//uint8_t KEY_FLAG=0;   //收到开锁请求
uint8_t once_flag=0;
uint8_t db_flag=0;
uint8_t test_mode_flag;
//50ms 任务
static void ls_user_event_timer_cb_1(void *param)
{
    //第一次上电从flash读出默认值不为aa进入测试代码
    if(test_mode_flag!=0xAA) {
        if(wd_FLAG==0)HAL_IWDG_Refresh();	  //喂狗
        TEST_LED_TASK();										//LED显示效果
        Buzzer_Task();											//蜂鸣器任务
        TEST_AT_GET_IMEI_TASK();
        TEST_AT_GET_IMSI_TASK();
        TEST_AT_GET_DB_TASK();
        State_Change_Task();
				Lock_task() ;                        //开锁任务
    }
    else {
        //LOG_I("fMODE:%X",test_mode_flag);

        sleep_time++;			 									 //记录休眠时间,在收到蓝牙数据，和开锁数据时重新计数
        if(wd_FLAG==0)HAL_IWDG_Refresh();	 	 //喂狗
        LED_TASK();													 //LED显示效果
        Buzzer_Task();											 //蜂鸣器任务
        Sleep_Task();	     								 	 //休眠,  Set_Sleep_Time（s）设置休眠时间
#ifdef USER_TEST
        AT_User_Set_Task();
        AT_User_Reply_Task();
        LOG_I("db_%x",db_flag);
#endif
        db_flag=AT_GET_DB_TASK();						 //获取信号强度
        if(db_flag==0xff) {
            if(AT_INIT()==0xff) {//向服务器注册消息，只在初始化后运行一次
                //名称有变化
                if(strncmp((char*)NEW_SHORT_NAME,(char*)SHORT_NAME,SHORT_NAME_LEN)!=0) {
                    LOG_I("read_id");
                    LOG_I("%s",SHORT_NAME);

                    LOG_I("read_id N");
                    LOG_I("%s",NEW_SHORT_NAME);
                    tinyfs_write(ID_dir_2,1,NEW_SHORT_NAME,sizeof(NEW_SHORT_NAME));
                    tinyfs_write_through();

                    uint8_t  tmp[10];
                    uint16_t length = 10;
                    tinyfs_read(ID_dir_2,1,tmp,&length);//读到tmp中
                    LOG_I("read_id");
                    LOG_I("%s",tmp);
                    wd_FLAG=1;
                }
                if(UDP_INIT()==1) {
                    //启动信息上报
                    if(Start_Lock_Send_Task()==0) {
                        //心跳包
                        if(Tick_Lock_Send_Task()==0) {
                            //信息上报
                            if(Open_Lock_Data_Send_Task()==0) {
                                //按键按下，向服务器查询
                                if(Open_Lock_Send_Task()==0) {
                                }
                            }
                        }
                    }
//										Scan_RDIF_Task();                      //扫描卡片任务
										Delay_test_task();
										//User_Mfrc522_2(&M1_Card_2,0);
                    User_Mfrc522(&M1_Card,0);            //扫描卡片任务
										State_Change_Task();								 //扫描开关状态，改变蓝牙发送，和NB启动上报数据
										Lock_task() ;                        //开锁任务
                }
            }
            //蓝牙连接下
            if(BLE_connected_flag==1) {
                Password_Task();//改开锁的密码
                Key_Task();     //改aes128的密钥
                sleep_time=0;   //不休眠
            }
        }
    }
		        ls_uart_server_send_notification();  //蓝牙数据发送
        builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);
}

static void ls_uart_init(void)
{
    uart3_io_init(PA12, PA13);
    UART_Config.UARTX = UART1;
    UART_Config.Init.BaudRate = UART_BAUDRATE_9600;
    UART_Config.Init.MSBEN = 0;
    UART_Config.Init.Parity = UART_NOPARITY;
    UART_Config.Init.StopBits = UART_STOPBITS1;
    UART_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Config);
}


static void ls_uart3_init(void)
{
		//串口用uart1，
    io_pull_write(PA14, IO_PULL_UP);  				//设置上拉
    io_pull_write(PA15, IO_PULL_UP);  				//设置上拉
    uart1_io_init(PA14, PA15);
    UART_Config_RFID.UARTX = UART3;
    UART_Config_RFID.Init.BaudRate = UART_BAUDRATE_9600;
    UART_Config_RFID.Init.MSBEN = 0;
    UART_Config_RFID.Init.Parity = UART_NOPARITY;
    UART_Config_RFID.Init.StopBits = UART_STOPBITS1;
    UART_Config_RFID.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Config_RFID);
}



void AT_uart_init(void)
{
    //uart2_io_init(PA13, PA14);
    io_pull_write(PA11, IO_PULL_UP);  				//设置上拉
    io_pull_write(PA10, IO_PULL_UP);  				//设置上拉
    uart2_io_init(PA10, PA11);
    UART_Config_AT.UARTX = UART2;
    UART_Config_AT.Init.BaudRate 	= UART_BAUDRATE_115200;
    UART_Config_AT.Init.MSBEN 			= 0;
    UART_Config_AT.Init.Parity  	  = UART_NOPARITY;
    UART_Config_AT.Init.StopBits 	= UART_STOPBITS1;
    UART_Config_AT.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_Config_AT);
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

void Set_TOKEN(uint8_t TOKEN_0,uint8_t TOKEN_1,uint8_t TOKEN_2,uint8_t TOKEN_3) {
    TOKEN[0]=TOKEN_0;
    TOKEN[1]=TOKEN_1;
    TOKEN[2]=TOKEN_2;
    TOKEN[3]=TOKEN_3;
}

static void User_BLE_Data_Handle() {
    switch(DATA_BUF[0]) {
    //接收到GET_TOKEN命令发送TOKEN
    case 0x01:
        user_ble_send_flag=1;
        TX_DATA_BUF[0]=0x01;		// CMD
        TX_DATA_BUF[1]=TOKEN[0];
        TX_DATA_BUF[2]=TOKEN[1];
        TX_DATA_BUF[3]=TOKEN[2];
        TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
        TX_DATA_BUF[5]=0x06;    //LEN
        TX_DATA_BUF[6]=0xA0;		//VER1
        TX_DATA_BUF[7]=0xA1;		//VER2
        TX_DATA_BUF[8]=0x00;   	//STA
        TX_DATA_BUF[9]=0x00;   	//CNT[2]
        TX_DATA_BUF[10]=0xFF;   	//CNT[2]
        TX_DATA_BUF[11]=0x02;   	//
        break;
    case 0x10:
        if(strncmp((char*)TOKEN,(char*)&DATA_BUF[1],4)==0) {
            //if(DATA_BUF[5]==0x01 && DATA_BUF[6]==0x00){
            user_ble_send_flag=1;
            TX_DATA_BUF[0]=0x10;		// CMD
            TX_DATA_BUF[1]=TOKEN[0];
            TX_DATA_BUF[2]=TOKEN[1];
            TX_DATA_BUF[3]=TOKEN[2];
            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
            TX_DATA_BUF[5]=0x01;    //LEN
            TX_DATA_BUF[6]=VBat_value;    //LV 电量
            //}
        }
        break;

    case 0x51: //关锁
        if(strncmp((char*)TOKEN,(char*)&DATA_BUF[1],4)==0) {
            user_ble_send_flag=1;
            TX_DATA_BUF[0]=0x51;		// CMD
            TX_DATA_BUF[1]=TOKEN[0];
            TX_DATA_BUF[2]=TOKEN[1];
            TX_DATA_BUF[3]=TOKEN[2];
            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
            TX_DATA_BUF[5]=0x01;    //LEN

            if(strncmp((char*)PASSWORD,(char*)&DATA_BUF[6],6)==0) {

                //TX_DATA_BUF[6]=Open_Lock_Moto();    //RET 00为开锁成功，01为密码错误  FF为开锁失败
            }
            else {
                TX_DATA_BUF[6]=0x01;    		//RET 00为开锁成功，01为密码错误
            }
        }
        break;

    case 0x50:  //开锁
        if(strncmp((char*)TOKEN,(char*)&DATA_BUF[1],4)==0) {
            //user_ble_send_flag=1;
            TX_DATA_BUF[0]=0x50;			// CMD
            TX_DATA_BUF[1]=TOKEN[0];
            TX_DATA_BUF[2]=TOKEN[1];
            TX_DATA_BUF[3]=TOKEN[2];
            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
            TX_DATA_BUF[5]=0x01;    	//LEN

            if(strncmp((char*)PASSWORD,(char*)&DATA_BUF[6],6)==0) {
                if(DATA_BUF[12]==0x01) {
                    lock_task_flag_1=1;   //
                    //lock_task_flag_1=1;   //
                }
                else if(DATA_BUF[12]==0x02) {
                    lock_task_flag_2=1;
                }
                //moro_task_flag=1;                    ////密码正确 开启电机动作
                //TX_DATA_BUF[6]=Close_Lock_Moto();    //RET 00为开锁成功，01为密码错误  FF为开锁失败
            }
            else {
                TX_DATA_BUF[6]=0x01;    		//RET 00为开锁成功，01为密码错误
                user_ble_send_flag=1;				//密码错误直接回复
            }
        }
        break;
//		case 0x52:
//		break;
    //查询锁状态
    case 0x55:
        if(strncmp((char*)TOKEN,(char*)&DATA_BUF[1],4)==0) {
            user_ble_send_flag=1;
            //这里0x55查询锁状态 回复没有更新，用0x52上报锁信息可以更新
            //TX_DATA_BUF[0]=0x55;		// CMD
            TX_DATA_BUF[0]=0x55;		// CMD
            TX_DATA_BUF[1]=TOKEN[0];
            TX_DATA_BUF[2]=TOKEN[1];
            TX_DATA_BUF[3]=TOKEN[2];
            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
            TX_DATA_BUF[5]=0x03;    //LEN
            TX_DATA_BUF[6]=0x01;    //锁状态

            TX_DATA_BUF[7]=0x06;    // 在线情况  1，2全在线，具体见协议文档
            TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //

            //TX_DATA_BUF[7]=0x00;    //ONLINE
            //TX_DATA_BUF[8]=0x00;    //STATUS
            //}
        }
        break;
    //修改广播名称
    case 0xA0:
        if(strncmp((char*)TOKEN,(char*)&DATA_BUF[1],4)==0) {
            //user_ble_send_flag=1;
            //SHORT_NAME_LEN=DATA_BUF[6];
            //for(uint8_t i=0;i<10;i++){
            //	SHORT_NAME[i]=0;
            //}


            SHORT_NAME_LEN=DATA_BUF[5];
            NEW_SHORT_NAME[0]=0x00;
            NEW_SHORT_NAME[1]=0x00;
            NEW_SHORT_NAME[2]=0x00;
            NEW_SHORT_NAME[3]=0x00;
            NEW_SHORT_NAME[4]=0x00;
            NEW_SHORT_NAME[5]=0x00;
            NEW_SHORT_NAME[6]=0x00;
            NEW_SHORT_NAME[7]=0x00;
            NEW_SHORT_NAME[8]=0x00;
            NEW_SHORT_NAME[9]=0x00;

            memcpy (&NEW_SHORT_NAME[0], &DATA_BUF[6], SHORT_NAME_LEN);
            buzzer_task_flag=1;
            //wd_FLAG=1;
        }

        break;
    //修改密码
    case 0xA1:
        memcpy (&NEW_PASSWORD_BUF[0], &DATA_BUF[6], 6);
        psaaword_task_flag=0xA1;
        break;

    case 0xA2:
        memcpy (&NEW_PASSWORD_BUF[0], &DATA_BUF[6], 6);
        if(psaaword_task_flag==0xA1) {
            psaaword_task_flag=0xA2;
        }
        break;

    //修改秘钥
    case 0xA5:
        memcpy (&NEW_KEY_BUF[0], &DATA_BUF[6], 8);
        key_task_flag=0xA5;
        break;

    case 0xA6:
        memcpy (&NEW_KEY_BUF[0], &DATA_BUF[6], 8);
        if(key_task_flag==0xA5) {
            key_task_flag=0xA6;
        }
        break;
    }
}

//蓝牙数据写入，数据处理
static void user_write_req_ind(uint8_t att_idx, uint16_t length, uint8_t const *value)
{
    //uint8_t buf[100];
    if(att_idx == UART_SVC_IDX_RX_VAL)// && uart_server_tx_buf[0] != UART_SYNC_BYTE)
    {
        sleep_time=0;
        LS_ASSERT(length <= UART_TX_PAYLOAD_LEN_MAX);
        memcpy(&RX_DATA_BUF[0],value,16);

#ifdef USER_TEST
        if(RX_DATA_BUF[0]=='A' && RX_DATA_BUF[1]=='T') {
            if(AT_tset_flag==0) AT_tset_flag=RX_DATA_BUF[2]-'0';
        }
#endif

        User_Decoden(&RX_DATA_BUF[0],&DATA_BUF[0],16);
        LOG_I("接收到的");
        LOG_HEX(&RX_DATA_BUF[0],16);
        LOG_HEX(&DATA_BUF[0],16);
        LOG_I("%d",user_ble_send_flag);
        User_BLE_Data_Handle();
        LOG_I("%d",user_ble_send_flag);
        //memcpy(&buf[0],&RX_DATA_BUF[0],16);
        //memcpy(&buf[16],&DATA_BUF[0],16);
        //HAL_UART_Transmit_IT(&UART_Config, &buf[0],32);
    }
    else if (att_idx == UART_SVC_IDX_TX_NTF_CFG)
    {
        LS_ASSERT(length == 2);
        memcpy(&cccd_config, value, length);
    }
}

//蓝牙 50ms 判断接受到数据是否回复
#define RECORD_KEY4  4  //蓝牙通信密钥 低8位
#define RECORD_KEY5  5  //蓝牙通信密钥 高8位
static void ls_uart_server_send_notification(void)
{
    uint8_t AF_TX_DATA_BUF[16];
    User_Encryption(TX_DATA_BUF,AF_TX_DATA_BUF,16);

    if(key_task_flag==0xff) {
        key_task_flag=0;
        static uint8_t tmp[10];
        uint16_t length_8  = 8;
        tinyfs_read(ID_dir_3,RECORD_KEY4,tmp,&length_8);//读到tmp中
        LOG_I("KEY4");
        LOG_HEX(tmp,8);
        memcpy (&key[0], &tmp[0],8);

        tinyfs_read(ID_dir_3,RECORD_KEY5,tmp,&length_8);//读到tmp中
        LOG_I("KEY5");
        LOG_HEX(tmp,8);
        memcpy (&key[8], &tmp[0],8);


    }
    //LOG_I("uart_server_ntf_done:%d,%d,",uart_server_ntf_done,user_ble_send_flag);
    uint32_t cpu_stat = enter_critical();
    if(user_ble_send_flag==1 && BLE_connected_flag==1) {
        uart_server_ntf_done = false;
        user_ble_send_flag=0;
        uint16_t handle = gatt_manager_get_svc_att_handle(&ls_uart_server_svc_env, UART_SVC_IDX_TX_VAL);
        //TX_DATA_BUF[6]=lock_state[0];
        //uint16_t tx_len = uart_server_recv_data_length > co_min(UART_SERVER_MAX_DATA_LEN, UART_SVC_TX_MAX_LEN) ?
        //co_min(UART_SERVER_MAX_DATA_LEN, UART_SVC_TX_MAX_LEN) : uart_server_recv_data_length;
        //uart_server_recv_data_length -= tx_len;
        gatt_manager_server_send_notification(con_idx_server, handle, &AF_TX_DATA_BUF[0], 16, NULL);
        LOG_I("发送的");
        LOG_HEX(&TX_DATA_BUF[0],16);
        LOG_HEX(&AF_TX_DATA_BUF[0],16);



        // memcpy((void*)&uart_server_ble_buf[0], (void*)&uart_server_ble_buf[tx_len], uart_server_recv_data_length);
    }
    exit_critical(cpu_stat);
}





static void create_adv_obj()
{
    struct legacy_adv_obj_param adv_param = {
        .adv_intv_min = 0x80,
        .adv_intv_max = 0x80,
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
    //  uint8_t FF_NAME[]= {0xff,0xFF};
    uint8_t temp_len;
    temp_len=strlen((char*)&SHORT_NAME[0]);
    if(temp_len>=10) {
        temp_len=10;
    }
    LS_ASSERT(adv_obj_hdl != 0xff);
    uint8_t adv_data_length = ADV_DATA_PACK(advertising_data, 2, GAP_ADV_TYPE_SHORTENED_NAME,     &SHORT_NAME[0], temp_len
                                            ,GAP_ADV_TYPE_COMPLETE_NAME,      &SHORT_NAME[0], temp_len
                                            // ,GAP_ADV_TYPE_MANU_SPECIFIC_DATA, &FF_NAME[0], sizeof(FF_NAME)
                                           );
    dev_manager_start_adv(adv_obj_hdl, advertising_data, adv_data_length, scan_response_data, 0);
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
    uart_tx_busy = false;
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
						//gatt_manager_client_mtu_exch_send(con_idx);
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
        uint16_t temp;
        BLE_connected_flag=1;
        srand (sleep_time);
        temp=rand()%0xffff;
        Set_TOKEN(temp>>12,temp>>8,temp>>4,temp);
        LOG_I("connected! new con_idx = %d", con_idx);
        break;
    case DISCONNECTED:
        BLE_connected_flag=0;
        LOG_I("disconnected! delete con_idx = %d", con_idx);
        if (CONNECTION_IS_SERVER(con_idx))
        {
#if SLAVE_SERVER_ROLE == 1
            disconnect_pattern_send_prepare(con_idx, LS_BLE_ROLE_SLAVE);
            con_idx_server = CON_IDX_INVALID_VAL;
            uart_server_mtu = UART_SERVER_MTU_DFT;
            start_adv();
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
            user_write_req_ind(evt->server_write_req.att_idx,evt->server_write_req.length, evt->server_write_req.value);
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

uint8_t RTC_flag;
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
        memcpy(&MAC_ADDR[0],&addr[0],6);

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
        Button_Gpio_Init();
        uint8_t button_flag=io_read_pin(PB15);
        DELAY_US(200*1000);
        Read_Last_Data();
				
        AT_uart_init();
        ls_app_timer_init();
				
			  //HAL_UART_Receive_IT(&UART_Config,uart_buffer,1);
        HAL_UART_Receive_IT(&UART_Config_AT,uart_2_buffer,1);			// 使能串口2接收中断
			//HAL_UART_Receive_IT(&UART_Config_RFID,uart_3_buffer,1);		// 使能串口3接收中断
        User_Init();

//				HAL_UART_Transmit(&UART_Config_RFID,(uint8_t*)"UART3_OK",sizeof("UART3_OK"),100);
//        if(Check_SW2()==1 && Check_SW1()==0 ) {
//            lock_state[0]=1;
//        }
//        else {
//            lock_state[0]=0;
//        }

        last_lock_state_0=!Check_SW1();   //获取初始锁状态
        last_lock_state_1=!Check_SW2();  //获取初始锁状态
        lock_state[0]= last_lock_state_0 ;
        lock_state[1]= last_lock_state_1 ;

        uint8_t wkup_source = get_wakeup_source();   //获取唤醒源
        LOG_I("wkup_source:%x",wkup_source) ;

        //来自RTC的启动，发送心跳包
        if ((RTC_WKUP & wkup_source) != 0) {
            AT_tset_flag=2;
            RTC_flag=1;
            Set_Task_State(TICK_LOCK_SEND,START);
        }
        //来自按键和锁开关唤醒
        else if ((PB15_IO_WKUP & wkup_source) != 0) {
            AT_tset_flag=2;
            //Set_Task_State(OPEN_LOCK_SEND,START);
            //KEY_ONCE=1;            //由启动按键唤醒
            if(button_flag==0) {
                //启动开锁请求发送
                buzzer_task_flag=1;
                Set_Task_State(OPEN_LOCK_SEND,START);
                KEY_ONCE=1;
                LOG_I("key0") ;
            }
            //由锁开关唤醒
            else {
                LOG_I("key1") ;
                if(lock_state[0]==1) {
                    buzzer_task_flag=1;
                    LOG_I("key11")  ;
                }
                if(lock_state[1]==1) {
                    buzzer_task_flag=1;
                    LOG_I("key12") ;
                }
                Set_Task_State(OPEN_LOCK_DATA_SEND,START);
                look_status_send_count+=2;
                if(look_status_send_count>=3)  look_status_send_count=3;
                //启动锁数据上报命令
            }
        }
        //断电重启
        else if (wkup_source == 0) {
						//rfid_task_flag_1=1;
            //AT_tset_flag=2;
            reset_flag=1;
            Set_Task_State(START_LOCK_SEND,START);
        }
        //RTC_flag=1;
        // AT_tset_flag=0;
        // LOG_I("wkup_source:%x",wkup_source) ;
        //HAL_UART_Transmit(&UART_Config_AT,(unsigned char*)"ATE1\r\n",sizeof("ATE1\r\n"),100);

        if(test_mode_flag!=0xAA) {
            Set_Task_State(TEST_GET_IMEI_VAL,START);
            Set_Task_State(TEST_GET_IMSI_VAL,STOP);
            Set_Task_State(TEST_GET_DB_VAL,STOP);
					
            Set_Task_State(START_LOCK_SEND,STOP);
            Set_Task_State(OPEN_LOCK_SEND,STOP);
            Set_Task_State(TICK_LOCK_SEND,STOP);
            Set_Task_State(OPEN_LOCK_DATA_SEND,STOP);
            Set_Task_State(GET_DB_VAL,STOP);
					
            //START_LOCK_SEND,     		//启动数据上报
            //OPEN_LOCK_SEND,  				//开锁数据请求
            //TICK_LOCK_SEND, 				//心跳包发送
            //OPEN_LOCK_DATA_SEND,    // 20信息上报
            //GET_DB_VAL,   				 	//获取信号强度


        }
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
#if 0
        LOG_I("adv received, addr: %2x:%2x:%2x:%2x:%2x:%2x", evt->adv_report.adv_addr->addr[5],
              evt->adv_report.adv_addr->addr[4],
              evt->adv_report.adv_addr->addr[3],
              evt->adv_report.adv_addr->addr[2],
              evt->adv_report.adv_addr->addr[1],
              evt->adv_report.adv_addr->addr[0]);
#endif
        if (!memcmp(peer_slave_addr0, evt->adv_report.adv_addr->addr, BLE_ADDR_LEN))
        {
            if (init_obj_hdl != 0xff && init_status == INIT_IDLE)
            {
                dev_addr_type = evt->adv_report.adv_addr_type;
                next_connect_addr = (uint8_t*)&peer_slave_addr0[0];
                dev_manager_stop_scan(scan_obj_hdl);
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
