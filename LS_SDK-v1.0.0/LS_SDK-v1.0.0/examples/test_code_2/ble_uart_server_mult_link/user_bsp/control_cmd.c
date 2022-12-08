#include "control_cmd.h"
#include "user_main.h"

//uint8_t address;  //485通信地址
//uint8_t lockid[16]={0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30}; //锁16位id号



uint8_t hw_lock_status;    //锁状态 真实硬件上的 
//0x00 初始位置 ，0x01 上升中 ， 0x00 90度 ， 0x11 超出 0xff 放到
uint8_t tag_lock_status;    //锁状态 目标
uint8_t lock_mode;          //锁模式
uint8_t ble_close_atc_flag; //

uint8_t err_val; //
uint8_t vbat_val;
uint8_t car_val;
uint8_t distance_val;

//收到命令处理
//传入接收到的命令数据buf
void CMD_Processing(uint8_t *p){
//uint8_t len;

uint8_t send_buf[64];
uint8_t command;
uint8_t data;
if(*p==0x17){
	command= *(p+1);
switch(command){
	case 0x01:
			data= *(p+2);
			switch(data){
						case 0x01:
								tag_lock_status=POS_0;
								lock_mode= 0x00;
						break;

						case 0x02:
								tag_lock_status=POS_0_90;
								lock_mode= 0x00;
						break;

						case 0x03:
								tag_lock_status=POS_0;
								lock_mode= 0x01;
						break;

						case 0x04:
								tag_lock_status=POS_90;
								lock_mode= 0x00;
						break;

						case 0x05:
								tag_lock_status=POS_0;
								lock_mode= 0x00;
								ble_close_atc_flag=1;
						break;
			}
			send_buf[0]= 0x17;
			send_buf[1]= 0x01;
			send_buf[2]= err_val;
			send_buf[3]= tag_lock_status;
			send_buf[4]= vbat_val;
			send_buf[5]= '\0';			
	break;

	case 0x02:
			send_buf[0]= 0x17;
			send_buf[1]= 0x02;
			send_buf[2]= err_val;          //错误码
			send_buf[3]= hw_lock_status;   //检测开关状态
			send_buf[4]= vbat_val;         //电量
			send_buf[5]= car_val;          //是否有车
			send_buf[6]= distance_val;          //超声波距离
			send_buf[7]= '\0';          	
	break;

	case 0x03:
	
	break;

	case 0x04:
	
	break;

	case 0x05:
	
	break;

	case 0x06:
	
	break;

	case 0x07:
	
	break;

	case 0x08:
	
	break;
		
	case 0x09:
	
	break;
	
	case 0x0A:
	
	break;
	
	
	case 0x0B:
	
	break;
	
	
	case 0x0C:
	
	break;
	
	case 0x0D:
	
	break;
	
	case 0x10:
	
	break;
}
}


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
