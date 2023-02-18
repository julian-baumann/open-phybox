#include <Arduino.h>
#include "Main.hpp"

void setup() {
    Serial.begin(115200);
    _displayService = new DisplayService();
    Serial.println("Hello, World");
    Serial.println("Height: " + String(_displayService->Height) + ", Width: " + String(_displayService->Width));

    uint8_t x = _displayService->Width / 2 - 20;
    uint8_t y = _displayService->Height / 2 - 10;
    Serial.println("x: " + String(x) + ", y: " + String(y));

    _displayService->SetCursor(x, y);
    _displayService->Print("Hello, Universe!");
}

void loop() {
}