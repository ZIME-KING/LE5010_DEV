#include "user_main.h"
#include "LPM2100.h"

/////////////////////��ʼ�ռ�//////////////
#include <string.h>
#include "crc.h"
uint8_t buzou,yanshi;//����ʼ��ʼ��Ϊ��
uint8_t at_state=0; //ģ��״̬
uint8_t at_xinhao=200; //�ź�ֵ
uint8_t LMM_IMEI[15];
uint8_t LMM_IMSI[15];
uint8_t qi_dong[]=  {0x58,0x59,0x11,0x07,0x01,0xa0,0x00,0xa1,0x00,0xa2,0x10,0xa6,0x01,0xb0,0x50,0xff,0xff};
uint8_t qi_dong_id=0;
/*
		if(qi_dong_id==0xff)
		{
			qi_dong[3]=qi_dong_id;//����ID��ÿ�ε���
			qi_dong_id=0;
		}
		else
		{
			qi_dong[3]=qi_dong_id;//����ID��ÿ�ε���
			qi_dong_id++;
		}
		qi_dong[6]=;//Ӳ���汾��
		qi_dong[8]=;//����汾��
		qi_dong[10]=;//�ź�ǿ��
		qi_dong[12]=;//֧�ֵĴ�������
		qi_dong[14]=;//������ص���
*/
uint8_t xin_tiao[]= {0x58,0x59,0x11,0x00,0x02,0xa0,0x00,0xa1,0x00,0xa2,0x10,0xa6,0x01,0xb0,0x50,0xff,0xff};
uint8_t xin_tiao_id=0;
/*
		if(xin_tiao_id==0xff)
		{
			xin_tiao[3]=xin_tiao_id;//����ID��ÿ�ε���
			xin_tiao_id=0;
		}
		else
		{
			xin_tiao[3]=xin_tiao_id;//����ID��ÿ�ε���
			xin_tiao_id++;
		}
		xin_tiao[6]=;//Ӳ���汾��
		xin_tiao[8]=;//����汾��
		xin_tiao[10]=;//�ź�ǿ��
		xin_tiao[12]=;//֧�ֵĴ�������
		xin_tiao[14]=;//������ص���
*/
uint8_t kai_suo[]=   {0x58,0x59,0x09,0x00,0x10,0x00,0x00,0xff,0xff};
uint8_t kai_suo_id=0;
/*
		if(kai_suo_id==0xff)
		{
			kai_suo[3]=kai_suo_id;//����ID��ÿ�ε���
			kai_suo_id=0;
		}
		else
		{
			kai_suo[3]=kai_suo_id;//����ID��ÿ�ε���
			kai_suo_id++;
		}
		kai_suo[6]=0;//��Ŀǰ״̬
*/	
uint8_t shang_bao[]={0x58,0x59,0x09,0x00,0x20,0x00,0x00,0xff,0xff};
uint8_t shang_bao_id=0;
/*
		if(shang_bao_id==0xff)
		{
			shang_bao[3]=shang_bao_id;//����ID��ÿ�ε���
			shang_bao_id=0;
		}
		else
		{
			shang_bao[3]=shang_bao_id;//����ID��ÿ�ε���
			shang_bao_id++;
		}
		shang_bao[6]=0;//��Ŀǰ״̬
*/
uint8_t mokuai_sate1=0,mokuai_sate2=0,mokuai_sate3=0,mokuai_sate4=0;
uint8_t lmm_jishu=0;
int mi(int a,int b)
{
	int i=0,number=1;
	for(i=0;i<b+1;i++)
	{
		number = number*a;
	}
	return number;
}
//��ʮ�����ַ���ת����ʮ��������
int number(char str[],int len)
{
	int i=0,number=0;
	for(i=0;i<len;i++)
	{
		switch(str[i])
			{
			case '0':
				number = number + 0;
				break;
			case '1':
				number = number + 1*mi(10,(len-i-2));
				break;
			case '2':
				number = number + 2*mi(10,(len-i-2));
				break;
			case '3':
				number = number + 3*mi(10,(len-i-2));
				break;
			case '4':
				number = number + 4*mi(10,(len-i-2));
				break;
			case '5':
				number = number + 5*mi(10,(len-i-2));
				break;
			case '6':
				number = number + 6*mi(10,(len-i-2));
				break;
			case '7':
				number = number + 7*mi(10,(len-i-2));
				break;
			case '8':
				number = number + 8*mi(10,(len-i-2));
				break;
			case '9':
				number = number + 9*mi(10,(len-i-2));
				break;
			default:
				;
		  }
	}
	return number;
}
void at_ceshi(void)
{
	uint8_t TX_BUF[100];
	static uint8_t temp_a,temp_b;
	uint16_t i,j;
	static int temp_number;
	if(buzou==40)
	{
	}
	if(buzou==39)
	{
		if(yanshi>100)
		{
			sprintf((char*)&TX_BUF[0],"����������Ӧ\r\n");
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		}
		else
		{
			sprintf((char*)&TX_BUF[0],"��Ϣ�ϱ��ɹ�8=%d\r\n",temp_number);
			//HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
			HAL_UART_Transmit(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),10);
		}
		buzou++;
	}
	if(buzou==38)
	{
			if(frame_2[uart_2_frame_id].status!=0) {    			//���յ����ݺ�status=1;
			
				HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
				frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
				buzou++;
				yanshi=0;
				i=0;
				//�ó����ݳ���
				for(i=0;i<frame_2[uart_2_frame_id].length;i++)
				{
					if(frame_2[uart_2_frame_id].buffer[i]==':')temp_a=i;
					if(frame_2[uart_2_frame_id].buffer[i]==',')temp_b=i;
				}
				if(temp_b-temp_a==3)
				{
					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
					TX_BUF[1]=frame_2[uart_2_frame_id].buffer[temp_b-1];
					temp_number=number(TX_BUF,2);
					i=temp_number;
				}
				if(temp_b-temp_a==2)
				{
					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
					temp_number=number(TX_BUF,1);
					i=temp_number;
				}
				//ʮ�������ַ���תʮ��������
				if(frame_2[uart_2_frame_id].buffer[temp_b+i*2]=='3'&&frame_2[uart_2_frame_id].buffer[temp_b+i*2]=='A')
				{
					//�������ݰ��ϴ��ɹ�
					mokuai_sate4=1;
				}
				else
				{
					//�������ݰ��ϴ�ʧ��
					mokuai_sate4=0;
				}
			}
			if(yanshi>100)
			{
				sprintf((char*)&TX_BUF[0],"û���յ��ظ�\r\n");
	    	HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				buzou++;
			}
  }
	if(buzou==37)
	{
		if(frame_2[uart_2_frame_id].status!=0)
		{
			lmm_jishu++;
			yanshi=0;
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
			if(lmm_jishu<2)frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
		}
		if(lmm_jishu==2||yanshi>100)
		{
			buzou++;
			lmm_jishu=0;
		}
		else
		{
			yanshi++;
		}
	}
	if(buzou==36)
	{
		//����״̬
/*
		if(shang_bao_id==0xff)
		{
			shang_bao[3]=shang_bao_id;//����ID��ÿ�ε���
			shang_bao_id=0;
		}
		else
		{
			shang_bao[3]=shang_bao_id;//����ID��ÿ�ε���
			shang_bao_id++;
		}
		shang_bao[6]=0;//��Ŀǰ״̬
*/
		crc_temp = 0xFFFF;
		for(i=0;i<7;i++)
		{
			crc_char(shang_bao[i]);
		}
		shang_bao[7]=(crc_temp>>8);shang_bao[8]=(crc_temp&0x00ff);
		
		sprintf((char*)&TX_BUF[0],"AT+CTM2MSEND=%02X%02X%02X%02X%02X%02X%02X%02X%02X,1\r\n",shang_bao[0],shang_bao[1],shang_bao[2],shang_bao[3],shang_bao[4],shang_bao[5],shang_bao[6],shang_bao[7],shang_bao[8]);
		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),40);
		buzou++;
		yanshi=0;
	}
//////////////////////////////////////////////////////////////
	if(buzou==35)
	{
	}
	if(buzou==34)
	{
		if(yanshi>100)
		{
			sprintf((char*)&TX_BUF[0],"����������Ӧ\r\n");
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		}
		else
		{
			if(temp_number==1)
			{
				sprintf((char*)&TX_BUF[0],"ִ�п���\r\n");
				//HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				HAL_UART_Transmit(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),10);
			}
			if(temp_number==0)
			{
				sprintf((char*)&TX_BUF[0],"ִ�й���\r\n");
				//HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				HAL_UART_Transmit(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),10);
			}
		}
		buzou++;
	}
	if(buzou==33)
	{
			if(frame_2[uart_2_frame_id].status!=0) {    			//���յ����ݺ�status=1;
			
				HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
				frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
				buzou++;
				yanshi=0;
				i=0;
				//�ó����ݳ���
				for(i=0;i<frame_2[uart_2_frame_id].length;i++)
				{
					if(frame_2[uart_2_frame_id].buffer[i]==':')temp_a=i;
					if(frame_2[uart_2_frame_id].buffer[i]==',')temp_b=i;
				}
				if(temp_b-temp_a==3)
				{
					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
					TX_BUF[1]=frame_2[uart_2_frame_id].buffer[temp_b-1];
					temp_number=number(TX_BUF,2);
					i=temp_number;
				}
				if(temp_b-temp_a==2)
				{
					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
					temp_number=number(TX_BUF,1);
					i=temp_number;
				}
				//ʮ�������ַ���תʮ��������
				if(frame_2[uart_2_frame_id].buffer[temp_b+14]=='1'&&frame_2[uart_2_frame_id].buffer[temp_b+12]=='0'&&frame_2[uart_2_frame_id].buffer[temp_b+11]=='0')
				{
					//����
					temp_number=1;
					mokuai_sate3=1;
				}
				else
				{
					//����
					temp_number=0;
					mokuai_sate3=0;
				}
			}
			if(yanshi>100)
			{
				sprintf((char*)&TX_BUF[0],"û���յ��ظ�\r\n");
	    	HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				buzou++;
			}
  }
	if(buzou==32)
	{
		if(frame_2[uart_2_frame_id].status!=0)
		{
			lmm_jishu++;
			yanshi=0;
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
			if(lmm_jishu<2)frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
		}
		if(lmm_jishu==2||yanshi>100)
		{
			buzou++;
			lmm_jishu=0;
		}
		else
		{
			yanshi++;
		}
	}
	if(buzou==31)
	{
		//����״̬
/*
		if(kai_suo_id==0xff)
		{
			kai_suo[3]=kai_suo_id;//����ID��ÿ�ε���
			kai_suo_id=0;
		}
		else
		{
			kai_suo[3]=kai_suo_id;//����ID��ÿ�ε���
			kai_suo_id++;
		}
		kai_suo[6]=0;//��Ŀǰ״̬
*/	
		crc_temp = 0xFFFF;
		for(i=0;i<7;i++)
		{
			crc_char(kai_suo[i]);
		}
		kai_suo[7]=(crc_temp>>8);kai_suo[8]=(crc_temp&0x00ff);
		
		sprintf((char*)&TX_BUF[0],"AT+CTM2MSEND=%02X%02X%02X%02X%02X%02X%02X%02X%02X,1\r\n",kai_suo[0],kai_suo[1],kai_suo[2],kai_suo[3],kai_suo[4],kai_suo[5],kai_suo[6],kai_suo[7],kai_suo[8]);
		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),40);
		buzou++;
		yanshi=0;
	}
//////////////////////////////////////////////////////////////
		if(buzou==30)
	{
	}
	if(buzou==29)
	{
		if(yanshi>100)
		{
			sprintf((char*)&TX_BUF[0],"��������Ӧ\r\n");
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
			mokuai_sate2=0;
		}
		else
		{
			sprintf((char*)&TX_BUF[0],"����ͨѶ�ɹ�\r\n");
			//HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
			HAL_UART_Transmit(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),10);
			mokuai_sate2=1;
		}
		buzou++;
	}
	if(buzou==28)
	{
			if(frame_2[uart_2_frame_id].status!=0) {    			//���յ����ݺ�status=1;
			
				HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
				frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
				buzou++;
				yanshi=0;
				i=0;
				    ////�������ݲ������κδ���////
//				//�ó����ݳ���
//				for(i=0;i<frame_2[uart_2_frame_id].length;i++)
//				{
//					if(frame_2[uart_2_frame_id].buffer[i]==':')temp_a=i;
//					if(frame_2[uart_2_frame_id].buffer[i]==',')temp_b=i;
//				}
//				if(temp_b-temp_a==3)
//				{
//					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
//					TX_BUF[1]=frame_2[uart_2_frame_id].buffer[temp_b-1];
//					temp_number=number(TX_BUF,2);
//					i=temp_number;
//				}
//				if(temp_b-temp_a==2)
//				{
//					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
//					temp_number=number(TX_BUF,1);
//					i=temp_number;
//				}
//				//ʮ�������ַ���תʮ��������ȡ����ֱ�ӽ����ַ��Ա���֤
//				if(frame_2[uart_2_frame_id].buffer[temp_b+i*2]=='3'&&frame_2[uart_2_frame_id].buffer[temp_b+i*2]=='A')
//				{
//					//�����ɹ����յ�����������
//				}
//				else
//				{
//					//�����ϴ�ʧ��
//				}
			}
			if(yanshi>100)
			{
				sprintf((char*)&TX_BUF[0],"û���յ��ظ�\r\n");
	    	HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				buzou++;
			}
  }
	if(buzou==27)
	{
		if(frame_2[uart_2_frame_id].status!=0)
		{
			lmm_jishu++;
			yanshi=0;
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
			if(lmm_jishu<2)frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
		}
		if(lmm_jishu==2||yanshi>100)
		{
			buzou++;
			lmm_jishu=0;
		}
		else
		{
			yanshi++;
		}
	}
	if(buzou==26)
	{
		//����״̬
/*
		if(xin_tiao_id==0xff)
		{
			xin_tiao[3]=xin_tiao_id;//����ID��ÿ�ε���
			xin_tiao_id=0;
		}
		else
		{
			xin_tiao[3]=xin_tiao_id;//����ID��ÿ�ε���
			xin_tiao_id++;
		}
		xin_tiao[6]=;//Ӳ���汾��
		xin_tiao[8]=;//����汾��
		xin_tiao[10]=;//�ź�ǿ��
		xin_tiao[12]=;//֧�ֵĴ�������
		xin_tiao[14]=;//������ص���
*/
		crc_temp = 0xFFFF;
		for(i=0;i<15;i++)
		{
			crc_char(xin_tiao[i]);
		}
		xin_tiao[15]=(crc_temp>>8);xin_tiao[16]=(crc_temp&0x00ff);//����crcУ������
		
		sprintf((char*)&TX_BUF[0],"AT+CTM2MSEND=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,1\r\n",xin_tiao[0],xin_tiao[1],xin_tiao[2],xin_tiao[3],xin_tiao[4],xin_tiao[5],xin_tiao[6],xin_tiao[7],xin_tiao[8],xin_tiao[9],xin_tiao[10],xin_tiao[11],xin_tiao[12],xin_tiao[13],xin_tiao[14],xin_tiao[15],xin_tiao[16]);
		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),40);
		buzou++;
		yanshi=0;
	}
//////////////////////////////////////////////////////////////
	if(buzou==25)
	{
	}
	if(buzou==24)
	{
		if(yanshi>100)
		{
			sprintf((char*)&TX_BUF[0],"����������Ӧ\r\n");
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		}
		else
		{
			sprintf((char*)&TX_BUF[0],"�������ݳɹ�8=%d\r\n",temp_number);
			//HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
			HAL_UART_Transmit(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),10);
		}
		buzou++;
	}
	if(buzou==23)
	{
			if(frame_2[uart_2_frame_id].status!=0) {    			//���յ����ݺ�status=1;
			
				HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
				frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
				buzou++;
				yanshi=0;
				i=0;
				//�ó����ݳ���
				for(i=0;i<frame_2[uart_2_frame_id].length;i++)
				{
					if(frame_2[uart_2_frame_id].buffer[i]==':')temp_a=i;
					if(frame_2[uart_2_frame_id].buffer[i]==',')temp_b=i;
				}
				if(temp_b-temp_a==3)
				{
					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
					TX_BUF[1]=frame_2[uart_2_frame_id].buffer[temp_b-1];
					temp_number=number(TX_BUF,2);
					i=temp_number;
				}
				if(temp_b-temp_a==2)
				{
					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
					temp_number=number(TX_BUF,1);
					i=temp_number;
				}
				//ʮ�������ַ���תʮ��������
				if(frame_2[uart_2_frame_id].buffer[temp_b+i*2]=='3'&&frame_2[uart_2_frame_id].buffer[temp_b+i*2]=='A')
				{
					//�������ݰ��ϴ��ɹ�
					mokuai_sate1=1;
				}
				else
				{
					//�������ݰ��ϴ�ʧ��
					mokuai_sate1=0;
				}
			}
			if(yanshi>100)
			{
				sprintf((char*)&TX_BUF[0],"û���յ��ظ�\r\n");
	    	HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				buzou++;
			}
  }
	if(buzou==22)
	{
		if(frame_2[uart_2_frame_id].status!=0)
		{
			lmm_jishu++;
			yanshi=0;
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
			if(lmm_jishu<2)frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
		}
		if(lmm_jishu==2||yanshi>100)
		{
			buzou++;
			lmm_jishu=0;
		}
		else
		{
			yanshi++;
		}
	}
	if(buzou==21)
	{
		//����״̬
/*
		if(qi_dong_id==0xff)
		{
			qi_dong[3]=qi_dong_id;//����ID��ÿ�ε���
			qi_dong_id=0;
		}
		else
		{
			qi_dong[3]=qi_dong_id;//����ID��ÿ�ε���
			qi_dong_id++;
		}
		qi_dong[6]=;//Ӳ���汾��
		qi_dong[8]=;//����汾��
		qi_dong[10]=at_xinhao;//�ź�ǿ��
		qi_dong[12]=;//֧�ֵĴ�������
		qi_dong[14]=;//������ص���
*/
		crc_temp = 0xFFFF;
		for(i=0;i<15;i++)
		{
			crc_char(qi_dong[i]);
		}
		qi_dong[15]=(crc_temp>>8);qi_dong[16]=(crc_temp&0x00ff);
		
		sprintf((char*)&TX_BUF[0],"AT+CTM2MSEND=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,1\r\n",qi_dong[0],qi_dong[1],qi_dong[2],qi_dong[3],qi_dong[4],qi_dong[5],qi_dong[6],qi_dong[7],qi_dong[8],qi_dong[9],qi_dong[10],qi_dong[11],qi_dong[12],qi_dong[13],qi_dong[14],qi_dong[15],qi_dong[16]);
		HAL_UART_Transmit(&UART_Config_AT,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),40);
		buzou++;
		yanshi=0;
	}
//////////////////////////////////////////////////////////////
	if(buzou==20)
	{
	}
	if(buzou==19)
	{
		if(yanshi>100)
		{
			sprintf((char*)&TX_BUF[0],"ģ��ͨ��ʧ��\r\n");
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		}
		else
		{
			sprintf((char*)&TX_BUF[0],"����IMSI��%s\r\n",LMM_IMSI);
			//HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
			HAL_UART_Transmit(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),10);
		}
		buzou++;
	}
	if(buzou==18)
	{
			if(frame_2[uart_2_frame_id].status!=0) {    			//���յ����ݺ�status=1;
			
				HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
				frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
				buzou++;
				yanshi=0;
				i=0;
				for(i=0;i<frame_2[uart_2_frame_id].length;i++)
				{
					if(frame_2[uart_2_frame_id].buffer[i]=='I')temp_a=i;
				}
				j=0;
				for(j=0;j<15;j++)
				{
					LMM_IMSI[j]=frame_2[uart_2_frame_id].buffer[temp_a+j+3];
				}
       }
			if(yanshi>100)
			{
				sprintf((char*)&TX_BUF[0],"û���յ��ظ�\r\n");
	    	HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				buzou++;
			}
  }
	if(buzou==17)
	{
		if(frame_2[uart_2_frame_id].status!=0||yanshi>100)
		{
			buzou++;
		}
		else
		{
			yanshi++;
		}
	}
	if(buzou==16)
	{
		sprintf((char*)&TX_BUF[0],"AT+CIMI\r\n");
		HAL_UART_Transmit_IT(&UART_Config_AT,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		buzou++;
		yanshi=0;
	}
////////////////////////////////////////////////////////////
	if(buzou==15)
	{
	}
	if(buzou==14)
	{
		if(yanshi>100)
		{
			sprintf((char*)&TX_BUF[0],"ģ��ͨ��ʧ��\r\n");
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		}
		else
		{
			sprintf((char*)&TX_BUF[0],"�豸IMEI��%s\r\n",LMM_IMEI);
			//HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
			HAL_UART_Transmit(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]),10);
		}
		buzou++;
	}
	if(buzou==13)
	{
			if(frame_2[uart_2_frame_id].status!=0) {    			//���յ����ݺ�status=1;
			
				HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
				frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
				buzou++;
				yanshi=0;
				i=0;
				for(i=0;i<frame_2[uart_2_frame_id].length;i++)
				{
					if(frame_2[uart_2_frame_id].buffer[i]==':')temp_a=i;
				}
				j=0;
				for(j=0;j<15;j++)
				{
					LMM_IMEI[j]=frame_2[uart_2_frame_id].buffer[temp_a+j+2];
				}
       }
			if(yanshi>100)
			{
				sprintf((char*)&TX_BUF[0],"û���յ��ظ�\r\n");
	    	HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				buzou++;
			}
  }
	if(buzou==12)
	{
		if(frame_2[uart_2_frame_id].status!=0||yanshi>100)
		{
			buzou++;
		}
		else
		{
			yanshi++;
		}
	}
	if(buzou==11)
	{
		sprintf((char*)&TX_BUF[0],"AT+CGSN=1\r\n");
		HAL_UART_Transmit_IT(&UART_Config_AT,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		buzou++;
		yanshi=0;
	}
////////////////////////////////////////////////////////////	
	if(buzou==10)
	{
	}
	if(buzou==9)
	{
		if(yanshi>100)
		{
			sprintf((char*)&TX_BUF[0],"ģ��ͨ��ʧ��\r\n");
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		}
		else
		{
			sprintf((char*)&TX_BUF[0],"�ź�������%d\r\n",at_xinhao);
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		}
		buzou++;
	}
	if(buzou==8)
	{

			if(frame_2[uart_2_frame_id].status!=0) {    			//���յ����ݺ�status=1;
			
				HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
				frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
				buzou++;
				yanshi=0;
				i=0;
				for(i=0;i<frame_2[uart_2_frame_id].length;i++)
				{
					if(frame_2[uart_2_frame_id].buffer[i]==':')temp_a=i;
					if(frame_2[uart_2_frame_id].buffer[i]==',')temp_b=i;
				}
				if(temp_b-temp_a==3)
				{
					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
					TX_BUF[1]=frame_2[uart_2_frame_id].buffer[temp_b-1];
					at_xinhao=number(TX_BUF,2);
				}
				if(temp_b-temp_a==2)
				{
					TX_BUF[0]=frame_2[uart_2_frame_id].buffer[temp_a+1];
					at_xinhao=number(TX_BUF,1);
				}
       }
			if(yanshi>100)
			{
				sprintf((char*)&TX_BUF[0],"û���յ��ظ�\r\n");
	    	HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				buzou++;
			}
  }
	if(buzou==7)
	{
		if(frame_2[uart_2_frame_id].status!=0||yanshi>100)
		{
			buzou++;
		}
		else
		{
			yanshi++;
		}
	}
	if(buzou==6)
	{
		sprintf((char*)&TX_BUF[0],"AT+CSQ\r\n");
		HAL_UART_Transmit_IT(&UART_Config_AT,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		buzou++;
		yanshi=0;
	}
////////////////////////////////////////////////////////////	
	if(buzou==5)
	{
	}
	if(buzou==4)
	{
		if(at_state==1)
		{
			sprintf((char*)&TX_BUF[0],"ģ��ͨ�ųɹ�\r\n");
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		}
		else
		{
			sprintf((char*)&TX_BUF[0],"ģ��ͨ��ʧ��\r\n");
			HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		}
		buzou++;
	}
	if(buzou==3)
	{

			if(frame_2[uart_2_frame_id].status!=0) {    			//���յ����ݺ�status=1;
			
				HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)frame_2[uart_2_frame_id].buffer,frame_2[uart_2_frame_id].length);
				frame_2[uart_2_frame_id].status=0;					//���������ݺ�status ��0;
				buzou++;
				yanshi=0;
				if(frame_2[uart_2_frame_id].buffer[4]=='O'&&frame_2[uart_2_frame_id].buffer[5]=='K')
				{
					at_state=1; //ģ��״̬
				}
				else
				{
					at_state=0; //ģ��״̬
				}
       }
			if(yanshi>100)
			{
				yanshi=0;
				sprintf((char*)&TX_BUF[0],"û���յ��ظ�\r\n");
	    	HAL_UART_Transmit_IT(&UART_Config,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
				buzou++;
				at_state=0; //ģ��״̬
			}
  }
	if(buzou==2)
	{
		if(frame_2[uart_2_frame_id].status!=0||yanshi>100)
		{
			buzou++;
		}
		else
		{
			yanshi++;
		}
	}
	if(buzou==1)
	{
		sprintf((char*)&TX_BUF[0],"AT\r\n");
		HAL_UART_Transmit_IT(&UART_Config_AT,(uint8_t*)(char*)&TX_BUF[0],strlen((char*)&TX_BUF[0]));
		buzou++;
		yanshi=0;
	}
}

/////////////////////�����ռ�//////////////
