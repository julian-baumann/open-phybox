//
//  MeasurementData.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 19.02.23.
//

import Foundation

struct RawMeasurement: Identifiable {
    var id = UUID()
    var voltageMeasurement: Double
    var timeDifference: UInt8
}

struct MeasurementData: Identifiable {
    var id = UUID()
    var voltageMeasurement: Double
    var time: UInt
}
