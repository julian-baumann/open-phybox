#pragma once
#include <Arduino.h>

class MeasurementService
{
public:
    double GetVoltageMeasurement(uint8_t GpioPin);
};
