# SafewaterFirmware
Safewater UV-C Rainwater Sanitisation Controller for Arduino

## Documentation

Doxygen Documentation at [Github Pages](https://acksen.github.io/SafewaterFirmware/docs/html)

## Introduction

Documentation for the firmware of the UV-C Rainwater Sanitisation Controller, designed by Acksen Ltd (http://www.acksen.com https://www.aquaductor.com) for the Safewater Research Initiative (https://www.safewater-research.com/).

## Description

This is the firmware for the UV-C Rainwater Sanitisation Controller, designed by Acksen Ltd
(http://www.acksen.com https://www.aquaductor.com) for the Safewater Research 
Initiative (https://www.safewater-research.com/).

This firmware is designed around the Arduino Uno SMD, or ATMega328P Microcontroller.
This firmware can be easily ported to other 8-bit Atmel Microcontrollers.

The Safewater Ultraviolet Water Purification Controller is designed to automatically provide safe,
sanitised and potable water for you and your family.

The controller automatically takes untreated water from your storage tank and treats it, to ensure that you
have a steady supply of clean water. No manual intervention is needed – the controller will function by
itself and notify you via an alarm if any problems occur.

This is designed for a system which:
- Collects rainwater into an untreated water tank.
- Uses a UV-C sanitisation lamp to disinfect the collected rainwater and ensure that it is safe for
human consumption.  A filter is also used to remove physical contaminants.
- Disinfected water is placed into a treated water tank.
- When the treated water tank reaches a lower bound float switch (Switch T1), the controller automatically refills it using water from the treated tank.
- When the upper bound float switch on the treated tank is triggered (Switch T2), disinfection and pumping stops.
- The untreated water tank features a float switch at the base (Switch S) to confirm that there is rainwater to pump and disinfect.

The controller features a number of safety features, such as:
- Warm-up period for the UV-C Lamp to ensure it has reached operating temperature
- Automatically ends the pumping sequence after a timeout period (based around the tank size/pump rate).
This is presently set for the Safewater systems deployed to Mexico and Colombia, and is altered using a external jumper on an input line.
- Override button to immediately start the disinfection sequence and provide water, regardless of present state.
- Uses Hardware Watchdog Timer on the Microcontroller to reboot if normal firmware operation is interrupted
- Checks the UV-C Lamp operating current while disinfecting, to ensure that it is operating correctly.
- Provides user alarms for UV-C Lamp Fault, Maximum Runtime Exceeded (due to e.g. Water Pump Fault), Treated Tank Float Switch Fault 
and Source Tank Empty Fault.
- Anti-cycling management
- Automatic retry system in case of transient faults

## Circuit

- Firmware is designed to operate on an ATMega328P Microcontroller, 32-pin 
Surface Mount version.
- For PCB Layout and Schematic, please see the accompanying SafewaterHardware repository on Github.
(https://www.github.com/Acksen/SafewaterHardware)
- Further information is available on the Aquaductor website for the commercial implementation of the controller,
including instruction manuals, Wiring Diagrams, Troubleshooting, Installation & Faults Overview.
(https://www.aquaductor.com/resources)

## Dependencies

- AcksenButton (https://github.com/acksen/AcksenButton) - Processes button inputs to the system.
- AcksenIntEEPROM (https://github.com/acksen/AcksenIntEEPROM) - Used to store variables and data in the internal EEPROM in the microcontroller.
- AcksenPump (https://github.com/acksen/AcksenPump) - Provides control for the Water Pump used with the Safewater Tank System.
- AcksenUtils (https://github.com/acksen/AcksenUtils) - Miscellaneous functions.
- Flash (https://github.com/schinken/Flash) - Used to store text strings in Program Memory rather than RAM.
- Arduino Time Library (https://github.com/PaulStoffregen/Time) - Provide time-related variable types and functions.

Requires Arduino IDE v1.8.10 or greater.

## Author
Written by Richard Phillips for Acksen Ltd.

## License
This source file is licenced using the GNU General Public License version 3.

Copyright (c) 2020, 2022 Acksen Ltd, All rights reserved.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.