#pragma once

#include <string>
#include <TFT_eSPI.h> 

static TFT_eSPI TftBackend = TFT_eSPI();

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

private:
};