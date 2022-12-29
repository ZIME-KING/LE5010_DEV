#include "user_main.h"
#include "moto.h"

uint8_t moro_task_flag=0;;


void Moto_IO_Init()
{
    io_cfg_output(PB03);   //PB09 config output
    io_write_pin(PB03,0);
    io_cfg_output(PB04);   //PB10 config output
    io_write_pin(PB04,0);  //PB10 write low power
}
void Moto_N() {
    io_write_pin(PB03,1);
    io_write_pin(PB04,0);
}
void Moto_P() {
    io_write_pin(PB03,0);
    io_write_pin(PB04,1);
}
void Moto_S() {
    io_write_pin(PB03,1);
    io_write_pin(PB04,1);
}
void Moto_NULL() {
    io_write_pin(PB03,0);
    io_write_pin(PB04,0);
}

void Moto_Task() {
    static uint16_t time_out;
    static uint16_t count=0;
    static uint16_t stop_flag=0;

    //check_sw();
    if(moro_task_flag==1) {
        time_out++;
        if(time_out>=80) { 
            Moto_S();
            moro_task_flag=0;
            return;
        }
        if(hw_lock_status!=tag_lock_status) {
            if(tag_lock_status==POS_0) {
                Moto_P();
            }
            else if(tag_lock_status==POS_90) {
                switch(hw_lock_status) {
                case  POS_0:
                    Moto_N();
                    break;

                case  POS_0_90:
                    Moto_N();
                    break;

                case  POS_90_180:
                    Moto_P();
                    break;

                case  POS_OUT:
                    Moto_P();
                    break;
                }
            }
        } else {
				
						if(hw_lock_status==POS_0){
							 Moto_S();
						}else{
							stop_flag=1;
						}
            //Moto_S();
            moro_task_flag=0;
        }
    }
    else {
        time_out=0;
    }

    if(stop_flag==1) {
        count++;
        if(count>5) {
            count=0;
						stop_flag=0;
            Moto_S();
        }
    }

}
