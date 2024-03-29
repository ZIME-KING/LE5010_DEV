#ifndef HW_KEYBOARD_H
#define HW_KEYBOARD_H

#include "main.h"



extern const uint8_t IO_NUMBER ;// Used 11 pcs 74HC165
extern const uint8_t KEY_NUMBER ;

//static const uint8_t TOUCHPAD_NUMBER = 6;	//没有触摸按键

//static const uint8_t LED_NUMBER = 104;    //没有LED灯

extern const uint16_t KEY_REPORT_SIZE ;
extern const uint16_t RAW_REPORT_SIZE;
extern const uint16_t HID_REPORT_SIZE ;

//extern int16_t keyMap[5][88] ;




typedef enum 
{    
        /*------------------------- HID report data -------------------------*/
        LEFT_CTRL = -8,LEFT_SHIFT = -7,LEFT_ALT = -6,LEFT_GUI = -5,
        RIGHT_CTRL = -4,RIGHT_SHIFT = -3,RIGHT_ALT = -2,RIGHT_GUI = -1,

        RESERVED = 0,ERROR_ROLL_OVER,POST_FAIL,ERROR_UNDEFINED,
        A,B,C,D,E,F,G,H,I,J,K,L,M,
        N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
        NUM_1/*1!*/,NUM_2/*2@*/,NUM_3/*3#*/,NUM_4/*4$*/,NUM_5/*5%*/,
        NUM_6/*6^*/,NUM_7/*7&*/,NUM_8/*8**/,NUM_9/*9(*/,NUM_0/*0)*/,
        ENTER,ESC,BACKSPACE,TAB,SPACE,
        MINUS/*-_*/,EQUAL/*=+*/,LEFT_U_BRACE/*[{*/,RIGHT_U_BRACE/*]}*/,
        BACKSLASH/*\|*/,NONE_US/**/,SEMI_COLON/*;:*/,QUOTE/*'"*/,
        GRAVE_ACCENT/*`~*/,COMMA/*,<*/,PERIOD/*.>*/,SLASH/*/?*/,
        CAP_LOCK,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
        PRINT,SCROLL_LOCK,PAUSE,INSERT,HOME,PAGE_UP,DELETE,END,PAGE_DOWN,
        RIGHT_ARROW,LEFT_ARROW,DOWN_ARROW,UP_ARROW,PAD_NUM_LOCK,
        PAD_SLASH,PAD_ASTERISK,PAD_MINUS,PAD_PLUS,PAD_ENTER,
        PAD_NUM_1,PAD_NUM_2,PAD_NUM_3,PAD_NUM_4,PAD_NUM_5,
        PAD_NUM_6,PAD_NUM_7,PAD_NUM_8,PAD_NUM_9,PAD_NUM_0,
        PAD_PERIOD , NONUS_BACKSLASH,APPLICATION,POWER,PAD_EQUAL,
        F13,F14,F15,F16,F17,F18,F19,F20,F21,F22,F23,F24, EXECUTE,
        HELP,MENU,SELECT,STOP,AGAIN,UNDO,CUT,COPY,PASTE,FIND,MUTE,VOLUME_UP,VOLUME_DOWN,
        FN = 1000
        /*------------------------- HID report data -------------------------*/
}KeyCode_t;



#endif
