#include "user_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG_AES_128

//unsigned char PlainText[16] = { 0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34 },
//	CipherKey[16] = { 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c },
//	CipherKey1[16] = { 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c },
//	PlainText1[16] = { 0 },
//	PlainText2[16] = { 0 };
unsigned char PlainText[16] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10 },
	CipherKey[16] = { 0x58,0x49,0x4e,0x4a,0x59,0x55,0x45,0x5a,0x4e,0x2e,0x43,0x4f,0x4d,0x30,0x35,0x31 },
	CipherKey1[16] = { 0x58,0x49,0x4e,0x4a,0x59,0x55,0x45,0x5a,0x4e,0x2e,0x43,0x4f,0x4d,0x30,0x35,0x31 },
	PlainText1[16] = { 0 },
	PlainText2[16] = { 0 };
unsigned int CipherKey_word[44] = { 0 };
void AddRoundKey(unsigned char *plaintext, unsigned char * CipherKey)/*????*/
{
	int j=0;
	for (j = 0;j < 16;j++)  plaintext[j] = plaintext[j] ^ CipherKey[j];
}
void SubBytes(unsigned char *plaintext, unsigned char *plaintextencrypt, int count)/*S???*/
{
	unsigned int row, column;
	int i=0;
	unsigned char Sbox[16][16] = {
		      /* 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
		/*0*/{ 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76 },
		/*1*/{ 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0 },
		/*2*/{ 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15 },
		/*3*/{ 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75 },
		/*4*/{ 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84 },
		/*5*/{ 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf },
		/*6*/{ 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8 },
		/*7*/{ 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2 },
		/*8*/{ 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73 },
		/*9*/{ 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb },
		/*a*/{ 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79 },
		/*b*/{ 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08 },
		/*c*/{ 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a },
		/*d*/{ 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e },
		/*e*/{ 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf },
		/*f*/{ 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }
	};// ??Sbox??
	for (i = 0; i < count; i++)
	{
		row = (plaintext[i] & 0xF0) >> 4;
		column = plaintext[i] & 0x0F;
		plaintextencrypt[i] = Sbox[row][column];
	}
}
void SubBytesRe(unsigned char *plaintext, unsigned char *plaintextencrypt, int count)/*S????*/
{
	unsigned int row, column;
	int i=0;
	unsigned char Sbox[16][16] = {
		/* 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
		{0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb},
		{0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb},
		{0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e},
		{0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25},
		{0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92},
		{0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84},
		{0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06},
		{0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b},
		{0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73},
		{0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e},
		{0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b},
		{0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4},
		{0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f},
		{0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef},
		{0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61},
		{0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d}
	};	// ??Sbox??
	for (i = 0; i < count; i++)
	{
		row = (plaintext[i] & 0xF0) >> 4;
		column = plaintext[i] & 0x0F;
		plaintextencrypt[i] = Sbox[row][column];
	}
}
void ShiftRowsRe(unsigned char *plaintextencrypt)/*?????*/
{
	unsigned char temp = 0;
	int i=0,j=0,k=0;
	for (i = 0; i < 4; i++)//?i?
	{
		for (j = 0; j < 4 - i; j++)//?j???
		{
			temp = plaintextencrypt[i];
			for (k = 0; k < 4; k++)
				plaintextencrypt[i + 4 * k] = plaintextencrypt[i + 4 * (k + 1)];
			plaintextencrypt[i + 12] = temp;
		}
	}
}
void ShiftRows(unsigned char *plaintextencrypt)/*???*/
{
	unsigned char temp = 0;
	int i=0,j=0,k=0;
	for (i = 0; i < 4; i++)//?i?
	{
		for (j = 0; j < i; j++)//?j???
		{
			temp = plaintextencrypt[i];
			for (k = 0; k < 4; k++)
				plaintextencrypt[i + 4 * k] = plaintextencrypt[i + 4 * (k + 1)];
			plaintextencrypt[i + 12] = temp;
		}
	}
}
unsigned char Mult2(unsigned char num)/*???*/
{
	unsigned char temp = num << 1;
	if ((num >> 7) & 0x01)
		temp = temp ^ 27;
	return temp;
}
unsigned char Mult3(unsigned char num)
{
	return Mult2(num) ^ num;
}
void MixColumns(unsigned char *plaintextencrypt, unsigned char *plaintextcrypt)
{
	int i;
	for (i = 0; i < 4; i++)
		plaintextcrypt[4 * i] = Mult2(plaintextencrypt[4 * i]) ^ Mult3(plaintextencrypt[4 * i + 1]) ^ plaintextencrypt[4 * i + 2] ^ plaintextencrypt[4 * i + 3];
	for (i = 0; i < 4; i++)
		plaintextcrypt[4 * i + 1] = plaintextencrypt[4 * i] ^ Mult2(plaintextencrypt[4 * i + 1]) ^ Mult3(plaintextencrypt[4 * i + 2]) ^ plaintextencrypt[4 * i + 3];
	for (i = 0; i < 4; i++)
		plaintextcrypt[4 * i + 2] = plaintextencrypt[4 * i] ^ plaintextencrypt[4 * i + 1] ^ Mult2(plaintextencrypt[4 * i + 2]) ^ Mult3(plaintextencrypt[4 * i + 3]);
	for (i = 0; i < 4; i++)
		plaintextcrypt[4 * i + 3] = Mult3(plaintextencrypt[4 * i]) ^ plaintextencrypt[4 * i + 1] ^ plaintextencrypt[4 * i + 2] ^ Mult2(plaintextencrypt[4 * i + 3]);
}
/*????*/ 
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))
#define Multiply(x,y) (((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))
void MixColumnsRe(unsigned char *state)
{
	unsigned char a, b, c, d;
	int i=0;
	for (i = 0; i < 4; i++)
	{
		a = state[4*i];
		b = state[4*i+1];
		c = state[4*i+2];
		d = state[4*i+3];
		state[4 * i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
		state[4 * i + 1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
		state[4 * i + 2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
		state[4 * i + 3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
	}
}
int CharToWord(unsigned char *character, int first)/*????*/
{
	return (((int)character[first] & 0x000000ff) << 24) | (((int)character[first + 1] & 0x000000ff) << 16) | (((int)character[first + 2] & 0x000000ff) << 8) | ((int)character[first + 3] & 0x000000ff);
}
void WordToChar(unsigned int word, unsigned char *character)/*????*/
{
	int i=0;
	for (i = 0; i < 4; character[i++] = (word >> (8 * (3 - i))) & 0xFF);
}
void ExtendCipherKey(unsigned int *CipherKey_word, int round)/*????*/
{
	int i=0;
	unsigned char CipherKeyChar[4] = { 0 },CipherKeyCharEncrypt[4] = { 0 };
	unsigned int Rcon[10] = { 0x01000000,0x02000000,0x04000000,0x08000000,0x10000000,0x20000000,0x40000000,0x80000000,0x1B000000,0x36000000 };	//???
	for (i = 4; i < 8; i++)
	{
		if (!(i % 4))
		{
			WordToChar((CipherKey_word[i - 1] >> 24) | (CipherKey_word[i - 1] << 8), CipherKeyChar);
			SubBytes(CipherKeyChar, CipherKeyCharEncrypt, 4);
			CipherKey_word[i] = CipherKey_word[i - 4] ^ CharToWord(CipherKeyCharEncrypt, 0) ^ Rcon[round];
		}
		else
			CipherKey_word[i] = CipherKey_word[i - 4] ^ CipherKey_word[i - 1];
	}
}


//调用前重新给
void AES_ceshi(void)
{
	int i;
	int k;
//	unsigned char PlainText[16] = { 0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34 },
//		CipherKey[16] = { 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c },
//		CipherKey1[16] = { 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c },
//		PlainText1[16] = { 0 },
//		PlainText2[16] = { 0 };
//	unsigned int CipherKey_word[44] = { 0 };
////	printf("**************AES加解密***************\r\n");
////	printf("密钥:");
////	for (k = 0; k < 16; k++) printf("%2X ", CipherKey[k]);
////	printf("\r\n明文:");
////	for (k = 0; k < 16; k++) printf("%02X ", PlainText[k]);
	aes_jia_mi();
	aes_jie_mi();
	while(1)
	{
	}
	printf("\r\n**************开始加密****************");
	AddRoundKey(PlainText, CipherKey);
	for (i = 0; i < 9; i++)
	{
//		printf("\r\n第%d轮循环:\r\n", i + 1);
		SubBytes(PlainText, PlainText1, 16);/*S盒置换*/
		ShiftRows(PlainText1);	/*行位移*/
		MixColumns(PlainText1, PlainText2);	/*列混淆*/
		ExtendCipherKey(CipherKey_word + 4 * i, i);/*子密钥生成*/
		for (k = 0; k < 4; k++)  WordToChar(CipherKey_word[k + 4 * (i + 1)], CipherKey + 4 * k);
//		printf("此时的密钥为:    ");
//		for (k = 0; k < 16; k++)  printf("%02X ", CipherKey[k]);
		AddRoundKey(PlainText2, CipherKey);/*轮密钥加*/
		for (k = 0; k < 16; k++)  PlainText[k] = PlainText2[k];
//		printf("\r\n当前明文加密后:");
//		for (k = 0; k < 16; k++)  printf("%02X ", PlainText2[k]);
//		printf("\r\n");
	}
//	printf("\r\n最后一次循环:\r\n");
	SubBytes(PlainText, PlainText1, 16);
	ShiftRows(PlainText1);
	ExtendCipherKey(CipherKey_word + 4 * i, i);
	for (k = 0; k < 4;WordToChar(CipherKey_word[k + 4 * (i + 1)], CipherKey + 4 * k), k++);
	printf("此时的子密钥:     ");
	for (k = 0; k < 16; k++)  printf("%02X ", CipherKey[k]);
	AddRoundKey(PlainText1, CipherKey);
	printf("\r\n\r\n最终AES加密后的密文为:");
	for (i = 0; i < 16; i++)  printf("%02X ", PlainText1[i]);
	
	printf("\r\n\r\n**************开始解密***************");
	AddRoundKey(PlainText1, CipherKey);
	for (i = 0; i < 9; i++)
	{
//		printf("\r\n第%d次循环:", i + 1);
	    SubBytesRe(PlainText1, PlainText, 16);/*S???*/
		for (k = 0; k < 4; WordToChar(CipherKey_word[k + 40 - 4 * (i + 1)], CipherKey + 4 * k),k++);/*?????*/
		ShiftRowsRe(PlainText);/*????*/
		AddRoundKey(PlainText, CipherKey);/*????*/
		MixColumnsRe(PlainText);/*??????*/
		for (k = 0; k < 16;PlainText1[k] = PlainText[k],k++);
//		printf("\r\n当前密文解密之后为:");
//		for (k = 0; k < 16; k++)printf("%02X ", PlainText[k]);
//		printf("\r\n");
	}
//	printf("\r\n最后一次循环:");
	ShiftRowsRe(PlainText);/*????*/
	SubBytesRe(PlainText, PlainText1, 16);/*S???*/
	AddRoundKey(PlainText1, CipherKey1);
	printf("\r\n最终AES解密后的明文为:");
	for (i = 0; i < 16; i++)  printf("%02X ", PlainText1[i]);
	printf("\r\n");
	
}

//需要加密的数组PlainText
void aes_jia_mi(void)
{
	int i,j,k;
	
	#ifdef  DEBUG_AES_128
	printf("\r\n**************开始加密****************");
	#endif
	
	for (i = 0; i < 4; CipherKey_word[i++] = CharToWord(CipherKey, 4 * i));
	AddRoundKey(PlainText, CipherKey);
	for (i = 0; i < 9; i++)
	{
//		printf("\r\n第%d轮循环:\r\n", i + 1);
		SubBytes(PlainText, PlainText1, 16);/*S盒置换*/
		ShiftRows(PlainText1);	/*行位移*/
		MixColumns(PlainText1, PlainText2);	/*列混淆*/
		ExtendCipherKey(CipherKey_word + 4 * i, i);/*子密钥生成*/
		for (k = 0; k < 4; k++)  WordToChar(CipherKey_word[k + 4 * (i + 1)], CipherKey + 4 * k);
//		printf("此时的密钥为:    ");
//		for (k = 0; k < 16; k++)  printf("%02X ", CipherKey[k]);
		AddRoundKey(PlainText2, CipherKey);/*轮密钥加*/
		for (k = 0; k < 16; k++)  PlainText[k] = PlainText2[k];
//		printf("\r\n当前明文加密后:");
//		for (k = 0; k < 16; k++)  printf("%02X ", PlainText2[k]);
//		printf("\r\n");
	}
//	printf("\r\n最后一次循环:\r\n");
	SubBytes(PlainText, PlainText1, 16);
	ShiftRows(PlainText1);
	ExtendCipherKey(CipherKey_word + 4 * i, i);
	for (k = 0; k < 4;WordToChar(CipherKey_word[k + 4 * (i + 1)], CipherKey + 4 * k), k++);
//	printf("此时的子密钥:     ");
//	for (k = 0; k < 16; k++)  printf("%02X ", CipherKey[k]);
	AddRoundKey(PlainText1, CipherKey);

	#ifdef  DEBUG_AES_128
	printf("\r\n\r\n最终AES加密后的密文为:");
	for (i = 0; i < 16; i++)  printf("%02X ", PlainText1[i]);
	#endif
}

//需要解密的数组PlainText1
void aes_jie_mi(void)
{
	int i,j,k;
	
	#ifdef  DEBUG_AES_128
	printf("\r\n\r\n**************开始解密***************");
	#endif
	AddRoundKey(PlainText1, CipherKey);
	for (i = 0; i < 9; i++)
	{
//		printf("\r\n第%d次循环:", i + 1);
	    SubBytesRe(PlainText1, PlainText, 16);/*S???*/
		for (k = 0; k < 4; WordToChar(CipherKey_word[k + 40 - 4 * (i + 1)], CipherKey + 4 * k),k++);/*?????*/
		ShiftRowsRe(PlainText);/*????*/
		AddRoundKey(PlainText, CipherKey);/*????*/
		MixColumnsRe(PlainText);/*??????*/
		for (k = 0; k < 16;PlainText1[k] = PlainText[k],k++);
//		printf("\r\n当前密文解密之后为:");
//		for (k = 0; k < 16; k++)printf("%02X ", PlainText[k]);
//		printf("\r\n");
	}
//	printf("\r\n最后一次循环:");
	ShiftRowsRe(PlainText);/*????*/
	SubBytesRe(PlainText, PlainText1, 16);/*S???*/
	AddRoundKey(PlainText1, CipherKey1);
	
	#ifdef  DEBUG_AES_128
	printf("\r\n最终AES解密后的明文为:");
	for (i = 0; i < 16; i++)  printf("%02X ", PlainText1[i]);
	printf("\r\n");
	#endif
	//需要进行长度判断，要正好等于十六字节
}
//   https://blog.csdn.net/Laoynice/article/details/79196993
//根据获取到的数据进行解析
void DATA_deal(unsigned char temp[])
{
	int i,j;
	if(temp[0]==0x01)//获取令牌
	{
		//产生令牌并回复
	}
	if(temp[0]==0x01)//获取电量
	{
	}
	if(temp[0]==0x01)//开锁
	{
	}
	if(temp[0]==0x01)//关锁
	{
	}
	if(temp[0]==0x01)//关锁通知
	{
	}
	if(temp[0]==0x01)//查询锁状态
	{
	}
	if(temp[0]==0x01)//启动指纹采集
	{
	}
	if(temp[0]==0x01)//采集第n次指纹
	{
	}
	if(temp[0]==0x01)//采集指纹完成
	{
	}
	if(temp[0]==0x01)//取消采集指纹
	{
	}
	if(temp[0]==0x01)//删除指纹
	{
	}
	if(temp[0]==0x01)//修改名称
	{
	}
	if(temp[0]==0x01)//修改密码
	{
	}
	if(temp[0]==0x01)//修改密码
	{
	}
	if(temp[0]==0x01)//修改密钥
	{
	}
	if(temp[0]==0x01)//修改密钥
	{
	}
	if(temp[0]==0x01)//批量数据传输
	{
		//标志位置一，接收数据后走另外一个函数，等接收完标志位再置零
	}
}

//char unsigned in[16]={0x82,0x04,0x4c,0x91,0x20,0xee,0x7a,0xc4,0x7f,0x03,0xE0,0x70,0xAA,0x5D,0x25,0xAE}; 
char unsigned in[16]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10}; 

char unsigned out[16];
char unsigned buf[16];

//加密 
//输入数据地址 
//输出数据地址
//长度
void User_Encryption(unsigned char *in_data,unsigned char *out_data , unsigned char len){
	memcpy(&PlainText[0],in_data,len);
	aes_jia_mi();
	memcpy(out_data,&PlainText1[0],len);
}
//解密 
//输入数据地址 
//输出数据地址
//长度
void User_Decoden(unsigned char *in_data,unsigned char *out_data , unsigned char len){
	memcpy(&PlainText[0],in_data,len);
	aes_jie_mi();
	memcpy(out_data,&PlainText1[0],len);
}
