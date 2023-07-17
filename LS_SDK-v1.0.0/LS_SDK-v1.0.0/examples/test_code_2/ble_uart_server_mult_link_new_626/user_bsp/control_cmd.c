#include "control_cmd.h"
#include "user_main.h"

uint8_t address=0x00;  //485ͨ�ŵ�ַ
uint8_t lockid[18]= {'I','N',0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x31,0x32,0x33,0x34,0x35,0x36}; //��16λid��
uint8_t def_password[16]  = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff}; //��16λ�������
uint8_t rand_password[16] = {0x01,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff}; //��16λԤ������
uint8_t lock_sn[16]= 				{0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00}; // SN
uint8_t lock_mac[6]= {0}; // MAC
uint8_t reset_flag;

uint8_t hw_lock_status=POS_90;     //��״̬ ��ʵӲ���ϵ�  //0x00 ��ʼλ�� ��0x01 ������ �� 0x00 90�� �� 0x11 ���� 0xff �ŵ�
uint8_t tag_lock_status=POS_90;    //��״̬ Ŀ��
uint8_t lock_mode;          //��ģʽ 1�Զ���0�ֶ�(����) ��2�쳣��ײ����
uint8_t ble_close_atc_flag; //

uint8_t err_val=0x00; //
uint8_t vbat_val=0xff;
uint8_t car_val;
uint8_t lock_success_flag=1;   //����Ȩ�ɹ����

uint16_t first_rise_time=600;  //��λs
uint16_t second_rise_time=30;  //��λs 
uint8_t low_vbat_val=90;     	 //�͵�ѹ����ֵ4.5V    
uint8_t led_sleep_time=10;     //��λs

uint32_t user_time_cont;       //�޲���ʱ�����



uint8_t hw_err_out_time=5;     				//��ʱʱ�䣬С��90��0������90��90
uint8_t hw_err_buzzer_all_time=3;      //������ʱ�� 
uint8_t hw_err_buzzer_on_time=1;       //��Ч     
uint8_t hw_err_buzzer_off_time=0;      //��Ч     




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
		
		user_time_cont=0;
		
		
		//0xAA 
		if(*p==0xAA){

				LOG_I("OTP1:%d",lock_sw.opt1);
        LOG_I("OTP2:%d",lock_sw.opt2);
        LOG_I("OTP3:%d",lock_sw.opt3);
        LOG_I("hw_lock_status:%x",hw_lock_status);
        LOG_I("tag_lock_status:%x",tag_lock_status);
				LOG_I("lock_mode:%d",lock_mode);

		
				if(length==6 && *(p+1)==0xAA &&	*(p+2)==0x11 && *(p+3)==0x22 && *(p+4)==0x33 && *(p+5)==0x44){
						reset_flag=1;
            tinyfs_write(ID_dir_1,RECORD_KEY5,(uint8_t*)"no",sizeof("no"));
            tinyfs_write_through();
				}
				
				
				

//*

//Enter_Power_Mode_NH()
//Enter_Power_Mode_NL()
//Enter_Power_Mode_LL()
//Enter_Power_Mode_LH()


				else if(length==2 && *(p+1)==0xAA){
				Enter_Power_Mode_NH();
						//machine.current_state=STATE_A;
				}
				else if(length==2 && *(p+1)==0xBB){
					Enter_Power_Mode_NL();
				}
				else if(length==2 && *(p+1)==0xCC){
						//machine.current_state=  STATE_C;
						Enter_Power_Mode_LH();
				}
				else if(length==2 && *(p+1)==0xDD){
						Enter_Power_Mode_LL();
						//machine.current_state=STATE_D   ;
				}

//*/

				else if(length==2 && *(p+1)==0x11){
						test_moro_task_flag=1;
				}
				
				else if(length==2 && *(p+1)==0x22){
						test_moro_task_flag=2;
				}
				else{
						err_val=0x05;
						if(length==6 || length==2)  err_val=0x04;
				}
				
				send_buf[1]= *p;
        send_buf[2]= *p+1;
        send_buf[3]= err_val;          //������
				send_buf[0]= 0x03;
		}
		else if(*p==0x00 && length==1){
				//ֻ�����ݴ���
		}
		
    else if(*p==0x17) {
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
                    lock_mode= 0x01;    //�Զ�����ģʽ
                    moro_task_flag=1;
										auto_mode_function(USER_SET);
                    break;

                case 0x04:
                    tag_lock_status=POS_90;
                    lock_mode= 0x01;    //�Զ�����ģʽ
                    moro_task_flag=1;
                    break;

                case 0x05:
                    tag_lock_status=POS_0;
                    lock_mode= 0x00;
                    moro_task_flag=1;
                    ble_close_atc_flag=1;
                    break;
								
								default : /* ��ѡ�� */
										err_val=0x04;
                }
            } else {
                err_val=0x05; //���ȴ���				
                if(lock_success_flag==0)  err_val=0x0B;
            }
						
						if(lock_success_flag==1 && car_val==0x02)    			err_val=0x0C;		
						
            send_buf[1]= 0x17;
            send_buf[2]= 0x01;
            send_buf[3]= err_val;
            send_buf[4]= tag_lock_status;
            send_buf[5]= vbat_val;
            send_buf[0]= 0x05;
            break;

        case 0x02:      //������״̬

            if(length==2 ) {
                //check_sw_();
                //check_sw_();     //һ��Ҫ2��
//                DYP_distance=DYP_distance/10;
//                if(DYP_distance>=250) DYP_distance=250;
//            } else {
//                err_val=0x05;
            }

            send_buf[1]= 0x17;
            send_buf[2]= 0x02;
            send_buf[3]= err_val;           //������
            send_buf[4]= hw_lock_status;    //��⿪��״̬
            send_buf[5]= vbat_val;          //����
            send_buf[6]= vbat_val=0xff;;    //��������
            send_buf[7]= car_val;     			//�Ƿ��г�
						
						//send_buf[8]= DYP_distance;      //����������
						send_buf[8]= 0xEE;      					//���ʹ�ú��ײ��״�
						
						
						send_buf[9]= lock_mode;         //��״̬
						
						send_buf[10]= LED_status;    		// LED
						send_buf[11]= buf[0];        		// LED
						send_buf[12]= buf[1];        		// LED
						send_buf[13]= buf[2];        // LED
						send_buf[14]= max_brightness;// LED
						send_buf[15]= min_brightness;// LED
						send_buf[16]= set_t0;        //	LED		
						send_buf[17]= set_t1;        //	LED
						send_buf[18]= set_t2;        //	LED	
						send_buf[19]= set_t3;        //	LED	
						send_buf[20]= hw_err_buzzer_all_time;        //		
						send_buf[21]= hw_err_buzzer_on_time;        //		
						send_buf[22]= hw_err_buzzer_off_time;        //		
						send_buf[23]= hw_err_out_time;        //		
						send_buf[24]= first_rise_time>>8;        //		
						send_buf[25]= first_rise_time;        //		
						send_buf[26]= second_rise_time>>8;        //		
						send_buf[27]= second_rise_time;        //		
						send_buf[28]= led_sleep_time;        //	
						
            send_buf[0]=28;
            break;

        case 0x03:

            if(length==5 && lock_success_flag==1 ) {
                
								//�������������ȷ					
								if( *(p+2)>=1 && *(p+2)<=9){
									Set_buzzer_Task_val(*(p+2),*(p+3),*(p+4));	
									buzzer_task_flag=1;   //����������
								}else{
									 err_val=0x04;
								}
								
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
                tinyfs_write(ID_dir_1,RECORD_KEY3,(uint8_t*)rand_password,sizeof(rand_password));
                tinyfs_write_through();

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
								err_val=0x00;
						} else {
                Set_buzzer_Task_val(3,1,1);
                buzzer_task_flag=1;   //����������
								err_val=0x06;
            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x06;
            send_buf[3]= err_val;
            send_buf[0]= 3;

            break;

        case 0x07:    //����485��ַ
            if(length==3 && lock_success_flag==1) {
                address=*(p+2);
                tinyfs_write(ID_dir_1,RECORD_KEY0,&address,sizeof(address));
                tinyfs_write_through();
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

                tinyfs_write(ID_dir_1,RECORD_KEY2,(uint8_t*)def_password,sizeof(def_password));
                tinyfs_write_through();

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
                tinyfs_write(ID_dir_1,RECORD_KEY1,(uint8_t*)lockid,sizeof(lockid));
                tinyfs_write_through();


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
                tinyfs_write(ID_dir_1,RECORD_KEY4,(uint8_t*)lock_sn,sizeof(lock_sn));
                tinyfs_write_through();
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

//        case 0x0F:    //����ɨ�迪��
//
///////////////////��ʱ����һ�������ظ�flash��ģʽֵ///////////////////////////
//            reset_flag=1;
//            tinyfs_write(ID_dir_1,RECORD_KEY5,(uint8_t*)"no",sizeof("no"));
//            tinyfs_write_through();

//////////////////////////////////////////////
//            break;

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
				
				
				
				case 0x11:  //���õ͵�����������
            if(length==3 && lock_success_flag==1) {
                low_vbat_val=*(p+2);
								
								tinyfs_write(ID_dir_1,RECORD_KEY8,&low_vbat_val,1);
								tinyfs_write_through();
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }

            send_buf[1]= 0x17;
            send_buf[2]= 0x11;
            send_buf[3]= err_val;          //������
            send_buf[0]= 0x03;
            break;

				case 0x12:  //�����Զ���������
            if(length==6 && lock_success_flag==1) {
                
								first_rise_time= (*(p+2)<<8)+ *(p+3);    //
								second_rise_time=(*(p+4)<<8)+ *(p+5);    
								
								tinyfs_write(ID_dir_1,RECORD_KEY6,(uint8_t*)&first_rise_time,2);
								tinyfs_write(ID_dir_1,RECORD_KEY7,(uint8_t*)&second_rise_time,2);				
                tinyfs_write_through();
								
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }
						
            send_buf[1]= 0x17;
            send_buf[2]= 0x12;
            send_buf[3]= err_val;          //������
            send_buf[0]= 0x03;
            break;

				case 0x13:  //�����쳣״̬������������
            if(length==6 && lock_success_flag==1 ) {
								hw_err_out_time=*(p+5);
								
								hw_err_buzzer_all_time= *(p+2)*10 / (*(p+3)+ *(p+4));
								hw_err_buzzer_on_time= *(p+3);
								hw_err_buzzer_off_time= *(p+4);
								
								Set_buzzer_Task_val( hw_err_buzzer_all_time,hw_err_buzzer_on_time,hw_err_buzzer_off_time);	
								buzzer_task_flag=1;   //����������
								
								LOG_I("hw_err_buzzer_all_time:%d",hw_err_buzzer_all_time);
								
								tinyfs_write(ID_dir_1,RECORD_KEY9,&hw_err_out_time,1);
								tinyfs_write(ID_dir_1,RECORD_KEY10,&hw_err_buzzer_all_time,1);
								tinyfs_write(ID_dir_1,RECORD_KEY11,&hw_err_buzzer_on_time,1);				
								tinyfs_write(ID_dir_1,RECORD_KEY12,&hw_err_buzzer_off_time,1);
				        tinyfs_write_through();
								
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }
            send_buf[1]= 0x17;
            send_buf[2]= 0x13;
            send_buf[3]= err_val;          //������
            send_buf[0]= 0x03;
            break;
						
						
						case 0x14:  //����LED����ʱ��
            if(length==3 && lock_success_flag==1) {
                led_sleep_time=*(p+2);
								
								tinyfs_write(ID_dir_1,RECORD_KEY13,&led_sleep_time,1);
				        tinyfs_write_through();
								
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }

            send_buf[1]= 0x17;
            send_buf[2]= 0x14;
            send_buf[3]= err_val;          //������
            send_buf[0]= 0x03;
            break;
						
						
						
						case 0x15:  //����LED����ʱ��
            if(length==2 && lock_success_flag==1) {
               // led_sleep_time=*(p+2);
								
								global_set_UAR196_flag=1;
//								tinyfs_write(ID_dir_1,RECORD_KEY13,&led_sleep_time,1);
//				        tinyfs_write_through();
								
            } else {
                err_val=0x05;
                if(lock_success_flag==0)  err_val=0x0B;
            }

            send_buf[1]= 0x17;
            send_buf[2]= 0x15;
            send_buf[3]= err_val;          //������
            send_buf[0]= 0x03;
            break;
						
						
						
				default :		
						
					 send_buf[1]= 0x17;
           send_buf[2]= *p+1;
           send_buf[3]= 0x04;          //������
					 send_buf[0]= 0x03;
					 break;	
        }
    }
		else{
					send_buf[1]= *p;
          send_buf[2]= *p+1;
          send_buf[3]= 0x04;          //������
					send_buf[0]= 0x03;
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
