//
//  BluetoothDisabledView.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 19.02.23.
//

import SwiftUI

struct BluetoothDisabledView: View {
    var body: some View {
            VStack(alignment: .center) {
                Image(systemName: "xmark.circle.fill")
                    .symbolRenderingMode(.hierarchical)
                    .resizable()
                    .frame(width: 50.0, height: 50.0)
                    .foregroundColor(.red)

                Text("Bluetooth is switched off")
                    .bold()
                    .foregroundColor(.red)
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            .padding()
    }
}

struct BluetoothDisabledView_Previews: PreviewProvider {
    static var previews: some View {
        BluetoothDisabledView()
    }
}
