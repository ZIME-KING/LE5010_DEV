#include "control_cmd.h"
#include "user_main.h"

uint8_t address=0x00;  //485ͨ�ŵ�ַ
uint8_t lockid[18]={'I','N',0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35,0x36};  //��16λid��
uint8_t def_password[16]  ={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};  //��16λ�������
uint8_t rand_password[16] ={0x01,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};  //��16λԤ������
uint8_t lock_sn[16]={0};  // SN
uint8_t lock_mac[6]={0};  // MAC
uint8_t reset_flag;

uint8_t hw_lock_status;     //��״̬ ��ʵӲ���ϵ�  //0x00 ��ʼλ�� ��0x01 ������ �� 0x00 90�� �� 0x11 ���� 0xff �ŵ�
uint8_t tag_lock_status;    //��״̬ Ŀ��
uint8_t lock_mode;          //��ģʽ �Զ����ֶ�
uint8_t ble_close_atc_flag; //

uint8_t err_val=0x00; //
uint8_t vbat_val=0xff;
uint8_t car_val;
uint8_t lock_success_flag=0;   //����Ȩ�ɹ����

//�յ������
//������յ�����������buf
uint8_t* CMD_Processing(uint8_t *p,uint16_t length) {
//uint8_t len;

    static uint8_t send_buf[64];
    uint16_t temp;
    uint16_t *p1;
		
		uint8_t err_val=0x00;
    
		uint8_t command;
    uint8_t data;
    if(*p==0x17) {
        command= *(p+1);
        switch(command) {
        case 0x01:    //��������
            if(length==3 && lock_success_flag==1) {
                data= *(p+2);
                switch(data) {
                case 0x01:
                    tag_lock_status=POS_0;
                    lock_mode= 0x00; 
										moro_task_flag=1;
                    break;

                case 0x02:
                    tag_lock_status=POS_90;
                    lock_mode= 0x00;
										moro_task_flag=1;
                    break;

                case 0x03:
                    tag_lock_status=POS_0;
                    lock_mode= 0x01;
										moro_task_flag=1;
                    break;

                case 0x04:
                    tag_lock_status=POS_90;
                    lock_mode= 0x01;
										moro_task_flag=1;
                    break;

                case 0x05:
                    tag_lock_status=POS_0;
                    lock_mode= 0x00;
										moro_task_flag=1;
                    ble_close_atc_flag=1;
                    break;
                }
            } else {
                err_val=0x05; //���ȴ���
                if(lock_success_flag==0)  err_val=0x0B;
            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x01;
            send_buf[3]= err_val;
            send_buf[4]= tag_lock_status;
            send_buf[5]= vbat_val;
            send_buf[0]= 0x05;
            break;

        case 0x02:      //������״̬

            if(length==2 ) {
								check_sw();check_sw();     //һ��Ҫ2��
                DYP_distance=DYP_distance/10;
                if(DYP_distance>=250) DYP_distance=250;
            } else {
                err_val=0x05;
            }

            send_buf[1]= 0x17;
            send_buf[2]= 0x02;
            send_buf[3]= err_val;          //������
            send_buf[4]= hw_lock_status;   //��⿪��״̬
            send_buf[5]= vbat_val;         //����
            send_buf[6]= car_val;          //�Ƿ��г�
            send_buf[7]= DYP_distance;     //����������
            send_buf[0]= 0x07;
            break;

        case 0x03:

            if(length==5 && lock_success_flag==1 ) {
                Set_buzzer_Task_val(*(p+2),*(p+3),*(p+4));
                buzzer_task_flag=1;   //����������
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x03;
            send_buf[3]= err_val;          //������
            send_buf[4]= *(p+2);   				 //״̬
            send_buf[5]= vbat_val;         //����
            send_buf[0]= 0x05;
            break;

        case 0x04:                           //LED
            if(length==12 && lock_success_flag==1 ) {
                Set_LED_Function_val(*(p+2),
                                     *(p+3),*(p+4),*(p+5),
                                     *(p+6),*(p+7),
                                     *(p+8),*(p+9),*(p+10),*(p+11));
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x04;
            send_buf[3]= err_val;          //������
            send_buf[4]= *(p+2);   				 //״̬
            send_buf[5]= vbat_val;         //����
            send_buf[0]= 0x05;
            break;

        case 0x05:                         //��ȡ�������
            if(length==2) {
                for(uint8_t i; i<16; i++) {
                    srand (time_count+i);
                    rand_password[i]=rand(); 				//��16λ�������
                }
            } else {
                err_val=0x05;
            }

            send_buf[1]= 0x17;
            send_buf[2]= 0x05;
            memcpy(&send_buf[3],&rand_password[0],16);
            send_buf[0]= 18;
            break;

        case 0x06:      //��Ȩ

            if(Check_Password(p+2)) {
                lock_success_flag=1;
                Set_buzzer_Task_val(1,1,1);
                buzzer_task_flag=1;   //����������
            } else {
                Set_buzzer_Task_val(3,1,1);
								buzzer_task_flag=1;   //����������
            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x06;
            send_buf[3]= err_val;
            send_buf[0]= 3;

            break;

        case 0x07:    //����485��ַ
            if(length==3 && lock_success_flag==1) {
                address=*(p+2);
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x07;
            send_buf[3]= err_val;          //������
            send_buf[0]= 0x03;
            break;

        case 0x08:     //дԤ������
            if(length==18 && lock_success_flag==1) {
                for(uint8_t i; i<16; i++) {
                    def_password[i]=*(p+2+i);
                }
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;

            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x08;
            send_buf[3]= err_val;          //������
            send_buf[0]= 0x03;
            break;

        case 0x09:    //д��ID
            if(length==18 && lock_success_flag==1) {
                for(uint8_t i; i<16; i++) {
                    lockid[i+2]=*(p+2+i);
                }
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x09;
            send_buf[3]= err_val;          //������
            send_buf[0]= 0x03;
            break;

        case 0x0A:   //����ID
            send_buf[1]= 0x17;
            send_buf[2]= 0x0A;
            memcpy(&send_buf[3],&lockid[2],16);
            send_buf[0]= 18;
            break;


        case 0x0B:    //��MAC
            send_buf[1]= 0x17;
            send_buf[2]= 0x0B;
            memcpy(&send_buf[3],&lock_mac[0],6);
            send_buf[0]= 8;
            break;


        case 0x0C:    //��485 address
            send_buf[1]= 0x17;
            send_buf[2]= 0x0C;
            send_buf[3]= address;
            send_buf[0]= 3;

            break;

        case 0x0D:    //дSN��
            if(length==18 && lock_success_flag==1) {
                for(uint8_t i; i<16; i++) {
                    lock_sn[i]=*(p+2+i);
                }
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;

            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x0D;
            memcpy(&send_buf[3],&lock_sn[0],16);
            send_buf[0]= 18;
            break;

        case 0x0E:    //��SN��

            send_buf[1]= 0x17;
            send_buf[2]= 0x0E;
            memcpy(&send_buf[3],&lock_sn[0],16);
            send_buf[0]= 18;
            break;

        case 0x0F:    //����ɨ�迪��

            break;

        case 0x10:  //�����豸
            if(length==2 && lock_success_flag==1) {
                reset_flag=1;
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }

            send_buf[1]= 0x17;
            send_buf[2]= 0x10;
            send_buf[3]= err_val;          //������
            send_buf[0]= 0x03;
            break;
        }
    }
    return &send_buf[0];
}




















//typedef struct cavan_user_desc {
//	cavan_timer_t timer;
//	uint8_t lock_salt[16];
//	uint8_t conn_id;
//	bool login;
//	bool connected;
//	bool (*transfer)(struct cavan_user_desc *user, uint8_t *buff, uint8_t length);
//	bool (*check_passwd)(struct cavan_user_desc *user, const void *passwd);
//	bool (*check_permission)(struct cavan_user_desc *user, bool force);
//	void (*send_response)(struct cavan_user_desc *user, uint8_t address, void *buff, uint8_t length);
//} cavan_user_t;


//static uint8_t *command_lock_control(cavan_user_t *user, uint8_t command, uint8_t *args, uint8_t length, uint8_t *rsp)
//{
//	*rsp++ = command;

//	switch (command) {
//	case 0x01:
//		return cavan_process_set_lock_state(user, args, length, rsp);

//  	case 0x02:
//    return cavan_process_get_lock_state(user, args, length, rsp);

//	case 0x03:
//		return cavan_process_find_lock(user, args, length, rsp);

//	case 0x04:
//		return cavan_process_set_led(user, args, length, rsp);

//	case 0x05:
//		return cavan_process_read_passwd(user, args, length, rsp);

//	case 0x06:
//		return cavan_process_login(user, args, length, rsp);

//	case 0x07:
//		return cavan_process_write_address(user, args, length, rsp);

//	case 0x08:
//		return cavan_process_write_passwd(user, args, length, rsp);

//	case 0x09:
//		return cavan_process_write_lock_id(user, args, length, rsp);

//	case 0x0A:
//		return cavan_process_read_lock_id(user, args, length, rsp);

//	case 0x0B:
//		return cavan_process_read_mac(user, args, length, rsp);

//	case 0x0C:
//		rsp = cavan_process_read_address(user, args, length, rsp);
//		break;

//	case 0x0D:
//		return cavan_process_write_sn(user, args, length, rsp);

//	case 0x0E:
//		return cavan_process_read_sn(user, args, length, rsp);

//	case 0x0F:
//		return cavan_process_set_scan(user, args, length, rsp);

//	case 0x10:
//		return cavan_process_reboot(user, args, length, rsp);

//	default:
//		*rsp++ = CAVAN_ERR_InvalidCommand;
//		break;
//	}

//	return rsp;
//}
