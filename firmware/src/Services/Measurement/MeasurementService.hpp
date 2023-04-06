#pragma once

#include <Arduino.h>
#include <map>

class MeasurementService
{
public:
    MeasurementService();

    static uint32_t GetVoltageMeasurement(uint8_t GpioPin);
    static std::map<uint32_t, double> GetCurrentPartialMeasurement();
    void MeasureCurrentVoltage();

private:
    // hw_timer_t &m_measurementTimer;

    static void onTimer();
};
