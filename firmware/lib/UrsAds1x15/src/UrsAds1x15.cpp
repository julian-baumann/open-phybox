#include <UrsAds1x15.h>
#include <wiring_private.h> // Wird benötigt für EXTERNAL_NUM_INTERRUPTS

#if defined(__AVR__) 
#define ICACHE_RAM_ATTR
#endif 
/*=========================================================================
POINTER REGISTER
-----------------------------------------------------------------------*/
#define ADS1X15_REG_POINTER_MASK        (0x03)
#define ADS1X15_REG_POINTER_CONVERT     (0x00)
#define ADS1X15_REG_POINTER_CONFIG      (0x01)
#define ADS1X15_REG_POINTER_LOWTHRESH   (0x02)
#define ADS1X15_REG_POINTER_HITHRESH    (0x03)
/*=========================================================================*/

/*=========================================================================
CONFIG REGISTER
-----------------------------------------------------------------------*/
#define ADS1X15_REG_CONFIG_OS_MASK      (0x8000)
#define ADS1X15_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion
#define ADS1X15_REG_CONFIG_OS_BUSY      (0x0000)  // Read: Bit = 0 when conversion is in progress
#define ADS1X15_REG_CONFIG_OS_NOTBUSY   (0x8000)  // Read: Bit = 1 when device is not performing a conversion

#define ADS1X15_REG_CONFIG_MUX_MASK     (0x7000)  // Maske für Multiplexer
#define ADS1X15_REG_CONFIG_PGA_MASK     (0x0E00)  // Maske für Gain

#define ADS1X15_REG_CONFIG_MODE_MASK    (0x0100)
#define ADS1X15_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
#define ADS1X15_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

#define ADS1X15_REG_CONFIG_DR_MASK      (0x00E0)  // Maske für SamplesPerSecond

#define ADS1X15_REG_CONFIG_CMODE_MASK   (0x0010)
#define ADS1X15_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
#define ADS1X15_REG_CONFIG_CMODE_WINDOW (0x0010)  // Window comparator

#define ADS1X15_REG_CONFIG_CPOL_MASK    (0x0008)
#define ADS1X15_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
#define ADS1X15_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

#define ADS1X15_REG_CONFIG_CLAT_MASK    (0x0004)  // Determines if ALERT/RDY pin latches once asserted
#define ADS1X15_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
#define ADS1X15_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

#define ADS1X15_REG_CONFIG_CQUE_MASK    (0x0003)
#define ADS1X15_REG_CONFIG_CQUE_1CONV   (0x0000)  // Assert ALERT/RDY after one conversions
#define ADS1X15_REG_CONFIG_CQUE_2CONV   (0x0001)  // Assert ALERT/RDY after two conversions
#define ADS1X15_REG_CONFIG_CQUE_4CONV   (0x0002)  // Assert ALERT/RDY after four conversions
#define ADS1X15_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)
/*=========================================================================*/

void UrsAds1x15::writeRegister(uint8_t reg, uint16_t value) {
  _twi.beginTransmission(_i2cAddress);
  _twi.write(reg);
  _twi.write((uint8_t)(value >> 8));
  _twi.write((uint8_t)(value & 0xFF));
  _twi.endTransmission(true);
}

uint16_t ICACHE_RAM_ATTR UrsAds1x15::readRegister(uint8_t reg) {
  _twi.beginTransmission(_i2cAddress);
  _twi.write(reg);
  _twi.endTransmission(true);
  _twi.requestFrom(_i2cAddress, (uint8_t) 2, (uint8_t)true);
  return ((_twi.read() << 8) | _twi.read());
}

// Constructor UrsAds1x15

UrsAds1x15::UrsAds1x15(Ads1x15TwiClass& twi, Ads1x15Address i2cAddress) : _twi(twi) {
  _i2cAddress = (uint8_t) i2cAddress;
  setGain(Ads1x15Gain::Gain_2_3); /* +/- 6.144V range (limited to VDD +0.3V max!) */
}

Ads1x15Gain UrsAds1x15::getGain() const {
  return _gain;
}

void UrsAds1x15::reset() {
   static const uint8_t general_i2c_addr = 0x00;
   static const uint8_t ads1115_i2c_reset = 0x06;
   _twi.beginTransmission(general_i2c_addr);
   _twi.write(ads1115_i2c_reset);
   _twi.endTransmission(true);
}

void UrsAds1x15::setGain(const Ads1x15Gain gain) {
  _gain = gain;
  switch (gain) {
  case Ads1x15Gain::Gain_2_3: _mVoltsPerBit = 0.1875;    break;
  case Ads1x15Gain::Gain_1:   _mVoltsPerBit = 0.125;     break;
  case Ads1x15Gain::Gain_2:   _mVoltsPerBit = 0.0625;    break;
  case Ads1x15Gain::Gain_4:   _mVoltsPerBit = 0.03125;   break;
  case Ads1x15Gain::Gain_8:   _mVoltsPerBit = 0.015625;  break;
  case Ads1x15Gain::Gain_16:  _mVoltsPerBit = 0.0078125; break;
  }
}

int16_t UrsAds1x15::readAdcRaw(Ads1x15Mux channel) {
  if (_isContinuousMode)
    return 0;

  // Config Register zusammenstellen
  uint16_t config = ADS1X15_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
                    ADS1X15_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
                    ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1X15_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
                    ADS1X15_REG_CONFIG_MODE_SINGLE |  // Single-shot mode (default)
                    ADS1X15_REG_CONFIG_OS_SINGLE;     // Set 'start single-conversion' bit

  config |= (uint16_t)_SpsCode; // Messrate/-dauer auswählen
  config |= (uint16_t)_gain;    // Vorverstärker auswählen
  config |= (uint16_t)channel;  // Kanal auswählen

  // Config Register ausgeben
  writeRegister(ADS1X15_REG_POINTER_CONFIG, config);

  // Auf das Ende der Messung warten
  delay(_conversionDelay);

  // Messwert auslesen
  return (int16_t)readRegister(ADS1X15_REG_POINTER_CONVERT);
}

void UrsAds1x15::startContinuousMode(const Ads1x15Mux mux, Ads1x15ISR ISR, const uint8_t alertPin, const bool usePullUpForAlertPin) {
   intNo = digitalPinToInterrupt(alertPin);
   if (intNo == NOT_AN_INTERRUPT)
      return;

  uint16_t config = ADS1X15_REG_CONFIG_CQUE_1CONV |   // Disable the comparator
                    ADS1X15_REG_CONFIG_CLAT_NONLAT |  // Non-latching 
                    ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low
                    ADS1X15_REG_CONFIG_CMODE_TRAD |   // Traditional comparator
                    ADS1X15_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

  config |= (uint16_t)_SpsCode; // Messrate/-dauer auswählen
  config |= (uint16_t)_gain;    // Vorverstärker auswählen
  config |= (uint16_t)mux;      // Kanal auswählen

  writeRegister( ADS1X15_REG_POINTER_HITHRESH, 0x8000); // High- & Low-Threshold-Register für RDY konfigurieren
  writeRegister( ADS1X15_REG_POINTER_LOWTHRESH, 0);

  // Interrupt initialisieren
  if (usePullUpForAlertPin)
    pinMode(alertPin, INPUT_PULLUP); // PullUp auf dem Interrupt-Pin
  else
    pinMode(alertPin, INPUT);

  // Wandlung starten
  writeRegister(ADS1X15_REG_POINTER_CONFIG, config);

  // Auslesen des Konvertierungsregisters vorbereiten
  _twi.beginTransmission(_i2cAddress);
  _twi.write(ADS1X15_REG_POINTER_CONVERT);
  _twi.endTransmission(true);
  userISR = ISR;

  attachInt(); // ISR registrieren

  _isContinuousMode = true;
}

void UrsAds1x15::stopContinuousMode() {
  if (!_isContinuousMode)
    return;

  detachInt();
  // Config Register zusammenstellen
  uint16_t config = ADS1X15_REG_CONFIG_CQUE_NONE |    // Disable the comparator
                    ADS1X15_REG_CONFIG_CLAT_NONLAT |  // Non-latching
                    ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low
                    ADS1X15_REG_CONFIG_CMODE_TRAD |   // Traditional comparator
                    ADS1X15_REG_CONFIG_MODE_SINGLE;

  writeRegister(ADS1X15_REG_POINTER_CONFIG, config);
  writeRegister(ADS1X15_REG_POINTER_HITHRESH, 0x7FFF); // Standardwerte lt. Datenblatt
  writeRegister(ADS1X15_REG_POINTER_LOWTHRESH, 0x8000);
  _isContinuousMode = false;
}


int16_t ICACHE_RAM_ATTR UrsAds1x15::readContinuousModeRaw() const {
  if (!_isContinuousMode) // kontinuierlicher Modus aktiv?
    return 0;

  // Konvertierungsregister abrufen
  // Pointer-Register wurde bereits in startContinuousMode gesetzt.
  _twi.requestFrom(_i2cAddress, (uint8_t)2, (uint8_t)true); 

  uint16_t res = ((_twi.read() << 8) | _twi.read());
  return (int16_t)res;
}


// ======================================================================================================
// ===  UrsAds1015Class
// ======================================================================================================
UrsAds1015::UrsAds1015(Ads1x15TwiClass& twi, Ads1x15Address i2cAddress) : UrsAds1x15(twi, i2cAddress) {
   setSps(Ads1015Sps::Sps1600);
}
 
void UrsAds1015::setSps(Ads1015Sps Sps) {
  _Sps = Sps;
  _SpsCode = (uint16_t)Sps;
  switch (Sps) {
  case Ads1015Sps::Sps128:
    _SamplesPerSecond = 128;
    _conversionDelay = 10;
    break;
  case Ads1015Sps::Sps250:
    _SamplesPerSecond = 250;
    _conversionDelay = 5;
    break;
  case Ads1015Sps::Sps490:
    _SamplesPerSecond = 490;
    _conversionDelay = 3;
    break;
  case Ads1015Sps::Sps920:
    _SamplesPerSecond = 920;
    _conversionDelay = 1;
    break;
  case Ads1015Sps::Sps1600:
    _SamplesPerSecond = 1600;
    _conversionDelay = 1;
    break;
  case Ads1015Sps::Sps2400:
    _SamplesPerSecond = 2400;
    _conversionDelay = 1;
    break;
  case Ads1015Sps::Sps3300:
    _SamplesPerSecond = 3300;
    _conversionDelay = 1;
    break;
  default: // Fehler
    _SamplesPerSecond = 0;
    _conversionDelay = 1;
    break;
  }
}


// ======================================================================================================
// ===  UrsAds1115Class
// ======================================================================================================

UrsAds1115::UrsAds1115(Ads1x15TwiClass& twi, Ads1x15Address i2cAddress) : UrsAds1x15(twi, i2cAddress) {
  setSps(Ads1115Sps::Sps128);
}

void UrsAds1115::setSps(Ads1115Sps Sps) {
  _Sps = Sps;
  _SpsCode = (uint16_t)Sps;
  switch (Sps) {
  case Ads1115Sps::Sps8:
    _SamplesPerSecond = 8;
    _conversionDelay = 150;
    break;
  case Ads1115Sps::Sps16:
    _SamplesPerSecond = 16;
    _conversionDelay = 100;
    break;
  case Ads1115Sps::Sps32:
    _SamplesPerSecond = 32;
    _conversionDelay = 40;
    break;
  case Ads1115Sps::Sps64:
    _SamplesPerSecond = 64;
    _conversionDelay = 20;
    break;
  case Ads1115Sps::Sps128:
    _SamplesPerSecond = 128;
    _conversionDelay = 8;
    break;
  case Ads1115Sps::Sps250:
    _SamplesPerSecond = 250;
    _conversionDelay = 5;
    break;
  case Ads1115Sps::Sps475:
    _SamplesPerSecond = 475;
    _conversionDelay = 3;
    break;
  case Ads1115Sps::Sps860:
    _SamplesPerSecond = 820;
    _conversionDelay = 2;
    break;
  default: // Fehler
    _SamplesPerSecond = 0;
    _conversionDelay = 1;
    break;
  }
}


Ads1x15Gain & operator++(Ads1x15Gain& p_oRight) { // ++A
  switch (p_oRight) {
  case Ads1x15Gain::Gain_2_3:
    p_oRight = Ads1x15Gain::Gain_1;
    break;
  case Ads1x15Gain::Gain_1:
    p_oRight = Ads1x15Gain::Gain_2;
    break;
  case Ads1x15Gain::Gain_2:
    p_oRight = Ads1x15Gain::Gain_4;
    break;
  case Ads1x15Gain::Gain_4:
    p_oRight = Ads1x15Gain::Gain_8;
    break;
  case Ads1x15Gain::Gain_8:
    p_oRight = Ads1x15Gain::Gain_16;
    break;
  case Ads1x15Gain::Gain_16:
    p_oRight = Ads1x15Gain::Gain_16;
    break;
  }
  return p_oRight;
}

Ads1x15Gain operator++(Ads1x15Gain & p_oRight, int) { // A++
  Ads1x15Gain oCopy = p_oRight;
  ++p_oRight;
  return oCopy;
}

Ads1x15Gain & operator--(Ads1x15Gain& p_oRight) { // --A
  switch (p_oRight) {
  case Ads1x15Gain::Gain_2_3:
    p_oRight = Ads1x15Gain::Gain_2_3;
    break;
  case Ads1x15Gain::Gain_1:
    p_oRight = Ads1x15Gain::Gain_2_3;
    break;
  case Ads1x15Gain::Gain_2:
    p_oRight = Ads1x15Gain::Gain_1;
    break;
  case Ads1x15Gain::Gain_4:
    p_oRight = Ads1x15Gain::Gain_2;
    break;
  case Ads1x15Gain::Gain_8:
    p_oRight = Ads1x15Gain::Gain_4;
    break;
  case Ads1x15Gain::Gain_16:
    p_oRight = Ads1x15Gain::Gain_8;
    break;
  }
  return p_oRight;
}

String UrsAds1x15::getGainString() {
  switch (_gain) {
  case Ads1x15Gain::Gain_2_3: return "2/3";
  case Ads1x15Gain::Gain_1: return "1";
  case Ads1x15Gain::Gain_2: return "2";
  case Ads1x15Gain::Gain_4: return "4";
  case Ads1x15Gain::Gain_8: return "8";
  case Ads1x15Gain::Gain_16: return "16";
  default:
    return "?";
  }
}

Ads1x15Gain operator--(Ads1x15Gain & p_oRight, int) { // A--
  Ads1x15Gain oCopy = p_oRight;
  --p_oRight;
  return oCopy;
}


// ===========================================================================
// Interrupt handling beim Continuous Mode
// ===========================================================================
// Für jeden möglichen Interrupt, wird hier abgelegt, welches die zugeordnete UrsAds1x15-Instanz ist.
static UrsAds1x15* adsList[EXTERNAL_NUM_INTERRUPTS];

// Zentrale ISR
void ICACHE_RAM_ATTR UrsAdsISR_ALL(int intNum) {
   interrupts(); // TWI braucht freigegebene Interrupts
   int16_t result = adsList[intNum]->readContinuousModeRaw();
   noInterrupts();
   adsList[intNum]->userISR(result, adsList[intNum]);

}

// Für jeden möglichen Interrupt, wird eine eigene ISR angelegt,
// die UrsAdsISR_ALL aufruft und die Interrupt-Nummer übergibt.
#define IMPLEMENT_ISR(interrupt) \
  static void ICACHE_RAM_ATTR ADCISR##interrupt() { \
    UrsAdsISR_ALL(interrupt); \
  } \

IMPLEMENT_ISR(0)
#if EXTERNAL_NUM_INTERRUPTS > 1
IMPLEMENT_ISR(1)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 2
IMPLEMENT_ISR(2)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 3
IMPLEMENT_ISR(3)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 4 
IMPLEMENT_ISR(4)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 5 
IMPLEMENT_ISR(5)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 6 
IMPLEMENT_ISR(6)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 7 
IMPLEMENT_ISR(7)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 8 
IMPLEMENT_ISR(8)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 9 
IMPLEMENT_ISR(9)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 10
IMPLEMENT_ISR(10)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 11 
IMPLEMENT_ISR(11)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 12 
IMPLEMENT_ISR(12)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 13 
IMPLEMENT_ISR(13)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 14 
IMPLEMENT_ISR(14)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 15 
IMPLEMENT_ISR(15)
#endif

#if EXTERNAL_NUM_INTERRUPTS > 16
   #ifndef LESS_THAN_16_INTERRUPTS
      #error You cannot use more than 16 interrupts. Define LESS_THAN_16_INTERRUPTS to supress this error.
   #endif // !LESS_THAN_16_INTERRUPTS
#endif

#define SWITCH_CASE(interrupt) \
case interrupt: attachInterrupt(interrupt, ADCISR##interrupt, FALLING); break; // ISR registrieren


#if defined(_VMICRO_INTELLISENSE)
void UrsAds1x15::attachInt() {} // Intellisense braucht diese Angabe doppelt :-(
#endif
void UrsAds1x15::attachInt() {
   adsList[intNo] = this;
   switch (intNo) {
   case 0: attachInterrupt(0, ADCISR0, FALLING); break; // ISR registrieren
#if EXTERNAL_NUM_INTERRUPTS > 1
    SWITCH_CASE(1)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 2
    SWITCH_CASE(2)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 3
    SWITCH_CASE(3)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 4 
    SWITCH_CASE(4)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 5 
    SWITCH_CASE(5)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 6 
    SWITCH_CASE(6)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 7 
    SWITCH_CASE(7)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 8 
    SWITCH_CASE(8)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 9 
    SWITCH_CASE(9)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 10
    SWITCH_CASE(10)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 11 
    SWITCH_CASE(11)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 12 
    SWITCH_CASE(12)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 13 
    SWITCH_CASE(13)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 14 
    SWITCH_CASE(14)
#endif
#if EXTERNAL_NUM_INTERRUPTS > 15 
    SWITCH_CASE(15)
#endif
   } // switch
}

void UrsAds1x15::detachInt() {
   detachInterrupt(intNo);
   adsList[intNo] = nullptr;
}