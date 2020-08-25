#include <stdlib.h>
#include "epd4in2b.h"

Epd::~Epd() {
};

Epd::Epd() {
    reset_pin = RST_PIN;
    dc_pin = DC_PIN;
    cs_pin = CS_PIN;
    busy_pin = BUSY_PIN;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

int Epd::Init(void) {
    /* Вызов интерфейса из epdif */
    if (IfInit() != 0) {
        return -1;
    }
    /* Инициализация EPD, начало работы с дисплеем */
    Reset();
    SendCommand(BOOSTER_SOFT_START);
    SendData(0x17);
    SendData(0x17);
    SendData(0x17);     //07 0f 17 1f 27 2F 37 2f
    SendCommand(POWER_ON);
    WaitUntilIdle();
    SendCommand(PANEL_SETTING);
    SendData(0x0F);     // LUT from OTP
    /* Закончили инициализацию EPD */
    return 0;
}

/**
 *  @brief: Базовая функция отправки комаманд дисплею
 */
void Epd::SendCommand(unsigned char command) {
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: Базовая функция отправки данных дисплею
 */
void Epd::SendData(unsigned char data) {
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Ждем пока BUSY_PIN не станет ВЫСОКИМ (+3,3В)
 */
void Epd::WaitUntilIdle(void) {
    // 0: занято, 1: ожидание
    while(DigitalRead(busy_pin) == 0) {
        DelayMs(100);
    }      
}

/**
 *  @brief: Сброс модуля.
 *  Часто используется для пробуждения дисплея из глубокого сна,
 *  смотрим Epd::Sleep();
 */
void Epd::Reset(void) {
    DigitalWrite(reset_pin, LOW);
    DelayMs(200);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);   
}

/**
 *  @brief: Передача порции данных в SRAM память дисплея
 */
void Epd::SetPartialWindow(const unsigned char* buffer_black, const unsigned char* buffer_red, int x, int y, int w, int l) {
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    // x должно быть кратно 8, последние 3 бита всегда будут игнорироваться
    SendData(x & 0xf8);
    SendData(((x & 0xf8) + w  - 1) >> 8);
    SendData(((x & 0xf8) + w  - 1) | 0x07);
    SendData(y >> 8);        
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);        
    SendData((y + l - 1) & 0xff);
    SendData(0x01);         // Gates scan both inside and outside of the partial window. (default) 
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_1);
    if (buffer_black != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(buffer_black[i]);  
        }  
    }
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    if (buffer_red != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(buffer_red[i]);  
        }  
    }
    DelayMs(2);
    SendCommand(PARTIAL_OUT);  
}

/**
 *  @brief: Передаем порцию данных в буфер черного цвета SRAM памяти дисплея
 */
void Epd::SetPartialWindowBlack(const unsigned char* buffer_black, int x, int y, int w, int l) {
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    // x должно быть кратно 8, последние 3 бита всегда будут игнорироваться
    SendData(x & 0xf8);
    SendData(((x & 0xf8) + w  - 1) >> 8);
    SendData(((x & 0xf8) + w  - 1) | 0x07);
    SendData(y >> 8);        
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);        
    SendData((y + l - 1) & 0xff);
    // Gates scan both inside and outside of the partial window. (default) 
    SendData(0x01);
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_1);
    if (buffer_black != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(buffer_black[i]);  
        }  
    }
    DelayMs(2);
    SendCommand(PARTIAL_OUT);  
}

/**
 *  @brief: Передаем порцию данных в буфер красного цвета SRAM памяти дисплея
 */
void Epd::SetPartialWindowRed(const unsigned char* buffer_red, int x, int y, int w, int l) {
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    // x должно быть кратно 8, последние 3 бита всегда будут игнорироваться
    SendData(x & 0xf8);
    SendData(((x & 0xf8) + w  - 1) >> 8);
    SendData(((x & 0xf8) + w  - 1) | 0x07);
    SendData(y >> 8);        
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);        
    SendData((y + l - 1) & 0xff);
    // Gates scan both inside and outside of the partial window. (default) 
    SendData(0x01);
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    if (buffer_red != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(buffer_red[i]);  
        }  
    }
    DelayMs(2);
    SendCommand(PARTIAL_OUT);  
}

/**
 * @brief: Обновляем и отображаем фрейм
 */
void Epd::DisplayFrame(const unsigned char* frame_black, const unsigned char* frame_red) {
    if (frame_black != NULL) {
        SendCommand(DATA_START_TRANSMISSION_1);
        DelayMs(2);
        for (int i = 0; i < this->width / 8 * this->height; i++) {
            SendData(pgm_read_byte(&frame_black[i]));
        }
        DelayMs(2);
    }
    if (frame_red != NULL) {
        SendCommand(DATA_START_TRANSMISSION_2);
        DelayMs(2);
        for (int i = 0; i < this->width / 8 * this->height; i++) {
            SendData(pgm_read_byte(&frame_red[i]));
        }
        DelayMs(2);
    }
    SendCommand(DISPLAY_REFRESH);
    WaitUntilIdle();
}

/**
 * @brief: Рисуем готовую картинку черным цветом (из массива)
 */
void Epd::DisplayBlackImage(const unsigned char* frame_black) {
    if (frame_black != NULL) {
        SendCommand(DATA_START_TRANSMISSION_1);
        DelayMs(2);
        for (int i = 0; i < this->width / 8 * this->height; i++) {
            SendData(pgm_read_byte(&frame_black[i]));
        }
        DelayMs(2);
    }
    SendCommand(DISPLAY_REFRESH);
    WaitUntilIdle();
}


/**
 * @brief: Рисуем готовую картинку черным цветом (из массива)
 */
void Epd::DisplayRedImage(const unsigned char* frame_red) {
    if (frame_red != NULL) {
        SendCommand(DATA_START_TRANSMISSION_2);
        DelayMs(2);
        for (int i = 0; i < this->width / 8 * this->height; i++) {
            SendData(pgm_read_byte(&frame_red[i]));
        }
        DelayMs(2);
    }
    SendCommand(DISPLAY_REFRESH);
    WaitUntilIdle();
}

/**
 * @brief: Очищаем буфер от данных в SRAM дисплея, но это не обновляет картинку на дисплее.
 * Для обновления нужно вызвать DisplayFrame()
 */
void Epd::ClearFrame(void) {
    SendCommand(DATA_START_TRANSMISSION_1);           
    DelayMs(2);
    for(int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);  
    }  
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);           
    DelayMs(2);
    for(int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);  
    }  
    DelayMs(2);
}

/**
 * @brief: Отображение данных в SRAM дисплея на экране.
 */
void Epd::DisplayFrame(void) {
    SendCommand(DISPLAY_REFRESH); 
    DelayMs(100);
    WaitUntilIdle();
}

/**
 * @brief: После вызова, МК дисплея перейдет в режим глубокого сна для экономии энергии.
 *  Режим глубокого сна вернется в режим ожидания после аппаратного сброса.
 *  Единственный параметр - это код проверки, команда будет выполена, если код проверки = 0xA5
 *  Можно использовать Epd::Reset() для пробуждения и Epd::Init() для инициализации.
 */
void Epd::Sleep() {
    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0xF7);     // border floating
    SendCommand(POWER_OFF);
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);
    SendData(0xA5);     // check code
}
