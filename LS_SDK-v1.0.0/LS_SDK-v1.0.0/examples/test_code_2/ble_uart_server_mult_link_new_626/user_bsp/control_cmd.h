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
extern uint8_t vbat_val;		 //��ص���
extern uint8_t car_val;      //����״̬
extern uint8_t address ;

extern uint16_t first_rise_time;    //
extern uint16_t second_rise_time;     
extern uint8_t low_vbat_val;     //�͵�ѹ����ֵ4.5V   

extern uint8_t hw_err_out_time;     				//��ʱʱ�䣬С��90��0������90��90
extern uint8_t hw_err_buzzer_all_time;      //������ʱ�� 
extern uint8_t hw_err_buzzer_on_time;       //��Ч     
extern uint8_t hw_err_buzzer_off_time;      //��Ч     


extern uint8_t  led_sleep_time;     					//��λs
extern uint32_t user_time_cont;       			  //�޲���ʱ�����

extern uint8_t  send_buf[64];

uint8_t* CMD_Processing(uint8_t *p,uint16_t length);


#endif
