# Freemodbus avr2560 example

## Requirements

This example contains a simple demo program for FreeMODBUS for an ATMega2560 processor.

It requires a wired serial port to a host processor and a Modbus Master Software on the PC side to be useful.

In addition it requires an ATMega2560 target microcontroller. The micro-controller must be configured for 16 Mhz if the binary image is going to be used. Otherwise the constant F_CPU in the Makefile must be changed and the source code recompiled.

## Installation

If avrdude is used for flashing the Makefile contains a target 'flash' which can be executed by calling 'make flash'. This programs the micro-controller with the current binary named 'demo.hex'.

Example of manually flashing demo.hex:
```
$ avrdude -b 115200 -c wiring -p atmega2560 -U flash:w:demo.hex -P /dev/ttyACM0 -D
```

## Source

If one wants to recompile the software the WinAVR tools should be installed if the host platform is Microsoft Windows. For Unix the necessary tools must be recompiled from source.

## Testing

Example:
```
$ ./modpoll -m rtu -a 0x0c -r 1000 -c 4 -t 3:hex -b 57600 -d 8 -p even -s1 /dev/ttyUSB0 -l 1
```

Start the Modbus Sample Application and test if the input registers starting
at protocol address 1000 can be read. There are four registers value available
and the output should look like:

```
-- Polling slave... (Ctrl-C to stop)
[1000]: 0x8424
[1001]: 0x000C
[1002]: 0x4212
[1003]: 0x0000
-- Polling slave... (Ctrl-C to stop)
[1000]: 0x8977
[1001]: 0x000C
[1002]: 0x44BB
[1003]: 0x0000
-- Polling slave... (Ctrl-C to stop)
```

## Notes

* This demo was tested on a Meduino Mega2560 Pro Mini R3 board.
* Code is based on demo/AVR.
* We use ISR instead of (deprecated) SIGNAL.
* Tested Compiler version: avr-gcc v8.2.0

## Links

* [Meduino Mega2560 Pro Mini R3](http://wiki.epalsite.com/index.php?title=Mega2560_Pro_Mini)
* [Modpoll](https://www.modbusdriver.com/modpoll.html)

