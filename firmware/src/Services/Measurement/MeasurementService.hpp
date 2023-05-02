#pragma once

#include <Arduino.h>
#include <map>

class MeasurementService
{
public:
    MeasurementService();

    static double GetVoltageMeasurement();
    static void CheckActiveMeasurement();
    static std::map<uint32_t, double> GetCurrentPartialMeasurement();
    static void MeasureCurrentVoltage();
    static bool HasNewCompletedMeasurement();
};
