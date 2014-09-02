# Content of PV-SysReader
* [Description](#description)
* [Version](#version)
* [Supported Devices](#supported-devices)
* [Requirements](#requirements)
* [Installation](#installation)
* [How to use PV-SysReader](#how-to-use-pv-sysreader)
* [Contributing](#contributing)
* [Thanks to](#thanks-to)
* [Author](#author)
* [License](#license)
* [History](#history)

## Description
PV-SysReader is a tool to retrieve information from Photovoltaic Systems.
- Photovoltaic inverters
- Battery inverters
- Battery monitors
- MPPT Battery charger

## Version
0.1alpha

## Supported Devices
- Victron MultiPlus and Quattro
- Victron Blue Solar Grid Inverters
- Victron BMV 700/702 battery monitors
- Victron MPPT 70/15 chargers
- Litrade BMS 16S-PMOS

## Requirements
 - RS232-USB interface for Victron Blue Solar Grid Inverter
 - TTL-USB interface for Victron Battery Monitor
 - MK2-USB interface for Victron MultiPlus/Quattro  
Note: The MK2-USB interface must have the firmware 1130128 or greater.

## Installation
Please make sure you have the following software on your Linux system:
 - build-essential
 - gcc
 - cmake
 - doxygen*
 - latex*

*Doxygen and Latex is required to build the protocol specification and code documentation scratch.  
The protocol specification pdf is already included.

```bash
##On a Debian based distribution you can install necessary packages with:##

apt-get install build-essential gcc cmake doxygen

```

Get the source:
```bash
git clone https://github.com/egophil/PV-SysReader
```

To compile the source:
```bash
cd PV-SysReader/src
cmake ./
make
```
The executable is called PV-SysReader.
***
To build source documentation:
```bash
cd PV-SysReader
doxygen doxygen.conf
cd docs
```

## How to use PV-SysReader
Please make sure that you use the proper interface.  
For example if  you have a TTL-USB and RS232-USB interface hooked up, you will 
have a ttyUSB0 and ttyUSB1 device node. So one of them will be the TTL-USB and 
the other one the RS232-USB interface.  
If you would like a fixed device node for your interfaces you can setup a udev 
rule.  
Each time you connect your device to the system (i.e. reboot) it will be 
assigned to the same device node.

To read from a Victron BMV Battery monitor:
```bash
./PV-SysReader -i /dev/ttyUSB0 -d bmv
```
To read from a Victron MPPT Charger like the MPPT 70/15 :
```bash
./PV-SysReader -i /dev/ttyUSB0 -d mppt
```

To read from a Victron Blue Solar Grind Inverter.
Note there are three options available
 * stat - get only common parameters 
 * param - get device specific parameters
 * all - get common and specific paramerters

```bash
./PV-SysReader -i /dev/ttyUSB0 -o stat -d bs_inverter
./PV-SysReader -i /dev/ttyUSB0 -o param -d bs_inverter
./PV-SysReader -i /dev/ttyUSB0 -o all -d bs_inverter
```

Requesting parameters with a MK2-USB Interface 
```
./PV-SysReader -i /dev/ttyUSB0 -d mk2
```
***
**For debugging it is useful to get the raw output in hex**

Instead of usind the -d operator you use -r for raw output
```bash
./PV-SysReader -i /dev/ttyUSB0 -r bmv
./PV-SysReader -i /dev/ttyUSB0 -r mppt
./PV-SysReader -i /dev/ttyUSB0 -r bs_inverter
./PV-SysReader -i /dev/ttyUSB0 -r mk2
```
Note: The option for Blue Solar Grid Inverter does not exists in raw output 
mode.

## Contributing
The project needs support:
 - Tester
 - Developer
 - New ideas
 - Bug reports

## Thanks to
Thanks for testing to Ultraschall81 from <http://www.photovoltaikforum.com/>
Thanks to mikele9 from <http://www.photovoltaikforum.com/>

## Author
Philipp Drieß: <egophil@gmx.de>

## License
GPLv3

## History
######0.2 alpha
Support for Litrade Battery Management System added - BMS 16S-PMOS  <http://www.litrade.de/>

######0.1 alpha
First release