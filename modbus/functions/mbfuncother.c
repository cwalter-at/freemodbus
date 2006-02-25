/*
 * FreeRTOS Modbus Library: A Modbus serial implementation for FreeRTOS
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
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"

#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/
static UCHAR    ucMBSlaveID[MB_FUNC_OTHER_REP_SLAVEID_BUF];
static USHORT   usMBSlaveIDLen;

/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBSetSlaveID( const UCHAR *pucSlaveID, USHORT usSlaveIDLen, BOOL xIsRunning )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( usSlaveIDLen + 1 < MB_FUNC_OTHER_REP_SLAVEID_BUF )
    {
        memcpy( &ucMBSlaveID[0], pucSlaveID, usSlaveIDLen );
        ucMBSlaveID[usSlaveIDLen] = xIsRunning ? 0xFF : 0x00;
        usMBSlaveIDLen = usSlaveIDLen + 1;
    }
    else
    {
        eStatus = MB_ENORES;
    }
    return eStatus;
}

eMBException
eMBFuncReportSlaveID( UCHAR *pucFrame, USHORT *usLen )
{
    memcpy( pucFrame + MB_PDU_DATA_OFF, &ucMBSlaveID[0], usMBSlaveIDLen );
    *usLen = MB_PDU_DATA_OFF + usMBSlaveIDLen;
    return MB_EX_NONE;
}

#endif
