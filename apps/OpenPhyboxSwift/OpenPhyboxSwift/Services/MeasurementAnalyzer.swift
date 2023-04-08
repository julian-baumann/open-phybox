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

    private var activeMeasuring = false
    private var lastVoltageMeasurement = 0.0
    
    public func clear() {
        currentMeasurement = []
    }
    
    public func add(measurement: RawMeasurement) {
        if !activeMeasuring {
            if measurement.voltageMeasurement > 0.0 {
                activeMeasuring = true
                
                currentMeasurement = [MeasurementData(
                    voltageMeasurement: 0.0,
                    time: 0
                )]
                
                print(measurement.timeDifference)
                
                currentMeasurement.append(MeasurementData(
                    voltageMeasurement: measurement.voltageMeasurement,
                    time: UInt(measurement.timeDifference)
                ))
            } else {
                lastVoltageMeasurement = measurement.voltageMeasurement
            }
        } else if measurement.voltageMeasurement > 0.0 {
            guard let lastMeasurement = currentMeasurement.last else {
                return;
            }
            
            currentMeasurement.append(MeasurementData(
                voltageMeasurement: measurement.voltageMeasurement,
                time: lastMeasurement.time + UInt(measurement.timeDifference)
            ))
        } else if measurement.voltageMeasurement <= 0.0 {
            guard let lastMeasurement = currentMeasurement.last else {
                return;
            }
            
            currentMeasurement.append(MeasurementData(
                voltageMeasurement: measurement.voltageMeasurement,
                time: lastMeasurement.time + UInt(measurement.timeDifference)
            ))

            activeMeasuring = false
        }
    }
}
