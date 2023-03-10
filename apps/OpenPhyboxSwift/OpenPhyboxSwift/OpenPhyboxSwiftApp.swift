//
//  OpenPhyboxSwiftApp.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 19.02.23.
//

import SwiftUI

@main
struct OpenPhyboxSwiftApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}

prefix func ! (value: Binding<Bool>) -> Binding<Bool> {
    Binding<Bool>(
        get: { !value.wrappedValue },
        set: { value.wrappedValue = !$0 }
    )
}
