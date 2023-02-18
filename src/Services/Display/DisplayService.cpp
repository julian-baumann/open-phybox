#include "DisplayService.hpp"

DisplayService::DisplayService()
{
    TftBackend.init();
    TftBackend.setRotation(1);
    Width = TftBackend.width();
    Height = TftBackend.height();
    TftBackend.fillScreen(TFT_BLACK);
    TftBackend.setTextSize(2);
    TftBackend.setTextColor(TFT_WHITE);
    TftBackend.setCursor(0, 0);
    TftBackend.setTextDatum(MC_DATUM);
    TftBackend.setTextSize(1);

    // Turn backlight on
    if (TFT_BL > 0) {
        pinMode(TFT_BL, OUTPUT);
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
    }
}

void DisplayService::ShowSplashScreen()
{
}

void DisplayService::SetCursor(int16_t x, int16_t y)
{
    TftBackend.setCursor(x, y);
}

void DisplayService::SetTextSize(uint8_t size)
{
    TftBackend.setTextSize(size);
}

void DisplayService::Print(String text)
{
    TftBackend.print(text);
}
