#define LOG_TAG "USER"
#include "user_main.h"
#include "user_function.h"

//蓝牙启动成功跑一次
void User_BLE_Ready(){
		Buzzer_IO_Init();
		Moto_IO_Init();
}
