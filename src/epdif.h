
#ifndef EPDIF_H
#define EPDIF_H

#include <Arduino.h>

// Определим пины подключения дисплея
#define RST_PIN D1
#define DC_PIN D0
#define CS_PIN D8
#define BUSY_PIN D2

class EpdIf
{
public:
    EpdIf(void);
    ~EpdIf(void);

    static int IfInit(void);
    static void DigitalWrite(int pin, int value);
    static int DigitalRead(int pin);
    static void DelayMs(unsigned int delaytime);
    static void SpiTransfer(unsigned char data);
};

#endif
