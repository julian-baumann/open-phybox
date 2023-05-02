#if defined(ESP8266) || defined(ESP32)

#include "UrsTwi.h"

#if defined(ESP8266)
#define SDA_LOW()   (GPES = (1 << sda)) //Enable SDA (becomes output and since GPO is 0 for the pin, it will pull the line low)
#define SDA_HIGH()  (GPEC = (1 << sda)) //Disable SDA (becomes input and since it has pullup it will go high)
#define SDA_READ()  ((GPI & (1 << sda)) != 0)
#define SCL_LOW()   (GPES = (1 << scl))
#define SCL_HIGH()  (GPEC = (1 << scl))
#define SCL_READ()  ((GPI & (1 << scl)) != 0)
#elif defined (ESP32)
#define SDA_LOW()   (GPIO.enable_w1ts = sda_mask)
#define SDA_HIGH()  (GPIO.enable_w1tc = sda_mask)
#define SDA_READ()  ((GPIO.in & sda_mask) != 0)
#define SCL_LOW()   (GPIO.enable_w1ts = scl_mask)
#define SCL_HIGH()  (GPIO.enable_w1tc = scl_mask)
#define SCL_READ()  ((GPIO.in & scl_mask) != 0)
#endif

#ifndef FCPU80
#define FCPU80 80000000L
#endif

#if F_CPU == FCPU80
#define TWI_CLOCK_STRETCH_MULTIPLIER 3
#else
#define TWI_CLOCK_STRETCH_MULTIPLIER 6
#endif

size_t ICACHE_RAM_ATTR UrsTwi::requestFrom(uint8_t address, size_t size, bool sendStop) {
   if (size > BUFFER_LENGTH) {
      size = BUFFER_LENGTH;
   }
   size_t read = (readFrom(address, rxBuffer, size, sendStop) == 0) ? size : 0;
   rxBufferIndex = 0;
   rxBufferLength = read;
   return read;
}


bool ICACHE_RAM_ATTR UrsTwi::writeStart(void) {
   SCL_HIGH();
   SDA_HIGH();
   if (SDA_READ() == 0) return false;
   twi_delay(dCount);
   SDA_LOW();
   twi_delay(dCount);
   return true;
}

bool ICACHE_RAM_ATTR UrsTwi::writeStop(void) {
   uint32_t i = 0;
   SCL_LOW();
   SDA_LOW();
   twi_delay(dCount);
   SCL_HIGH();
   while (SCL_READ() == 0 && (i++) < clockStretchLimit); // Clock stretching
   twi_delay(dCount);
   SDA_HIGH();
   twi_delay(dCount);

   return true;
}

bool ICACHE_RAM_ATTR UrsTwi::writeByte(uint8_t byte) {
   unsigned char bit;
   for (bit = 0; bit < 8; bit++) {
      writeBit(byte & 0x80);
      byte <<= 1;
   }
   return !readBit();//NACK/ACK
}

uint8_t ICACHE_RAM_ATTR UrsTwi::readByte(bool nack) {
   unsigned char byte = 0;
   unsigned char bit;
   for (bit = 0; bit < 8; bit++) byte = (byte << 1) | readBit();
   writeBit(nack);
   return byte;
}

bool ICACHE_RAM_ATTR UrsTwi::readBit(void) {
   uint32_t i = 0;
   SCL_LOW();
   SDA_HIGH();
   twi_delay(dCount + 2);
   SCL_HIGH();
   while (SCL_READ() == 0 && (i++) < clockStretchLimit);// Clock stretching
   bool bit = SDA_READ();
   twi_delay(dCount);
   return bit;
}

bool ICACHE_RAM_ATTR UrsTwi::writeBit(bool bit) {
   uint32_t i = 0;
   SCL_LOW();
   if (bit) SDA_HIGH();
   else SDA_LOW();
   twi_delay(dCount + 1);
   SCL_HIGH();
   while (SCL_READ() == 0 && (i++) < clockStretchLimit);// Clock stretching
   twi_delay(dCount);
   return true;
}

#if defined(ESP8266)
void ICACHE_RAM_ATTR UrsTwi::twi_delay(uint8_t v) {
   unsigned int i;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
   unsigned int reg;
   for (i = 0; i < v; i++) reg = GPI;
#pragma GCC diagnostic pop
}
#else // ESP32
void ICACHE_RAM_ATTR UrsTwi::twi_delay(uint16_t v) {
   uint32_t start = ESP.getCycleCount();
   while ((ESP.getCycleCount() - start) < v);
}
#endif

size_t ICACHE_RAM_ATTR UrsTwi::readFrom(uint8_t address, uint8_t* buf, size_t len, bool sendStop) {
   unsigned int i;
   if (!writeStart()) return 4;//line busy
   if (!writeByte(((address << 1) | 1) & 0xFF)) {
      if (sendStop) writeStop();
      return 2;//received NACK on transmit of address
   }
   for (i = 0; i < (len - 1); i++) buf[i] = readByte(false);
   buf[len - 1] = readByte(true);
   if (sendStop) writeStop();
   i = 0;
   while (SDA_READ() == 0 && (i++) < 10) {
      SCL_LOW();
      twi_delay(dCount);
      SCL_HIGH();
      twi_delay(dCount);
   }
   return 0;
}


uint8_t ICACHE_RAM_ATTR UrsTwi::requestFrom(uint8_t address, uint8_t quantity) {
   return requestFrom(address, static_cast<size_t>(quantity), true);
}

UrsTwi::UrsTwi(uint8_t sda, uint8_t scl, bool initPins) {
   this->sda = sda;
   this->scl = scl;
   sda_mask = 0;
   scl_mask = 0;
   if (sda < 32) {
      sda_mask = ((uint32_t)1 << sda);
   }

   if (scl < 32) {
      scl_mask = ((uint32_t)1 << scl);
   }

   if (initPins) {
      pinMode(sda, INPUT_PULLUP);
      pinMode(scl, INPUT_PULLUP);
   }
   setClock(100000);
   setClockStretchLimit(230); // default value is 230 uS
   flush();
}

void UrsTwi::begin() {
   pinMode(sda, INPUT_PULLUP);
   pinMode(scl, INPUT_PULLUP);
   flush();
}

void UrsTwi::begin(int sda, int scl) {
   this->sda = sda;
   this->scl = scl;
   sda_mask = 0;
   scl_mask = 0;
   if (sda < 32) {
      sda_mask = ((uint32_t)1 << sda);
   }

   if (scl < 32) {
      scl_mask = ((uint32_t)1 << scl);
   }

   begin();
}

void UrsTwi::setClockStretchLimit(uint32_t limit) {
   clockStretchLimit = limit * TWI_CLOCK_STRETCH_MULTIPLIER;
}

void UrsTwi::setClock(uint32_t freq) {
#if defined(ESP8266)
#if F_CPU == FCPU80
   if (freq <= 100000) dCount = 19;//about 100KHz
   else if (freq <= 200000) dCount = 8;//about 200KHz
   else if (freq <= 300000) dCount = 3;//about 300KHz
   else if (freq <= 400000) dCount = 1;//about 400KHz
   else dCount = 1;//about 400KHz
#else
#error CPU Frequency not supported
#endif
#elif defined(ESP32)
#if F_CPU == 240000000L
   if (freq <= 100000) dCount = 1200;//about 100KHz
   else if (freq <= 200000) dCount = 600;//about 200KHz
   else if (freq <= 300000) dCount = 400;//about 300KHz
   else if (freq <= 400000) dCount = 300;//about 400KHz
   else dCount = 300;//about 400KHz
#else
#error CPU Frequency not supported
#endif
#endif
}

void ICACHE_RAM_ATTR UrsTwi::beginTransmission(uint8_t address) {
   transmitting = 1;
   txAddress = address;
   txBufferIndex = 0;
   txBufferLength = 0;
}

uint8_t ICACHE_RAM_ATTR UrsTwi::endTransmission(uint8_t sendStop) {
   int8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, sendStop);
   txBufferIndex = 0;
   txBufferLength = 0;
   transmitting = 0;
   return ret;
}


size_t ICACHE_RAM_ATTR UrsTwi::write(uint8_t data) {
   if (transmitting) {
      if (txBufferLength >= BUFFER_LENGTH) {
         setWriteError();
         return 0;
      }
      txBuffer[txBufferIndex] = data;
      ++txBufferIndex;
      txBufferLength = txBufferIndex;
      return 1;
   }

   return 0;
}
uint8_t ICACHE_RAM_ATTR UrsTwi::twi_writeTo(uint8_t address, uint8_t * buf, unsigned int len, bool sendStop) {
   unsigned int i;
   if (!writeStart()) return 4;//line busy
   if (!writeByte(((address << 1) | 0) & 0xFF)) {
      if (sendStop) writeStop();
      return 2; //received NACK on transmit of address
   }
   for (i = 0; i < len; i++) {
      if (!writeByte(buf[i])) {
         if (sendStop) writeStop();
         return 3;//received NACK on transmit of data
      }
   }
   if (sendStop) writeStop();
   i = 0;
   while (SDA_READ() == 0 && (i++) < 10) {
      SCL_LOW();
      twi_delay(dCount);
      SCL_HIGH();
      twi_delay(dCount);
   }
   return 0;
}
int ICACHE_RAM_ATTR UrsTwi::read(void) {
   int value = -1;
   if (rxBufferIndex < rxBufferLength) {
      value = rxBuffer[rxBufferIndex];
      ++rxBufferIndex;
   }
   return value;
}

int UrsTwi::peek(void) {
   int value = -1;
   if (rxBufferIndex < rxBufferLength) {
      value = rxBuffer[rxBufferIndex];
   }
   return value;
}

int UrsTwi::available(void) {
   return rxBufferLength - rxBufferIndex;
}

void UrsTwi::flush(void) {
   rxBufferIndex = 0;
   rxBufferLength = 0;
   txBufferIndex = 0;
   txBufferLength = 0;
}

size_t UrsTwi::write(const uint8_t *data, size_t quantity) {
   if (transmitting) {
      for (size_t i = 0; i < quantity; ++i) {
         if (!write(data[i])) return i;
      }
      return quantity;
   }

   return 0;
}
TwiStatus UrsTwi::getStatus() {
   if (SCL_READ() == 0)     return TwiStatus::SCL_HELD_LOW;          //SCL held low by another device, no procedure available to recover
   int clockCount = 20;

   while (SDA_READ() == 0 && clockCount > 0) {                          //if SDA low, read the bits slaves have to sent to a max
      readBit();
      if (SCL_READ() == 0) return TwiStatus::SCL_HELD_LOW_AFTER_READ;  //I2C bus error. SCL held low beyond slave clock stretch time
   }

   if (SDA_READ() == 0)   return TwiStatus::SDA_HELD_LOW;       		   //I2C bus error. SDA line held low by slave/another_master after n bits.

   if (!writeStart())     return TwiStatus::SDA_HELD_LOW_AFTER_INIT;  //line busy. SDA again held low by another device. 2nd master?
   else                   return TwiStatus::OK;       				         //all ok 
}

#endif