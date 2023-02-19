#pragma once

#include "NimBLEServer.h"
#include "BleService.hpp"

class BluetoothLowEnergyStack : public NimBLEServerCallbacks
{
public:
    NimBLEServer* NimBleServer;

    BluetoothLowEnergyStack();

    void AddService(BleService& service);
    void StartAdvertising();
    size_t GetConnectedCount() const;

private:
    NimBLEAdvertising* m_advertising;

    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) override;
    void onDisconnect(NimBLEServer* pServer) override;
    void onAuthenticationComplete(ble_gap_conn_desc* desc) override;
};
