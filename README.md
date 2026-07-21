# Adafruit CH224 Library [![Build Status](https://github.com/adafruit/Adafruit_CH224/workflows/Arduino%20Library%20CI/badge.svg)](https://github.com/adafruit/Adafruit_CH224/actions)[![Documentation](https://github.com/adafruit/ci-arduino/blob/master/assets/doxygen_badge.svg)](https://adafruit.github.io/Adafruit_CH224/html/index.html)

This is the Adafruit Arduino library for the WCH CH224A and CH224Q I2C-controlled USB Power Delivery sink controllers.

Both variants can request fixed USB PD voltages and report active protocols, current, and source capabilities. The CH224Q also supports PPS and AVS voltage requests.

Call `ch224.begin()` to probe the official CH224A address (`0x23`) first, then the CH224Q address (`0x22`). Some CH224A chips acknowledge both addresses and there is no silicon ID register, so a response at `0x23` is conservatively reported as an A. A response at only `0x22` is reported as a Q.

Use the `Adafruit_CH224` base class with `getVariant()` and `getI2CAddress()` when the connected variant is not known. Use the `Adafruit_CH224A` or `Adafruit_CH224Q` subclass when the board's silicon is known. The base class contains the common fixed-PD and status API; programmable `setPPSVoltage()` and `setAVSVoltage()` calls exist only on `Adafruit_CH224Q` and require a compatible USB PD source.

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
