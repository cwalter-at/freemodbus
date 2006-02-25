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
#define MB_PDU_FUNC_READ_ADDR_OFF           ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_READ_REGCNT_OFF         ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE               ( 4 )
#define MB_PDU_FUNC_READ_REGCNT_MAX         ( 0x007D )

#define MB_PDU_FUNC_WRITE_ADDR_OFF          ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_VALUE_OFF         ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_SIZE              ( 4 )

#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF      ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF    ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF   ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF    ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN      ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX    ( 0x0078 )

/* ----------------------- Start implementation -----------------------------*/

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0

eMBException
eMBFuncWriteHoldingRegister( UCHAR *pucFrame, USHORT *usLen )
{
    USHORT          usRegAddress;
    eMBException    eStatus = MB_ENOERR;
    eMBErrorCode    eRegStatus;

    if( *usLen == ( MB_PDU_FUNC_WRITE_SIZE + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress = pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF] << 8;
        usRegAddress |= pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF + 1];
        usRegAddress++;

        /* Make callback to update the value. */
        eRegStatus = eMBRegHoldingCB( &pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF], usRegAddress, 1, MB_REG_WRITE );
        switch ( eRegStatus )
        {
        case MB_ENOERR:
            /* We don't modify the length because we wan't to reply
             * the request. */
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
        /* Can't be a valid request because the length is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}
#endif

#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
eMBException
eMBFuncWriteMultipleHoldingRegister( UCHAR *pucFrame, USHORT *usLen )
{
    USHORT          usRegAddress;
    USHORT          usRegCount;
    UCHAR           ucRegByteCount;

    eMBException    eStatus = MB_ENOERR;
    eMBErrorCode    eRegStatus;

    if( *usLen >= ( MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress = pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8;
        usRegAddress |= pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1];
        usRegAddress++;

        usRegCount = pucFrame[MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF] << 8;
        usRegCount |= pucFrame[MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF + 1];

        ucRegByteCount = pucFrame[MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF];

        if( ( usRegCount >= 1 ) && ( usRegCount <= MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX )
            && ( ucRegByteCount == 2 * usRegCount ) )
        {
            /* Make callback to update the register values. */
            eRegStatus =
                eMBRegHoldingCB( &pucFrame[MB_PDU_FUNC_WRITE_MUL_VALUES_OFF], usRegAddress, usRegCount, MB_REG_WRITE );
            switch ( eRegStatus )
            {
            case MB_ENOERR:
                /* The response contains the function code, the starting
                 * address and the quantity of registers. We reuse the
                 * old values in the buffer because they are still valid.
                 */
                *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
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
        /* Can't be a valid request because the length is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}
#endif

#if MB_FUNC_READ_HOLDING_ENABLED > 0

eMBException
eMBFuncReadHoldingRegister( UCHAR *pucFrame, USHORT *usLen )
{
    USHORT          usRegAddress;
    USHORT          usRegCount;
    UCHAR          *pucFrameCur;

    eMBException    eStatus = MB_ENOERR;
    eMBErrorCode    eRegStatus;

    if( *usLen == ( MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress = pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8;
        usRegAddress |= pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1];
        usRegAddress++;

        usRegCount = pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF] << 8;
        usRegCount = pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF + 1];

        /* Check if the number of registers to read is valid. If not
         * return Modbus illegal data value exception.
         */
        if( ( usRegCount >= 1 ) && ( usRegCount <= MB_PDU_FUNC_READ_REGCNT_MAX ) )
        {
            /* Set the current PDU data pointer to the beginning. */
            pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
            *usLen = MB_PDU_FUNC_OFF;

            /* First byte contains the function code. */
            *pucFrameCur++ = MB_FUNC_READ_HOLDING_REGISTER;
            *usLen += 1;

            /* Second byte in the response contain the number of bytes. */
            *pucFrameCur++ = usRegCount * 2;
            *usLen += 1;

            /* Make callback to fill the buffer. */
            eRegStatus = eMBRegHoldingCB( pucFrameCur, usRegAddress, usRegCount, MB_REG_READ );
            switch ( eRegStatus )
            {
            case MB_ENOERR:
                *usLen += usRegCount * 2;
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
        /* Can't be a valid request because the length is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

#endif
