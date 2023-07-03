#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H
#include "user_main.h"

// ״̬������
// ����״̬����״̬
typedef enum {
    STATE_A,  //�� �͹���
    STATE_B,  //�� ����
    STATE_C,  //�� �͹���
		STATE_D   //�� ����
} State;

// ����״̬���ṹ��
typedef struct {
    State current_state;
    void (*state_handler)(void); 			// ״̬������ָ��
		void (*state_once_handler)(void); // ״̬�л�������һ�εĺ���ָ��
} StateMachine;

extern StateMachine machine;


extern UART_HandleTypeDef UART2_Config;
extern UART_HandleTypeDef UART1_Config;
extern uint16_t DYP_distance;


extern uint8_t lockid[18];
extern uint8_t rand_password[16]; //��16λ�������
extern uint8_t def_password[16];  //��16λԤ������
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
