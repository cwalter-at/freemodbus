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
#define MB_PDU_FUNC_READ_COILCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE               ( 4 )
#define MB_PDU_FUNC_READ_COILCNT_MAX        ( 0x07D0 )

#define MB_PDU_FUNC_WRITE_ADDR_OFF          ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_VALUE_OFF         ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_SIZE              ( 4 )

#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF      ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF   ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF   ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF    ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN      ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_COILCNT_MAX   ( 0x07B0 )

/* ----------------------- Static functions ---------------------------------*/
eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );

/* ----------------------- Start implementation -----------------------------*/

#if MB_FUNC_READ_COILS_ENABLED > 0

eMBException
eMBFuncReadCoils( UCHAR *pucFrame, USHORT *usLen )
{
    USHORT          usRegAddress;
    USHORT          usCoilCount;
    UCHAR           ucNBytes;
    UCHAR          *pucFrameCur;

    eMBException    eStatus = MB_EX_NONE;
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
                ucNBytes = ( UCHAR ) ( usCoilCount / 8 + 1 );
            }
            else
            {
                ucNBytes = ( UCHAR ) ( usCoilCount / 8 );
            }
            *pucFrameCur++ = ucNBytes;
            *usLen += 1;

            eRegStatus = eMBRegCoilsCB( pucFrameCur, usRegAddress, usCoilCount, MB_REG_READ );

            /* If an error occured convert it into a Modbus exception. */
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBError2Exception( eRegStatus );
            }
            else
            {
                /* The response contains the function code, the starting address
                 * and the quantity of registers. We reuse the old values in the
                 * buffer because they are still valid. */
                *usLen += ucNBytes;
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

#if MB_FUNC_WRITE_COIL_ENABLED > 0
eMBException
eMBFuncWriteCoil( UCHAR *pucFrame, USHORT *usLen )
{
    USHORT          usRegAddress;
    UCHAR           ucBuf[2];

    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;

    if( *usLen == ( MB_PDU_FUNC_WRITE_SIZE + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress = pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF] << 8;
        usRegAddress |= pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF + 1];
        usRegAddress++;

        if( ( pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF + 1] == 0x00 )
            && ( ( pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF] == 0xFF )
                 || ( pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF] == 0x00 ) ) )
        {
            ucBuf[1] = 0;
            if( pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF] == 0xFF )
            {
                ucBuf[0] = 1;
            }
            else
            {
                ucBuf[0] = 0;
            }
            eRegStatus = eMBRegCoilsCB( &ucBuf[0], usRegAddress, 1, MB_REG_WRITE );

            /* If an error occured convert it into a Modbus exception. */
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBError2Exception( eRegStatus );
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    {
        /* Can't be a valid write coil register request because the length
         * is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

#endif

#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
eMBException
eMBFuncWriteMultipleCoils( UCHAR *pucFrame, USHORT *usLen )
{
    USHORT          usRegAddress;
    USHORT          usCoilCnt;
    UCHAR           ucByteCount;
    UCHAR           ucByteCountVerify;

    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;

    if( *usLen > ( MB_PDU_FUNC_WRITE_SIZE + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress = pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF] << 8;
        usRegAddress |= pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF + 1];
        usRegAddress++;

        usCoilCnt = pucFrame[MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF] << 8;
        usCoilCnt |= pucFrame[MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF + 1];

        ucByteCount = pucFrame[MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF];

        /* Compute the number of expected bytes in the request. */
        if( ( usCoilCnt & 0x0007 ) != 0 )
        {
            ucByteCountVerify = ( UCHAR ) ( usCoilCnt / 8 + 1 );
        }
        else
        {
            ucByteCountVerify = ( UCHAR ) ( usCoilCnt / 8 );
        }

        if( ( usCoilCnt >= 1 ) && ( usCoilCnt <= MB_PDU_FUNC_WRITE_MUL_COILCNT_MAX )
            && ( ucByteCountVerify == ucByteCount ) )
        {
            eRegStatus =
                eMBRegCoilsCB( &pucFrame[MB_PDU_FUNC_WRITE_MUL_VALUES_OFF], usRegAddress, usCoilCnt, MB_REG_WRITE );

            /* If an error occured convert it into a Modbus exception. */
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBError2Exception( eRegStatus );
            }
            else
            {
                /* The response contains the function code, the starting address
                 * and the quantity of registers. We reuse the old values in the
                 * buffer because they are still valid. */
                *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    {
        /* Can't be a valid write coil register request because the length
         * is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

#endif

#endif
