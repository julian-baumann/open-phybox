#include "DisplayService.hpp"

static TFT_eSPI TftBackend = TFT_eSPI();

DisplayService::DisplayService()
{
    TftBackend.init();
    TftBackend.setRotation(3);
    Width = TftBackend.width();
    Height = TftBackend.height();
    TftBackend.fillScreen(TFT_BLACK);
    TftBackend.setTextColor(TFT_WHITE, TFT_BLACK);
    TftBackend.setCursor(0, 0);
    TftBackend.setTextDatum(MC_DATUM);
    TftBackend.setTextSize(2);

    // Turn backlight on
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
}

void DisplayService::ShowSplashScreen()
{
}

void DisplayService::UpdateVoltageMeasurement(double measurement)
{
    SetCursor(10, 10);
    Print("Voltage: " + String(measurement));
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

void DisplayService::Clear()
{
    TftBackend.fillScreen(TFT_BLACK);
}
