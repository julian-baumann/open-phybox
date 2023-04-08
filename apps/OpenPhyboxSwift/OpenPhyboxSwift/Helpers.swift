//
//  Helpers.swift
//  OpenPhyboxSwift
//
//  Created by Julian Baumann on 06.04.23.
//

import Foundation
import SwiftUI
import Charts

extension FloatingPoint {
    
    /// Convert a byte array to Float/Double
    init?(_ bytes: [UInt8]) {
        guard bytes.count == MemoryLayout<Self>.size else { return nil }
        
        self = bytes.withUnsafeBytes {
            return $0.load(as: Self.self)
        }
    }
}

extension UInt32 {
    
    /// Convert a byte array to UInt32
    init?(_ bytes: [UInt8]) {
        guard bytes.count == MemoryLayout<Self>.size else { return nil }
        
        let littleEndianValue = bytes.withUnsafeBufferPointer {
            ($0.baseAddress!.withMemoryRebound(to: UInt32.self, capacity: 1) { $0 })
        }.pointee
        
        self = UInt32(littleEndian: littleEndianValue)
    }
}

extension Array {
    
    /// Splice an array into chunks
    func chunked(into size: Int) -> [[Element]] {
        return stride(from: 0, to: count, by: size).map {
            Array(self[$0 ..< Swift.min($0 + size, count)])
        }
    }
}


/// Negotiate a binding
prefix func ! (value: Binding<Bool>) -> Binding<Bool> {
    Binding<Bool>(
        get: { !value.wrappedValue },
        set: { value.wrappedValue = !$0 }
    )
}

extension View {
    /// Applies the given transform if the given condition evaluates to `true`.
    /// - Parameters:
    ///   - condition: The condition to evaluate.
    ///   - transform: The transform to apply to the source `View`.
    /// - Returns: Either the original `View` or the modified `View` if the condition is `true`.
    @ViewBuilder func `if`<Content: View>(_ condition: Bool, transform: (Self) -> Content) -> some View {
        if condition {
            transform(self)
        } else {
            self
        }
    }
}
