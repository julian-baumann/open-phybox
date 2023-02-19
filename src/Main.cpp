#include <Arduino.h>
#include "Main.hpp"

void setup()
{
    Serial.begin(115200);
    m_displayService = new DisplayService();
    m_measurementService = new MeasurementService();
    m_bleService = new BluetoothLowEnergyStack();

    m_bleMeasurementService = new BleMeasurementService(*m_bleService);
    m_bleService->AddService(*m_bleMeasurementService);

    m_bleService->StartAdvertising();

    pinMode(36, INPUT);
    Serial.println("ESP is ready.");
}

void loop()
{
    double voltage = m_measurementService->GetVoltageMeasurement(36);
    m_bleMeasurementService->UpdateValue(voltage);
    m_displayService->UpdateVoltageMeasurement(voltage);

    delay(100);
}
