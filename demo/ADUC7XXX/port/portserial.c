/*
 * FreeModbus Libary: ADuC7xxx Port
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>,
 *							 2021 Arend Lammertink <lamare@gmail.com>.
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
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void 		sio_irq( void );
static void     prvvUARTTxReadyISR( void );
static void     prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable )
    {
        COMIEN0 |= 0x01; 	// Enable receive buffer full interrupt
    }
    else
    {
        COMIEN0 &= ~0x01;
    }
    if( xTxEnable )
    {
        COMIEN0 |= 0x02;	// Enable transmit buffer empty interrupt
        prvvUARTTxReadyISR(  );
    }
    else
    {
        COMIEN0 &= ~0x02;
    }
}

void vMBPortClose( void )
{
}


/*


					DL = HCLK										   
						_______										   
						Baudrate * 2 *16							   



COMTX, COMRX, and COMDIV0 share the same address location. COMTX 
and COMRX can be accessed when Bit 7 in the COMCON0 
register is cleared. COMDIV0 can be accessed when Bit 7 
of COMCON0 is set. 

*/
BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            bInitialized = TRUE;
    USHORT          cfg = 0;
    ULONG           DL = ( ( HCLK / ulBaudRate ) / 32UL );
    volatile char   dummy;

    ( void )ucPORT;
	
    /* Configure UART Pins */
		GP1CON = 0x011;							// Setup tx & rx pins on P1.0 and P1.1

    switch ( ucDataBits )
    {
    case 5:
        break;

    case 6:
        cfg |= 0x00000001;
        break;

    case 7:
        cfg |= 0x00000002;
        break;

    case 8:
        cfg |= 0x00000003;
        break;

    default:
        bInitialized = FALSE;
    }

    switch ( eParity )
    {
    case MB_PAR_NONE:
        break;

    case MB_PAR_ODD:
        cfg |= 0x00000008;
        break;

    case MB_PAR_EVEN:
        cfg |= 0x00000018;
        break;
    }

    if( bInitialized )
    {
        COMCON0 = cfg;            /* Configure Data Bits and Parity */
        COMIEN0 = 0;              /* Disable UART1 Interrupts */

																	/* DLAB - Divisor latch access. Set by user to enable access 
																		to the COMDIV0 and COMDIV1 registers. Cleared 
																		by user to disable access to COMDIV0 and 
																		COMDIV1 and enable access to COMRX and 
																		COMTX. */
        COMCON0 |= 0x80;          /* Set DLAB, divisor latch access bit */
        COMDIV0 = DL;         		/* Set Baud */
        COMDIV1 = 0;    		
				COMCON0 &= ~0x80;         /* Clear DLAB */

        /* Configure UART1 Interrupt */
				FIQ = sio_irq;						// use fast interrupt for serial comm.
			
				if ( InCriticalSection )
					FIRQ_Temp |= UART_BIT;	// Enable UART IRQ when leaving critical section.
				else
					FIQEN 		|= UART_BIT;	// Enable UART IRQ
			
        dummy = COMIID0;          /* Required (on LPC214X) to Get Interrupts Started. Just leave it in for now. */
    }

    return bInitialized;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Wait till U0THR and U0TSR are both empty */
    while( !(0x020==(COMSTA0 & 0x020)) )
    {}

    COMTX = ((int)ucByte);
		
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    while( !(0x01==(COMSTA0 & 0x01)) )
    {}

    /* Receive Byte */
    *pucByte = COMRX;

    return TRUE;
}


void sio_irq( void )
{
    volatile char   dummy;
    volatile char   IIR;
	
    while( ( ( IIR = COMIID0 ) & 0x01 ) == 0 )
    {
        switch ( IIR & 0x06 )
        {
        case 0x06:             /* Receive Line Status */
            dummy = COMSTA0;   /* Just clear the interrupt source */
            break;

        case 0x04:             /* Receive buffer Full */
            prvvUARTRxISR();
            break;

        case 0x02:             /* Transmit buffer empty */
            prvvUARTTxReadyISR();
            break;

        case 0x00:             /* Modem Interrupt */
            dummy = COMSTA1;   /* Just clear the interrupt source */
            break;

        default:
            break;
        }
    }
}


/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void 
prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}


/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void
prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}
