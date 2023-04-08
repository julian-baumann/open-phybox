//
//  ContentView.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 19.02.23.
//

import SwiftUI
import Charts

struct ContentView: View {
    @StateObject var peripheralCommunication = PeripheralCommunication()
    @StateObject var measurementAnalyzer = MeasurementAnalyzer.shared
    
    @State var showCircleMarks = false
    
    var body: some View {
        NavigationStack {
            if (peripheralCommunication.isPoweredOn) {
                List {
                    Section {
                        Toggle("Show Marks", isOn: $showCircleMarks)
                    }
                    Section {
                        Chart(measurementAnalyzer.currentMeasurement) { dataPoint in
                            if showCircleMarks {
                                Plot {
                                    LineMark(
                                        x: .value("Time", dataPoint.time),
                                        y: .value("Voltage", dataPoint.voltageMeasurement)
                                    )
                                }
                                .symbol(.circle)
                                .interpolationMethod(.catmullRom)
                            } else {
                                Plot {
                                    LineMark(
                                        x: .value("Time", dataPoint.time),
                                        y: .value("Voltage", dataPoint.voltageMeasurement)
                                    )
                                }
                                .interpolationMethod(.catmullRom)
                            }
                        }
                        .chartYScale(domain: -15...15)
                        .padding()
                    }
                    .frame(minHeight: 500)
                }
                .navigationTitle("Open Phybox")
                .sheet(isPresented: !$peripheralCommunication.connected) {
                    SelectDeviceView()
                        .environmentObject(peripheralCommunication)
                        .interactiveDismissDisabled(true)
                }
                .toolbar {
                    ToolbarItem() {
                        if peripheralCommunication.connected {
                            HStack {
                                Image(systemName: "personalhotspot")
                                Text("Connected")
                            }
                            .foregroundColor(.green)
                        } else {
                            HStack {
                                Image(systemName: "personalhotspot")
                                Text("Not Connected")
                            }
                            .foregroundColor(.red)
                        }
                    }
                }
            } else {
                BluetoothDisabledView()
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
