# NoSpark firmware for OpenEVSE v3

## Disclaimer
**THIS FIRMWARE WILL SET YOUR HOUSE ON FIRE, ANNIHILATE YOUR ELECTRIC VEHICLE
AND ELECTRECUTE YOUR CAT. IN OTHER WORDS: DO NOT USE THIS FIRMWARE** (yet)

If you *DO* use this firmware, the NoSpark authors accept no responsiblity for any
damages done or caused by this firmware. **USE AT YOUR OWN RISK**.

## About
NoSpark is a completely new firmware written for the OpenEVSE controller board.
It has been written from the ground up using AVR C++11. NoSpark aims to be a drop-in
replacement for the OpenEVSE firmware.

## Features
* Event driven charge controller
* Serial port console and API (simultaneuously)
* Charge scheduling
* Per-charge kWh limit
* Large font LCD clock display
* Custom RGB LCD, real-time clock and I2C drivers

## Design goals
* Modern C++
* Modular architecture
* Easy to read/maintain/extend code

## Compatibility
Currently NoSpark supports *only OpenEVSE v3 and v2.5* boards. Also, only the
RGB LCD and RTC backpack from OpenEVSE are currently supported. Compatibility
for other controllers, LCD etc. can be added in the future.

## Compiling / uploading
Simply run _make_ to compile (or _ninja_).
_make upload_ (_ninja upload_) will upload the hex file to your device using a
usbtiny based programmer. The programmer and upload protocol can be set in the
top level Makefile.inc (or rules.ninja).

## Recent revision history
* v0.15 - Continuous relay monitoring
* v0.14 - Added DC relay support (untested); Ninja build support
* v0.13 - Interrupt driven ADC sampling
* v0.12 - Cost per kWh added
* v0.11 - Added suspend state pin; LCD sleep mode fixed
* v0.10 - Charge timer added
* ...

