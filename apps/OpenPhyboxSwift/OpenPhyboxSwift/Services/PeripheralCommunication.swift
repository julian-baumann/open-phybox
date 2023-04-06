//
//  PeripheralCommunicationService.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 19.02.23.
//

import Foundation
import CoreBluetooth

class PeripheralCommunication: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    public static let shared = PeripheralCommunication()

    private let serviceUUID = CBUUID(string: "B0F151EE-E5D8-45C5-A908-E713DACB728C")
    private let measurementCharacteristicUUID = CBUUID(string: "0D279FAD-980C-4C50-8FFF-3F3C266251D0")
    private var centralManager: CBCentralManager!
    private var peripheralESP: CBPeripheral?
    private var count = 0
    
    @Published public var isPoweredOn = false
    @Published public var connected = false
    @Published public var connecting = false
    @Published public var currentMeasurement = MeasurementData()
    @Published public var connectedPeripheral: CBPeripheral?
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil, options: [CBCentralManagerOptionShowPowerAlertKey: true])
    }

    func startAdvertising() {
        centralManager.scanForPeripherals(withServices: [serviceUUID], options: nil)
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if connectedPeripheral == nil {
            connectedPeripheral = peripheral
            connecting = true
            centralManager.connect(peripheral, options: nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        centralManager.stopScan()
        connected = true
        connecting = false
        connectedPeripheral?.delegate = self
        connectedPeripheral?.discoverServices([serviceUUID])
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if (error != nil) {
            print("Error discovering services: \(error!.localizedDescription)")
            return
        }
        
        guard let services = peripheral.services else {
            return
        }
        
        for service in services {
            peripheral.discoverCharacteristics([measurementCharacteristicUUID], for: service)
        }
        
        print("Discovered Services: \(services)")
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        if (error != nil) {
            print("Error discovering services: \(error!.localizedDescription)")
            return
        }
        
        guard let characteristics = service.characteristics else {
            return
        }
        
        for characteristic in characteristics {
            if characteristic.uuid == measurementCharacteristicUUID {
                peripheral.setNotifyValue(true, for: characteristic)
                peripheral.readValue(for: characteristic)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        guard characteristic == characteristic,
              let rawValue = characteristic.value else {
            return
        }
        
        let rawByteArray = [UInt8](rawValue)
        let chunked = rawByteArray.chunked(into: 12)
        
        for chunk in chunked {
            let time = UInt32(Array(chunk[...3]))
            let measurement = Double(Array(chunk[4...]))

            guard
                let measurement = measurement,
                let time = time else {
                continue;
            }
            
            currentMeasurement.data.append(.init(voltageMeasurement: measurement, time: time))
            currentMeasurement.latestReading = measurement
        }

        currentMeasurement = currentMeasurement
        count += 1
    }
    
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        connectedPeripheral = nil
        print(error ?? "Failed to connect to device")
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        connected = false
        connectedPeripheral = nil
        startAdvertising()
    }
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            isPoweredOn = true
            startAdvertising()
        }
        else {
            isPoweredOn = false
        }
    }
}
