//#include "user_main.h"
//#include "user_rc522_function.h"
//#include "mfrc522.h"


//unsigned char  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  //Ĭ����֤���벻��
//unsigned char  SelectedSnr[4];  //����ײ�����·��صĿ���
////unsigned char  temp_buf[16]={0x10,0x10,0xfe,0x01,0x01,0x01,0x01,0x01,0x01};
//Card_TypeDef  M1_Card;

////�ȴ����뿪
//void WaitCardOff(void)
//{
//    unsigned char status, TagType[2];
//    while(1)
//    {
//        status = PcdRequest(REQ_ALL, TagType);
//        if(status)
//        {
//            status = PcdRequest(REQ_ALL, TagType);
//            if(status)
//            {
//                status = PcdRequest(REQ_ALL, TagType);
//                if(status)
//                {
//                    return;
//                }
//            }
//        }
//        DELAY_US(1000);
//    }
//}

//uint8_t	rfid_task_flag_1=0;
//uint8_t	rfid_task_flag_2=0;

////������ز���
////���濨�ṹ��
////��ȡ������       volatile
//uint8_t User_Mfrc522(Card_TypeDef *card,char snr) {
//		//static Card_TypeDef last_data;
//    static unsigned char  last_SelectedSnr[4];  //����ײ�����·��صĿ���
//		char status;
//    static uint16_t 	count;
//    static uint8_t 		once_flag;
//    static uint8_t 		i;
//		
//    uint8_t user_test;
//		
//		if(i>=10) {											//500m��һ�ο�  
//        i=0;
//        return rfid_task_flag_1;
//    }
//    else {
//        i++;
//    }
//		
//		//ֻ�������������ɨ������
//    if(rfid_task_flag_1==0 ) {
//        if(once_flag!=0xAA) {
//            RC522_GPIO_INIT();
//            PcdReset();
//						
//						user_test=ReadRawRC(0x33);
//						LOG_HEX(&user_test,1);
//						
//						PcdAntennaOff();
//            DELAY_US(10*1000);
//            PcdAntennaOn();
//            status=M500PcdConfigISOType( 'A' );
//						
//						if(user_test==0x80){    //��ʼ���ɹ����豸���ڣ�����ɨ������
//									LOG_I("FIND_RFID_DRIVE");
//									rfid_task_flag_1=0;
//						}
//						else{             		//��ʼ��ʧ�ܣ��豸�����ڣ�������ɨ������
//									LOG_I("NOT_FIND_RFID_DRIVE");
//									rfid_task_flag_1=1;
//						}
//            once_flag=0xAA;
//						return 1;
//        }
//        if(count>20) {						
//						count=0; 
//						last_SelectedSnr[0]=0;
//						last_SelectedSnr[1]=0;
//						last_SelectedSnr[2]=0;
//						last_SelectedSnr[3]=0;
//				
//						if((RFID_DATA[0]+RFID_DATA[1]+RFID_DATA[2]+RFID_DATA[3])!=0){
//						RFID_DATA[0]=0;RFID_DATA[1]=0;RFID_DATA[2]=0;RFID_DATA[3]=0;						
//						LOG_I("NOT_FIND_CARD");
//						LOG_HEX(&RFID_DATA[0],4);
//						Set_Task_State(OPEN_LOCK_DATA_SEND,START);   //�����ϴ�����������
//						user_ble_send_flag=1;                        //�������ݷ��Ϳ���
//            TX_DATA_BUF[0]=0x52;		// CMD
//            TX_DATA_BUF[1]=TOKEN[0];
//            TX_DATA_BUF[2]=TOKEN[1];
//            TX_DATA_BUF[3]=TOKEN[2];
//            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
//            TX_DATA_BUF[5]=0x08;    	//LEN
//            TX_DATA_BUF[6]=0x01;			//�����ޣ��ر�ģʽ
//            TX_DATA_BUF[7]=0x06;    // �������  1��2ȫ���ߣ������Э���ĵ�
//            TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
//            TX_DATA_BUF[9]=0x01;
//            TX_DATA_BUF[10]=RFID_DATA[0];
//            TX_DATA_BUF[11]=RFID_DATA[1];
//            TX_DATA_BUF[12]=RFID_DATA[2];
//            TX_DATA_BUF[13]=RFID_DATA[3];			
//						}		
//        }
//        status= PcdRequest(0x52,&card->TagType[0]);
//        if(!status)
//        {
//            status = PcdAnticoll(SelectedSnr);
//            if(!status)
//            {
//                status=PcdSelect(SelectedSnr);
//                if(!status)
//                {
//                    card->snr=snr;      //������0������
//                    status = PcdAuthState(KEYA, ((card->snr)*4+3), DefaultKey, SelectedSnr);// У��1�������룬����λ��ÿһ������3��
//                    if(!status)
//                    {
//                        status = PcdRead(((card->snr) * 4 + 0), &card->buf[0]);  // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
//												// status = PcdWrite((1*4+0), &temp_buf[0]);  // д������buf[0]-buf[16]д��1����0��
//                        if(!status)
//                        {
//														count=0;			//�ҵ���������0
//                            //LOG_HEX(&M1_Card.buf[0],16);
//                            //LOG_HEX(&SelectedSnr[0],4);
//														
//														if(last_SelectedSnr[0]!=SelectedSnr[0] ||last_SelectedSnr[1]!=SelectedSnr[1]||last_SelectedSnr[2]!=SelectedSnr[2]||last_SelectedSnr[3]!=SelectedSnr[3]){
//																		
//																		LOG_I("FIND_CARD");
//																		
////
//																		last_SelectedSnr[0]=SelectedSnr[0];
//																		last_SelectedSnr[1]=SelectedSnr[1];
//																		last_SelectedSnr[2]=SelectedSnr[2];
//																		last_SelectedSnr[3]=SelectedSnr[3];
//																		
//																		RFID_DATA[0]=SelectedSnr[0];
//																		RFID_DATA[1]=SelectedSnr[1];
//																		RFID_DATA[2]=SelectedSnr[2];
//																		RFID_DATA[3]=SelectedSnr[3];		
//																		memcpy(RFID_DATA,SelectedSnr,4);																		
//																		LOG_HEX(&RFID_DATA[0],4);
//																		LOG_I("FIND_CARD");
//																		
//																		Set_Task_State(OPEN_LOCK_DATA_SEND,START);   //�����ϴ�����������
//																		user_ble_send_flag=1;                        //�������ݷ��Ϳ���
//																		
//																		TX_DATA_BUF[0]=0x52;		// CMD
//																		TX_DATA_BUF[1]=TOKEN[0];
//																		TX_DATA_BUF[2]=TOKEN[1];
//																		TX_DATA_BUF[3]=TOKEN[2];
//																		TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
//																		TX_DATA_BUF[5]=0x08;    	//LEN
//																		TX_DATA_BUF[6]=0x01;			//������    ���ر�ģʽ
//																		TX_DATA_BUF[7]=0x06;      //������� 1��2ȫ���ߣ������Э���ĵ�
//																		TX_DATA_BUF[8]=((!lock_state[1])<<2)+((!lock_state[0])<<1);    //
//																		TX_DATA_BUF[9]=0x01;
//																		TX_DATA_BUF[10]=RFID_DATA[0];
//																		TX_DATA_BUF[11]=RFID_DATA[1];
//																		TX_DATA_BUF[12]=RFID_DATA[2];
//																		TX_DATA_BUF[13]=RFID_DATA[3];
//													}
//                        }
//                    }
//                }
//            }
//        }
//				if(status){
//					 count++;
//				}
//    }
//    else {
//        count=0;
//        once_flag=0;
//    }
//		return rfid_task_flag_1;
//}




////void User_Mfrc522_(Card_TypeDef *card,char snr){
////  char status;
////	static uint16_t 	count;
////	//static char last_SelectedSnr[4];
////
////	RC522_GPIO_INIT();
////	PcdReset();
////  DELAY_US(10*1000);
////	PcdAntennaOff();
////  DELAY_US(10*1000);
////	PcdAntennaOn();
////	M500PcdConfigISOType( 'A' );
////
////while(1){
////	if(count>200){
////			io_write_pin(PA06,1);    // POWER_OFF();
////			LOG_HEX(&SelectedSnr[0],4);
////			LOG_I("TIME_OUT");
////			return ;
////	}
////  status= PcdRequest(0x52,&card->TagType[0]);
////  if(!status)
////  {
////    status = PcdAnticoll(SelectedSnr);
////    if(!status)
////    {
////      status=PcdSelect(SelectedSnr);
////      if(!status)
////      {
////        card->snr=snr;      //������0������
////        status = PcdAuthState(KEYA, ((card->snr)*4+3), DefaultKey, SelectedSnr);// У��1�������룬����λ��ÿһ������3��
////        //{
////          if(!status)
////          {
////            status = PcdRead(((card->snr) * 4 + 0), &card->buf[0]);  // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
////           // status = PcdWrite((1*4+0), &temp_buf[0]);  // д������buf[0]-buf[16]д��1����0��
////            if(!status)
////            {
////              //��д�ɹ�������LED
////              //LED_GREEN = 0;
////							 memcpy(RFID_DATA,SelectedSnr,4);
////							 //LOG_HEX(&M1_Card.buf[0],16);
////							 LOG_HEX(&SelectedSnr[0],4);
////							 io_write_pin(PA06,1);    // POWER_OFF();          //
////               return ;
////            }
////        }
////      }
////    }
////  }
////	else{
////		count++;
////	}
////	}
////}

