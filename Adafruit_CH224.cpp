/**************************************************************************/
/*!
  @file Adafruit_CH224.cpp

  @mainpage Adafruit CH224 Arduino Library

  @section intro Introduction

  Arduino library for the WCH CH224A and CH224Q I2C-controlled USB Power
  Delivery sink controllers.

  Adafruit invests time and resources providing this open source code.
  Please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  @section author Author

  Written by Limor Fried for Adafruit Industries.

  @section license License

  MIT license. See LICENSE for more information.
*/
/**************************************************************************/

#include "Adafruit_CH224.h"

/** Voltage control register value that selects PPS mode. */
#define CH224_PPS_MODE 6
/** Voltage control register value that selects AVS mode. */
#define CH224_AVS_MODE 7

/**************************************************************************/
/*!
    @brief Creates a new CH224 driver object.
*/
/**************************************************************************/
Adafruit_CH224::Adafruit_CH224() {}

/**************************************************************************/
/*!
    @brief Initializes the I2C connection to the CH224.
    @param i2c_addr The 7-bit I2C address. Use 0x23 for CH224A or 0x22 for
    CH224Q.
    @param theWire Pointer to the TwoWire interface to use.
    @return True when the CH224 acknowledges on the I2C bus.
*/
/**************************************************************************/
bool Adafruit_CH224::begin(uint8_t i2c_addr, TwoWire* theWire) {
  if (i2c_dev) {
    delete i2c_dev;
  }

  i2c_dev = new Adafruit_I2CDevice(i2c_addr, theWire);
  return i2c_dev->begin();
}

/**************************************************************************/
/*!
    @brief Reads the CH224 protocol status register.
    @param status Pointer that receives the status bits.
    @return True when the register was read successfully.
*/
/**************************************************************************/
bool Adafruit_CH224::getStatus(uint8_t* status) {
  return readRegister(CH224_STATUS_REGISTER, status);
}

/**************************************************************************/
/*!
    @brief Checks whether a protocol status bit is active.
    @param protocol Protocol bit to check.
    @return True when the protocol bit is set. A failed I2C read also returns
    false; use getStatus() when the distinction is important.
*/
/**************************************************************************/
bool Adafruit_CH224::isProtocolActive(ch224_protocol_t protocol) {
  uint8_t status = 0;
  if (!getStatus(&status)) {
    return false;
  }

  return (status & (uint8_t)protocol) != 0;
}

/**************************************************************************/
/*!
    @brief Requests one of the CH224 fixed USB PD voltages.
    @param voltage Fixed voltage selection.
    @return True when the request was written successfully.
*/
/**************************************************************************/
bool Adafruit_CH224::setVoltage(ch224_voltage_t voltage) {
  uint8_t setting = (uint8_t)voltage;
  if (setting > CH224_VOLTAGE_28V) {
    return false;
  }

  return writeRegister(CH224_VOLTAGE_REGISTER, setting);
}

/**************************************************************************/
/*!
    @brief Requests a USB PD Programmable Power Supply voltage.
    @param volts Requested voltage in volts, from 3.3 V through 21.0 V. The
    value is rounded to the nearest 0.1 V.
    @return True when the PPS voltage and mode were written successfully.
*/
/**************************************************************************/
bool Adafruit_CH224::setPPSVoltage(float volts) {
  if (isnan(volts) || volts < CH224_PPS_MIN_VOLTAGE ||
      volts > CH224_PPS_MAX_VOLTAGE) {
    return false;
  }

  uint8_t setting = (uint8_t)(volts * 10.0F + 0.5F);
  if (!writeRegister(CH224_PPS_REGISTER, setting)) {
    return false;
  }

  return writeRegister(CH224_VOLTAGE_REGISTER, CH224_PPS_MODE);
}

/**************************************************************************/
/*!
    @brief Requests an Adjustable Voltage Supply voltage.
    @param volts Requested voltage in volts, from 5.0 V through 28.0 V. The
    value is rounded to the nearest 0.025 V.
    @return True when the AVS voltage and mode were written successfully.
*/
/**************************************************************************/
bool Adafruit_CH224::setAVSVoltage(float volts) {
  if (isnan(volts) || volts < CH224_AVS_MIN_VOLTAGE ||
      volts > CH224_AVS_MAX_VOLTAGE) {
    return false;
  }

  uint16_t setting = (uint16_t)(volts * 40.0F + 0.5F);
  uint8_t highByte = (uint8_t)(setting >> 8);
  uint8_t lowByte = (uint8_t)(setting & 0xFF);

  if (!writeRegister(CH224_AVS_HIGH_REGISTER, highByte)) {
    return false;
  }
  if (!writeRegister(CH224_AVS_LOW_REGISTER, lowByte)) {
    return false;
  }

  return writeRegister(CH224_VOLTAGE_REGISTER, CH224_AVS_MODE);
}

/**************************************************************************/
/*!
    @brief Reads the maximum current available for the negotiated PD voltage.
    @param amps Pointer that receives the current in amperes.
    @return True when the current register was read successfully.
*/
/**************************************************************************/
bool Adafruit_CH224::getMaxCurrent(float* amps) {
  if (!amps) {
    return false;
  }

  uint8_t setting = 0;
  if (!readRegister(CH224_CURRENT_REGISTER, &setting)) {
    return false;
  }

  *amps = setting * 0.05F;
  return true;
}

/**************************************************************************/
/*!
    @brief Checks whether the USB PD source advertises a fixed voltage.
    @param voltage Fixed voltage selection to find.
    @return True when the requested fixed voltage appears in the source
    capabilities. A failed I2C read also returns false.
*/
/**************************************************************************/
bool Adafruit_CH224::isVoltageAvailable(ch224_voltage_t voltage) {
  float amps = 0.0F;
  return findFixedSourceCapability(voltage, &amps);
}

/**************************************************************************/
/*!
    @brief Reads the advertised current for a fixed USB PD source voltage.
    @param voltage Fixed voltage selection to find.
    @param amps Pointer that receives the advertised current in amperes.
    @return True when the requested fixed voltage appears in the source
    capabilities and its current was returned.
*/
/**************************************************************************/
bool Adafruit_CH224::getAvailableCurrent(ch224_voltage_t voltage, float* amps) {
  return findFixedSourceCapability(voltage, amps);
}

/**************************************************************************/
/*!
    @brief Finds a fixed supply PDO in the USB PD Source_Capabilities message.
    @param voltage Fixed voltage selection to find.
    @param amps Pointer that receives the advertised current in amperes.
    @return True when the requested fixed PDO was found.
*/
/**************************************************************************/
bool Adafruit_CH224::findFixedSourceCapability(ch224_voltage_t voltage,
                                               float* amps) {
  if (!amps) {
    return false;
  }

  uint16_t targetVoltageUnits = 0;
  switch (voltage) {
    case CH224_VOLTAGE_5V:
      targetVoltageUnits = 100;
      break;
    case CH224_VOLTAGE_9V:
      targetVoltageUnits = 180;
      break;
    case CH224_VOLTAGE_12V:
      targetVoltageUnits = 240;
      break;
    case CH224_VOLTAGE_15V:
      targetVoltageUnits = 300;
      break;
    case CH224_VOLTAGE_20V:
      targetVoltageUnits = 400;
      break;
    case CH224_VOLTAGE_28V:
      targetVoltageUnits = 560;
      break;
    default:
      return false;
  }

  uint8_t powerData[CH224_POWER_DATA_LENGTH];
  if (!readPowerData(powerData)) {
    return false;
  }

  uint16_t messageHeader = powerData[0] | ((uint16_t)powerData[1] << 8);
  uint8_t objectCount = (messageHeader >> 12) & 0x07;
  if (objectCount == 0 || (2 + objectCount * 4) > CH224_POWER_DATA_LENGTH) {
    return false;
  }

  for (uint8_t index = 0; index < objectCount; index++) {
    uint8_t offset = 2 + index * 4;
    uint32_t pdo = powerData[offset];
    pdo |= (uint32_t)powerData[offset + 1] << 8;
    pdo |= (uint32_t)powerData[offset + 2] << 16;
    pdo |= (uint32_t)powerData[offset + 3] << 24;

    uint8_t supplyType = (pdo >> 30) & 0x03;
    if (supplyType != 0) {
      continue;
    }

    uint16_t voltageUnits = (pdo >> 10) & 0x03FF;
    if (voltageUnits == targetVoltageUnits) {
      uint16_t currentUnits = pdo & 0x03FF;
      *amps = currentUnits * 0.01F;
      return true;
    }
  }

  return false;
}

/**************************************************************************/
/*!
    @brief Reads raw USB PD source capability bytes.
    @param data Buffer that receives the source capability data.
    @param length Number of bytes to read, from 1 through 48.
    @return True when every requested byte was read successfully.
*/
/**************************************************************************/
bool Adafruit_CH224::readPowerData(uint8_t* data, size_t length) {
  if (!data || length == 0 || length > CH224_POWER_DATA_LENGTH) {
    return false;
  }

  for (size_t index = 0; index < length; index++) {
    uint8_t address = CH224_POWER_DATA_REGISTER + (uint8_t)index;
    if (!readRegister(address, &data[index])) {
      return false;
    }
  }

  return true;
}

/**************************************************************************/
/*!
    @brief Reads one byte from a CH224 register.
    @param address Register address.
    @param value Pointer that receives the register value.
    @return True when the register was read successfully.
*/
/**************************************************************************/
bool Adafruit_CH224::readRegister(uint8_t address, uint8_t* value) {
  if (!i2c_dev || !value) {
    return false;
  }

  Adafruit_BusIO_Register deviceRegister(i2c_dev, address);
  return deviceRegister.read(value, 1);
}

/**************************************************************************/
/*!
    @brief Writes one byte to a CH224 register.
    @param address Register address.
    @param value Register value to write.
    @return True when the register was written successfully.
*/
/**************************************************************************/
bool Adafruit_CH224::writeRegister(uint8_t address, uint8_t value) {
  if (!i2c_dev) {
    return false;
  }

  Adafruit_BusIO_Register deviceRegister(i2c_dev, address);
  return deviceRegister.write(value);
}
