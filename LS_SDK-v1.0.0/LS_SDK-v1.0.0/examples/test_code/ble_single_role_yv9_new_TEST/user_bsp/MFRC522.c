//#include <intrins.h>
//#include "reg52.h"
//#include "stm8s.h"
#include "user_main.h"
#include "mfrc522.h"

#define SPI_NSS	    PB02

#define SPI_NSS_2   PA12
#define SPI_MOSI    PA14
#define SPI_MISO    PA15
#define SPI_CLK     PA13

#define POWER       PA06


uint8_t rfid_flag =1;

void RC522_GPIO_INIT(){

    io_cfg_output(SPI_NSS); //
		
		io_cfg_output(SPI_NSS_2); //
		io_write_pin(SPI_NSS,1); 

   	io_cfg_output(SPI_MOSI);  //
		io_cfg_output(SPI_CLK);   //
		io_cfg_output(POWER);   	//		
		io_write_pin(POWER,0);		
		
		io_cfg_input(SPI_MISO);                       
    //io_pull_write(SPI_MISO, IO_PULL_UP);  
		rfid_flag=1;
}

void RC522_GPIO_INIT_2(){

   io_cfg_output(SPI_NSS); //
		io_write_pin(SPI_NSS,1);
	
		io_cfg_output(SPI_NSS_2); //
		io_cfg_output(SPI_MOSI);  //
		io_cfg_output(SPI_CLK);   //
		io_cfg_output(POWER);   	//		
		io_write_pin(POWER,0);		
		
		io_cfg_input(SPI_MISO);                       
    rfid_flag=2;
		
		//io_pull_write(SPI_MISO, IO_PULL_UP);  
}

void SPI_NSS_L(){
	if(rfid_flag==1){
			io_write_pin(SPI_NSS,0) ;
	}
	else if(rfid_flag==2){
		 io_write_pin(SPI_NSS_2,0)  ;
	}
}

void SPI_NSS_H(){
		if(rfid_flag==1){
		io_write_pin(SPI_NSS,1)  ;
	}
	else if(rfid_flag==2){
		io_write_pin(SPI_NSS_2,1)   ;
	
	}
}


#define SPI_MISO_READ()  io_read_pin(SPI_MISO)	 //
#define SPI_MOSI_L()     io_write_pin(SPI_MOSI,0)	 //
#define SPI_MOSI_H()     io_write_pin(SPI_MOSI,1) //

#define SPI_SCK_L()    	 io_write_pin(SPI_CLK,0)	 //
#define SPI_SCK_H()  	 	 io_write_pin(SPI_CLK,1)	 //

//#define SPI_NSS_L()  	 io_write_pin(SPI_NSS,0)	 //
//#define SPI_NSS_H()  	 io_write_pin(SPI_NSS,1)	 //

//#define SPI_NSS_L_2()  	 io_write_pin(SPI_NSS_2,0)	 //
//#define SPI_NSS_H_2()  	 io_write_pin(SPI_NSS_2,1)	 //

#define RC522_RST_L()  	
#define RC522_RST_H()  	 

#define POWER_ON()  	 	io_write_pin(POWER,0);
#define POWER_OFF()  	 	io_write_pin(POWER,1);

#define MAXRLEN 18  
//#define UART
#define SPI_MODE
void Short_Delay(unsigned int ii){
  while(ii--);
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 
//  unsigned char xTest ;
   ClearBitMask(Status2Reg,0x08);
   WriteRawRC(BitFramingReg,0x07);

//  xTest = ReadRawRC(BitFramingReg);
//  if(xTest == 0x07 )
 //   { LED_GREEN  =0 ;}
 // else {LED_GREEN =1 ;while(1){}}
   SetBitMask(TxControlReg,0x03);
 
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
//     if(status  == MI_OK )
//   { LED_GREEN  =0 ;}
//   else {LED_GREEN =1 ;}
   if ((status == MI_OK) && (unLen == 0x10))
   {    
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }
   
   return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
  char status;
  unsigned char i,snr_check=0;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  
  ClearBitMask(Status2Reg,0x08);
  WriteRawRC(BitFramingReg,0x00);
  ClearBitMask(CollReg,0x80);
  
  ucComMF522Buf[0] = PICC_ANTICOLL1;
  ucComMF522Buf[1] = 0x20;
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);
  
  if (status == MI_OK)
  {
    for (i=0; i<4; i++)
    {   
      *(pSnr+i)  = ucComMF522Buf[i];
      snr_check ^= ucComMF522Buf[i];
    }
    if (snr_check != ucComMF522Buf[i])
    {   status = MI_ERR;    }
  }
  
  SetBitMask(CollReg,0x80);
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
  char status;
  unsigned char i;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_ANTICOLL1;
  ucComMF522Buf[1] = 0x70;
  ucComMF522Buf[6] = 0;
  for (i=0; i<4; i++)
  {
    ucComMF522Buf[i+2] = *(pSnr+i);
    ucComMF522Buf[6]  ^= *(pSnr+i);
  }
  CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
  ClearBitMask(Status2Reg,0x08);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
  
  if ((status == MI_OK) && (unLen == 0x18))
  {   status = MI_OK;  }
  else
  {   status = MI_ERR;    }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
  char status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = auth_mode;
  ucComMF522Buf[1] = addr;
  for (i=0; i<6; i++)
  {    ucComMF522Buf[i+2] = *(pKey+i);   }
  for (i=0; i<6; i++)
  {    ucComMF522Buf[i+8] = *(pSnr+i);   }
  //   memcpy(&ucComMF522Buf[2], pKey, 6); 
  //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
  
  status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
  if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
  {   status = MI_ERR;   }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr,unsigned char *pData)
{
  char status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_READ;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  if ((status == MI_OK) && (unLen == 0x90))
    //   {   memcpy(pData, ucComMF522Buf, 16);   }
  {
    for (i=0; i<16; i++)
    {    *(pData+i) = ucComMF522Buf[i];   }
  }
  else
  {   status = MI_ERR;   }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(unsigned char addr,unsigned char *pData)
{
  char status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_WRITE;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
  
  if (status == MI_OK)
  {
    //memcpy(ucComMF522Buf, pData, 16);
    for (i=0; i<16; i++)
    {    ucComMF522Buf[i] = *(pData+i);   }
    CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
    
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
  }
  
  return status;
}



/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_HALT;
  ucComMF522Buf[1] = 0;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
  unsigned char i,n;
  ClearBitMask(DivIrqReg,0x04);
  WriteRawRC(CommandReg,PCD_IDLE);
  SetBitMask(FIFOLevelReg,0x80);
  for (i=0; i<len; i++)
  {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
  WriteRawRC(CommandReg, PCD_CALCCRC);
  i = 0xFF;
  do 
  {
    n = ReadRawRC(DivIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x04));
  pOutData[0] = ReadRawRC(CRCResultRegL);
  pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
  int ii=10;
//  MF522_RST=1;
//  _nop_();
//  MF522_RST=0;
//  _nop_();
//  MF522_RST=1;
//  _nop_();
//  WriteRawRC(CommandReg,PCD_RESETPHASE);
//  _nop_();

	POWER_OFF();
  RC522_RST_H();
  while(ii--);
  ii=10;
  RC522_RST_L();
	POWER_ON();
  while(ii--);
  ii=10;
  RC522_RST_H();
  while(ii--);
  ii=10;
  WriteRawRC(CommandReg,PCD_RESETPHASE);
  while(ii--);

  WriteRawRC(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
  WriteRawRC(TReloadRegL,30);           
  WriteRawRC(TReloadRegH,0);
  WriteRawRC(TModeReg,0x8D);
  WriteRawRC(TPrescalerReg,0x3E);
  WriteRawRC(TxAutoReg,0x40);     
  return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//����RC632�Ĺ�����ʽ 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(unsigned char type)
{
  if (type == 'A')                     //ISO14443_A
  { 
    ClearBitMask(Status2Reg,0x08);
    /*     WriteRawRC(CommandReg,0x20);    //as default   
    WriteRawRC(ComIEnReg,0x80);     //as default
    WriteRawRC(DivlEnReg,0x0);      //as default
    WriteRawRC(ComIrqReg,0x04);     //as default
    WriteRawRC(DivIrqReg,0x0);      //as default
    WriteRawRC(Status2Reg,0x0);//80    //trun off temperature sensor
    WriteRawRC(WaterLevelReg,0x08); //as default
    WriteRawRC(ControlReg,0x20);    //as default
    WriteRawRC(CollReg,0x80);    //as default
    */
    WriteRawRC(ModeReg,0x3D);//3F
    /*	   WriteRawRC(TxModeReg,0x0);      //as default???
    WriteRawRC(RxModeReg,0x0);      //as default???
    WriteRawRC(TxControlReg,0x80);  //as default???
    
    WriteRawRC(TxSelReg,0x10);      //as default???
    */
    WriteRawRC(RxSelReg,0x86);//84
    //      WriteRawRC(RxThresholdReg,0x84);//as default
    //      WriteRawRC(DemodReg,0x4D);      //as default
    
    //      WriteRawRC(ModWidthReg,0x13);//26
    WriteRawRC(RFCfgReg,0x7F);   //4F
    /*   WriteRawRC(GsNReg,0x88);        //as default???
    WriteRawRC(CWGsCfgReg,0x20);    //as default???
    WriteRawRC(ModGsCfgReg,0x20);   //as default???
    */
    WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    
    
    //     PcdSetTmo(106);
    delay_10ms(1);
    PcdAntennaOn();
  }
  else{ return -1; }
  
  return MI_OK;
}
#ifdef UART
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address){
  unsigned char ucResult=0;   
  
  UART1_SendData8(Address|0x80); 
  while(!UART1_GetFlagStatus(UART1_FLAG_TXE)); 
  //�ȴ�������
  while(uart_data.len<=0);
  
  read_buf(&uart_data,&RX_Buf[0]);
  ucResult=RX_Buf[0];
  return ucResult;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value){
  unsigned char ucAddr;
  
  ucAddr = (Address&0x3f); //�������ֲ�uart�����и�ʽҪ���
  unsigned char buff[2]={ucAddr,value};
  UART1_SendData8(buff[0]);    
  while(!UART1_GetFlagStatus(UART1_FLAG_TXE)); 
  UART1_SendData8(buff[1]);
    while(!UART1_GetFlagStatus(UART1_FLAG_TXE)); 

}
#endif



#ifdef SPI_MODE
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
  unsigned char i, ucAddr;
  unsigned char ucResult=0;
  
	SPI_NSS_L();
  
  ucAddr = ((Address<<1)&0x7E)|0x80;
  
  for(i=8;i>0;i--)
  {
    SPI_SCK_L();
    if((ucAddr&0x80)==0x80)
      SPI_MOSI_H();
    else
      SPI_MOSI_L();
    SPI_SCK_H();
    ucAddr <<= 1;
  }
  
  for(i=8;i>0;i--)
  {
    
    SPI_SCK_L();
    ucResult <<= 1;

    SPI_SCK_H();
      
    if(SPI_MISO_READ())
      ucResult|=1;
    else
      ucResult|=0;  
}
  
  SPI_NSS_H();
  SPI_SCK_H();

  return ucResult;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{
  unsigned char i, ucAddr;
  
//  MF522_SCK = 0;
//  MF522_NSS = 0;
  
  SPI_SCK_L();
  SPI_NSS_L();
  
  ucAddr = ((Address<<1)&0x7E);
  
  for(i=8;i>0;i--)
  {
    if((ucAddr&0x80)==0x80)
      SPI_MOSI_H();
    else
      SPI_MOSI_L();
    
    SPI_SCK_H();
    ucAddr <<= 1;
    SPI_SCK_L();

  }
  
  for(i=8;i>0;i--)
  {
    if((value&0x80)==0x80)
      SPI_MOSI_H();
    else
      SPI_MOSI_L();
    
    SPI_SCK_H();
    value <<= 1;
    SPI_SCK_L();
  }
  SPI_NSS_H();
  SPI_SCK_H();
}
#endif

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0;
  tmp = ReadRawRC(reg);
  WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////

char aaaa;
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0;
  tmp = ReadRawRC(reg);
  aaaa= tmp;
	WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
  char status = MI_ERR;
  unsigned char irqEn   = 0x00;
  unsigned char waitFor = 0x00;
  unsigned char lastBits;
  unsigned char n;
  unsigned int i;
  switch (Command)
  {
  case PCD_AUTHENT:
    irqEn   = 0x12;
    waitFor = 0x10;
    break;
  case PCD_TRANSCEIVE:
    irqEn   = 0x77;
    waitFor = 0x30;
    break;
  default:
    break;
  }
  
  WriteRawRC(ComIEnReg,irqEn|0x80);
  ClearBitMask(ComIrqReg,0x80);
  WriteRawRC(CommandReg,PCD_IDLE);
  SetBitMask(FIFOLevelReg,0x80);
  
  for (i=0; i<InLenByte; i++)
  {   WriteRawRC(FIFODataReg, pInData[i]);    }
  WriteRawRC(CommandReg, Command);
  
  
  if (Command == PCD_TRANSCEIVE)
  {    SetBitMask(BitFramingReg,0x80);  }
  
  //    i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
  i = 2000;
  do 
  {
    n = ReadRawRC(ComIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x01) && !(n&waitFor));
  ClearBitMask(BitFramingReg,0x80);
  
  if (i!=0)
  {    
    if(!(ReadRawRC(ErrorReg)&0x1B))
    {
      status = MI_OK;
      if (n & irqEn & 0x01)
      {   status = MI_NOTAGERR;   }
      if (Command == PCD_TRANSCEIVE)
      {
        n = ReadRawRC(FIFOLevelReg);
        lastBits = ReadRawRC(ControlReg) & 0x07;
        if (lastBits)
        {   *pOutLenBit = (n-1)*8 + lastBits;   }
        else
        {   *pOutLenBit = n*8;   }
        if (n == 0)
        {   n = 1;    }
        if (n > MAXRLEN)
        {   n = MAXRLEN;   }
        for (i=0; i<n; i++)
        {   pOutData[i] = ReadRawRC(FIFODataReg);    }
      }
    }
    else
    {   status = MI_ERR;   }
    
  }
  
  
  SetBitMask(ControlReg,0x80);           // stop timer now
  WriteRawRC(CommandReg,PCD_IDLE); 
  return status;
}


/////////////////////////////////////////////////////////////////////
//��������  
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
  unsigned char i;
  i = ReadRawRC(TxControlReg);
  if (!(i & 0x03))
  {
    SetBitMask(TxControlReg, 0x03);
  }
}


/////////////////////////////////////////////////////////////////////
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
  ClearBitMask(TxControlReg, 0x03);
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = dd_mode;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
  
  if (status == MI_OK)
  {
    memcpy(ucComMF522Buf, pValue, 4);
    //       for (i=0; i<16; i++)
    //       {    ucComMF522Buf[i] = *(pValue+i);   }
    CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
    unLen = 0;
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
    if (status != MI_ERR)
    {    status = MI_OK;    }
  }
  
  if (status == MI_OK)
  {
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
    
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
  }
  return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_RESTORE;
  ucComMF522Buf[1] = sourceaddr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
  
  if (status == MI_OK)
  {
    ucComMF522Buf[0] = 0;
    ucComMF522Buf[1] = 0;
    ucComMF522Buf[2] = 0;
    ucComMF522Buf[3] = 0;
    CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
    
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
    if (status != MI_ERR)
    {    status = MI_OK;    }
  }
  
  if (status != MI_OK)
  {    return MI_ERR;   }
  
  ucComMF522Buf[0] = PICC_TRANSFER;
  ucComMF522Buf[1] = goaladdr;
  
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
  
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  
  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
  
  return status;
}


///////////////////////////////////////////////////////////////////////
// Delay 10ms
///////////////////////////////////////////////////////////////////////
void delay_10ms(unsigned int _10ms)
{
DELAY_US(10*1000);
  
}