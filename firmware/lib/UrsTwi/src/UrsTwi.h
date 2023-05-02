// UrsTwi.h

// Autor: https://UllisRoboterSeite.de

// Doku:  https://ullisroboterseite.de/esp8266-UrsTwi.html
// Created: 2018-03-01
//
// Bitbang Version des I2C-Interface für ESP8266 und ESP32
// Alle Methoden befinden sich im IRAM. Deshalb kann auf das Interface
// auch innerhalb einer ISR zugegriffen werden.
//
// Version 1 (2018-03-01)
// -------------------------
// - Basis-Version
//
// Version 1.1 (2020-12-21)
// -------------------------
// - BUFFER_LENGTH auf 128 erhöht
// - mit "#if defined" für ESP8266 oder ESP32 spezifiziert
// - Parameter initPins im Konstruktor hinzugefügt
// Hinweis: die TWI-Taktfrequenz für den ESP32 ist nicht justiert und zu niedrig.

#ifndef _URSTWI_h
#define _URSTWI_h
#if defined(ESP8266) || defined(ESP32)
#include <Arduino.h>
#include <Stream.h>

#define BUFFER_LENGTH 128

enum class TwiStatus {
  OK,
  SCL_HELD_LOW,
  SCL_HELD_LOW_AFTER_READ,
  SDA_HELD_LOW,
  SDA_HELD_LOW_AFTER_INIT
};


class UrsTwi : public Stream {
protected:
  uint8_t rxBuffer[BUFFER_LENGTH];
  uint8_t rxBufferIndex;
  uint8_t rxBufferLength;
  uint8_t sda;
  uint8_t scl;
  uint32_t sda_mask = 0; // Masken zum beschreiben der GPIO-Register
  uint32_t scl_mask = 0;
  uint32_t clockStretchLimit;

#if defined(ESP8266)
  uint8_t dCount = 18; // Twi-Interner delay
  void twi_delay(uint8_t v);
#else
  uint16_t dCount = 1200; // Ticks für Clock-Pulse (SCL)
  void twi_delay(uint16_t v);
#endif

  uint8_t transmitting = 0;
  uint8_t txAddress;
  uint8_t txBuffer[BUFFER_LENGTH];
  uint8_t txBufferIndex;
  uint8_t txBufferLength;

  bool writeStart(void);
  bool writeStop(void);
  bool writeByte(uint8_t byte);
  bool writeBit(bool bit);
  bool readBit(void);
  uint8_t readByte(bool nack);
  size_t readFrom(uint8_t address, uint8_t* buf, size_t len, bool sendStop);
  uint8_t twi_writeTo(uint8_t address, uint8_t * buf, unsigned int len, bool sendStop);

public:
   /**
   * Initializes a new instance of the UrsTwi class.
   * @param sda       SDA pin of the TWI interface. Default ESP8266: 4 (D2); ESP32: 21
   * @param scl       SCL pin of the TWI interface. Default ESP8266: 5 (D1); ESP32: 22
   * @param initPins  If true, SDA and SCL are set to INPUT_PULLUP
   */
  UrsTwi(uint8_t sda = SDA, uint8_t scl = SCL, bool initPins = true);

  void begin(int sda, int scl);
  void begin();

  size_t requestFrom(uint8_t address, size_t size, bool sendStop);
  uint8_t requestFrom(uint8_t address, uint8_t quantity);
  
  void setClockStretchLimit(uint32_t limit);
  void setClock(uint32_t freq);
  void beginTransmission(uint8_t address);
  uint8_t endTransmission(uint8_t sendStop);
  TwiStatus getStatus();

  virtual size_t write(uint8_t data);
  virtual size_t write(const uint8_t * data, size_t quantity);
  virtual int available(void);
  virtual int read(void);
  virtual int peek(void);
  virtual void flush(void);

};

#endif
#endif