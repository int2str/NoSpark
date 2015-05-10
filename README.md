# NoSpark firmware for OpenEVSE v3

## Disclaimer
**THIS FIRMWARE WILL SET YOUR HOUSE ON FIRE, ANNIHILATE YOUR ELECTRIC VEHICLE
AND ELECTRECUTE YOUR CAT. IN OTHER WORDS: DO NOT USE THIS FIRMWARE** (yet)

This is *NOT FUNCTIONAL* at this point and intended for development purposes
only.

## About
NoSpark is a completely new firmware written for the OpenEVSE v3. It has been
written from the ground up using AVR C++11. NoSpark aims to be a drop-in
replacement for the OpenEVSE firmware.

## Features
* Event driven charge controller
* Serial port console
* RGB LCD driver
* RTC driver

## Design goals
* Modern C++
* Modular architecture
* Easy to read/maintain/extend code

## Compatibility
Currently NoSpark supports *only OpenEVSE v3* boards. Also, only the RGB LED
and RTC backpack from OpenEVSE are currently supported. Compatibility for other
controllers, LCD etc. can be added in the future.

## Compiling / uploading

Simply run _make_ to compile.
_make upload_ will upload the hex file to your device using a usbtiny based
programmer. The programmer and upload protocol can be set in the top level
Makefile.inc.

## Revision history
* v0.09 - Important GFI fixes; ability to limit per-charge energy/suspend charge
* v0.08 - Serial API implementation
* v0.07 - Time & date settings UI; large time display; settable sleep mode; German UI
* v0.06 - Changed statistics roll-over; compile size improvements
* v0.05 - Added kWh statistics
* v0.04 - End-of-charge display and critical fixes
* v0.03 - Added charge timer and current measurement
* v0.02 - First charge milestone
* v0.01 - Initial development release

