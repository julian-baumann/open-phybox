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
    
    private let measurementAnalyzer = MeasurementAnalyzer.shared
    private let serviceUUID = CBUUID(string: "B0F151EE-E5D8-45C5-A908-E713DACB728C")
    private let measurementCharacteristicUUID = CBUUID(string: "0D279FAD-980C-4C50-8FFF-3F3C266251D0")
    private var centralManager: CBCentralManager!
    private var peripheralESP: CBPeripheral?
    
    @Published public var isPoweredOn = false
    @Published public var connected = false
    @Published public var connecting = false
    @Published public var connectedPeripheral: CBPeripheral?
    @Published public var measurementCompleted: Bool = false
    
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
        measurementAnalyzer.clear()
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
        
        
        if rawByteArray.count == 1 {
            if rawByteArray.first == 0x85 {
                measurementCompleted = true
                
                return
            }
        }
        
        if measurementCompleted {
            measurementAnalyzer.clear()
            measurementCompleted = false
        }
        
        let chunked = rawByteArray.chunked(into: 9)
        
        for chunk in chunked {
            let timeSpan = UInt8(chunk[0])
            let measurement = Double(Array(chunk[1...]))

            guard let measurement = measurement else {
                continue;
            }
            
            let currentMeasurementDataPoint = RawMeasurement(
                voltageMeasurement: measurement,
                timeDifference: timeSpan
            )
            
            measurementAnalyzer.add(measurement: currentMeasurementDataPoint)
        }
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
