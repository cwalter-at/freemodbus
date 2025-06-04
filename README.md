This project sees little-to-no activity from the original creators, yet it seems it is still very popular. Let's try a more open and courageous way forward - everyone is welcomed to participate, create PRs, and contribute in any way. The PRs will be merged if properly justified and no pushback from community. It's understood that bugs and errors may enter the codebase with this approach, hence the last version 1.6.0 Christian Walter is the latest official release. You will therefore always have the option to decide to use either `master` or `1.6.0`

# FreeModbus Library

## Introduction

FreeModbus is a Modbus ASCII/RTU and Modbus TCP implementation for embedded systems. It provides an implementation of the Modbus Application Protocol v1.1a and supports both the RTU and ASCII transmission modes as defined in the Modbus over serial line specification 1.0.

## Documentation

Complete documentation of the original FreeModbus implementation can be found in the `doc/html` directory.

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


