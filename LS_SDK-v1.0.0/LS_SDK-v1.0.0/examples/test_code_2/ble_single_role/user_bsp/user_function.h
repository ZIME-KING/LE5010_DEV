#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "user_main.h"


#define POS_0      0x11
#define POS_0_90   0x10
#define POS_90     0x00
#define POS_90_180 0x01
#define POS_OUT    0xFF




extern uint8_t lockid[18] ;//Ëø16Î»idºÅ
extern uint8_t def_password[16] ; //Ëø16Î»Ëæ»úÃÜÂë
extern uint8_t rand_password[16]; //Ëø16Î»Ô¤ÖÃÃÜÂë
extern uint32_t no_act_count;

extern uint32_t led_open_count;
extern uint8_t  led_open_flag;
extern uint32_t globle_count;
extern uint8_t link_flag;

extern uint8_t sign_flag ;




void User_Processing_data(const uint8_t *p,uint8_t lenght);


 void ls_sleep_enter_LP3(void);
 
 
 void user_init(void);



void user_send_sign_in();
void user_send_led_on();
void user_send_led_off();
void user_send_lock_up();
void user_send_lock_down();

//extern UART_HandleTypeDef UART2_Config;
//extern UART_HandleTypeDef UART1_Config;
//extern uint16_t DYP_distance;


//extern uint8_t lockid[18];
//extern uint8_t rand_password[16]; //Ëø16Î»Ëæ»úÃÜÂë
//extern uint8_t def_password[16];  //Ëø16Î»Ô¤ÖÃÃÜÂë
//extern uint8_t lock_sn[16];  //
//extern uint8_t lock_mac[6];  //
//extern uint8_t reset_flag;

//extern uint8_t  address ;
//extern uint32_t time_count;

//extern uint8_t ble_adv_data[6];

//extern tinyfs_dir_t ID_dir_1;
////extern tinyfs_dir_t ID_dir_2;

//void User_BLE_Ready(void);
//void loop_task(void);

//bool Check_Password(uint8_t *password);
//void auto_mode_function(uint8_t mode);


#endif
