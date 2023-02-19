#pragma once

#include <string>
#include <TFT_eSPI.h>

class DisplayService
{
public:
    DisplayService();

    int Width;
    int Height;

    void ShowSplashScreen();
    void SetCursor(int16_t x, int16_t y);
    void SetTextSize(uint8_t size);
    void Print(String text);
    void Clear();
    void UpdateVoltageMeasurement(double measurement);

private:
};
