
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
xMBPortSerialGetByte( CHAR * pucByte )
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
