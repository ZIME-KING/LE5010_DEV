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

uint8_t	rfid_task_flag_1=0;
uint8_t	rfid_task_flag_2=0;

//������ز���
//���濨�ṹ��
//��ȡ������       volatile
void User_Mfrc522(Card_TypeDef *card,char snr) {
    char status;
    static uint16_t 	count;
    static uint8_t 		once_flag;
    static uint8_t 		i;

    if(i>=4) {
        i=0;
        return;
    }
    else {
        i++;
    }


    if(rfid_task_flag_1) {
        if(once_flag!=0xAA) {
            RC522_GPIO_INIT();
            PcdReset();
            //DELAY_US(10*1000);
            PcdAntennaOff();
            //DELAY_US(10*1000);
            PcdAntennaOn();
            M500PcdConfigISOType( 'A' );
            once_flag=0xAA;
        }
        if(count>20) {
            io_write_pin(PA06,1);    // POWER_OFF();
            LOG_HEX(&SelectedSnr[0],4);
            LOG_I("TIME_OUT");

            RFID_DATA[0]=0x00;
            RFID_DATA[1]=0x00;
            RFID_DATA[2]=0x00;
            RFID_DATA[3]=0x00;


            user_ble_send_flag=1;
            TX_DATA_BUF[0]=0x52;		// CMD
            TX_DATA_BUF[1]=TOKEN[0];
            TX_DATA_BUF[2]=TOKEN[1];
            TX_DATA_BUF[3]=TOKEN[2];
            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
            TX_DATA_BUF[5]=0x08;    	//LEN
            TX_DATA_BUF[6]=0x01;			//�����ޣ��ر�ģʽ
            TX_DATA_BUF[7]=0x06;    // �������  1��2ȫ���ߣ������Э���ĵ�
            TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
            TX_DATA_BUF[9]=0x01;

            TX_DATA_BUF[10]=RFID_DATA[0];
            TX_DATA_BUF[11]=RFID_DATA[1];
            TX_DATA_BUF[12]=RFID_DATA[2];
            TX_DATA_BUF[13]=RFID_DATA[3];




            //RFID_DATA_2[4]={0X00,0x00,0x00,0x00};

            rfid_task_flag_1=0;
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
                            rfid_task_flag_1=0;

                            user_ble_send_flag=1;
                            TX_DATA_BUF[0]=0x52;		// CMD
                            TX_DATA_BUF[1]=TOKEN[0];
                            TX_DATA_BUF[2]=TOKEN[1];
                            TX_DATA_BUF[3]=TOKEN[2];
                            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
                            TX_DATA_BUF[5]=0x08;    	//LEN
                            TX_DATA_BUF[6]=0x01;			//�����ޣ��ر�ģʽ
                            TX_DATA_BUF[7]=0x06;    // �������  1��2ȫ���ߣ������Э���ĵ�
                            TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
                            TX_DATA_BUF[9]=0x01;

                            TX_DATA_BUF[10]=RFID_DATA[0];
                            TX_DATA_BUF[11]=RFID_DATA[1];
                            TX_DATA_BUF[12]=RFID_DATA[2];
                            TX_DATA_BUF[13]=RFID_DATA[3];
                        }
                    }
                }
            }
        }
        else {
            count++;
        }
    }
    else {
        count=0;
        once_flag=0;
    }
}




//void User_Mfrc522_(Card_TypeDef *card,char snr){
//  char status;
//	static uint16_t 	count;
//	//static char last_SelectedSnr[4];
//
//	RC522_GPIO_INIT();
//	PcdReset();
//  DELAY_US(10*1000);
//	PcdAntennaOff();
//  DELAY_US(10*1000);
//	PcdAntennaOn();
//	M500PcdConfigISOType( 'A' );
//
//while(1){
//	if(count>200){
//			io_write_pin(PA06,1);    // POWER_OFF();
//			LOG_HEX(&SelectedSnr[0],4);
//			LOG_I("TIME_OUT");
//			return ;
//	}
//  status= PcdRequest(0x52,&card->TagType[0]);
//  if(!status)
//  {
//    status = PcdAnticoll(SelectedSnr);
//    if(!status)
//    {
//      status=PcdSelect(SelectedSnr);
//      if(!status)
//      {
//        card->snr=snr;      //������0������
//        status = PcdAuthState(KEYA, ((card->snr)*4+3), DefaultKey, SelectedSnr);// У��1�������룬����λ��ÿһ������3��
//        //{
//          if(!status)
//          {
//            status = PcdRead(((card->snr) * 4 + 0), &card->buf[0]);  // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
//           // status = PcdWrite((1*4+0), &temp_buf[0]);  // д������buf[0]-buf[16]д��1����0��
//            if(!status)
//            {
//              //��д�ɹ�������LED
//              //LED_GREEN = 0;
//							 memcpy(RFID_DATA,SelectedSnr,4);
//							 //LOG_HEX(&M1_Card.buf[0],16);
//							 LOG_HEX(&SelectedSnr[0],4);
//							 io_write_pin(PA06,1);    // POWER_OFF();          //
//               return ;
//            }
//        }
//      }
//    }
//  }
//	else{
//		count++;
//	}
//	}
//}

