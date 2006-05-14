/*
 * FreeModbus Library: ATMega168 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.2 2006/05/14 21:55:01 wolti Exp $
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#define UART_BAUD_RATE 9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)

#define UART_UCSRB  UCSR0B

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    UCSR0B |= _BV( TXEN0 );

    if( xRxEnable )
        UCSR0B |= _BV( RXEN0 ) | _BV( RXCIE0 );
    else
        UCSR0B &= ~( _BV( RXEN0 ) | _BV( RXCIE0 ) );

    if( xTxEnable )
        UCSR0B |= _BV( TXEN0 ) | _BV( UDRE0 );
    else
        //UCSR0B &= ~( _BV(TXEN0) | _BV(UDRE0) );
        UCSR0B &= ~( _BV( UDRE0 ) );
}

BOOL
xMBPortSerialInit( ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    UBRR0 = UART_BAUD_CALC( ulBaudRate, F_CPU );
    // UCSR0A = UCSR0A = UCSR0C = 0x00;
    switch ( eParity )
    {
    case MB_PAR_EVEN:
        UCSR0C |= _BV( UPM01 );
        break;
    case MB_PAR_ODD:
        UCSR0C |= _BV( UPM01 ) | _BV( UPM00 );
        break;
    case MB_PAR_NONE:
        break;
    }

    switch ( ucDataBits )
    {
    case 8:
        UCSR0C |= _BV( UCSZ00 ) | _BV( UCSZ01 );
        break;
    case 7:
        UCSR0C |= _BV( UCSZ01 );
        break;
    }
    vMBPortSerialEnable( FALSE, FALSE );
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    UDR0 = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR *pucByte )
{
    *pucByte = UDR0;
    return TRUE;
}

SIGNAL( SIG_USART_DATA )
{
    pxMBFrameCBTransmitterEmpty(  );
}

SIGNAL( SIG_USART_RECV )
{
    //xMBPortSerialPutByte ( 'a');
    pxMBFrameCBByteReceived(  );
}
