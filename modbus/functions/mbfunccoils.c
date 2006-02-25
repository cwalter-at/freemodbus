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

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_READ_ADDR_OFF       ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_READ_COILCNT_OFF    ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE           ( 4 )
#define MB_PDU_FUNC_READ_COILCNT_MAX    ( 0x07D0 )

/* ----------------------- Start implementation -----------------------------*/

#if MB_FUNC_READ_COILS_ENABLED > 0

eMBException
eMBFuncReadCoils( UCHAR *pucFrame, USHORT *usLen )
{
    USHORT          usRegAddress;
    USHORT          usCoilCount;
    UCHAR           usNBytes;
    UCHAR          *pucFrameCur;

    eMBException    eStatus = MB_ENOERR;
    eMBErrorCode    eRegStatus;

    if( *usLen == ( MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress = pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8;
        usRegAddress |= pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1];
        usRegAddress++;

        usCoilCount = pucFrame[MB_PDU_FUNC_READ_COILCNT_OFF] << 8;
        usCoilCount = pucFrame[MB_PDU_FUNC_READ_COILCNT_OFF + 1];

        /* Check if the number of registers to read is valid. If not
         * return Modbus illegal data value exception.
         */
        if( ( usCoilCount >= 1 ) && ( usCoilCount < MB_PDU_FUNC_READ_COILCNT_MAX ) )
        {
            /* Set the current PDU data pointer to the beginning. */
            pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
            *usLen = MB_PDU_FUNC_OFF;

            /* First byte contains the function code. */
            *pucFrameCur++ = MB_FUNC_READ_COILS;
            *usLen += 1;

            /* Test if the quantity of coils is a multiple of 8. If not last
             * byte is only partially field with unused coils set to zero. */
            if( ( usCoilCount & 0x0007 ) != 0 )
            {
                usNBytes = ( UCHAR ) ( usCoilCount / 8 ) + 1;
            }
            else
            {
                usNBytes = ( UCHAR ) ( usCoilCount / 8 );
            }
            *pucFrameCur++ = usNBytes;
            *usLen += 1;

            eRegStatus = eMBRegCoilsCB( pucFrameCur, usRegAddress, usCoilCount, MB_REG_READ );
            switch ( eRegStatus )
            {
            case MB_ENOERR:
                *usLen += usNBytes;
                break;

            case MB_ENOREG:
                eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
                break;

            case MB_ETIMEDOUT:
                eStatus = MB_EX_SLAVE_BUSY;
                break;

            default:
                eStatus = MB_EX_SLAVE_DEVICE_FAILURE;
                break;
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    {
        /* Can't be a valid read coil register request because the length
         * is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

#endif
