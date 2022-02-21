#include "user_main.h"
#include "user_function.h"
void User_Init(){
	 HAL_UART_Transmit_IT(&UART_Config,(unsigned char*)"usart_OK",8);

}
