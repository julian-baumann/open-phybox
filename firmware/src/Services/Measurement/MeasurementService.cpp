#include "MeasurementService.hpp"
#include "../../Config.hpp"
#include "UrsAds1x15.h"
#include <UrsTwi.h>

// static Adafruit_ADS1115 ads;
UrsTwi twi;
UrsAds1115 ads(twi);

const uint8_t MaxData = 32;
const uint8_t AlertPin = 15;
int16_t CurrentData[MaxData];
uint8_t CurrentIndex = 0;

std::map<uint32_t, int32_t> CurrentPartialMeasurement;

void interruptHandler(int16_t rawValue, UrsAds1x15* ads1x15)
{
    if (CurrentIndex < MaxData)
    {
        CurrentData[CurrentIndex] = rawValue;
        CurrentIndex += 1;
    }
}

MeasurementService::MeasurementService()
{
    // ads.setGain(GAIN_ONE);
    // ads.begin();

    twi.setClock(400000L); // I²C-Taktfrequenz: 400 kHz
    twi.begin();           // I²C-Interface starten

    ads.setSps(Ads1115Sps::Sps16);   // 1600 Messungen pro Sekude
    ads.setGain(Ads1x15Gain::Gain_1); // Höchste Empfindlichkeit
    ads.startContinuousMode(Ads1x15Mux::Single_0, interruptHandler, AlertPin, false);
}


int16_t MeasurementService::GetVoltageMeasurement(uint8_t GpioPin)
{
    // return ads.readADC_SingleEnded(0);
}

std::map<uint32_t, double> MeasurementService::GetCurrentPartialMeasurement()
{
    std::map<uint32_t, double> result{};

    // for (auto valuePair: CurrentPartialMeasurement)
    // {
    //     uint32_t time = valuePair.first;
    //     double measurement = ads.computeVolts(valuePair.second);

    //     result.insert({ time, measurement });
    // }

    // CurrentPartialMeasurement.clear();

    for (uint8_t index = 0; index < CurrentIndex; index++)
    {
        double measurement = CurrentData[index] * 0.000125;

        Serial.println("Measurement: " + String(measurement));

        result.insert({ millis(), measurement });
    }

    CurrentIndex = 0;


    return result;
}

void MeasurementService::MeasureCurrentVoltage()
{
    auto measurement = GetVoltageMeasurement(MEASUREMENT_PIN);
    CurrentPartialMeasurement.insert({ millis(), measurement });
}
