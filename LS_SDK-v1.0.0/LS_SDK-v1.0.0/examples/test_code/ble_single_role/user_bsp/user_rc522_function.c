#include "user_main.h"
#include "user_rc522_function.h"
#include "mfrc522.h"


unsigned char  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  //Ĭ����֤���벻��
unsigned char  SelectedSnr[4];  //����ײ�����·��صĿ���      
//unsigned char  temp_buf[16]={0x10,0x10,0xfe,0x01,0x01,0x01,0x01,0x01,0x01};
Card_TypeDef  M1_Card;

//�ȴ����뿪
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

//������ز���
//���濨�ṹ��
//��ȡ������       volatile

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
        card->snr=snr;      //������0������
        status = PcdAuthState(KEYA, ((card->snr)*4+3), DefaultKey, SelectedSnr);// У��1�������룬����λ��ÿһ������3��
        //{
          if(!status)
          {
            status = PcdRead(((card->snr) * 4 + 0), &card->buf[0]);  // ��������ȡ1����0�����ݵ�buf[0]-buf[16] 
           // status = PcdWrite((1*4+0), &temp_buf[0]);  // д������buf[0]-buf[16]д��1����0��
            if(!status)
            {
              //��д�ɹ�������LED
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