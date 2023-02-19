#include "BleMeasurementService.hpp"

BleMeasurementService::BleMeasurementService(BluetoothLowEnergyStack& bleStack)
        : m_service(*bleStack.NimBleServer->createService("B0F151EE-E5D8-45C5-A908-E713DACB728C")),
          m_measurement_characteristic(*m_service.createCharacteristic(
                  "0D279FAD-980C-4C50-8FFF-3F3C266251D0",
                  NIMBLE_PROPERTY::READ |
                  NIMBLE_PROPERTY::NOTIFY
          ))
{
    m_measurement_characteristic.setValue("0.0");
    m_measurement_characteristic.setCallbacks(this);

    m_service.start();
}

NimBLEUUID BleMeasurementService::GetUUID()
{
    return m_service.getUUID();
}

void BleMeasurementService::UpdateValue(unsigned long currentMilliseconds, double value)
{
    m_measurement_characteristic.setValue(value);

    if (m_measurement_characteristic.getSubscribedCount() > 0)
    {
        m_measurement_characteristic.notify();
    }
}

void BleMeasurementService::onRead(NimBLECharacteristic* characteristic)
{
    Serial.print(characteristic->getUUID().toString().c_str());
    Serial.print(": onRead(), value: ");
    Serial.println(characteristic->getValue().c_str());
}

void BleMeasurementService::onWrite(NimBLECharacteristic* characteristic)
{
    Serial.print(characteristic->getUUID().toString().c_str());
    Serial.print(": onWrite(), value: ");
    Serial.println(characteristic->getValue().c_str());
}

void BleMeasurementService::onNotify(NimBLECharacteristic* characteristic)
{
    Serial.println("Sending notification to clients");
}

void BleMeasurementService::onStatus(NimBLECharacteristic* characteristic, NimBLECharacteristicCallbacks::Status status, int code)
{
    String str = "Notification/Indication status code: ";
    str += status;
    str += ", return code: ";
    str += code;
    str += ", ";
    str += NimBLEUtils::returnCodeToString(code);

    Serial.println(str);
}

void BleMeasurementService::onSubscribe(NimBLECharacteristic* characteristic, ble_gap_conn_desc* desc, uint16_t subValue)
{
    String str = "Client ID: ";
    str += desc->conn_handle;
    str += " Address: ";
    str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
    if (subValue == 0)
    {
        str += " Unsubscribed to ";
    }
    else if (subValue == 1)
    {
        str += " Subscribed to notfications for ";
    }
    else if (subValue == 2)
    {
        str += " Subscribed to indications for ";
    }
    else if (subValue == 3)
    {
        str += " Subscribed to notifications and indications for ";
    }
    str += std::string(characteristic->getUUID()).c_str();

    Serial.println(str);
}
