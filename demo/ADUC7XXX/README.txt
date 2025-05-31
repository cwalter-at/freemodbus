
                      FREEMODBUS 1.2 ADuC7xxx PORT
                      ===========================

REQUIREMENTS
============

This demo application provides a port for the ADuC7xxx series  of  processors
from Analog Devices[1]. The port was done using the QuickStart Plus for 
ADUC702X-Series development board which features a ADuC7026 processor and
the Keil ARM development tools[2] version 5.29.

It requires a wired serial port to a host  processor  and  a  Modbus  Master
Software on the PC side to be  useful.   Demo  versions  of  Modbus  Masters
can be found in [3] and [4].


TESTING
=======

Start the Modbus Sample Application and test if the input registers starting
at protocol  address 1000 can be  read. There are four registers value avai-
lable and the output should look like:

Polling slave (Ctrl-C to stop) ...
[1000]: 6474
[1001]: 0
[1002]: 0
[1003]: 0
Polling slave (Ctrl-C to stop) ...
[1000]: -8831
[1001]: 0
[1002]: 0
[1003]: 0
Polling slave (Ctrl-C to stop) ...

The simple testing utility used in the 'demo_rtu.bat' script  can  be  found
at [5].


[1] Keil MCB2140 kit: https://www.analog.com/en/design-center/landing-pages/001/microconverter-development-tools.html
[2] https://www2.keil.com/mdk5
[3] WinTech ModScan32: http://www.win-tech.com/html/modscan32.htm
[4] Modus Poll: http://www.modbustools.com/modbus_poll.asp
[5] FieldTalk Modpoll: http://www.focus-sw.com/fieldtalk/modpoll.html




