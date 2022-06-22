#ifndef __KEY_H
#define __KEY_H


extern unsigned char key_status;	
extern unsigned char key_busy; 		//按键事件忙标记，	

extern unsigned char touch_key_busy;
extern unsigned char touch_key_staus;

enum {
SHORT=1,
LONG,
DOUBLE,
LONG_NO_RELEASE
};

void scan_touch_key(void);
void scan_key(void);

#endif
