#include "MeasurementService.hpp"
#include "../../Config.hpp"
#include <Adafruit_ADS1X15.h>

static Adafruit_ADS1115 ads;

std::map<uint32_t, int32_t> CurrentPartialMeasurement;
double LastMeasurement = 0.0;
uint32_t LastMeasurementTime = 0;
bool MeasurementActive = false;

MeasurementService::MeasurementService()
{
    ads.setGain(GAIN_ONE);
    ads.begin();

    LastMeasurement = GetVoltageMeasurement();
    LastMeasurementTime = millis();
}

double MeasurementService::GetVoltageMeasurement()
{
    return ads.computeVolts(ads.readADC_SingleEnded(MEASUREMENT_PIN));
}

void MeasurementService::CheckActiveMeasurement()
{
    double currentMeasurement = GetVoltageMeasurement();

    if (!MeasurementActive)
    {
        if (currentMeasurement >= MEASUREMENT_THRESHOLD)
        {
            MeasurementActive = true;
            CurrentPartialMeasurement.insert({ LastMeasurementTime, LastMeasurement });
        }
    }

    if (MeasurementActive)
    {
        CurrentPartialMeasurement.insert({ millis(), currentMeasurement });

        if (currentMeasurement < MEASUREMENT_THRESHOLD)
        {
            MeasurementActive = false;
        }
    }

    LastMeasurement = currentMeasurement;
}

bool MeasurementService::HasNewCompletedMeasurement()
{
    return !MeasurementActive && !CurrentPartialMeasurement.empty();
}

std::map<uint32_t, double> MeasurementService::GetCurrentPartialMeasurement()
{
    std::map<uint32_t, double> result{};

    for (auto valuePair: CurrentPartialMeasurement)
    {
        result.insert({ valuePair.first, valuePair.second });
    }

    CurrentPartialMeasurement.clear();

    return result;
}

void MeasurementService::MeasureCurrentVoltage()
{
    auto measurement = GetVoltageMeasurement();
    CurrentPartialMeasurement.insert({ millis(), measurement });
}
