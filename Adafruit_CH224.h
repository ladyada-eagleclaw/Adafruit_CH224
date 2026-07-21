/**************************************************************************/
/*!
  @file Adafruit_CH224.h

  Arduino library for the WCH CH224A and CH224Q USB PD sink controllers.

  Adafruit invests time and resources providing this open source code.
  Please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried for Adafruit Industries.

  MIT license. See LICENSE for more information.
*/
/**************************************************************************/

#ifndef ADAFRUIT_CH224_H
#define ADAFRUIT_CH224_H

#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Arduino.h>
#include <Wire.h>

/** Default I2C address for the CH224A. */
#define CH224A_I2CADDR_DEFAULT 0x23
/** Default I2C address for the CH224Q. */
#define CH224Q_I2CADDR_DEFAULT 0x22
/** Default address used by begin(). */
#define CH224_I2CADDR_DEFAULT CH224A_I2CADDR_DEFAULT

/** I2C status register. */
#define CH224_STATUS_REGISTER 0x09
/** Voltage control register. */
#define CH224_VOLTAGE_REGISTER 0x0A
/** Maximum current register. */
#define CH224_CURRENT_REGISTER 0x50
/** AVS voltage high-byte register. */
#define CH224_AVS_HIGH_REGISTER 0x51
/** AVS voltage low-byte register. */
#define CH224_AVS_LOW_REGISTER 0x52
/** PPS voltage register. */
#define CH224_PPS_REGISTER 0x53
/** First register containing raw PD source capability data. */
#define CH224_POWER_DATA_REGISTER 0x60
/** Number of raw PD source capability bytes. */
#define CH224_POWER_DATA_LENGTH 48

/** Lowest supported PPS request voltage. */
#define CH224_PPS_MIN_VOLTAGE 3.3F
/** Highest supported PPS request voltage. */
#define CH224_PPS_MAX_VOLTAGE 21.0F
/** Lowest supported AVS request voltage. */
#define CH224_AVS_MIN_VOLTAGE 5.0F
/** Highest supported AVS request voltage. */
#define CH224_AVS_MAX_VOLTAGE 28.0F

/** Fixed voltage requests supported by the CH224. */
typedef enum {
  CH224_VOLTAGE_5V = 0,  ///< Request 5 V.
  CH224_VOLTAGE_9V = 1,  ///< Request 9 V.
  CH224_VOLTAGE_12V = 2, ///< Request 12 V.
  CH224_VOLTAGE_15V = 3, ///< Request 15 V.
  CH224_VOLTAGE_20V = 4, ///< Request 20 V.
  CH224_VOLTAGE_28V = 5  ///< Request 28 V EPR.
} ch224_voltage_t;

/** Protocol status bits reported by the CH224. */
typedef enum {
  CH224_PROTOCOL_BC = 0x01,            ///< BC protocol is active.
  CH224_PROTOCOL_QC2 = 0x02,           ///< QC 2.0 protocol is active.
  CH224_PROTOCOL_QC3 = 0x04,           ///< QC 3.0 protocol is active.
  CH224_PROTOCOL_PD = 0x08,            ///< USB PD protocol is active.
  CH224_PROTOCOL_EPR = 0x10,           ///< EPR contract is active.
  CH224_PROTOCOL_EPR_AVAILABLE = 0x20, ///< EPR source is available.
  CH224_PROTOCOL_AVS_AVAILABLE = 0x40  ///< AVS source is available.
} ch224_protocol_t;

/**************************************************************************/
/*!
    @brief Driver for the WCH CH224A and CH224Q USB PD sink controllers.
*/
/**************************************************************************/
class Adafruit_CH224 {
 public:
  Adafruit_CH224();

  bool begin(uint8_t i2c_addr = CH224_I2CADDR_DEFAULT,
             TwoWire* theWire = &Wire);

  bool getStatus(uint8_t* status);
  bool isProtocolActive(ch224_protocol_t protocol);
  bool setVoltage(ch224_voltage_t voltage);
  bool setPPSVoltage(float volts);
  bool setAVSVoltage(float volts);
  bool getMaxCurrent(float* amps);
  bool isPPSAvailable();
  bool isVoltageAvailable(ch224_voltage_t voltage);
  bool getAvailableCurrent(ch224_voltage_t voltage, float* amps);
  bool readPowerData(uint8_t* data, size_t length = CH224_POWER_DATA_LENGTH);

 private:
  bool readSourceCapabilities(uint8_t* powerData, uint8_t* objectCount);
  bool findFixedSourceCapability(ch224_voltage_t voltage, float* amps);
  bool readRegister(uint8_t address, uint8_t* value);
  bool writeRegister(uint8_t address, uint8_t value);

  Adafruit_I2CDevice* i2c_dev = NULL;
};

#endif
