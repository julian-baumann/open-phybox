#pragma once

#include "NimBLECharacteristic.h"
#include "NimBLEUUID.h"

class BleService
{
public:
    virtual NimBLEUUID GetUUID();
};
