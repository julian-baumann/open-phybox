#include "MeasurementService.hpp"

double MeasurementService::GetVoltageMeasurement(uint8_t GpioPin)
{
    analogReadResolution(16);
    uint32_t sensorValue = analogRead(GpioPin);
    return (sensorValue * 15.0) / (65536.0);
}
