//
//  SelectDeviceView.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 19.02.23.
//

import Foundation

import SwiftUI

struct SelectDeviceView: View {
    @EnvironmentObject var peripheralCommunication: PeripheralCommunication
    @State var animate = true
    private let color: Color = Color.green
    
    var body: some View {
        NavigationStack {
            VStack {
                Spacer()
                
                ZStack {
                    Circle().fill(color.opacity(0.25)).frame(width: 200, height: 200).scaleEffect(self.animate ? 1 : 0)
                    Circle().fill(color.opacity(0.35)).frame(width: 150, height: 150).scaleEffect(self.animate ? 1 : 0)
                    Circle().fill(color.opacity(0.45)).frame(width: 100, height: 100).scaleEffect(self.animate ? 1 : 0)
                    Circle().fill(color).frame(width: 50, height: 50)
                }
                .onAppear { self.animate.toggle() }
                .animation(Animation.easeInOut(duration: 1.5).repeatForever(autoreverses: true), value: self.animate)
                
                Spacer()
                
                if peripheralCommunication.connecting {
                    HStack {
                        ProgressView()
                        Text("Connecting to open-phybox device")
                            .padding(.leading, 10)
                            .foregroundColor(.secondary)
                    }
                }
                
                GroupBox {
                    VStack {
                        Text("Turn on your Open Phybox device")
                    }.padding()
                }
            }
            .padding()
            .navigationTitle("Searching...")
            #if os(iOS)
            .navigationBarTitleDisplayMode(.inline)
            #elseif os(macOS)
            .frame(width: 350, height: 350)
            #endif
        }
    }
}

struct SelectDeviceView_Previews: PreviewProvider {
    static var previews: some View {
        SelectDeviceView()
    }
}
