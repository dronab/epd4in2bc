#include <SPI.h>
#include "epd4in2b.h"
#include "imagedata.h"
#include "epdpaint.h"

#define COLORED 0
#define UNCOLORED 1

void setup()
{
    Serial.begin(9600);
    Epd epd;
    if (epd.Init() != 0)
    {
        Serial.print("e-Paper init failed");
        return;
    }

    epd.ClearFrame();

    unsigned char image[1500];
    Paint paint(image, 400, 28); //ширина должна быть кратной 8

    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 0, "e-Paper Demo", &Font24, COLORED);
    epd.SetPartialWindowBlack(paint.GetImage(), 100, 40, paint.GetWidth(), paint.GetHeight());

    paint.Clear(COLORED);
    paint.DrawStringAt(100, 2, "Hello world", &Font24, UNCOLORED);
    epd.SetPartialWindowRed(paint.GetImage(), 0, 64, paint.GetWidth(), paint.GetHeight());

    paint.SetWidth(64);
    paint.SetHeight(64);

    paint.Clear(UNCOLORED);
    paint.DrawRectangle(0, 0, 40, 50, COLORED);
    paint.DrawLine(0, 0, 40, 50, COLORED);
    paint.DrawLine(40, 0, 0, 50, COLORED);
    epd.SetPartialWindowBlack(paint.GetImage(), 72, 120, paint.GetWidth(), paint.GetHeight());

    paint.Clear(UNCOLORED);
    paint.DrawCircle(32, 32, 30, COLORED);
    epd.SetPartialWindowBlack(paint.GetImage(), 200, 120, paint.GetWidth(), paint.GetHeight());

    paint.Clear(UNCOLORED);
    paint.DrawFilledRectangle(0, 0, 40, 50, COLORED);
    epd.SetPartialWindowRed(paint.GetImage(), 72, 200, paint.GetWidth(), paint.GetHeight());

    paint.Clear(UNCOLORED);
    paint.DrawFilledCircle(32, 32, 30, COLORED);
    epd.SetPartialWindowRed(paint.GetImage(), 200, 200, paint.GetWidth(), paint.GetHeight());

    epd.DisplayFrame();

    epd.DisplayFrame(IMAGE_BLACK, IMAGE_RED);
    // epd.DisplayFrame(MY_IMAGE, NULL);

    epd.ClearFrame();

    epd.Sleep();
}

void loop()
{
    // put your main code here, to run repeatedly:
}
