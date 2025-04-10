# FreeModbus Library

## Introduction

This is a fork of the FreeModbus protocol stack, extended with ports for:

- ADuc702x processors
- STM32 processors using CMake build system

FreeModbus is a Modbus ASCII/RTU and Modbus TCP implementation for embedded systems. It provides an implementation of the Modbus Application Protocol v1.1a and supports both the RTU and ASCII transmission modes as defined in the Modbus over serial line specification 1.0.

## STM32 CMake Port

The STM32 CMake port is located in the `demo/STM32_CMAKE` directory and provides:

- A generic implementation that can target multiple STM32 families
- HAL-based peripheral access for portability
- Integration with CMake build systems
- Simple demo application
- Support for the STM32G4 family (tested on Nucleo-G431RB)

## Documentation

Complete documentation of the original FreeModbus implementation can be found in the `doc/html` directory.

For information specific to the STM32 CMake port, refer to the `demo/STM32_CMAKE/README.md` file.

## License

FreeModbus uses a multi-license approach:

- **Core Protocol Stack** (in `modbus/` directory): BSD License
- **Port Implementations** (in `demo/*/port/` directories): Generally LGPL License
- **Demo Applications** (in `demo/*/` directories): Generally GPL License

This licensing structure allows:
- Using the core protocol stack in both open-source and proprietary applications (BSD)
- Using the port implementations in proprietary applications with some restrictions (LGPL)
- Using the demo applications only in GPL-compatible projects (GPL)

Always check the license header in individual source files for specific licensing terms.

## References

- [Original FreeModbus project](https://www.embedded-solutions.at/en/freemodbus/)
- [Modbus specifications](https://modbus.org/specs.php)


