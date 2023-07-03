#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "user_main.h"

// 状态处理函数
// 定义状态机的状态
typedef enum {
    STATE_A,  //立 低功耗
    STATE_B,  //立 正常
    STATE_C,  //倒 低功耗
		STATE_D   //倒 正常
} State;

// 定义状态机结构体
typedef struct {
    State current_state;
    void (*state_handler)(void); 			// 状态处理函数指针
		void (*state_once_handler)(void); // 状态切换后运行一次的函数指针
} StateMachine;

extern StateMachine machine;


extern UART_HandleTypeDef UART2_Config;
extern UART_HandleTypeDef UART1_Config;
extern uint16_t DYP_distance;


extern uint8_t lockid[18];
extern uint8_t rand_password[16]; //锁16位随机密码
extern uint8_t def_password[16];  //锁16位预置密码
extern uint8_t lock_sn[16];  //
extern uint8_t lock_mac[6];  //
extern uint8_t reset_flag;

extern uint8_t  address ;
extern uint32_t time_count;

extern uint8_t ble_adv_data[2];

extern tinyfs_dir_t ID_dir_1;
//extern tinyfs_dir_t ID_dir_2;

void User_BLE_Ready(void);
void loop_task(void);

bool Check_Password(uint8_t *password);
void auto_mode_function(uint8_t mode);

void state_handler_A(void);
void state_handler_B(void);
void state_handler_C(void);
void state_handler_D(void);



#endif
