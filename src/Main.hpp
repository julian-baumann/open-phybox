#pragma once

#include "Services/Display/DisplayService.hpp"
#include "Services/Measurement/MeasurementService.hpp"
#include "Services/BluetoothLowEnergy/BluetoothLowEnergyStack.hpp"
#include "Services/BluetoothLowEnergy/Services/BleMeasurementService.hpp"


DisplayService* m_displayService;
MeasurementService* m_measurementService;
BluetoothLowEnergyStack* m_bleService;
BleMeasurementService* m_bleMeasurementService;
