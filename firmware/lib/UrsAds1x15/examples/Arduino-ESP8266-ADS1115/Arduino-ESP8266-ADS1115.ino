
#include <UrsAds1x15.h>

// 'TwiClass' ist ein Alias für die konkrete TWI-Klasse und wird prozessorspezifisch definiert.
#if defined(ESP8266) 
   // Beim ESP8266 wird die Klasse 'UrsTwi' in der Default-Konfiguration verwandt (SCL: D1, SDA: D2).
   #include <UrsTwi.h>
   // 'TwiClass' ist ein Alias für die konkrete TWI-Klasse und wird prozessorspezifisch definiert.
   using TwiClass = UrsTwi;
   // UrsTwi hat keine bereits vordefinierte Instanz und muss im Programm angelegt werden.
   TwiClass twi;
   constexpr uint8_t alertPin = D4;  // (GPIO2) Prozessorspezifischer Interrupt-Pin
#elif defined(ESP32)
   // Beim ESP32 wird die Klasse 'UrsTwi' in der Default-Konfiguration verwandt (SCL: D1, SDA: D2).
   #include <UrsTwi.h>
   // 'TwiClass' ist ein Alias für die konkrete TWI-Klasse und wird prozessorspezifisch definiert.
   using TwiClass = UrsTwi;
   // UrsTwi hat keine bereits vordefinierte Instanz und muss im Programm angelegt werden.
   TwiClass twi;
   constexpr uint8_t alertPin = 14;
#elif defined(ARDUINO_AVR_UNO)
   // Beim Arduino Uno soll die Klasse 'TwoWire' in der Default-Konfiguration verwandt werden (SCL: A5, SDA: A4).
   // Die vordefinierte Instan dieser Klasse heißt 'Wire'.
   #include <Wire.h>
   // 'ICACHE_RAM_ATTR' bewirkt beim ESP8266, dass die so markierte Methode im IRAM abgelegt werden.
   // Bei einem AVR gibt es so etwas nicht.
   #define ICACHE_RAM_ATTR
   // 'TwiClass' ist ein Alias für die konkrete TWI-Klasse und wird prozessorspezifisch definiert.
   using TwiClass = TwoWire; // SCL: A5, SDA: A4
   TwiClass& twi = Wire; // keine neue Instanz anlegen, sondern Wire benutzen.
   constexpr uint8_t alertPin = 2;  // Prozessorspezifischer Interrupt-Pin (D2, INT0)

#else 
#error Processor type not supported
#endif

UrsAds1015 ads(twi);
// UrsAds1115 ads(twi);


const    uint8_t maxData = 33;   // Anzahl Messwerte
                                 // Ergibt 32 valide Werte. Wert 0 wird verworfen, ist h�ufig ein Au�rei�er.
volatile int16_t data[maxData];  // Array zur Aufnahme der Daten
volatile uint8_t ind = 0;        // Index für data

// ISR
void ICACHE_RAM_ATTR ADCISR(int16_t rawValue, UrsAds1x15* ads1x15) {
   if (ind < maxData) {
      data[ind++] = rawValue;
   }
}

// Startet den Messvorgang
// Wird zweimal benutzt, sorgt für gleiche Parameter bei beiden Aufrufen.
void startContinuousMode(void) {
   ind = 0;
   ads.startContinuousMode(Ads1x15Mux::Single_0, ADCISR, alertPin); // Messvorgang starten
}

void setup(void) {
   ads.reset();
   Serial.begin(9600);
   Serial.println(F("\n\n----------------------------\nADS1015 1\n"));
   delay(1000);

   twi.begin(); // TWI-Interface starten

   // Im Continous-Mode und bei 3300 SPS beim ADS1015 muss die Takfrequenz auf min. 400kHz gesetzt weden.
   // Bei der Standardeinstellung 100kHz dauert das Einlesen gut 320us + die zusützlich auszuführenden Anweisungen.
   // Es stehen aber nur max. 300us zur Verfügung. Der nächste Interrupt erfolgt dann bevor die ISR des
   // des vorhergehenden beendet wurde.
   twi.setClock(400000L); // TWI-Taktfrequenz: 400 kHz
                          // setClock() muss N A C H begin() aufgerufen werden.
                          // begin() setzt die Transferate auf 100kHz.


   //ads.setSps(Ads1015Sps::Sps2400); // 2400 Messungen pro Sekude
   //ads.setSps(Ads1115Sps::Sps250);  // 250 Messungen pro Sekude
   ads.setMaxSps(); // 3300 Messungen pro Sekude beim ADS1015, 860 SPS beim ADS1115

   ads.setGain(Ads1x15Gain::Gain_1); // Höchste Empfindlichkeit

   Serial.print(F("Sample rate: ")); Serial.print(ads.getSamplesPerSeconds()); Serial.println(" SPS");
   Serial.print(F("Gain:        ")); Serial.println(ads.getGainString());
   Serial.print(F("mV per Bit:  ")); Serial.println(ads.getMilliVoltsPerBit());

   Serial.println(ads.readADCmV(Ads1x15Mux::Single_0));

   startContinuousMode(); // Messvorgang starten
} 

void loop(void) {
   if (ind < maxData)
      return; // Messung läuft noch

   ads.stopContinuousMode(); // Messung stoppen
   Serial.println(F("\n\n----------------------------\n"));

   float sum = 0;
   for (int i = 1; i < maxData; i++) { // Messwerte ausgeben, ohne 1. Wert, der ist häufig ein Ausreißer
      Serial.print(i); Serial.print("\t"); Serial.print(data[i]); Serial.print("\t");
      float mv = ads.getMilliVoltsPerBit()*data[i];
      Serial.print(mv); Serial.println("\tmV");
      sum += mv;
   }

   Serial.println(F("-----------------"));
   Serial.print(F("Mean:\t"));  Serial.print(sum / (maxData - 1)); Serial.println("\tmV");
   delay(5000); // 5 Sekunden Pause

   // Messung neu starten
   startContinuousMode();
}