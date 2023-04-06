#pragma once

#include <map>
#include "NimBLECharacteristic.h"
#include "Services/BluetoothLowEnergy/BleCharacteristic.hpp"
#include "Services/BluetoothLowEnergy/BleService.hpp"
#include "Services/BluetoothLowEnergy/BluetoothLowEnergyStack.hpp"

class BleMeasurementService : public BleService, public BleCharacteristic
{
public:
    explicit BleMeasurementService(BluetoothLowEnergyStack& bleStack);

    NimBLEUUID GetUUID() override;

    void UpdateValue(std::map<uint32_t, double>& currentPartialMeasurement);

private:
    NimBLEService& m_service;
    NimBLECharacteristic& m_measurement_characteristic;

    void onStatus(NimBLECharacteristic* characteristic, Status status, int code) override;
};
