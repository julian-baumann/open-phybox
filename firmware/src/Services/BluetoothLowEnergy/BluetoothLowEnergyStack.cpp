#include "BluetoothLowEnergyStack.hpp"
#include "NimBLEDevice.h"

BluetoothLowEnergyStack::BluetoothLowEnergyStack()
{
    NimBLEDevice::init("Open Phybox");

    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEDevice::setSecurityAuth(BLE_HS_IO_NO_INPUT_OUTPUT);

    NimBleServer = NimBLEDevice::createServer();
    NimBleServer->setCallbacks(this);

    m_advertising = NimBLEDevice::getAdvertising();
}

void BluetoothLowEnergyStack::AddService(BleService& service)
{
    m_advertising->addServiceUUID(service.GetUUID());
}

void BluetoothLowEnergyStack::StartAdvertising()
{
    m_advertising->setScanResponse(true);
    m_advertising->start();
}

size_t BluetoothLowEnergyStack::GetConnectedCount() const
{
    return NimBleServer->getConnectedCount();
}

void BluetoothLowEnergyStack::onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc)
{
    Serial.print("Client address: ");
    Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
    Serial.println("Multi-connect support: start advertising");
    NimBLEDevice::startAdvertising();

    pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
}

void BluetoothLowEnergyStack::onDisconnect(NimBLEServer* pServer)
{
    Serial.println("Client disconnected - start advertising");
    NimBLEDevice::startAdvertising();
}

void BluetoothLowEnergyStack::onAuthenticationComplete(ble_gap_conn_desc* desc)
{
    if (!desc->sec_state.encrypted)
    {
        NimBLEDevice::getServer()->disconnect(desc->conn_handle);
        Serial.println("Encrypt connection failed - disconnecting client");
        return;
    }

    Serial.println("Encryption successful!");
}
