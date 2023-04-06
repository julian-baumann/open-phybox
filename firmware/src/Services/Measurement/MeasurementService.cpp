#include "MeasurementService.hpp"
#include "../../Config.hpp"

// static Adafruit_ADS1115 ads;

std::map<uint32_t, uint32_t> CurrentPartialMeasurement;

MeasurementService::MeasurementService()
{
    // ads.begin();
}

uint32_t MeasurementService::GetVoltageMeasurement(uint8_t GpioPin)
{
    // uint16_t sensorValue = ads.readADC_Differential_2_3();
    // float multiplier = 0.1875F;
    // Serial.print("Differential: ");
    // Serial.print(sensorValue);
    // Serial.print("(");
    // Serial.print(sensorValue * multiplier);
    // Serial.println("mV)");
    // double currentValue = (sensorValue * 15.0) / (65536.0);

    analogReadResolution(16);
    uint32_t sensorValue = analogRead(GpioPin);

    return sensorValue;
}

std::map<uint32_t, double> MeasurementService::GetCurrentPartialMeasurement()
{
    std::map<uint32_t, double> result{};

    for (auto valuePair: CurrentPartialMeasurement)
    {
        uint32_t time = valuePair.first;
        double measurement = ((valuePair.second * 15.0) / 65536.0);

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
