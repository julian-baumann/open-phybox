#include <Arduino.h>
#include "Main.hpp"
#include "Config.hpp"

uint32_t lastMeasurementTime = 0;
uint32_t lastBleUpdateTime = 0;

void setup()
{
    Serial.begin(115200);

    m_displayService = new DisplayService();
    m_measurementService = new MeasurementService();
    m_bleService = new BluetoothLowEnergyStack();

    m_bleMeasurementService = new BleMeasurementService(*m_bleService);
    m_bleService->AddService(*m_bleMeasurementService);

    m_bleService->StartAdvertising();

    pinMode(MEASUREMENT_PIN, INPUT);

    Serial.println("ESP is ready.");

    m_displayService->ShowSplashScreen();
}

void loop()
{
    uint32_t currentMilliseconds = millis();

    if (currentMilliseconds - lastMeasurementTime >= MEASURE_INTERVAL_MS)
    {
        lastMeasurementTime = currentMilliseconds;
        MeasurementService::CheckActiveMeasurement();
    }

    if (MeasurementService::HasNewCompletedMeasurement())
    {
        Serial.println("New Measurement");
        auto currentPartialMeasurement = MeasurementService::GetCurrentPartialMeasurement();
        m_bleMeasurementService->UpdateValue(currentPartialMeasurement);
    }

//    if (currentMilliseconds - lastBleUpdateTime >= UPDATE_BLE_INTERVAL_MS)
//    {
//        lastBleUpdateTime = currentMilliseconds;
//        auto currentPartialMeasurement = MeasurementService::GetCurrentPartialMeasurement();
//        m_bleMeasurementService->UpdateValue(currentPartialMeasurement);
//    }
}
