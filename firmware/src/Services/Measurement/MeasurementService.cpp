#include "MeasurementService.hpp"
#include <Adafruit_ADS1X15.h>

static Adafruit_ADS1115 ads;
static const short MEASUREMENT_PIN = 36;

std::map<uint32_t, uint32_t> CurrentPartialMeasurement;

// uint32_t TimeBuffer[110];
// uint32_t MeasurementsBuffer[110];

// uint8_t CurrentPosition;

MeasurementService::MeasurementService()
{
    //    ads.begin();

    // timerAttachInterrupt(&m_measurementTimer, &MeasurementService::onTimer, true);
    // timerAlarmWrite(&m_measurementTimer, 100, true);
    // timerAlarmEnable(&m_measurementTimer);
}

uint32_t MeasurementService::GetVoltageMeasurement(uint8_t GpioPin)
{
    //    uint16_t sensorValue = ads.readADC_Differential_2_3();
    //    float multiplier = 0.1875F;
    //    Serial.print("Differential: ");
    //    Serial.print(sensorValue);
    //    Serial.print("(");
    //    Serial.print(sensorValue * multiplier);
    //    Serial.println("mV)");

    analogReadResolution(16);
    uint32_t sensorValue = analogRead(GpioPin);
    // double currentValue = (sensorValue * 15.0) / (65536.0);

    return sensorValue;
}

std::map<uint32_t, double> MeasurementService::GetCurrentPartialMeasurement()
{
    std::map<uint32_t, double> result{};

    for (auto valuePair : CurrentPartialMeasurement)
    {
        uint32_t time = valuePair.first;
        double measurement = ((valuePair.second * 15.0) / 65536.0);

        result.insert({time, measurement});
    }

    CurrentPartialMeasurement.clear();

    return result;
}

void MeasurementService::MeasureCurrentVoltage()
{
    auto measurement = GetVoltageMeasurement(MEASUREMENT_PIN);
    CurrentPartialMeasurement.insert({millis(), measurement});
}

// IMPORTANT: Don't do any floating point arithmetics inside an interrupt, since the registers may be broke afterwards
void MeasurementService::onTimer()
{
    // auto measurement = GetVoltageMeasurement(MEASUREMENT_PIN);
    // CurrentPartialMeasurement.insert({millis(), measurement});
}
