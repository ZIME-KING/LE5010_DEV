#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"

#define VER 0x01



// 0�����͹���ģʽ��1������������ģʽ��2�ŵ��͹���ģʽ ��3�ŵ���������ģʽ
uint8_t user_status=0;             //�ϵ������Ϊ����ģʽ


UART_HandleTypeDef UART2_Config;
UART_HandleTypeDef UART1_Config;


void ls_uart1_init(void);
void ls_uart2_init(void);
void User_io_Init(void);

uint32_t time_count=0x00;
uint8_t ble_adv_data[2]= {VER,0xFF};

//uint8_t cmd_buf[64]; //���յ��������

uint16_t check_car_mode_time_out=0;      //�������ģ����Ӧ��ʱʱ��

uint8_t VIN_12=0;
uint8_t global_set_UAR196_flag=0;        //����У׼���
uint8_t UAR196_Result=0Xff;     						 //���ս��



#define RISE_TIME 300


//ÿ���ճɹ�һ������һ������
void Uart_Data_Processing() {
    uint8_t *p;

    if(frame[uart_frame_id].status!=0) {    			//���յ����ݺ�status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        LOG_HEX((uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length);
        //���յ������� ��uart2  ͸��
        if (frame[uart_frame_id].buffer[0]==0xF5 &&
                frame[uart_frame_id].buffer[1]==address  &&
                frame[uart_frame_id].buffer[frame[uart_frame_id].length-1]==0xF6)
        {
            if(crc16_ccitt(frame[uart_frame_id].buffer, frame[uart_frame_id].length-2)==0x00) {
                p=CMD_Processing(&frame[uart_frame_id].buffer[3],(frame[uart_frame_id].length-6));     //��4λ��ʼ��Ӧ������
                //UART_Transmit_Str(&UART1_Config,p);
                HAL_UART_Transmit_IT(&UART1_Config,p,*p);
            }
        }
        frame[uart_frame_id].status=0;					//���������ݺ�status ��0;
    }
}


#ifdef URA196      //���ײ�
short int URA196_data[12]; // ����һ��int16���͵�����洢����

int Uart2_Data_Processing_URA196(char* input,uint16_t len) {
    // char* input = argv[1];
    char temp[128];
    char* header = "+MRSTATUS:";
    char* footer = "\r\n";
    char* start;
    char* end;
    char* token;
    char* delim = ",";
    int count = 0;

    memcpy(temp, input, len);
    // ���Ұ�ͷ��λ��
    start = strstr(temp, header);
    if (start == NULL) {
        //LOG_I("��Ч�İ�ͷ\n");
        return 1;
    }
		
    // ��λ����ͷ֮���λ��
    start += strlen(header);
    // ���Ұ�β��λ��
    end = strstr(temp, footer);
    if (end == NULL) {
        //LOG_I("��Ч�İ�β\n");
        return 1;
    }
    // ͨ��ѭ����������������
    token = strtok(start, delim);
    while (token != NULL && token < end && count < 12) {
        URA196_data[count] = (short int)strtol(token, NULL, 10);
        token = strtok(NULL, delim);
        count++;
    }

		LOG_I("���յ�״̬����");

    // ��ӡת���������
//    for (int i = 0; i < count; i++) {
//        LOG_I("data[%d]: %d", i, URA196_data[i]);
//    }
    return 0;
}
void Uart2_Data_Processing() {

    if(frame_2[uart_2_frame_id].status!=0) {   			//���յ����ݺ�status=1;
        frame_2[uart_frame_id].buffer[frame_2[uart_frame_id].length]='\0';
        check_car_mode_time_out=0;
        LOG_I("%s",(uint8_t*)frame_2[uart_frame_id].buffer);

				if(strstr((char*)frame_2[uart_frame_id].buffer, "+MRCALI:") != NULL){
						LOG_I("У׼�ɹ�");
						UAR196_Result=2;
				}
				
				if(strstr((char*)frame_2[uart_frame_id].buffer, "REBOOT_OK") != NULL){
						LOG_I("�����ɹ�");
						UAR196_Result=1;
				}


        Uart2_Data_Processing_URA196((char*)frame_2[uart_frame_id].buffer,frame_2[uart_frame_id].length);
        if(URA196_data[1]==0x01) {		//��λռ��
            car_val=0x01;
        }
        else if(URA196_data[1]==0x00) { //��λ����
            car_val=0xff;
        }
				LOG_I("car_val:%X",car_val);
				
        frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
    }
}


void Check_URA196() {
    static uint16_t count;
    count++;
    count=count%100;

    if(count==0) {
        HAL_UART_Transmit(&UART2_Config,(uint8_t*)"AT+MRSTATUS\r\n",strlen("AT+MRSTATUS\r\n"),10);
    }
    if(count==1) {
        check_car_mode_time_out++;
        //LOG_I("check_DYP_time_out:%d",check_DYP_time_out);
        if(check_car_mode_time_out>20) {
            car_val=0x02;   //ģ��û���ҵ�
            LOG_I("car_val:%d",car_val);
        }
    }
}

//������ʽ��ѯ���ײ�����
void Check_URA196_wait() {
    static uint16_t count;

    io_cfg_output(PA05);   //
    io_write_pin(PA05,1);
		io_pull_write(PA05,IO_PULL_DISABLE);
    ls_uart2_init();
    Uart_2_Time_Even();				//����2ɨ��

//    HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// ����ʹ�ܴ���2�����ж�
    HAL_UART_Transmit(&UART2_Config,(uint8_t*)"AT+MRSTATUS\r\n",strlen("AT+MRSTATUS\r\n"),10);



    //1ms ѭ������
    for(int i=0; i<500; i++) {
        Uart_2_Time_Even();				//����2ɨ��
        DELAY_US(1000);
        if(frame_2[uart_2_frame_id].status!=0) {   			//���յ����ݺ�status=1;

            frame_2[uart_frame_id].buffer[frame_2[uart_frame_id].length]='\0';
            LOG_I("%s",(uint8_t*)frame_2[uart_frame_id].buffer);
            //Uart2_Data_Processing_URA196((char*)frame_2[uart_frame_id].buffer,frame_2[uart_frame_id].length);

            if(Uart2_Data_Processing_URA196((char*)frame_2[uart_frame_id].buffer,frame_2[uart_frame_id].length)!= 0) {
                car_val=0xFE;   //���ݴ���
                LOG_I("URA196_error");
            }
            else {
//                DYP_distance= (frame_2[uart_2_frame_id].buffer[1]<<8) +  (frame_2[uart_2_frame_id].buffer[2]);
//                LOG_I("DYP_distance:%d",DYP_distance);
                if(URA196_data[1]==0x01) {		//��λռ��
                    car_val=0x01;
                }
                else if(URA196_data[1]==0x00) { //��λ����
                    car_val=0xff;
                }
								LOG_I("car_val:%d",car_val);
            }
            frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
            break;
        }
				if(i==499)LOG_I("URA196_NO_FIND");
        car_val=0x02;        //û�ж���ģ�鲻����
    }

//	io_write_pin(PA05,0);
//	io_cfg_disable(PA05);
//	io_pull_write(PA05,IO_PULL_DOWN);

    HAL_UART_DeInit(&UART2_Config);
    uart2_io_deinit();
    io_cfg_disable(PA15);
    io_cfg_disable(PA14);
    io_pull_write(PA15,IO_PULL_DOWN);
    io_pull_write(PA14,IO_PULL_DOWN);
}

#endif

#ifdef DYPA06
uint16_t DYP_distance;

//ÿ���ճɹ�һ������һ������  ���������
void Uart2_Data_Processing() {
    if(frame_2[uart_2_frame_id].status!=0) {   			//���յ����ݺ�status=1;
        //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
        if(frame_2[uart_2_frame_id].buffer[0] != 0xff) {

        } else {
            check_DYP_time_out=0;
        }

        //LOG_HEX((uint8_t*)frame_2[uart_frame_id].buffer,frame_2[uart_frame_id].length);
        DYP_distance= (frame_2[uart_2_frame_id].buffer[1]<<8) +  (frame_2[uart_2_frame_id].buffer[2]);
        LOG_I("DYP_distance:%d",DYP_distance);
        if(DYP_distance<=500) {
            car_val=0x01;
        }
        else {
            car_val=0xff;
        }
        //LOG_I("car_val:%d",car_val);

        frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
    }
}

void Check_DYP_distance() {
    static uint16_t count;
    count++;
    count=count%10;

    if(count==0) {
        HAL_UART_Transmit(&UART2_Config,(uint8_t*)"123456789",10,10);
    }
    if(count==1) {
        check_DYP_time_out++;
        //LOG_I("check_DYP_time_out:%d",check_car_mode_time_out);
        if(check_car_mode_time_out>2) {
            car_val=0x02;   //ģ��û���ҵ�
            //LOG_I("car_val:%d",car_val);
        }
    }
}

//������ʽ��ѯ����������
void Check_DYP_distance_wait() {
    static uint16_t count;

//    io_cfg_output(PA05);   //  ������
//    io_write_pin(PA05,1);
//		io_pull_write(PA05,IO_PULL_DISABLE);
    ls_uart2_init();


    HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// ����ʹ�ܴ���2�����ж�
    HAL_UART_Transmit(&UART2_Config,(uint8_t*)"123456789",10,10);



    //1ms ѭ������
    for(int i=0; i<200; i++) {
        Uart_2_Time_Even();				//����2ɨ��
        DELAY_US(1000);
        if(frame_2[uart_2_frame_id].status!=0) {   			//���յ����ݺ�status=1;
            //HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)frame[uart_frame_id].buffer,frame[uart_frame_id].length,100);
            if(frame_2[uart_2_frame_id].buffer[0] != 0xff) {
                car_val=0xFE;   //ģ��û���ҵ�
                LOG_I("DYP_NOT_error");
            }
            else {
                DYP_distance= (frame_2[uart_2_frame_id].buffer[1]<<8) +  (frame_2[uart_2_frame_id].buffer[2]);
                LOG_I("DYP_distance:%d",DYP_distance);
                if(DYP_distance<=500) {
                    car_val=0x01;
                }
                else {
                    car_val=0xff;
                }
            }
            frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
            break;
        }
        car_val=0x02;        //û�ж���ģ�鲻����
    }

//	io_write_pin(PA05,0);
//	io_cfg_disable(PA05);
//	io_pull_write(PA05,IO_PULL_DOWN);

    HAL_UART_DeInit(&UART2_Config);
    uart2_io_deinit();
    io_cfg_disable(PA15);
    io_cfg_disable(PA14);
    io_pull_write(PA15,IO_PULL_DOWN);
    io_pull_write(PA14,IO_PULL_DOWN);
}

#endif


void Auto_close_sleep() {
    //if(LED_status==0x01) {
        user_time_cont++;
        if(user_time_cont>=led_sleep_time*10) {
            LOG_I("Enter_LOW_Power_Mode");
            LED_status=0x02;
            if(tag_lock_status==POS_0) {
                Enter_Power_Mode_LL();
            }
            else if(tag_lock_status==POS_90) {
                Enter_Power_Mode_LH();
            }
        }
//    } else {
//        user_time_cont=0;
//    }
}

//03�����Զ���������
void auto_mode_function(uint8_t mode) {

    static uint16_t no_car_time;
    static uint16_t find_car_time;
    static uint16_t rise_time;


    static uint16_t count_time;
		count_time++;


    static uint8_t once_flag=0xff;

    if(once_flag==0xff) {
        once_flag=0xAA;
        rise_time= second_rise_time*1000;
    }

    if(mode==USER_SET) {
        rise_time=first_rise_time*1000;
    }

    else if(mode==USER_RUN && lock_mode==0x01) {

				if(count_time%5000==1){
					LOG_I("find_car_time:%d",find_car_time);
					LOG_I("no_car_time:%d",no_car_time);
					LOG_I("rise_time:%d",rise_time);
				}
        //�޳�
        if(car_val==0xff) {
            find_car_time=0;
            no_car_time+=20;
        }
        //�г�
        else if(car_val==0x01) {
            find_car_time+=20;
            no_car_time=0;

            if(find_car_time>=100) {
                rise_time=	second_rise_time*10;
            }
        }

        if(no_car_time>=rise_time) {
            tag_lock_status=POS_90;
            moro_task_flag=1;
            lock_mode=0x00;

            LOG_I("no cat_time_out");

        }
    } else {
        //rise_time=0;
        no_car_time=0;
        find_car_time=0;
    }
}

//������ѹʱ�����
void err_mode_function() {

    static uint16_t count;
    static uint16_t time_out;

//	static uint8_t last_lock_mode=0x01;
    count++;

    //1s
    if(count%2==1) {
        if(count>=65535) {
            count=51;
        }
        //���δ����
        if(moro_task_flag==0) {
            if(hw_lock_status!=tag_lock_status ) {
                // last_lock_mode=lock_mode;
                lock_mode=0x02;  //err
            }
            else {
                // lock_mode=last_lock_mode ;
            }
        }
    }
    if(lock_mode==0x02) {
        //	LOG_I("time_out:%d",time_out);
        if(hw_lock_status==tag_lock_status) {
            if(time_out>=hw_err_out_time*10  && count>=100) {
                lock_mode=0x01;
            }
            else {
                lock_mode=0x00;
            }
            time_out=0;
        }
        if(time_out>=(hw_err_out_time*10)+1  && count>=100) {
            tag_lock_status=POS_0;
        }

        //��ʼ����
        Set_buzzer_Task_val( hw_err_buzzer_all_time,hw_err_buzzer_on_time,hw_err_buzzer_off_time);
        buzzer_task_flag=1;   //����������


        if(tag_lock_status ==POS_90) {


            // LOG_I("time_out:%d",time_out);

            switch(hw_lock_status) {

            case POS_0_90:
                time_out++;
                //LOG_I("time_out:%d",time_out);
                moro_task_flag=1;
                break;

            case POS_0:
                time_out++;
                moro_task_flag=1;

                break;

            case POS_90_180:
                time_out=0;
                moro_task_flag=1;

                break;


            case POS_OUT:
                time_out=0;
                moro_task_flag=1;

                break;
            }
        }

        else if(tag_lock_status==POS_0) {
            switch(hw_lock_status) {
            case POS_90:
                moro_task_flag=1;

                break;

            case POS_0_90:
                moro_task_flag=1;

                break;

            case POS_0:
                moro_task_flag=1;

                break;

            case POS_90_180:
                moro_task_flag=1;

                break;

            case POS_OUT:
                moro_task_flag=1;

                break;

            }
        }
        else {
            time_out=0;
        }
    }
}




//�����ŵ�
void NL_io_init() {
    io_cfg_output(PA05);				//  �������
    io_write_pin(PA05,1);

    io_cfg_output(SW_EN_1);			//
    io_write_pin(SW_EN_1,0);

    io_cfg_output(SW_EN_2);   	//
    io_write_pin(SW_EN_2,0);

    io_cfg_input(SW_IN_1);			//
    io_cfg_input(SW_IN_2);
		
		io_cfg_disable (PB02);
    io_pull_write(SW_EN_2,IO_PULL_DOWN);   //rs485 off
		
		io_cfg_disable (PA08);
    io_pull_write(SW_EN_2,IO_PULL_DOWN);  //������� off
		
		
		LED_Init();
}
void NH_io_init() {


//		io_cfg_output(PA05);				//  �������
//    io_write_pin(PA05,1);


    io_cfg_disable (PA05);
    io_pull_write(PA05,IO_PULL_DOWN);

    io_cfg_output(SW_EN_1);   	//
    io_write_pin(SW_EN_1,0);

    io_cfg_output(SW_EN_2);   	//
    io_write_pin(SW_EN_2,0);

    io_cfg_input(SW_IN_1);			//
    io_cfg_input(SW_IN_2);
		
		
				io_cfg_disable (PB02);
    io_pull_write(SW_EN_2,IO_PULL_DOWN);   //rs485 off
		
		io_cfg_disable (PA08);
    io_pull_write(SW_EN_2,IO_PULL_DOWN);  //������� off
		
		LED_Init();
}

void LH_io_init() {

    io_cfg_disable (PA05);
    io_pull_write(PA05,IO_PULL_DOWN);
		
		
//		io_cfg_output(PA05);				//  �������
//    io_write_pin(PA05,1);

    io_cfg_output(SW_EN_1);   	//
    io_write_pin(SW_EN_1,0);

    io_cfg_output(SW_EN_2);   	//
    io_write_pin(SW_EN_2,0);

    io_cfg_input(SW_IN_1);			//
    io_cfg_input(SW_IN_2);
		
		
		
		io_cfg_disable (PB02);
    io_pull_write(SW_EN_2,IO_PULL_DOWN);   //rs485 off
		
		io_cfg_disable (PA08);
    io_pull_write(SW_EN_2,IO_PULL_DOWN);  //������� off
				
		LED_DeInit();
}

void LL_io_init() {

//    io_cfg_disable (PA05);
//    io_pull_write(PA05,IO_PULL_DOWN);

//User_io_Init();


    io_cfg_output(PA05);				//  �������
    io_write_pin(PA05,1);


    io_cfg_disable (SW_EN_1);
    io_pull_write(SW_EN_1,IO_PULL_DOWN);

    io_cfg_disable (SW_EN_2);
    io_pull_write(SW_EN_2,IO_PULL_DOWN);

    io_cfg_disable(SW_IN_1);   			//
    io_cfg_disable(SW_IN_2);
		
		
		
		io_cfg_disable (PB02);
    io_pull_write(SW_EN_2,IO_PULL_DOWN);   //rs485 off
		
		io_cfg_disable (PA08);
    io_pull_write(SW_EN_2,IO_PULL_DOWN);  //������� off
		
		LED_DeInit();		
		uart1_io_deinit();
		uart2_io_deinit();
		HAL_UART_DeInit(&UART1_Config);
		HAL_UART_DeInit(&UART2_Config);
		Moto_IO_DeInit();
		HAL_ADC_DeInit(&hadc);
		
		io_pull_write(PC01,IO_PULL_DISABLE);
    io_cfg_input(PC01);

		
		
//		uart3_io_deinit();		
}






void User_io_Init() {
    LSGPIOA->MODE = 0;
    LSGPIOA->IE = 0;
    LSGPIOA->OE = 0;
    LSGPIOA->OT = 0;
    LSGPIOA->PUPD = 0xAAAAAA88;  //pa00,PA02����������������ȫ������
    LSGPIOB->MODE &= 0x3c00;  //3C00
    LSGPIOB->IE = 0;
    LSGPIOB->OE = 0;
    LSGPIOB->OT = 0;
    // LSGPIOB->PUPD = 0x2800;
    LSGPIOB->PUPD =  0x2AA96AAA;	//	PB15 ����	 AAA9 6AAA
		
		
		io_pull_write(PC01,IO_PULL_DISABLE);
    io_cfg_input(PC01);
}


void User_Print_Log() {
    static uint8_t last_hw_lock_status;    //������λ��״̬
    static uint8_t last_current_state;     //ϵͳ����״̬
    static uint8_t last_lock_mode;         //��״̬��

//    if(last_current_state!=machine.current_state) {
//        last_current_state=machine.current_state;
//        LOG_I("current_state:%d",machine.current_state);
//    }
    if(last_hw_lock_status!=hw_lock_status) {
        last_hw_lock_status=hw_lock_status;
        LOG_I("OTP1:%d",lock_sw.opt1);
        LOG_I("OTP2:%d",lock_sw.opt2);
        LOG_I("OTP3:%d",lock_sw.opt3);
        LOG_I("hw_lock_status:%x",hw_lock_status);
        LOG_I("tag_lock_status:%x",tag_lock_status);
				LOG_I("lock_mode:%d",lock_mode);
    }
    if(last_lock_mode!=lock_mode) {
        last_lock_mode=lock_mode;
        LOG_I("lock_mode:%d",lock_mode);
    }
    //LOG_I("vbat_val: %d vbat_val",vbat_val);
}
//��������״̬�ŵ�ģʽ
void Enter_Power_Mode_NL(void) {
		user_time_cont=0;

		//ʹ��builtin_timer_stop��ͣ��ʱ�����ܽ���͹���ģʽ ��ʹ��builtin_timer_delete
	
		builtin_timer_delete(user_event_timer_inst_0);   //�رն�ʱ��0
		builtin_timer_delete(user_event_timer_inst_1);   //�رն�ʱ��1
		builtin_timer_delete(user_event_timer_inst_2);   //�رն�ʱ��2

		user_event_timer_inst_0 =builtin_timer_create(ls_user_event_timer_cb_0);
    builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);  //������ʱ��0
		
		user_event_timer_inst_1 =builtin_timer_create(ls_user_event_timer_cb_1);
    builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);  //������ʱ��1

    tag_lock_status=POS_0;    //Ŀ��λ��
    LOG_I("NL\n");
		
		
		LED_Init();
    Moto_IO_Init();
		ls_uart1_init();
    ls_uart2_init();
    User_ADC_Init();

    HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// ����ʹ�ܴ���2�����ж�
    HAL_UART_Receive_IT(&UART1_Config,uart_buffer,1);		// ����ʹ�ܴ���1�����ж�	
}
//��������״̬����ģʽ
void Enter_Power_Mode_NH(void) {
    user_time_cont=0;

		builtin_timer_delete(user_event_timer_inst_0);   //�رն�ʱ��0
		builtin_timer_delete(user_event_timer_inst_1);   //�رն�ʱ��1
		builtin_timer_delete(user_event_timer_inst_2);   //�رն�ʱ��2

		user_event_timer_inst_0 =builtin_timer_create(ls_user_event_timer_cb_0);
    builtin_timer_start(user_event_timer_inst_0, USER_EVENT_PERIOD_0, NULL);  //������ʱ��0
		
		user_event_timer_inst_1 =builtin_timer_create(ls_user_event_timer_cb_1);
    builtin_timer_start(user_event_timer_inst_1, USER_EVENT_PERIOD_1, NULL);  //������ʱ��1

    tag_lock_status=POS_90;   //Ŀ��λ��
    LOG_I("NH\n");
		LOG_I("tag_lock_status%d\n",tag_lock_status);
		
		LED_Init();
    Moto_IO_Init();
		ls_uart1_init();
    ls_uart2_init();
    User_ADC_Init();

    HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// ����ʹ�ܴ���2�����ж�
    HAL_UART_Receive_IT(&UART1_Config,uart_buffer,1);		// ����ʹ�ܴ���1�����ж�
}
//����͹�������ģʽ
void Enter_Power_Mode_LH(void) {
		builtin_timer_delete(user_event_timer_inst_0);   //�رն�ʱ��0
		builtin_timer_delete(user_event_timer_inst_1);   //�رն�ʱ��1
		builtin_timer_delete(user_event_timer_inst_2);   //�رն�ʱ��2

		user_event_timer_inst_2 =builtin_timer_create(ls_user_event_timer_cb_2);
    builtin_timer_start(user_event_timer_inst_2, USER_EVENT_PERIOD_2, NULL);  //������ʱ��2
		
    tag_lock_status=POS_90;   //Ŀ��λ��
    LOG_I("LH\n");
    gap_manager_disconnect(user_conid, 0x13);    //������������
		
		LED_DeInit();		
		Moto_IO_DeInit();
		HAL_UART_DeInit(&UART1_Config);
		HAL_UART_DeInit(&UART2_Config);
		HAL_ADC_DeInit(&hadc);
}
//����͹��ķŵ�ģʽ
void Enter_Power_Mode_LL(void) {

		builtin_timer_delete(user_event_timer_inst_0);   //�رն�ʱ��0
		builtin_timer_delete(user_event_timer_inst_1);   //�رն�ʱ��1
		builtin_timer_delete(user_event_timer_inst_2);   //�رն�ʱ��2

		user_event_timer_inst_2 =builtin_timer_create(ls_user_event_timer_cb_2);
    builtin_timer_start(user_event_timer_inst_2, USER_EVENT_PERIOD_2, NULL);  //������ʱ��2

    tag_lock_status=POS_0;   //Ŀ��λ��
    LOG_I("LL\n");

		gap_manager_disconnect(user_conid, 0x13);    	//������������



		LED_DeInit();		
		Moto_IO_DeInit();
		HAL_UART_DeInit(&UART1_Config);
		HAL_UART_DeInit(&UART2_Config);
		HAL_ADC_DeInit(&hadc);
}


//�޲�����ʱ����͹���ģʽ
//����͹���ģʽʱ�����Ͽ�ble����

void Init_UAR196(){
 static int step=0;
    static int count=0;
    static int count_out=0;
    static int wait_time=50;     //20ms��� 50Ϊ 1s���

//    if(global_set_UAR196_flag==0) {
//				global_seting_UAR196_flag=0;
//        return;
//    }
		if(step>=2){
			global_set_UAR196_flag=0;
			step=0;
		}
	
		
//		global_seting_UAR196_flag=1;
    count++;
    if(count%wait_time==0) {
						 LOG_I("step=%d",step);
				switch(step) {
        case 0:
            if(UAR196_Result==1) {
                UAR196_Result=0xff;
                step++;
                count_out=0;
            }
            else {
                count_out++;
                if(count_out>5) {
                    count_out=0	;
                    step++;
                }                            
                HAL_UART_Transmit(&UART2_Config,(uint8_t*)"AT+MRRB\r\n",strlen("AT+MRRB\r\n"),50);     //������λ
            }
            break;


        case 1:
            if(UAR196_Result==2) {
                UAR196_Result=0xff;
                step++;
                count_out=0;
            }
            else {
                count_out++;
                if(count_out>20) {          //����Ҫ20s����
                    count_out=0	;
                    step++;
                }
								if(count_out%5==1)HAL_UART_Transmit(&UART2_Config,(uint8_t*)"AT+MRCALI\r\n",strlen("AT+MRCALI\r\n"),50);
            }
            break;
			}
		}
}
void loop_task_normal_power() {

    static uint16_t time_count;
		static uint16_t temp_time_count;
		
    time_count++;
    if(time_count%100==0) {
        LOG_I("20ms");
    }

    Auto_close_sleep();
    Moto_Task();              //�������
    Test_Moto_Task();
    Uart2_Data_Processing();  //����������
    Uart_Data_Processing();   //RS485���ݴ���
    Buzzer_Task_100();
    if(tag_lock_status==POS_0) {
				if(global_set_UAR196_flag==0){
						Check_URA196();						//�ŵ�״̬����Ҫ��������״̬
				}
				else{
						Init_UAR196();
				}
				auto_mode_function(USER_RUN);
    }
    else if(tag_lock_status==POS_90) {
        err_mode_function();
    }
    if(reset_flag==0)HAL_IWDG_Refresh();	 	 //ι��
    vbat_val=Get_ADC_value()*20/1000;

    User_Print_Log();
//		LOG_I("20ms");

}

//500ms
void loop_task_lower_power() {
    static uint16_t time_count;
    time_count++;
    if(time_count%10==0) {
        LOG_I("500ms");
    }
    if(tag_lock_status==POS_0) {
        Check_URA196_wait();			 	
				for(uint8_t i=0;i<25;i++){         //25*20ms =500ms
					auto_mode_function(USER_RUN);    // �Զ�����auto_mode_function ͬ����ͨģʽ��20ms��ʱ�� 
				}
				if(tag_lock_status!=POS_0){
//					 Enter_Power_Mode_NH();
				}
    }
//    else if(tag_lock_status==POS_90) {
//        check_sw_wait();							//
//        //����ĵ͹���״̬��,ͨ�������ж��ƻ��Ĳ�����������ģʽ ����ת̬
//        if(hw_lock_status!=tag_lock_status) {
//            Enter_Power_Mode_NH();
//        }
//    }
    User_Print_Log();
}

void ls_uart2_init(void)
{
    uart2_io_init(PA15, PA14);
    io_pull_write(PA14, IO_PULL_UP);

    io_pull_write(PA15, IO_PULL_DISABLE);

    UART2_Config.UARTX = UART2;
    UART2_Config.Init.BaudRate = UART_BAUDRATE_9600;
    UART2_Config.Init.MSBEN = 0;
    UART2_Config.Init.Parity = UART_NOPARITY;
    UART2_Config.Init.StopBits = UART_STOPBITS1;
    UART2_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART2_Config);
}


void ls_uart1_init(void)
{
    uart1_io_init(PA12, PA13);
    io_pull_write(PA13, IO_PULL_UP);

    io_pull_write(PA12, IO_PULL_DISABLE);


    UART1_Config.UARTX = UART1;
    UART1_Config.Init.BaudRate = UART_BAUDRATE_115200;
    UART1_Config.Init.MSBEN = 0;
    UART1_Config.Init.Parity = UART_NOPARITY;
    UART1_Config.Init.StopBits = UART_STOPBITS1;
    UART1_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART1_Config);
}

//������룬��������
bool Check_Password(uint8_t *password)
{
    static struct cavan_sha_context context;
    u8 digest[MD5_DIGEST_SIZE];
    cavan_md5_init_context(&context);
    cavan_sha_init(&context);
    cavan_sha_update(&context, def_password, 16);
    cavan_sha_update(&context, rand_password, 16);
    cavan_sha_update(&context, &lockid[2], 16);
    cavan_sha_finish(&context, digest);

    LOG_I("password:");
    LOG_HEX(digest,16);

    return memcmp(digest, password, MD5_DIGEST_SIZE) == 0;
}



tinyfs_dir_t ID_dir_1;
void Read_Last_Data() {
    uint8_t *p;
    uint8_t tmp[18];
    uint16_t length_0 = 1;
    uint16_t length_1 = 18;
    uint16_t length_2 = 16;
    uint16_t length_3 = 16;
    uint16_t length_4 = 16;
    uint16_t length_5 = 3;

    uint16_t length_6 = 2;
    uint16_t length_7 = 2;
    uint16_t length_8 = 1;

    uint16_t length_9 = 1;
    uint16_t length_10 = 1;
    uint16_t length_11 = 1;
    uint16_t length_12 = 1;

    uint16_t length_13 = 1;

    tinyfs_mkdir(&ID_dir_1,ROOT_DIR,53);  //����Ŀ¼

    tinyfs_read(ID_dir_1,RECORD_KEY5,tmp,&length_5);//����tmp��
    LOG_I("flash_init");
    LOG_I("%s",tmp);

    if(strncmp("ok",(char*)tmp,sizeof("ok"))!=0) {
        LOG_I("write_init_ok");
        tinyfs_write(ID_dir_1,RECORD_KEY5,(uint8_t*)"ok",sizeof("ok"));

        tinyfs_write(ID_dir_1,RECORD_KEY0,&address,1);
        tinyfs_write(ID_dir_1,RECORD_KEY1,(uint8_t*)lockid,sizeof(lockid));
        tinyfs_write(ID_dir_1,RECORD_KEY2,&def_password[0],16);
        tinyfs_write(ID_dir_1,RECORD_KEY3,&rand_password[0],16);
        tinyfs_write(ID_dir_1,RECORD_KEY4,(uint8_t*)lock_sn,sizeof(lock_sn));

        //p=(uint8_t*)&first_rise_time;
        tinyfs_write(ID_dir_1,RECORD_KEY6,(uint8_t*)&first_rise_time,2);
        //p=(uint8_t*)&second_rise_time;
        tinyfs_write(ID_dir_1,RECORD_KEY7,(uint8_t*)&second_rise_time,2);
        //p=(uint8_t*)&low_vbat_val;
        tinyfs_write(ID_dir_1,RECORD_KEY8,&low_vbat_val,1);

        //p=(uint8_t*)&hw_err_out_time;
        tinyfs_write(ID_dir_1,RECORD_KEY9,&hw_err_out_time,1);
        //p=(uint8_t*)&hw_err_buzzer_all_time;
        tinyfs_write(ID_dir_1,RECORD_KEY10,&hw_err_buzzer_all_time,1);
        //p=(uint8_t*)&hw_err_buzzer_on_time;
        tinyfs_write(ID_dir_1,RECORD_KEY11,&hw_err_buzzer_on_time,1);
        //p=(uint8_t*)&hw_err_buzzer_off_time;
        tinyfs_write(ID_dir_1,RECORD_KEY12,&hw_err_buzzer_off_time,1);

        tinyfs_write(ID_dir_1,RECORD_KEY13,&led_sleep_time,1);

        tinyfs_write_through();
    }

    tinyfs_read(ID_dir_1,RECORD_KEY0,tmp,&length_0);//����tmp��
    address= tmp[0];
    LOG_I("485_address:%x",address);

    tinyfs_read(ID_dir_1,RECORD_KEY1,tmp,&length_1);//����tmp��
    memcpy (&lockid[0], &tmp[0], 18);
    LOG_I("ID:%s",lockid);

    tinyfs_read(ID_dir_1,RECORD_KEY2,tmp,&length_2);//����tmp��
    memcpy (&def_password[0], &tmp[0], 16);
    LOG_I("def_password:");
    LOG_HEX(&def_password[0],16);

    tinyfs_read(ID_dir_1,RECORD_KEY3,tmp,&length_3);//����tmp��
    memcpy (&rand_password[0], &tmp[0], 16);
    LOG_I("rand_password:");
    LOG_HEX(&rand_password[0],16);

    tinyfs_read(ID_dir_1,RECORD_KEY4,tmp,&length_4);//����tmp��
    memcpy (&lock_sn, &tmp[0],16);
    LOG_I("lock_sn:");
    LOG_HEX(&lock_sn[0],16);

    tinyfs_read(ID_dir_1,RECORD_KEY6,tmp,&length_6);//����tmp��
    memcpy (&first_rise_time, &tmp[0], 2);
    LOG_I("first_rise_time:");
    LOG_I("%d",first_rise_time);

    tinyfs_read(ID_dir_1,RECORD_KEY7,tmp,&length_7);//����tmp��
    memcpy (&second_rise_time, &tmp[0], 2);
    LOG_I("second_rise_time:");
    LOG_I("%d",second_rise_time);

    tinyfs_read(ID_dir_1,RECORD_KEY8,tmp,&length_8);//����tmp��
    memcpy (&low_vbat_val, &tmp[0], 1);
    LOG_I("low_vbat_val:");
    LOG_I("%d",low_vbat_val);

    tinyfs_read(ID_dir_1,RECORD_KEY9,tmp,&length_9);//����tmp��
    memcpy (&hw_err_out_time, &tmp[0], 1);
    LOG_I("hw_err_out_time:");
    LOG_I("%d",hw_err_out_time);

    tinyfs_read(ID_dir_1,RECORD_KEY10,tmp,&length_10);//����tmp��
    memcpy (&hw_err_buzzer_all_time, &tmp[0], 1);
    LOG_I("hw_err_buzzer_all_time:");
    LOG_I("%d",hw_err_buzzer_all_time);

    tinyfs_read(ID_dir_1,RECORD_KEY11,tmp,&length_11);//����tmp��
    memcpy (&hw_err_buzzer_on_time, &tmp[0], 1);
    LOG_I("hw_err_buzzer_on_time:");
    LOG_I("%d",hw_err_buzzer_on_time);

    tinyfs_read(ID_dir_1,RECORD_KEY12,tmp,&length_12);//����tmp��
    memcpy (&hw_err_buzzer_off_time, &tmp[0], 1);
    LOG_I("hw_err_buzzer_off_time:");
    LOG_I("%d",hw_err_buzzer_off_time);

    tinyfs_read(ID_dir_1,RECORD_KEY13,tmp,&length_12);//����tmp��
    memcpy (&led_sleep_time, &tmp[0], 1);
    LOG_I("led_sleep_time:");
    LOG_I("%d",led_sleep_time);



}





//���������ɹ���һ��
void User_BLE_Ready() {

    Read_Last_Data();
    Buzzer_IO_Init();
		
		LED_Init();
    Moto_IO_Init();
		ls_uart1_init();
    ls_uart2_init();
    User_ADC_Init();

    HAL_UART_Receive_IT(&UART2_Config,uart_2_buffer,1);		// ����ʹ�ܴ���2�����ж�
    HAL_UART_Receive_IT(&UART1_Config,uart_buffer,1);		// ����ʹ�ܴ���1�����ж�

    //HAL_IWDG_Init(32756*1);  	 //5s���Ź�

    tag_lock_status=POS_90;
    hw_lock_status= POS_90;

////////////��ȡmac////////////////////
    uint8_t addr[6];
    bool type;
    dev_manager_get_identity_bdaddr(addr,&type);
    LOG_I("type:%d,addr:",type);
    LOG_HEX(addr,sizeof(addr));
    memcpy(&lock_mac[0],&addr[0],6);

////////////////////////////////
		
		Enter_Power_Mode_LL();



}

extern UART_HandleTypeDef log_uart;


reg_lsgpio_t  *GPIOA_Instance=LSGPIOA;
reg_lsgpio_t  *GPIOB_Instance=LSGPIOB;
reg_lsgpio_t  *GPIOC_Instance=LSGPIOC;

__attribute__((constructor)) void AAAA() {

    GPIOA_Instance=LSGPIOA;
    GPIOB_Instance=LSGPIOB;
    GPIOC_Instance=LSGPIOC;
}
extern uint8_t adv_obj_hdl;
void update_adv_intv(uint32_t new_adv_intv)
{
    LOG_I("adv_intv:%d",new_adv_intv);
    dev_manager_update_adv_interval(adv_obj_hdl,new_adv_intv,new_adv_intv);
}
																																																																														 

