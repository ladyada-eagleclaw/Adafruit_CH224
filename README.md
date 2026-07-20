# Adafruit CH224 Library [![Build Status](https://github.com/adafruit/Adafruit_CH224/workflows/Arduino%20Library%20CI/badge.svg)](https://github.com/adafruit/Adafruit_CH224/actions)[![Documentation](https://github.com/adafruit/ci-arduino/blob/master/assets/doxygen_badge.svg)](https://adafruit.github.io/Adafruit_CH224/html/index.html)

This is the Adafruit Arduino library for the WCH CH224A and CH224Q I2C-controlled USB Power Delivery sink controllers.

The CH224 can request fixed USB PD voltages, PPS voltages, and AVS voltages. It can also report the negotiated protocol, maximum available current, and raw USB PD source capability data.

The CH224A uses I2C address `0x23`. The CH224Q uses I2C address `0x22`.

Adafruit invests time and resources providing this open source code. Please support Adafruit and open-source hardware by purchasing products from Adafruit!

## Installation

Use the Arduino Library Manager to install "Adafruit CH224" and its dependencies.

## Dependencies

* [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO)

## Datasheet

* [WCH CH224 Datasheet](https://www.wch-ic.com/downloads/CH224DS1_PDF.html)

## Documentation and Doxygen

Documentation is produced by Doxygen. Examples are included in the `examples` folder.

## About This Driver

Written by Limor Fried for Adafruit Industries.

MIT license. See `LICENSE` for more information.

All text above must be included in any redistribution.
