#include "user_main.h"
#include "moto.h"

void moto_gpio_init(void)
{
    io_cfg_output(PB03);   //PB09 config output
    io_write_pin(PB03,0);  
    io_cfg_output(PB04);   //PB10 config output
    io_write_pin(PB04,0);  //PB10 write low power
}
void Moto_N(){
	io_write_pin(PB03,1);   
	io_write_pin(PB04,0);   
}
void Moto_P(){
	io_write_pin(PB03,0);  
	io_write_pin(PB04,1);  
}
void Moto_S(){
	io_write_pin(PB03,1);  
	io_write_pin(PB04,1);  
}
void Moto_NULL(){
	io_write_pin(PB03,0);  
	io_write_pin(PB04,0);  
}

uint8_t temp_[20];
uint8_t moro_task_flag;

//塑料锁
//*
uint8_t Moto_Task(){
	uint8_t temp=0x03;
	static unsigned int count=0;
	static unsigned int step=0x10;
	if(moro_task_flag==1){
		count++;
		sleep_time=0;
		switch(step){
			case 0x10:
					Moto_P();
				#ifdef _DEBUG
										sprintf((char*)&temp_[0],"A:%d\r\n",count);
							HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);
							#endif
					if(count>(600/5)){
							count=0;
							step=1;
							//HAL_UART_Transmit(&UART_Config,(uint8_t*)"A",2,100);    
					}
				 	if(count>(200/5)&&Check_SW1()==1){//里面的开关断开 默认上拉 高电平 弹出
					  	count=0;
							step=1;
					}
			break;
					
			case 1:			
					Moto_S();
				#ifdef _DEBUG
					sprintf((char*)&temp_[0],"B:%d\r\n",count);
					HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);		
					HAL_UART_Transmit(&UART_Config,(uint8_t*)"B",2,100);
				#endif
					if(count>(500/5)){
							count=0;
							step=2;
							
							KEY_FLAG=0;  
							KEY_ONCE=0;
						
//							if(Check_SW1()){
//									temp=0;  		//00 打开成功  0x03 失败
//									lock_state[0]=0;
//							}
//							else{
//									lock_state[0]=1;
//							}

							
							if(Check_SW2()==1 && Check_SW1()==0 ) {
								lock_state[0]=1;
							}
							else {
								temp=0;  		//00 打开成功  0x03 失败
								lock_state[0]=0;
							}

							
							Set_Task_State(OPEN_LOCK_DATA_SEND,START);
							look_status_send_count++;
							
							
							TX_DATA_BUF[6]=temp;
							user_ble_send_flag=1;	//蓝牙发送数据
							
							//#
							//count++;
							//if(Get_Task_State(OPEN_LOCK_DATA_SEND)==0) { //信息上报任务未启动
							//		Set_Task_State(OPEN_LOCK_DATA_SEND,START);
							//}
					}
			break;
			case 2:			
					#ifdef _DEBUG
				sprintf((char*)&temp_[0],"C:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);
			#endif
					Moto_N();
					if(count>(500/5)){
						//HAL_UART_Transmit(&UART_Config,(uint8_t*)"C",2,100);
							count=0;
							step=3;
					}
			break;
					
			case 3:			
					#ifdef _DEBUG
				sprintf((char*)&temp_[0],"D:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);
					#endif
					Moto_S();
					if(count>(250/5)){
						//HAL_UART_Transmit(&UART_Config,(uint8_t*)"D",2,100);
							count=0;
							step=4;
					}
			break;
					
			case 4:			
				#ifdef _DEBUG
				sprintf((char*)&temp_[0],"E:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);			
				#endif
					Moto_P();
					if(count>(150/5)){
					//	HAL_UART_Transmit(&UART_Config,(uint8_t*)"E",2,100);
							count=0;
							step=5;
					}
			break;
					
			case 5:			
			#ifdef _DEBUG
				sprintf((char*)&temp_[0],"F:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);			
			#endif
				//HAL_UART_Transmit(&UART_Config,(uint8_t*)"F",2,100);
					Moto_NULL();
					step=0x10;
					count=0;
					moro_task_flag=0;
			
			break;
	}
	}
	else{
		count=0;
	}
	return temp;
}
// */




//* 金属锁_钢板锁
/*
uint8_t Moto_Task(){
	uint8_t temp=0x03;
	static unsigned int count=0;
	static unsigned int step=0x10;
	if(moro_task_flag==1){
		count++;
		sleep_time=0;
		switch(step){
			case 0x10:
					Moto_P();
				#ifdef _DEBUG
										sprintf((char*)&temp_[0],"A:%d\r\n",count);
							HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);
							#endif
					if(count>(800/5)){
							count=0;
							step=1;
							//HAL_UART_Transmit(&UART_Config,(uint8_t*)"A",2,100);    
					}
				 	if(count>(200/5) && Check_SW1()==0){    //里面的开关断开 默认上拉 高电平 弹出
					  	count=0;
							step=1;
					}

					
			break;
					
			case 1:			
					Moto_S();
				#ifdef _DEBUG
					sprintf((char*)&temp_[0],"B:%d\r\n",count);
					HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);		
					HAL_UART_Transmit(&UART_Config,(uint8_t*)"B",2,100);
				#endif
					if(count>(500/5)){
							count=0;
							step=2;
							
							KEY_FLAG=0;  
							KEY_ONCE=0;
						
							if(Check_SW1()){
									temp=0;  		//00 打开成功  0x03 失败
									lock_state[0]=0;
							}
							else{
									lock_state[0]=1;
							}

							
							if(Check_SW1()==1) {
								lock_state[0]=1;
							}
							
							else {
								temp=0;  		//00 打开成功  0x03 失败
								lock_state[0]=0;
							}

							
							Set_Task_State(OPEN_LOCK_DATA_SEND,START);
							look_status_send_count++;
							
							
							TX_DATA_BUF[6]=temp;
							user_ble_send_flag=1;	//蓝牙发送数据
							
							//#
							//count++;
							//if(Get_Task_State(OPEN_LOCK_DATA_SEND)==0) { //信息上报任务未启动
							//		Set_Task_State(OPEN_LOCK_DATA_SEND,START);
							//}
					}
			break;
			case 2:			
					#ifdef _DEBUG
				sprintf((char*)&temp_[0],"C:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);
			#endif
					Moto_N();
					if(count>(400/5)){
						//HAL_UART_Transmit(&UART_Config,(uint8_t*)"C",2,100);
							count=0;
							step=3;
					}
			break;
					
			case 3:			
					#ifdef _DEBUG
				sprintf((char*)&temp_[0],"D:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);
					#endif
					Moto_S();
					if(count>(250/5)){
						//HAL_UART_Transmit(&UART_Config,(uint8_t*)"D",2,100);
							count=0;
							step=4;
						//step=5;
					}
			break;
					
			case 4:			
				#ifdef _DEBUG
				sprintf((char*)&temp_[0],"E:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);			
				#endif
					Moto_P();
					if(count>(150/5)){
					//	HAL_UART_Transmit(&UART_Config,(uint8_t*)"E",2,100);
							count=0;
							step=5;
					}
			break;
					
			case 5:			
			#ifdef _DEBUG
				sprintf((char*)&temp_[0],"F:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);			
			#endif
				//HAL_UART_Transmit(&UART_Config,(uint8_t*)"F",2,100);
					Moto_NULL();
					step=0x10;
					count=0;
					moro_task_flag=0;
			
			break;
	}
	}
	else{
		count=0;
	}
	return temp;
}
*/
//*/

/*挂锁*/
/*
uint8_t Moto_Task(){
	uint8_t temp=0x03;
	static unsigned int count=0;
	static unsigned int step=0x10;
	if(moro_task_flag==1){
		count++;
		sleep_time=0;
		switch(step){
			case 0x10:
					Moto_N();
				#ifdef _DEBUG
							sprintf((char*)&temp_[0],"A:%d\r\n",count);
							HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);
							#endif
					if(count>(600/5)){
							count=0;
							step=1;
							//HAL_UART_Transmit(&UART_Config,(uint8_t*)"A",2,100);    
					}
				 	if(count>(50/5)&&Check_SW2()==0){//里面的开关断开 默认上拉 高电平 弹出
					  	count=0;
							step=1;
					}
			break;
					
			case 1:			
					Moto_S();
				#ifdef _DEBUG
					sprintf((char*)&temp_[0],"B:%d\r\n",count);
					HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);		
					HAL_UART_Transmit(&UART_Config,(uint8_t*)"B",2,100);
				#endif
					if(count>(500/5)){
							count=0;
							step=2;
							
							KEY_FLAG=0;  
							KEY_ONCE=0;
						
//							if(Check_SW1()){
//									temp=0;  		//00 打开成功  0x03 失败
//									lock_state[0]=0;
//							}
//							else{
//									lock_state[0]=1;
//							}

							
							if(Check_SW2()==1 && Check_SW1()==0 ) {
								lock_state[0]=1;
							}
							else {
								temp=0;  		//00 打开成功  0x03 失败
								lock_state[0]=0;
							}

							
							Set_Task_State(OPEN_LOCK_DATA_SEND,START);
							look_status_send_count++;
							
							
							TX_DATA_BUF[6]=temp;
							user_ble_send_flag=1;	//蓝牙发送数据
							
							//#
							//count++;
							//if(Get_Task_State(OPEN_LOCK_DATA_SEND)==0) { //信息上报任务未启动
							//		Set_Task_State(OPEN_LOCK_DATA_SEND,START);
							//}
					}
			break;
			case 2:			
					#ifdef _DEBUG
				sprintf((char*)&temp_[0],"C:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);
			#endif
					Moto_P();
					if(count>(170/5)){
						//HAL_UART_Transmit(&UART_Config,(uint8_t*)"C",2,100);
							count=0;
							step=3;
					}
			break;
					
			case 3:			
					#ifdef _DEBUG
				sprintf((char*)&temp_[0],"D:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);
					#endif
					Moto_S();
					if(count>(250/5)){
						//HAL_UART_Transmit(&UART_Config,(uint8_t*)"D",2,100);
							count=0;
							step=4;
					}
			break;
					
			case 4:			
				#ifdef _DEBUG
				sprintf((char*)&temp_[0],"E:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);			
				#endif
					//Moto_P();
					if(count>(150/5)){
					//	HAL_UART_Transmit(&UART_Config,(uint8_t*)"E",2,100);
							count=0;
							step=5;
					}
			break;
					
			case 5:			
			#ifdef _DEBUG
				sprintf((char*)&temp_[0],"F:%d\r\n",count);
				HAL_UART_Transmit(&UART_Config,(uint8_t*)temp_,10,100);			
			#endif
				//HAL_UART_Transmit(&UART_Config,(uint8_t*)"F",2,100);
					Moto_NULL();
					step=0x10;
					count=0;
					moro_task_flag=0;
			
			break;
	}
	}
	else{
		count=0;
	}
	return temp;
}
*/
/**/

