#include <Arduino.h>
#include "Main.hpp"

hw_timer_t *measurementTimer;

void onTimer()
{
    // auto currentPartialMeasurement = m_measurementService->GetCurrentPartialMeasurement();
    // m_bleMeasurementService->UpdateValue(currentPartialMeasurement);
    //    m_displayService->UpdateVoltageMeasurement(voltage);
}

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

    // measurementTimer = timerBegin(0, 80, true);
    // timerAttachInterrupt(measurementTimer, &onTimer, true);
    // timerAlarmWrite(measurementTimer, 1000, true);
    // timerAlarmEnable(measurementTimer);

    m_displayService->ShowSplashScreen();
}

void loop()
{
    unsigned long currentMilliseconds = millis();

    if ((currentMilliseconds % 10) == 0)
    {
        m_measurementService->MeasureCurrentVoltage();
        //    m_bleMeasurementService->UpdateValue(voltage);
        // m_displayService->UpdateVoltageMeasurement(voltage);
    }

    if ((currentMilliseconds % 1000) == 0)
    {
        auto currentPartialMeasurement = m_measurementService->GetCurrentPartialMeasurement();
        m_bleMeasurementService->UpdateValue(currentPartialMeasurement);
        //    m_bleMeasurementService->UpdateValue(voltage);
        // m_displayService->UpdateVoltageMeasurement(voltage);
    }
}
