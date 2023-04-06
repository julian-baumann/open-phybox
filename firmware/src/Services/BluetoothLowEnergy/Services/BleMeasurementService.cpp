#include "BleMeasurementService.hpp"

#pragma pack (1)
struct MeasurementMessage
{
    uint8_t time;
    double measurement;
};

BleMeasurementService::BleMeasurementService(BluetoothLowEnergyStack& bleStack)
        : m_service(*bleStack.NimBleServer->createService("B0F151EE-E5D8-45C5-A908-E713DACB728C")),
          m_measurement_characteristic(*m_service.createCharacteristic(
                  "0D279FAD-980C-4C50-8FFF-3F3C266251D0",
                  NIMBLE_PROPERTY::READ |
                  NIMBLE_PROPERTY::NOTIFY))
{
    m_measurement_characteristic.setValue(MeasurementMessage{
            .time = 0,
            .measurement = 0.0
    });

    m_measurement_characteristic.setCallbacks(this);

    m_service.start();
}

NimBLEUUID BleMeasurementService::GetUUID()
{
    return m_service.getUUID();
}

void BleMeasurementService::UpdateValue(std::map<uint32_t, double>& currentPartialMeasurement)
{
    const uint8_t bufferSize = 42;

    auto measurementsTaken = currentPartialMeasurement.size();
    Serial.println("\nMeasurements taken: " + String(measurementsTaken));

    uint16_t iterationsNeeded = std::ceil((float) measurementsTaken / bufferSize);

    Serial.println("Need " + String(iterationsNeeded) + " iterations to send all measurements");

    for (uint16_t iteration = 0; iteration < iterationsNeeded; iteration++)
    {
        MeasurementMessage data[bufferSize];

        int newStartIndex = bufferSize * iteration;
        auto currentChunkedMeasurementIterator = std::next(currentPartialMeasurement.begin(), newStartIndex);
        std::map<uint32_t, double> currentChunkedMeasurements(currentChunkedMeasurementIterator, currentPartialMeasurement.end());

        int index = 0;

        for (auto valuePair : currentChunkedMeasurements)
        {
            if (index >= bufferSize)
            {
                Serial.println("index exeeded bufferSize!");

                break;
            }

            uint8_t timeSpanSinceLastSample = valuePair.first - m_lastTimeStamp;
            m_lastTimeStamp = valuePair.first;

            Serial.println("Timespan: " + String(timeSpanSinceLastSample));

            data[index] = {
                    .time = timeSpanSinceLastSample,
                    .measurement = valuePair.second
            };

            index++;
        }

        // Serial.println("Array length: " + String(sizeof(data) / sizeof(data[0])));
        // Serial.println("Array size: " + String(sizeof(data)));

        m_measurement_characteristic.setValue(data);

        if (m_measurement_characteristic.getSubscribedCount() > 0)
        {
            m_measurement_characteristic.notify();
        }
    }
}

void BleMeasurementService::onStatus(NimBLECharacteristic* characteristic, NimBLECharacteristicCallbacks::Status status, int code)
{
    Serial.println(
            "Notification/Indication status code: " + String(status) + ", returned code: " + String(code) + ", " + NimBLEUtils::returnCodeToString(code));
}
