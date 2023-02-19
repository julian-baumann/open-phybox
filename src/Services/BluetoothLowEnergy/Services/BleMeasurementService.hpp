#pragma once

#include "NimBLECharacteristic.h"
#include "Services/BluetoothLowEnergy/BleCharacteristic.hpp"
#include "Services/BluetoothLowEnergy/BleService.hpp"
#include "Services/BluetoothLowEnergy/BluetoothLowEnergyStack.hpp"

class BleMeasurementService : public BleService, public BleCharacteristic
{
public:
    BleMeasurementService(BluetoothLowEnergyStack& bleStack);

    NimBLEUUID GetUUID() override;

    void UpdateValue(double value);

private:
    NimBLEService& m_service;
    NimBLECharacteristic& m_measurement_characteristic;

    void onRead(NimBLECharacteristic* characteristic) override;
    void onWrite(NimBLECharacteristic* characteristic) override;
    void onNotify(NimBLECharacteristic* characteristic) override;
    void onStatus(NimBLECharacteristic* characteristic, Status status, int code) override;
    void onSubscribe(NimBLECharacteristic* characteristic, ble_gap_conn_desc* desc, uint16_t subValue) override;
};
