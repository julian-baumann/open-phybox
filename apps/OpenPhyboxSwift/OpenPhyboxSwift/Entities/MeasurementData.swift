//
//  MeasurementData.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 19.02.23.
//

import Foundation

struct MeasurementDatapoint {
    var voltageMeasurement: Double
    var time: Int
}

class MeasurementData: ObservableObject {
    @Published var latestReading: Double = 0.0
    @Published var data: [MeasurementDatapoint] = []
}
