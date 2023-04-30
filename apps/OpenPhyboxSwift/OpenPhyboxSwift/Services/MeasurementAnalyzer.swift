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
    
    public func triggeredMeasurement(measurement: RawMeasurement) {
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
    
    public func continousMeasurement(measurement: RawMeasurement) {
        let lastMeasurement = currentMeasurement.last
        
        
        currentMeasurement.append(MeasurementData(
            voltageMeasurement: measurement.voltageMeasurement,
            time: lastMeasurement != nil ? lastMeasurement!.time + UInt(measurement.timeDifference) : UInt(measurement.timeDifference)
        ))
        
        if currentMeasurement.count > 2000 {
            currentMeasurement.remove(at: 0)
        }
    }
    
    public func add(measurement: RawMeasurement) {
        if continousMeasurement {
            continousMeasurement(measurement: measurement)
        } else {
            triggeredMeasurement(measurement: measurement)
        }
    }
}
