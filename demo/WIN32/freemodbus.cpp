/*
 * FreeModbus Library: Win32 Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 * Parts of crt0.S Copyright (c) 1995, 1996, 1998 Cygnus Support
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
 * File: $Id: freemodbus.cpp,v 1.2 2006/06/17 00:15:36 wolti Exp $
 */

#include "stdafx.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define PROG            _T("freemodbus")

#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4
#define REG_HOLDING_START 2000
#define REG_HOLDING_NREGS 130

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

int
_tmain( int argc, _TCHAR * argv[] )
{
    int             iExitCode;
    TCHAR           cCh;
    BOOL            bDoExit;

    const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };

    if( eMBInit( MB_RTU, 0x0A, 1, 38400, MB_PAR_EVEN ) != MB_ENOERR )
    {
        _ftprintf( stderr, _T( "%s: can't initialize modbus stack!\r\n" ),
                   PROG );
        iExitCode = EXIT_FAILURE;
    }
    else if( eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3 ) != MB_ENOERR )
    {
        _ftprintf( stderr, _T( "%s: can't set slave id!\r\n" ), PROG );
        iExitCode = EXIT_FAILURE;
    }
    else if( xMBPortStartPoolingThread(  ) != MB_ENOERR )
    {
        _ftprintf( stderr, _T( "%s: can't start pooling thread!\r\n" ),
                   PROG );
        iExitCode = EXIT_FAILURE;
    }
    else
    {
        _tprintf( _T( "Type 'q' for quit or 'h' for help!\r\n") );
        bDoExit = FALSE;
        do {
            _tprintf( _T( "> " ) );
            cCh = _gettchar();
            switch( cCh )
            {
            case _TCHAR('q'):
                bDoExit = TRUE;
                break;
            case _TCHAR('d'):
                xMBPortStopPoolingThread();
                break;
            case _TCHAR('e'):
                xMBPortStartPoolingThread();
                break;
            case _TCHAR('h'):
                _tprintf( _T( "FreeModbus demo application help:\r\n") );
                _tprintf( _T( "  'd' ... disable protocol stack.\r\n") );
                _tprintf( _T( "  'e' ... enabled the protocol stack\r\n") );
                _tprintf( _T( "  'q' ... quit applicationr\r\n") );
                _tprintf( _T( "  'h' ... this information\r\n") );
                _tprintf( _T( "\r\n") );
                _tprintf( _T( "Copyright 2006 Christian Walter <wolti@sil.at>\r\n") );
                break;
            default:
                _tprintf( _T( "illegal command '%c'!\r\n" ), cCh );
                break;
            }

            /* eat up everything untill return character. */
            while( _gettchar() != _TCHAR('\n') );
        } while( !bDoExit );
        /* Shut down pooling thread in case not already done. */
        (void)xMBPortStopPoolingThread(  );

        /* Release hardware resources. */
        (void)eMBClose();
        iExitCode = EXIT_SUCCESS;
    }
    return iExitCode;
}

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
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
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
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
                /* Pass current register values to the protocol stack. */
            case MB_REG_READ:
                while( usNRegs > 0 )
                {
                    *pucRegBuffer++ =
                        ( UCHAR )( usRegHoldingBuf[iRegIndex] >> 8 );
                    *pucRegBuffer++ =
                        ( UCHAR )( usRegHoldingBuf[iRegIndex] & 0xFF );
                    iRegIndex++;
                    usNRegs--;
                }
                break;

                /* Update current register values with new values from the
                 * protocol stack. */
            case MB_REG_WRITE:
                while( usNRegs > 0 )
                {
                    usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                    usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
