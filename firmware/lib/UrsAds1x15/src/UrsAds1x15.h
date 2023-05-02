// UrsAds1x15
// Erweiterte Alternative zu der Adafruit-Library: // https://www.adafruit.com/product/1083
//
// Autor: http://UllisRoboterSeite.de
// Doku:  http://UllisRoboterSeite.de/esp8266-ADS1x15.html
// Created: 2018-02-10
//
// Version 1.0 (2018-02-10)
// - Basis-Version
//
// Version 1.1 (2018-03-01)
// - readRegister mit ICACHE_RAM_ATTR versehen
// - umgestellt auf UrsTwi
// - Fehler bei Ads1x15Gain++ und Ads1x15Gain-- behoben
//
// Version 1.2 (2018-03-01)
// - getGainString() hinzugefügt
// 
//
// Version 1.3 (2020-04-12)
// - reset hinzugefügt
// - Ads1x14Address umbenannt in Ads1x15Address
// - ADS1015_REG_... umbenannt in ADS1X15_REG_...
// - attachInterrupt nach Schreiben des Konfigutionsregisters
//   verhindert versentliches auslösen der ISR
// - Für Arduino erweitert
//
// Version 1.4 (2020-04-13)
// - Werte für Ads1115Sps waren vollkommen falsch. Korrigiert.
//
// Version 1.5 (2020-04-14)
// - readRegister hat den übergebenen Registerparamter ignoriert
//   und immer nur das Conversion-Register ausgelesen.
//
// Version 1.6 (2020-04-14)
// - setMaxSps hinzugefügt.
//
// Version 2.0 (2020-04-23)
// - Auch die Arduino-Variante muss mit einer TWI-Instanz konstruiert werden.
// - Methode getType hinzugefügt
// - Die User-ISR beim Continuous-Mode umgestellt. Sie erhält jetzt den aktuell ermittelten Rohwert
//   und einen Zeiger auf die zugehörige UrsAds1x15-Instanz.
// - Interrupts sind beim Eintritt in die User-ISR beim Continuous-Mode wieder abgeschaltet (nointerrupts()).
//
// Version 2.1 (2020-12-18)
// - Für ESP32 erweitert.
//
// Version 2.2 (2021-11-06)
// - I2C-Adressen korrigiert.

#ifndef _URSADS1X15_h
#define _URSADS1X15_h

#include <Arduino.h>
// =================================================================
// TWI Interface declaration
// =================================================================
// Ads1x15TwiClass is an alias für the concrete TWI interface class.
// If you want another interface
//    - Define ADS1x15_TWI_CLASS withe the name of the interface class
//    - include the appropriate header files
// before including UrsAds1x15.h.
//    
// Example for Arduino:
//   #include <Wire.h> // Uses interface TwoWire when using an Arduino Uno
//   #define ADS1x15_TWI_CLASS TwoWire
//   #inlcude <UrsAds1x15.h>
//
// Best practice is: put this lines into a separate header file and include this instead of UrsAds1x15.h

#if defined(ESP8266) || defined(ESP32)
   #include <UrsTwi.h>               // Uses interface UrsTwi when using an ESP8266
   using Ads1x15TwiClass = UrsTwi;   // Define the alias
#elif defined(ARDUINO_AVR_UNO) 
   #include <Wire.h>                 // Uses interface TwoWire when using an Arduino Uno
   using Ads1x15TwiClass = TwoWire;  // Define the alias
#else
   #error No TWI interface available.
#endif
// =================================================================

#ifdef __AVR__
  #define ICACHE_RAM_ATTR  // Method attribute not valid for Avr chips.
#endif // __AVR__

// Funktionsdefinition für die ISR für den Continuous-Mode
class UrsAds1x15; // Forward-Deklaration der Klasse
typedef void(*Ads1x15ISR)(int16_t rawValue, UrsAds1x15* ads1x15);

// =========================================================================
// I²C-Adress-Varianten für die ADS1x15-Module
// -------------------------------------------------------------------------
enum class Ads1x15Address : uint8_t {
  Default = 0x48, // 100 1000 (ADDR -> GND)
  VDD = 0x49,     // 100 1001 (ADDR -> VDD)
  SDA = 0x4A,     // 100 1010 (ADDR -> SDA)
  SCL = 0x4B      // 100 1011 (ADDR -> SCL)
};
// =========================================================================
// Konstanten für die Multiplexer-Auswahl
// -------------------------------------------------------------------------
enum class Ads1x15Mux: uint16_t {
  Diff_0_1 = 0x0000,  // Differential P = AIN0, N = AIN1 (default)
  Diff_0_3 = 0x1000,  // Differential P = AIN0, N = AIN3
  Diff_1_3 = 0x2000,  // Differential P = AIN1, N = AIN3
  Diff_2_3 = 0x3000,  // Differential P = AIN2, N = AIN3
  Single_0 = 0x4000,  // Single-ended AIN0
  Single_1 = 0x5000,  // Single-ended AIN1
  Single_2 = 0x6000,  // Single-ended AIN2
  Single_3 = 0x7000   // Single-ended AIN3
};

// =========================================================================
// Konstanten für die Verstärkung
// -------------------------------------------------------------------------

// Die Eingangsspannung darf (VDD + 0.3V) nicht überschreiten!
enum class Ads1x15Gain : uint16_t {
  Gain_2_3 = 0x0000,  // +/-6.144V range = Gain 2/3  0.1875mV pro Bit (default)
  Gain_1   = 0x0200,  // +/-4.096V range = Gain 1    0.125mV pro Bit
  Gain_2   = 0x0400,  // +/-2.048V range = Gain 2    0.0625mV pro Bit
  Gain_4   = 0x0600,  // +/-1.024V range = Gain 4    0.03125mV pro Bit
  Gain_8   = 0x0800,  // +/-0.512V range = Gain 8    0.015625mV pro Bit
  Gain_16  = 0x0A00,  // +/-0.256V range = Gain 16   0.0078125mV pro Bit
};

extern Ads1x15Gain& operator++(Ads1x15Gain&); // ++A
extern Ads1x15Gain  operator++(Ads1x15Gain &, int); // A++
extern Ads1x15Gain& operator--(Ads1x15Gain&); // --A
extern Ads1x15Gain  operator--(Ads1x15Gain&, int);  // A--

// =========================================================================
// Konstanten für die ADS-Typ-Erkennung
// -------------------------------------------------------------------------

// Die Eingangsspannung darf (VDD + 0.3V) nicht überschreiten!
enum class Ads1x15Type : uint8_t {
   ADS1015,
   ADS1115
};

void ICACHE_RAM_ATTR UrsAdsISR_ALL(int intNum);
class UrsAds1x15 {
   friend void UrsAdsISR_ALL(int);
protected:
   uint8_t _id = 0;               // ID dieser Instanz
   Ads1x15TwiClass& _twi;         // TwoWire-Interface

   uint8_t     _i2cAddress;       // Eingestellte I²C-Adresse
   uint8_t     _conversionDelay;  // Zeitspanne in ms auf die für eine Konvertierung gewartet wird
   Ads1x15Gain _gain;             // Aktuell eingestellter Verstärkungsfaktor
   uint16_t    _SamplesPerSecond; // Messungen pro Sekunde
   float       _mVoltsPerBit;     // Auf _gain beruhender Umrechnungsfaktor

   bool        _isContinuousMode = false;// true, wenn der kontinuierliche Modus aktiv ist
   uint16_t    _SpsCode;          // SPS-Code als Zahl.
   uint8_t     _alertPin;         // Pin für Alert/RDY

   void        writeRegister(uint8_t reg, uint16_t value);  // Schreibt in das angegebene Register des ADS1x15 
   uint16_t    readRegister(uint8_t reg);                   // Liest das angegebene Register aus

   Ads1x15ISR userISR = nullptr;    // Callback für die Anwender-ISR zur Auswerung der Daten
   int8_t intNo = NOT_AN_INTERRUPT; // Interrupt-Nummer zum alertPin passend
   void attachInt();     // registiert den Interrupt-Handler
   void detachInt();     // meldet den Interrupt-Handler ab.

  // Initialisiert eine neue Instanz der UrsAds1x15Class-Klasse
   UrsAds1x15(Ads1x15TwiClass& twi, Ads1x15Address i2cAddress); 

 public:
   void        reset();                         // Versetzt den Chip in den Power-Down-Zustand
   void        setGain(const Ads1x15Gain gain); // Legt den Verstärkungsfaktor fest
   Ads1x15Gain getGain(void) const;             // Liefert den aktuell eingestellten Verstärkungsfaktor
   String      getGainString();
   float       getMilliVoltsPerBit(void) const { return _mVoltsPerBit; } // Liefert den aktuellen Umrechnungsfaktor

   uint8_t     getConversionDelay(void) const { return _conversionDelay; }     // Liefert die Zeitspanne in ms, auf die für eine Konvertierung gewartet wird
   uint16_t    getSamplesPerSeconds(void) const { return _SamplesPerSecond; }  // Liefert die Messrate (Messungen pro Sekunde)
   virtual void setMaxSps() = 0; // stellt die maximale Abtastrate ein.

   int16_t     readAdcRaw(const Ads1x15Mux mux);  // Führt eine Einzelmessung für den angegebenen Kanal durch, liefert den Register-Rohwert
   float       readADCmV(const Ads1x15Mux mux) { return readAdcRaw(mux) * getMilliVoltsPerBit();}  // Führt eine Einzelmessung für den angegebenen Kanal durch, liefert mV

   // Methoden zum Starten, Stoppen und Auslesen der kontunierlichen Wandlung
   void        startContinuousMode(const Ads1x15Mux mux, Ads1x15ISR ISR, const uint8_t alertPin, const bool usePullUpForAlertPin = true);
   void        stopContinuousMode(void);
   int16_t     readContinuousModeRaw(void) const; // Liefert den Register-Rohwert
   bool        isContinuousMode(void) const { return _isContinuousMode; }
   virtual Ads1x15Type getType() = 0; // Liefert den ADS1x15-Typ.
   void        setID(uint8_t id) { _id = id; } // legt die ID für diese Instanz fest
   uint8_t     getID() { return _id; } // ruft die ID dieser Instanz ab
};

// ======================================================================================================
// ===  UrsAds1015
// ======================================================================================================
enum class Ads1015Sps : uint16_t {
  Sps128  = 0x0000,  //  128 samples per second
  Sps250  = 0x0020,  //  250 samples per second
  Sps490  = 0x0040,  //  490 samples per second
  Sps920  = 0x0060,  //  920 samples per second
  Sps1600 = 0x0080,  // 1600 samples per second (default)
  Sps2400 = 0x00A0,  // 2400 samples per second
  Sps3300 = 0x00C0   // 3300 samples per second
};

class UrsAds1015 : public UrsAds1x15 {
protected:
  Ads1015Sps  _Sps;  // Messungen pro Sekunde (Code)

public:
   // Initialisiert eine neue Instanz der UrsAds1015Class-Klasse
   UrsAds1015(Ads1x15TwiClass& twi, Ads1x15Address i2cAddress = Ads1x15Address::Default);

  void        setSps(Ads1015Sps Sps);        // Legt die Anzahl der Messungen pro Sekunde fest
  Ads1015Sps  getSps(void) { return _Sps; }  // Liefert den Code für die Messrate
  void        setMaxSps() { setSps(Ads1015Sps::Sps3300); } // stellt die maximale Abtastrate ein.
  Ads1x15Type getType() { return Ads1x15Type::ADS1015; } // Liefert den ADS1x15-Typ.
};


// ======================================================================================================
// ===  UrsAds1115
// ======================================================================================================
enum class Ads1115Sps : uint16_t {
  Sps8   = 0x0000, //   8 samples per second
  Sps16  = 0x0020, //  16 samples per second
  Sps32  = 0x0040, //  32 samples per second
  Sps64  = 0x0060, //  64 samples per second
  Sps128 = 0x0080, // 128 samples per second (default)
  Sps250 = 0x00A0, // 250 samples per second
  Sps475 = 0x00C0, // 475 samples per second
  Sps860 = 0x00E0  // 860 samples per second
};

class UrsAds1115 : public UrsAds1x15 {
protected:
  Ads1115Sps  _Sps; // Messungen pro Sekunde (Code)

public:
   UrsAds1115(Ads1x15TwiClass& twi, Ads1x15Address i2cAddress = Ads1x15Address::Default);   // Initialisiert eine neue Instanz der UrsAds1115Class-Klasse

  void        setSps(Ads1115Sps Sps);        // Legt die Anzahl der Messungen pro Sekunde fest
  Ads1115Sps  getSps(void) { return _Sps; }  // Liefert den Code für die Messrate
  void        setMaxSps() { setSps(Ads1115Sps::Sps860); } // stellt die maximale Abtastrate ein.
  Ads1x15Type getType() { return Ads1x15Type::ADS1115; }  // Liefert den ADS1x15-Typ.
};
#endif