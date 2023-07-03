#include "hw_keyboard.h"
#include "main.h"
#include <cstring>

 const uint8_t IO_NUMBER = 11 * 8; // Used 11 pcs 74HC165
 const uint8_t KEY_NUMBER = 82;

//static const uint8_t TOUCHPAD_NUMBER = 6;	//没有触摸按键

//static const uint8_t LED_NUMBER = 104;    //没有LED灯

 const uint16_t KEY_REPORT_SIZE = 1 + 16;
 const uint16_t RAW_REPORT_SIZE = 1 + 32;
 const uint16_t HID_REPORT_SIZE = KEY_REPORT_SIZE + RAW_REPORT_SIZE;


    uint8_t spiBuffer[IO_NUMBER / 8 + 1];
    uint8_t* scanBuffer;
    uint8_t debounceBuffer[IO_NUMBER / 8 + 1];
    uint8_t hidBuffer[HID_REPORT_SIZE];
    uint8_t remapBuffer[IO_NUMBER / 8];
//    uint8_t rgbBuffer[LED_NUMBER][3][8]{};
    uint8_t wsCommit[64] = {0};
    uint8_t brightnessPreDiv = 2; // 1/4


int16_t keyMap[5][IO_NUMBER] = {
        {67,61,60,58,59,52,55,51,50,49,48,47,46,3,
            80,81,64,57,62,63,53,54,45,44,40,31,26,18,2,
            19,70,71,66,65,56,36,37,38,39,43,42,41,28,1,
            15,74,73,72,68,69,29,30,35,34,33,32,24,0,
            14,76,77,78,79,16,20,21,22,23,27,25,17,4,
            13,12,8,75,9,10,7,11,6,5,
            86,84,82,87,85,83}, // TouchBar index

        {ESC,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,PAUSE,
            GRAVE_ACCENT,NUM_1,NUM_2,NUM_3,NUM_4,NUM_5,NUM_6,NUM_7,NUM_8,NUM_9,NUM_0,MINUS,EQUAL,BACKSPACE,INSERT,
            TAB,Q,W,E,R,T,Y,U,I,O,P,LEFT_U_BRACE,RIGHT_U_BRACE,BACKSLASH,DELETE,
            CAP_LOCK,A,S,D,F,G,H,J,K,L,SEMI_COLON,QUOTE,ENTER,PAGE_UP,
            LEFT_SHIFT,Z,X,C,V,B,N,M,COMMA,PERIOD,SLASH,RIGHT_SHIFT,UP_ARROW,PAGE_DOWN,
            LEFT_CTRL,LEFT_GUI,LEFT_ALT,SPACE,RIGHT_ALT,FN,RIGHT_CTRL,LEFT_ARROW,DOWN_ARROW,RIGHT_ARROW },

        {ESC,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,PAUSE,
            GRAVE_ACCENT,NUM_1,NUM_2,NUM_3,NUM_4,NUM_5,NUM_6,NUM_7,NUM_8,NUM_9,NUM_0,MINUS,EQUAL,BACKSPACE,INSERT,
            TAB,A,B,C,D,E,F,G,H,I,J,LEFT_U_BRACE,RIGHT_U_BRACE,BACKSLASH,DELETE,
            CAP_LOCK,K,L,M,N,O,P,Q,R,S,SEMI_COLON,QUOTE,ENTER,PAGE_UP,
            LEFT_SHIFT,T,U,V,W,X,Y,Z,COMMA,PERIOD,SLASH,RIGHT_SHIFT,A,PAGE_DOWN,
            LEFT_CTRL,LEFT_GUI,LEFT_ALT,SPACE,RIGHT_ALT,FN,RIGHT_CTRL,LEFT_ARROW,DOWN_ARROW,RIGHT_ARROW }
};


inline void DelayUs(uint32_t _us)
{
    for (int i = 0; i < _us; i++)
        for (int j = 0; j < 8; j++)  // ToDo: tune this for different chips
         {}
				 //_nop_();
}


uint8_t* HWKeyboard_ScanKeyStates()
{
    memset(spiBuffer, 0xFF, IO_NUMBER / 8 + 1);
//    PL_GPIO_Port->BSRR = PL_Pin; // Latch

//    spiHandle->pRxBuffPtr = (uint8_t*) spiBuffer;
//    spiHandle->RxXferCount = IO_NUMBER / 8 + 1;
//    __HAL_SPI_ENABLE(spiHandle);
//    while (spiHandle->RxXferCount > 0U)
//    {
//        if (__HAL_SPI_GET_FLAG(spiHandle, SPI_FLAG_RXNE))
//        {
//            /* read the received data */
//            (*(uint8_t*) spiHandle->pRxBuffPtr) = *(__IO uint8_t*) &spiHandle->Instance->DR;
//            spiHandle->pRxBuffPtr += sizeof(uint8_t);
//            spiHandle->RxXferCount--;
//        }
//    }
//    __HAL_SPI_DISABLE(spiHandle);

//    PL_GPIO_Port->BRR = PL_Pin; // Sample
    return scanBuffer;
}


void HWKeyboard_ApplyDebounceFilter(uint32_t _filterTimeUs)
{
    memcpy(debounceBuffer, spiBuffer, IO_NUMBER / 8 + 1);

    DelayUs(_filterTimeUs);
    HWKeyboard_ScanKeyStates();

    uint8_t mask;
    for (int i = 0; i < IO_NUMBER / 8 + 1; i++)
    {
        mask = debounceBuffer[i] ^ spiBuffer[i];
        spiBuffer[i] |= mask;
    }
}


uint8_t* HWKeyboard_Remap(uint8_t _layer)
{
    int16_t index, bitIndex;

    memset(remapBuffer, 0, IO_NUMBER / 8);
    for (int16_t i = 0; i < IO_NUMBER / 8; i++)
    {
        for (int16_t j = 0; j < 8; j++)
        {
            index = (int16_t) (keyMap[0][i * 8 + j] / 8);
            bitIndex = (int16_t) (keyMap[0][i * 8 + j] % 8);
            if (scanBuffer[index] & (0x80 >> bitIndex))
                remapBuffer[i] |= 0x80 >> j;
        }
        remapBuffer[i] = ~remapBuffer[i];
    }

    memset(hidBuffer, 0, KEY_REPORT_SIZE);

    int i = 0, j = 0;
    while (8 * i + j < IO_NUMBER - 6)
    {
        for (j = 0; j < 8; j++)
        {
            index = (int16_t) (keyMap[_layer][i * 8 + j] / 8 + 1); // +1 for modifier
            bitIndex = (int16_t) (keyMap[_layer][i * 8 + j] % 8);
            if (bitIndex < 0)
            {
                index -= 1;
                bitIndex += 8;
            } else if (index > 100)
                continue;

            if (remapBuffer[i] & (0x80 >> j))
                hidBuffer[index + 1] |= 1 << (bitIndex); // +1 for Report-ID
        }
        i++;
        j = 0;
    }

    return hidBuffer;
}


bool HWKeyboard_FnPressed()
{
    return remapBuffer[9] & 0x02;
}


//void HWKeyboard_SetRgbBufferByID(uint8_t _keyId, HWKeyboard_Color_t _color, float _brightness)
//{
//    // To ensure there's no sequence zero bits, otherwise will case ws2812b protocol error.
//    if (_color.b < 1)_color.b = 1;

//    for (int i = 0; i < 8; i++)
//    {
//        rgbBuffer[_keyId][0][i] =
//            ((uint8_t) ((float) _color.g * _brightness) >> brightnessPreDiv) & (0x80 >> i) ? WS_HIGH : WS_LOW;
//        rgbBuffer[_keyId][1][i] =
//            ((uint8_t) ((float) _color.r * _brightness) >> brightnessPreDiv) & (0x80 >> i) ? WS_HIGH : WS_LOW;
//        rgbBuffer[_keyId][2][i] =
//            ((uint8_t) ((float) _color.b * _brightness) >> brightnessPreDiv) & (0x80 >> i) ? WS_HIGH : WS_LOW;
//    }
//}


//void HWKeyboard_SyncLights()
//{
//    while (isRgbTxBusy);
//    isRgbTxBusy = true;
//    HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*) rgbBuffer, LED_NUMBER * 3 * 8);
//    while (isRgbTxBusy);
//    isRgbTxBusy = true;
//    HAL_SPI_Transmit_DMA(&hspi2, wsCommit, 64);
//}


uint8_t* HWKeyboard_GetHidReportBuffer(uint8_t _reportId)
{
    switch (_reportId)
    {
        case 1:
            hidBuffer[0] = 1;
            return hidBuffer;
        case 2:
            hidBuffer[KEY_REPORT_SIZE] = 2;
            return hidBuffer + KEY_REPORT_SIZE;
        default:
            return hidBuffer;
    }
}


bool HWKeyboard_KeyPressed(KeyCode_t _key)
{
    int index, bitIndex;

    if (_key < RESERVED)
    {
        index = _key / 8;
        bitIndex = (_key + 8) % 8;
    } else
    {
        index = _key / 8 + 1;
        bitIndex = _key % 8;
    }

    return hidBuffer[index + 1] & (1 << bitIndex);
}


void HWKeyboard_Press(KeyCode_t _key)
{
    int index, bitIndex;

    if (_key < RESERVED)
    {
        index = _key / 8;
        bitIndex = (_key + 8) % 8;
    } else
    {
        index = _key / 8 + 1;
        bitIndex = _key % 8;
    }

    hidBuffer[index + 1] |= (1 << bitIndex);
}


void HWKeyboard_Release(KeyCode_t _key)
{
    int index, bitIndex;

    if (_key < RESERVED)
    {
        index = _key / 8;
        bitIndex = (_key + 8) % 8;
    } else
    {
        index = _key / 8 + 1;
        bitIndex = _key % 8;
    }

    hidBuffer[index + 1] &= ~(1 << bitIndex);
}