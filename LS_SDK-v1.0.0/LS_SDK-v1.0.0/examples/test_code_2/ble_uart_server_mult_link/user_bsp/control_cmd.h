#ifndef CONTROL_CMD_H_
#define CONTROL_CMD_H_

#include "user_main.h"

extern uint8_t hw_lock_status;      //��״̬ ��ʵӲ���ϵ� 
extern uint8_t now_lock_status;     //��״̬ ��ǰ ����ֵ
extern uint8_t tag_lock_status;     //��״̬ Ŀ��
extern uint8_t lock_mode;           //��ģʽ
extern uint8_t lock_success_flag;   //����Ȩ�ɹ����


extern uint8_t lockid[18];
extern uint8_t rand_password[16]; //��16λ�������
extern uint8_t def_password[16];  //��16λԤ������
extern uint8_t lock_sn[16];  //
extern uint8_t lock_mac[6];  //
extern uint8_t reset_flag;

extern uint8_t  address ;

uint8_t* CMD_Processing(uint8_t *p,uint16_t length);

#endif
