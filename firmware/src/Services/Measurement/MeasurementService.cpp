#include "MeasurementService.hpp"
#include <Adafruit_ADS1X15.h>
#include "../../Config.hpp"

static Adafruit_ADS1115 ads;

std::map<uint32_t, int32_t> CurrentPartialMeasurement;

size_t CurrentIndex = 0;
uint32_t Measurements[120];
uint32_t CorrespondingTimeSnapshot[120];

int interruptCount = 0;
hw_timer_t* timer;

MeasurementService::MeasurementService()
{
    ads.setGain(GAIN_ONE);
    ads.begin();

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 5000, true);
    timerAlarmEnable(timer);
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
    CurrentIndex = 0;

    return result;
}

void MeasurementService::MeasureCurrentVoltage()
{
    auto measurement = GetVoltageMeasurement(MEASUREMENT_PIN);
    CurrentPartialMeasurement.insert({ millis(), measurement });
}

void MeasurementService::onTimer()
{
    auto measurement = GetVoltageMeasurement(MEASUREMENT_PIN);
    Measurements[CurrentIndex] = measurement;
    CorrespondingTimeSnapshot[CurrentIndex] = millis();

    CurrentIndex++;
}
