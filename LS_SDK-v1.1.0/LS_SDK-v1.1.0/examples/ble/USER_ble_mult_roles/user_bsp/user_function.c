#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"

//���������ɹ���һ��
void User_BLE_Ready(){
		Buzzer_IO_Init();
		Moto_IO_Init();
}
