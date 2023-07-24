#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "user_main.h"


#define LL  0x00
#define LH  0x01
#define NL  0x02
#define NH  0x03

extern UART_HandleTypeDef UART2_Config;
extern UART_HandleTypeDef UART1_Config;
extern uint16_t DYP_distance;


extern uint8_t lockid[18];
extern uint8_t rand_password[16]; //Ëø16Î»Ëæ»úÃÜÂë
extern uint8_t def_password[16];  //Ëø16Î»Ô¤ÖÃÃÜÂë
extern uint8_t lock_sn[16];  //
extern uint8_t lock_mac[6];  //
extern uint8_t reset_flag;

extern uint8_t  address ;
extern uint32_t time_count;

extern uint8_t ble_adv_data[2];

extern tinyfs_dir_t ID_dir_1;

extern uint8_t global_set_UAR196_flag;

//extern tinyfs_dir_t ID_dir_2;

void User_BLE_Ready(void);
void loop_task(void);

bool Check_Password(uint8_t *password);
void auto_mode_function(uint8_t mode);


void Enter_Power_Mode_NL(void);
void Enter_Power_Mode_NH(void);
void Enter_Power_Mode_LL(void);
void Enter_Power_Mode_LH(void);


//void transition(StateMachine *machine);

void loop_task_lower_power(void);
void loop_task_normal_power(void);

void loop_task(void);
															
extern int init196_step ;
																	
extern uint8_t now_mode;




#endif
