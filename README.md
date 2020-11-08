# PoolReader - Board

This repo contains sources for the hardware board and the firmware that goes with it.

## Introduction

<Comming Soon: Photo of the real product>
<Comming Soon: Photo of the PCB>

pH and ORP mesurements are analog signals comming from probes. In my case the probes are far away (20 meters) from where they will be used (ESP8266). It is impossible to have a good analog signal from the pH or ORP probe after 20 meters of cable. One solution is to put the ADC as close as possible to the probe and then use a more robust signal such as digital signal. 

They are several boards available in the market but they do not fit my requirements.

1. A single board to aquire at least 2 probes (1 pH probe and 1 ORP probe)
2. Limited size: max 5 cm x 5cm for the board
3. Runs on 5V
4. Data is digitaly accessible over simple and effective protocol (for example 1-Wire)
5. Stay cheaper than commercialy availaible boards

## Usefull ressources

Documents and Websites:

- ATTINY85 Fuse calculator: http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny85
- ATTINY85 Reading fuses: https://rajeshkovvuri.wordpress.com/2012/06/01/usbasp-avrdude-reading-fuse-bits/
- ATTINYCore: https://github.com/SpenceKonde/ATTinyCore
- High level programmer for ATTINY85: https://sites.google.com/site/wayneholder/attiny-fuse-reset
- Texas Instrument AN-1852, Designing with pH electrodes: https://www.ti.com/lit/an/snoa529a/snoa529a.pdf
- Temperature Compensation for pH mesurments: https://www.hach.com/asset-get.download.jsa?id=17525673904
- Example of pH mesurement circuit and explanations: https://www.e-tinkers.com/2019/11/measure-ph-with-a-low-cost-arduino-ph-sensor-board/
- Usable instruction to setup the Arduino environment: https://makersportal.com/blog/2018/5/19/attiny85-arduino-board-how-to-flash-the-arduino-bootloader-and-run-a-simple-sketch
- Digispark ATTNIY85 board silk screen: https://i.stack.imgur.com/smi4Y.jpg
- Digispark ATTINY85 board pcb + schematic: https://i.imgur.com/GQbOyGP.png


Arduino / CPP libraries:

/!\ Libraries are included as submodules in the firmware lib folder /!\ 

- High Level Fuse ReProgrammer Sketch with modified timings: https://github.com/Willena/Hardware-PoolReader/AT85-HLFuseProgrammer
- PoolReader Device and Client library: https://github.com/Willena/Aduino-PoolReader-Library
- OneWire Library with internal pullup support: https://github.com/Willena/OneWire
- OneWireHub Library for OneWire device support: https://github.com/orgua/OneWireHub
- DallasTemperatureLibrary: https://github.com/milesburton/Arduino-Temperature-Control-Library
- HCSR04 Library: https://github.com/Martinsos/arduino-lib-hc-sr04

## PoolReader Board

### Build the board 

Complete Schematics, PCB and Gerbers are available in the BoardSource folder of this repository
All resistor and capacitor are of the 0402 size. A csv file with the Bill Of Material is also available.

Here is what you will need:
- 2 TLC4502 OpAmp chip from TI
- 1 ATTINY85
- 1 LM4140 Voltage Reference
- 1 DS18B20 chip to get ambiant temperature
- Some resistors and capacitors

Be carefull, R1, R2 must be open when programming the ATTiny in order to avoid sending random signals to the opamps. 

R3 and R4 are alternatives for the generation of the 2.048V voltage source. R3,R4 are a voltage divider and therefore depends on the VCC at the input, which is not the case if using the LM4140 chip.

### Schematic explanations

There are three main parts: 

1. pH Frontend
2. ORP Frontend
3. ATINY85 used as an ADC and OneWire device emulator

#### pH Frontend and ORP frontend

pH electrodes have high impedence (250M ohm) and are capable of generating between -500mV and 500mV at 50°C and between -300mV and 300mV at -10°C when put in a solution.

ORP electrodes are also high impedence voltage generators, but the range is very different: between -2V and 2V. 

As we are using an ATTiny85 for the built in ADC and the ability to emulate a OneWire device, we are constrained by caracteristics of the built in ADC:

- Input from 0 to 5V (max)
- 10 bits resolution (1024 steps)

This mean that negative signals cannot be measured by the ATTiny85 ADC. One simple solution constist in offseting the probe by the required voltage to become positive. For the pH probe, adding a 500mV offset to the probe will move the pH reading between 0mV and 1000mV. 

The same method can be used with the ORP probe. This time we are using a much larger offset with a value of 2000mV.

The offset is generated using voltage referance chip from Texas Intrument (the LM4140 voltage reférence with an output value of 2048mV). This reference is used directly to produce the offset of the ORP probe. For the pH probe the desired 0.5V offset is generated using the 2.048V source and a voltage divider.

One important thing to remember is that the probes are with very high impedence and we cannot apply each offset voltage directly to the probe. We use an opamp to "isoltate" each side. 

In order to measure the signal with the ADC it needs to go through another opamp. Without this, the ATTINY85 ADC wont be able to detect anything due to the very high impedence of the probes. 

The second opamp also act as an amplifier for the signal. The signal from the pH probe is amplified by a factor of five (0-1V become 0-5V) and the ORP signal is amplified by a factor of 1.25 (0-4V become 0-5V).

At this stage we have our two frontend for our probes.

## PoolReader Board embeded firmware

The ATTiny85 on the board gather all information from both probes (pH and ORP) and emulate a OneWire device. It has been designed to be able to manage an on board tmperature sensor and waterlevel measurment using an ultrasonic sensor. 

### Development Environment

The firmware has been developped using the PlatformIO extension for VSCode. The project is already configured to use this platform. Necessary libraries are integrated as git submodules inside the lib folder of the firmware. 

You can find more information on the PlatformIO website regarding the installation and configuration of the VSCode environment. 

## Usage intructions

Below, you can find instruction on how to use the board and the client that goes with it.

### Hardware - Device

1. Assemble the board with components listed in the bill of material.
2. Prepare the PlatformIO environment
3. Open the Arduino IDE and flash the ArduinoISP sketch to an Arduino Uno or equivalent.
4. Connect the MOSI, MISO, SCK pin to Arduino Uno or equivalent SPI header.
5. Configure the firmware using the defines (Enable waterlevel with ultrasonic sensor ? Enable the on board temperature sensor ?)
6. Connect the Arduino Uno and then change the COM port path in the PlatformIO.ini file
7. Buil then Upload the firmware to the device
8. Using a OneWire client and the PoolReader library, check that the device is working as intended
9. If using the ultrasonic sensor, change the value of the HFuse in the platformio.init file to allow the usage of the Reset pin as IO, and click on the "Set Fuses" task.

/!\ After the fuse has been writen you will not be able to program the chip using the SPI programmer. You will need to use at least an High Voltage Programmer to reset the fuses value or program the chip. /!\ 

### Software - Client

The PoolReader board comes with the PoolReader library (link in ressources listed above) that implement the OneWire protocol that comes with this device. It Works exclusivly with the arduino framework, but can be adapated without too much dificulties.

More informations can be found on the library page. 

### Wiring

As this project uses OneWire to communicate, the number of required cable is rather minimal.

- 1 5V VCC (pin VCC on the board)
- 1 GND (pin GND on the board)
- 1 OneWire data line (pin 1W on the board). (Remember that the OneWire needs a 4k7 pull-up to work correctly)

## Licence


```
PoolReader Hardware, Firmware, Library
Copyright 2020 Guillaume VILLENA aka "Willena"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```