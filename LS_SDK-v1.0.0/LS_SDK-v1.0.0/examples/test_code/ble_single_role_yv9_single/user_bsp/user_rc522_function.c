//#include "user_main.h"
//#include "user_rc522_function.h"
//#include "mfrc522.h"


//unsigned char  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  //默认验证密码不改
//unsigned char  SelectedSnr[4];  //防冲撞机制下返回的卡号
////unsigned char  temp_buf[16]={0x10,0x10,0xfe,0x01,0x01,0x01,0x01,0x01,0x01};
//Card_TypeDef  M1_Card;

////等待卡离开
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

////读卡相关操作
////保存卡结构体
////读取的扇区       volatile
//uint8_t User_Mfrc522(Card_TypeDef *card,char snr) {
//		//static Card_TypeDef last_data;
//    static unsigned char  last_SelectedSnr[4];  //防冲撞机制下返回的卡号
//		char status;
//    static uint16_t 	count;
//    static uint8_t 		once_flag;
//    static uint8_t 		i;
//		
//    uint8_t user_test;
//		
//		if(i>=10) {											//500m找一次卡  
//        i=0;
//        return rfid_task_flag_1;
//    }
//    else {
//        i++;
//    }
//		
//		//只有锁插入后会进行扫卡操作
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
//						if(user_test==0x80){    //初始化成功，设备存在，开启扫卡任务
//									LOG_I("FIND_RFID_DRIVE");
//									rfid_task_flag_1=0;
//						}
//						else{             		//初始化失败，设备不存在，不开启扫卡任务
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
//						Set_Task_State(OPEN_LOCK_DATA_SEND,START);   //数据上传服务器任务
//						user_ble_send_flag=1;                        //蓝牙数据发送开启
//            TX_DATA_BUF[0]=0x52;		// CMD
//            TX_DATA_BUF[1]=TOKEN[0];
//            TX_DATA_BUF[2]=TOKEN[1];
//            TX_DATA_BUF[3]=TOKEN[2];
//            TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
//            TX_DATA_BUF[5]=0x08;    	//LEN
//            TX_DATA_BUF[6]=0x01;			//主锁无，关闭模式
//            TX_DATA_BUF[7]=0x06;    // 在线情况  1，2全在线，具体见协议文档
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
//                    card->snr=snr;      //扇区号0读卡号
//                    status = PcdAuthState(KEYA, ((card->snr)*4+3), DefaultKey, SelectedSnr);// 校验1扇区密码，密码位于每一扇区第3块
//                    if(!status)
//                    {
//                        status = PcdRead(((card->snr) * 4 + 0), &card->buf[0]);  // 读卡，读取1扇区0块数据到buf[0]-buf[16]
//												// status = PcdWrite((1*4+0), &temp_buf[0]);  // 写卡，将buf[0]-buf[16]写入1扇区0块
//                        if(!status)
//                        {
//														count=0;			//找到卡计数清0
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
//																		Set_Task_State(OPEN_LOCK_DATA_SEND,START);   //数据上传服务器任务
//																		user_ble_send_flag=1;                        //蓝牙数据发送开启
//																		
//																		TX_DATA_BUF[0]=0x52;		// CMD
//																		TX_DATA_BUF[1]=TOKEN[0];
//																		TX_DATA_BUF[2]=TOKEN[1];
//																		TX_DATA_BUF[3]=TOKEN[2];
//																		TX_DATA_BUF[4]=TOKEN[3];  //TOKEN[4]
//																		TX_DATA_BUF[5]=0x08;    	//LEN
//																		TX_DATA_BUF[6]=0x01;			//主锁无    ，关闭模式
//																		TX_DATA_BUF[7]=0x06;      //在线情况 1，2全在线，具体见协议文档
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
////        card->snr=snr;      //扇区号0读卡号
////        status = PcdAuthState(KEYA, ((card->snr)*4+3), DefaultKey, SelectedSnr);// 校验1扇区密码，密码位于每一扇区第3块
////        //{
////          if(!status)
////          {
////            status = PcdRead(((card->snr) * 4 + 0), &card->buf[0]);  // 读卡，读取1扇区0块数据到buf[0]-buf[16]
////           // status = PcdWrite((1*4+0), &temp_buf[0]);  // 写卡，将buf[0]-buf[16]写入1扇区0块
////            if(!status)
////            {
////              //读写成功，点亮LED
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

