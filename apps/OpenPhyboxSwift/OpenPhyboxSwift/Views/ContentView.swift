//
//  ContentView.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 19.02.23.
//

import SwiftUI

struct ContentView: View {
    @StateObject var peripheralCommunication = PeripheralCommunication()
    
    var body: some View {
        NavigationStack {
            if (peripheralCommunication.isPoweredOn) {
                VStack {
                    Text(peripheralCommunication.currentMeasurementValue)
                }
                .padding()
                .sheet(isPresented: !$peripheralCommunication.connected) {
                    SelectDeviceView()
                        .environmentObject(peripheralCommunication)
                        .interactiveDismissDisabled(true)
                }
                .navigationTitle("Open Phybox")
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
