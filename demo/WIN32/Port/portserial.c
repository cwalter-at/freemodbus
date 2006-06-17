/*
 * FreeModbus Library: Win32 Port
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
 * File: $Id: portserial.c,v 1.2 2006/06/17 00:17:45 wolti Exp $
 */

#include <windows.h>

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"

/* ----------------------- Defines  -----------------------------------------*/
#if MB_ASCII_ENABLED == 1
#define BUF_SIZE    513         /* must hold a complete ASCII frame. */
#else
#define BUF_SIZE    256         /* must hold a complete RTU frame. */
#endif

/* ----------------------- Static variables ---------------------------------*/
static HANDLE   g_hSerial;
static BOOL     bRxEnabled;
static BOOL     bTxEnabled;

static UCHAR    ucBuffer[BUF_SIZE];
static INT      uiRxBufferPos;
static INT      uiTxBufferPos;

/* ----------------------- Begin implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL bEnableRx, BOOL bEnableTx )
{
    /* it is not allowed that both receiver and transmitter are enabled. */
    assert( !bEnableRx || !bEnableTx );

    if( bEnableRx )
    {
        PurgeComm( g_hSerial, PURGE_RXCLEAR );
        uiRxBufferPos = 0;
        bRxEnabled = TRUE;
    }
    else
    {
        bRxEnabled = FALSE;
    }
    if( bEnableTx )
    {
        bTxEnabled = TRUE;
        uiTxBufferPos = 0;
    }
    else
    {
        bTxEnabled = FALSE;
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    TCHAR           szDevice[8];
    BOOL            bStatus = TRUE;
    DCB             dcb;



    memset( &dcb, 0, sizeof( dcb ) );

    dcb.DCBlength = sizeof( dcb );
    dcb.BaudRate = ulBaudRate;

    _stprintf_s( szDevice, 8, L"COM%d", ucPort );

    switch ( eParity )
    {
    case MB_PAR_NONE:
        dcb.Parity = NOPARITY;
        dcb.fParity = 0;
        break;
    case MB_PAR_EVEN:
        dcb.Parity = EVENPARITY;
        dcb.fParity = 1;
        break;
    case MB_PAR_ODD:
        dcb.Parity = ODDPARITY;
        dcb.fParity = 1;
        break;
    default:
        bStatus = FALSE;
    }

    switch ( ucDataBits )
    {
    case 8:
        dcb.ByteSize = 8;
        break;
    case 7:
        dcb.ByteSize = 7;
        break;
    default:
        bStatus = FALSE;
    }



    if( bStatus )
    {
        /* we don't use XON/XOFF flow control. */
        dcb.fInX = dcb.fOutX = FALSE;
        /* we don't need hardware handshake. */
        dcb.fOutxCtsFlow = dcb.fOutxCtsFlow = FALSE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;

        /* misc parameters */
        dcb.fErrorChar = FALSE;
        dcb.fBinary = TRUE;
        dcb.fNull = FALSE;
        dcb.fAbortOnError = FALSE;
        dcb.wReserved = 0;
        dcb.XonLim = 2;
        dcb.XoffLim = 4;
        dcb.XonChar = 0x13;
        dcb.XoffChar = 0x19;
        dcb.EvtChar = 0;

        /* Open the serial device. */
        g_hSerial = CreateFile( szDevice, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );


        if( g_hSerial == INVALID_HANDLE_VALUE )
        {
            /* can't open serial device. */
            ERRORC( L"xMBPortSerialInit: can't open %s: ", GetLastError(  ), szDevice );
            bStatus = FALSE;
        }
        else if( !SetCommState( g_hSerial, &dcb ) )
        {
            ERRORC( L"xMBPortSerialInit: can't configure device %s: ", GetLastError(  ), szDevice );
            bStatus = FALSE;
            /* can set communication device state. */
        }
        else if( !SetCommMask( g_hSerial, 0 ) )
        {
            ERRORC( L"xMBPortSerialInit: can't set comm mask on device %s: ", GetLastError(  ), szDevice );
            bStatus = FALSE;
            /* can't disable all communication events. */
        }

        else
        {
            vMBPortSerialEnable( FALSE, FALSE );
            bStatus = TRUE;
        }
    }
    return bStatus;
}

BOOL
xMBPortSerialSetTimeout( DWORD dwTimeoutMs )
{
    BOOL            bStatus;
    COMMTIMEOUTS    cto;

    /* usTimeOut is the inter character timeout used to detect the end
     * of frame. The total timeout is set to 50ms to make sure we
     * can exit the blocking read. */
    cto.ReadIntervalTimeout = dwTimeoutMs;
    cto.ReadTotalTimeoutConstant = 50;
    cto.ReadTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;
    cto.WriteTotalTimeoutMultiplier = 0;

    if( !SetCommTimeouts( g_hSerial, &cto ) )
    {
        ERRORC( L"xMBPortSerialInit: can't set comm timeouts : ", GetLastError(  ) );
        bStatus = FALSE;
        /* can't set timeouts. */
    }
    else
    {
        bStatus = TRUE;
    }

    return bStatus;
}

void
xMBPortSerialClose( void )
{
    ( void )CloseHandle( g_hSerial );
}

BOOL
xMBPortSerialPoll(  )
{
    BOOL            bStatus = TRUE;
    DWORD           dwBytesRead;
    DWORD           dwBytesWritten;
    DWORD           i;

    while( bRxEnabled )
    {
        /* buffer wrap around. */
        if( uiRxBufferPos >= BUF_SIZE )
            uiRxBufferPos = 0;

        if( ReadFile( g_hSerial, &ucBuffer[uiRxBufferPos], BUF_SIZE - uiRxBufferPos, &dwBytesRead, NULL ) )
        {
            if( dwBytesRead == 0 )
            {
                /* timeout with no bytes. */
                break;
            }
            else if( dwBytesRead > 0 )
            {
                /* timeout occured and some characters are received. */
                TRACEC( L"vMBPortSerialPoll: t3.5 timeout with characters\r\n" );

                for( i = 0; i < dwBytesRead; i++ )
                {
                    /* Call the modbus stack and let him fill the buffers. */
                    ( void )pxMBFrameCBByteReceived(  );
                }
            }
        }
        else
        {
            ERRORC( L"vMBPortSerialPoll: ReadFile failed: ", GetLastError(  ) );
            /* read has failed - this is an I/O error. */
            bStatus = FALSE;
        }
    }
    if( bTxEnabled )
    {
        while( bTxEnabled )
        {
            ( void )pxMBFrameCBTransmitterEmpty(  );
            /* Call the modbus stack to let him fill the buffer. */
        }
        dwBytesWritten = 0;
        if( !WriteFile( g_hSerial, &ucBuffer[0], uiTxBufferPos, &dwBytesWritten, NULL )
            || ( dwBytesWritten != uiTxBufferPos ) )
        {
            ERRORC( L"vMBPortSerialPoll: WriteFile failed: ", GetLastError(  ) );
            bStatus = FALSE;
        }
    }

    return bStatus;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    assert( uiTxBufferPos < BUF_SIZE );
    ucBuffer[uiTxBufferPos] = ucByte;
    uiTxBufferPos++;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR *pucByte )
{
    assert( uiRxBufferPos < BUF_SIZE );
    *pucByte = ucBuffer[uiRxBufferPos];
    uiRxBufferPos++;
    return TRUE;
}

#ifdef _DEBUG
void
TRACEC( const TCHAR *pcFmt, ... )
{
    TCHAR           szUserBuf[160];
    int             iBufLen = sizeof( szUserBuf ) / sizeof( szUserBuf[0] );
    int             iBufWritten;
    va_list         args;

    assert( pcFmt != NULL );
    va_start( args, pcFmt );

    iBufWritten = _vstprintf_s( szUserBuf, iBufLen, pcFmt, args );
    if( iBufWritten != -1 )
    {
        OutputDebugString( szUserBuf );
    }

    va_end( args );
}

void
ERRORC( const TCHAR *pcFmt, DWORD dwError, ... )
{

    TCHAR           szUserBuf[160];
    LPTSTR          lpMsgBuf;
    int             iBufLen = sizeof( szUserBuf ) / sizeof( szUserBuf[0] );
    int             iBufWritten;
    va_list         args;

    assert( pcFmt != NULL );
    va_start( args, dwError );

    iBufWritten = _vstprintf_s( szUserBuf, iBufLen, pcFmt, args );
    if( iBufWritten != -1 )
    {
        iBufLen -= iBufWritten;
        ( void )FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError,
                               MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), ( LPTSTR ) & lpMsgBuf, 0, NULL );
        _stprintf_s( &szUserBuf[iBufWritten], iBufLen, L"%s\r\n", lpMsgBuf );
        LocalFree( lpMsgBuf );
        OutputDebugString( szUserBuf );
    }

    va_end( args );
}

#endif
