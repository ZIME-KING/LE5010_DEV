#ifndef CONTROL_CMD_H_
#define CONTROL_CMD_H_

#include "user_main.h"

extern uint8_t hw_lock_status;    //��״̬ ��ʵӲ���ϵ� 
extern uint8_t now_lock_status;    //��״̬ ��ǰ ����ֵ
extern uint8_t tag_lock_status;    //��״̬ Ŀ��
extern uint8_t lock_mode;          //��ģʽ

void CMD_Processing(uint8_t *p);

#endif
