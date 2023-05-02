//
//  ContentView.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 19.02.23.
//

import SwiftUI
import Charts

extension MeasurementData {
    var timeString: String {
        String(time)
    }
    
    var voltageMeasurementString: String {
        String(voltageMeasurement)
    }
}

struct ContentView: View {
    @StateObject var peripheralCommunication = PeripheralCommunication()
    @StateObject var measurementAnalyzer = MeasurementAnalyzer.shared
    
    @State var showCircleMarks = false
    @State var showAreaMark = false
    
    var chart: some View {
        Chart {
            ForEach(measurementAnalyzer.currentMeasurement) { dataPoint in
                Plot {
                    LineMark(
                        x: .value("Time", dataPoint.time),
                        y: .value("Voltage", dataPoint.voltageMeasurement)
                    )
                    .interpolationMethod(.catmullRom)
                    
                    if showAreaMark {
                        AreaMark(
                            x: .value("Time", dataPoint.time),
                            y: .value("Voltage", dataPoint.voltageMeasurement)
                        )
                        .foregroundStyle(Gradient(colors: [.blue.opacity(0.3), .clear]))
                        .interpolationMethod(.catmullRom)
                    }
                }
            }
        }
        .padding()
    }
    
    var chartWithMarks: some View {
        Chart {
            ForEach(measurementAnalyzer.currentMeasurement) { dataPoint in
                Plot {
                    LineMark(
                        x: .value("Time", dataPoint.time),
                        y: .value("Voltage", dataPoint.voltageMeasurement)
                    )
                    .symbol(.circle)
                    .interpolationMethod(.catmullRom)
                    
                    if showAreaMark {
                        AreaMark(
                            x: .value("Time", dataPoint.time),
                            y: .value("Voltage", dataPoint.voltageMeasurement)
                        )
                        .foregroundStyle(Gradient(colors: [.blue.opacity(0.3), .clear]))
                        .interpolationMethod(.catmullRom)
                    }
                }
            }
        }
        .padding()
    }

    var body: some View {
        NavigationStack {
            if (peripheralCommunication.isPoweredOn) {
                List {
                    Section {
                        if measurementAnalyzer.currentMeasurement.isEmpty {
                            VStack(alignment: .center) {
                                Image(systemName: "chart.line.uptrend.xyaxis")
                                    .symbolRenderingMode(.hierarchical)
                                    .resizable()
                                    .frame(width: 50.0, height: 50.0)
                                    .padding(.bottom)

                                Text("Waiting for Trigger")
                                    .bold()
                            }.frame(maxWidth: .infinity, maxHeight: .infinity)
                        } else {
                            if showCircleMarks {
                                chartWithMarks
                            } else {
                                chart
                            }
                        }
                    }
                    .frame(minHeight: 500)
                    
                    Section {
                        Toggle("Show Marks", isOn: $showCircleMarks)
                        Toggle("Show Area", isOn: $showAreaMark)
                    }
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
