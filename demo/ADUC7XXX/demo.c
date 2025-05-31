/*
 * FreeModbus Libary: ADuC7xxx Port
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>,
 *							 2021 Arend Lammertink <lamare@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];

/* ----------------------- Start implementation -----------------------------*/

/*

https://www.embedded-solutions.at/files/freemodbus-v1.6-apidoc/group__modbus.html

#include "mb.h" 

This module defines the interface for the application. It contains the basic 
functions and types required to use the Modbus protocol stack. A typical 
application will want to call eMBInit() first. If the device is ready to answer 
network requests it must then call eMBEnable() to activate the protocol stack. 
In the main loop the function eMBPoll() must be called periodically. The time 
interval between pooling depends on the configured Modbus timeout. If an RTOS 
is available a separate task should be created and the task should always call 
the function eMBPoll().


 // Initialize protocol stack in RTU mode for a slave with address 10 = 0x0A
 eMBInit( MB_RTU, 0x0A, 38400, MB_PAR_EVEN );

 // Enable the Modbus Protocol Stack.
 eMBEnable(  );

 for( ;; )
 {
     // Call the main polling loop of the Modbus protocol stack.
     eMBPoll(  );
     ...
 }

*/

int main( void )
 {
	
#ifdef OSCI_HIGHSPEED 		// Switch processor speed to 41.78MHz
  	POWKEY1 = 0x01;       // Overwrite protection 
  	POWCON  = 0x00;       // Switch clockdivider to 41.78MHz
  	POWKEY2 = 0xF4;       // Overwrite protection
#endif 
 
		eMBErrorCode    eStatus;

// 	Used some digital I/O's for debugging, see porttimer.c.	 
//	GP4DAT = 0x04000000;			// P4.2 configured as an output. LED is turned on	
//	GP4DAT = 0x0c000000;			// P4.2 and P4.3 configured as an output. LED is turned on	
	 
    eStatus = eMBInit( MB_RTU, 0x0A, 0, 38400, MB_PAR_EVEN );
//    eStatus = eMBInit( MB_ASCII, 0x0A, 0, 38400, MB_PAR_EVEN );
//    eStatus = eMBInit( MB_ASCII, 0x0A, 0, 9600, MB_PAR_EVEN );
//    eStatus = eMBInit( MB_RTU, 0x0A, 0, 9600, MB_PAR_EVEN );
	
    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );

    for( ;; )
    {
        ( void )eMBPoll(  );

        /* Here we simply count the number of poll cycles. */
        usRegInputBuf[0]++;
    }
	
}
 

/*	The following call-back functions need to be implemented for an
		actual application.

		See: https://www.embedded-solutions.at/files/freemodbus-v1.6-apidoc/group__modbus__registers.html 

		The protocol stack does not internally allocate any memory for the 
		registers. This makes the protocol stack very small and also usable 
		on low end targets. In addition the values don't have to be in the 
		memory and could for example be stored in a flash.

		Whenever the protocol stack requires a value it calls one of the 
		callback function with the register address and the number of registers 
		to read as an argument. The application should then read the actual 
		register values (for example the ADC voltage) and should store the 
		result in the supplied buffer.

		If the protocol stack wants to update a register value because a 
		write register function was received a buffer with the new register 
		values is passed to the callback function. The function should then 
		use these values to update the application register values.
*/

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNRegs;
    ( void )eMode;
    return MB_ENOREG;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNCoils;
    ( void )eMode;
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNDiscrete;
    return MB_ENOREG;
}


#ifndef NDEBUG
/* ALA 26/20/2021

	 This function is meant for debugging purposes and only used when NDEBUG is not defined and
	 is called when an assert() statement fails. These assert statements are meant to check
	 for certain situations during development and should not be needed in production code and
	 production code should be compiled with NDEBUG defined, for No Debug.

	 The version in the default library does not work and generates SWI's, software interrupts,
	 such that one cannot return with the debugger to the point where the assert failed.
*/
void __aeabi_assert(const char *expr, const char *file, int line)
{
	static const char* _expr = 0;
	static const char* _file = 0;
	static int 				 _line = 0;

	_expr = expr;
	_file = file;
  _line = line;
} 
#endif