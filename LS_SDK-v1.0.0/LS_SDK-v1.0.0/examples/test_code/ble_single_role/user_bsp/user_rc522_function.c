#include "user_main.h"
#include "user_rc522_function.h"
#include "mfrc522.h"


unsigned char  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  //默认验证密码不改
unsigned char  SelectedSnr[4];  //防冲撞机制下返回的卡号      
//unsigned char  temp_buf[16]={0x10,0x10,0xfe,0x01,0x01,0x01,0x01,0x01,0x01};
Card_TypeDef  M1_Card;

//等待卡离开
void WaitCardOff(void)
{
  unsigned char status, TagType[2];
  while(1)
  {
    status = PcdRequest(REQ_ALL, TagType);
    if(status)
    {
      status = PcdRequest(REQ_ALL, TagType);
      if(status)
      {
        status = PcdRequest(REQ_ALL, TagType);
        if(status)
        {
          return;
        }
      }
    }
    DELAY_US(1000);
  }
}

//读卡相关操作
//保存卡结构体
//读取的扇区       volatile

void User_Mfrc522(Card_TypeDef *card,char snr){
  char status;
	static uint16_t 	count;
	//static char last_SelectedSnr[4];	
	
	RC522_GPIO_INIT();
	PcdReset();	
  DELAY_US(10*1000);
	PcdAntennaOff(); 
  DELAY_US(10*1000);
	PcdAntennaOn();  
	M500PcdConfigISOType( 'A' );
	
while(1){	
	if(count>200){
			io_cfg_disable(PA06);
			LOG_HEX(&SelectedSnr[0],4);
			LOG_I("TIME_OUT");			
			return ;
	}
  status= PcdRequest(0x52,&card->TagType[0]);
  if(!status)
  {
    status = PcdAnticoll(SelectedSnr);
    if(!status)
    {
      status=PcdSelect(SelectedSnr);
      if(!status)
      {
        card->snr=snr;      //扇区号0读卡号
        status = PcdAuthState(KEYA, ((card->snr)*4+3), DefaultKey, SelectedSnr);// 校验1扇区密码，密码位于每一扇区第3块
        //{
          if(!status)
          {
            status = PcdRead(((card->snr) * 4 + 0), &card->buf[0]);  // 读卡，读取1扇区0块数据到buf[0]-buf[16] 
           // status = PcdWrite((1*4+0), &temp_buf[0]);  // 写卡，将buf[0]-buf[16]写入1扇区0块
            if(!status)
            {
              //读写成功，点亮LED
              //LED_GREEN = 0;	
							 memcpy(RFID_DATA,SelectedSnr,4);							 
							 //LOG_HEX(&M1_Card.buf[0],16);
							 LOG_HEX(&SelectedSnr[0],4);
							 io_write_pin(PA06,1);    // POWER_OFF();          //
							 io_cfg_disable(PA06);
               return ;
            }
        }
      }
    }
  }
	else{	
		count++;
	}
	}
}