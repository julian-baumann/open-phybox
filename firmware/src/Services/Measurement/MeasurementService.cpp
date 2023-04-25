#include "MeasurementService.hpp"
#include <Adafruit_ADS1X15.h>
#include "../../Config.hpp"

static Adafruit_ADS1115 ads;

std::map<uint32_t, int32_t> CurrentPartialMeasurement;

MeasurementService::MeasurementService()
{
    ads.setGain(GAIN_ONE);
    ads.begin();
}

int16_t MeasurementService::GetVoltageMeasurement(uint8_t GpioPin)
{
    return ads.readADC_SingleEnded(0);
}

std::map<uint32_t, double> MeasurementService::GetCurrentPartialMeasurement()
{
    std::map<uint32_t, double> result{};

    for (auto valuePair: CurrentPartialMeasurement)
    {
        uint32_t time = valuePair.first;
        double measurement = valuePair.second * 0.000125;

        Serial.println("Measurement: " + String(measurement));

        result.insert({ time, measurement });
    }

    CurrentPartialMeasurement.clear();

    return result;
}

void MeasurementService::MeasureCurrentVoltage()
{
    auto measurement = GetVoltageMeasurement(MEASUREMENT_PIN);
    CurrentPartialMeasurement.insert({ millis(), measurement });
}
