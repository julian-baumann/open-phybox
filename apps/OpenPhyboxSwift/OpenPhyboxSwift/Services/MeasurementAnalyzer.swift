//
//  MeasurementAnalyzer.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 07.04.23.
//

import Foundation

class MeasurementAnalyzer: ObservableObject {
    public static let shared = MeasurementAnalyzer()

    @Published var currentMeasurement: [MeasurementData] = []
    @Published var continousMeasurement = false;

    private var activeMeasuring = false
    private var lastVoltageMeasurement = 0.0
    
    private func isOverAverage(_ measurement: RawMeasurement) -> Bool {
        return abs(measurement.voltageMeasurement - lastVoltageMeasurement) > 0.5
    }
    
    public func clear() {
        currentMeasurement = []
    }
    
    public func add(measurement: RawMeasurement) {
        let lastMeasurement = currentMeasurement.last
        
        
        currentMeasurement.append(MeasurementData(
            voltageMeasurement: measurement.voltageMeasurement,
            time: lastMeasurement != nil ? lastMeasurement!.time + UInt(measurement.timeDifference) : UInt(0)
        ))
    }
}
