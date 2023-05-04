#ifndef USER_USER_SW_CHECK_H_
#define USER_USER_SW_CHECK_H_

 #include "user_main.h"
 
#define SW_EN_1  PA03    //红外1 开关
#define SW_EN_2  PA04    //红外2 开关

#define SW_IN_1  PB15    //红外1 输入
#define SW_IN_2  PA00    //红外2 输入
#define SW_IN_3  PA07    //霍尔  输入

#define POS_0      0x01
#define POS_0_90   0x11
#define POS_90     0x10
#define POS_90_180 0x00
#define POS_OUT    0xFF



typedef struct{
bool opt1;
bool opt2;  
bool opt3;
}SW_TypeDef;

extern SW_TypeDef lock_sw;
void	check_sw(void);
void	check_sw_(void);
void	check_sw_wait(void) ;
#endif
