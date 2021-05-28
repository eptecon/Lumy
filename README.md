Lumy: Smart Connected LED Lighting System
======

Lumy is a smart connected LED lighting system comprising multiple interconnectable modules.


### Hardware

Lumy hardware comprises following modules:

- LumyCtrl Module: power supply, main microcontroller (ATMega328P) interface to WiFi module and interface to LED driver
- LumyDrv Module: 3x TI LM3404 LED driver with 1.2A output each
- LumyLight Module: LED carrier board, can be populated with up to 12 LEDs arranged in 3 separate strings with up to 4 LEDs each
- LumySens Module: ambient light sensor TSL2591 and can be populated with other 


### Firmware

Lumy firmware is a simple bare metal C program running on the ATMega328P. It controls the brightness of connected LEDs over PWM inputs of the LED drivers
depending on sensor or user input. The user input is provided over a simple android app, which sends the data wirelessly over WiFi to the Lumy system.


---
