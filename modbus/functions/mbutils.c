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
#include "mbproto.h"

/* ----------------------- Defines ------------------------------------------*/
#define BITS_UCHAR      8U

/* ----------------------- Start implementation -----------------------------*/
void
xMBUtilSetBits( UCHAR *ucByteBuf, USHORT usBitOffset, UCHAR usNBits, UCHAR ucValue )
{
    USHORT          usWordBuf;
    USHORT          usMask;
    USHORT          usByteOffset;
    USHORT          usNPreBits;
    USHORT          usValue = ucValue;

    assert( usNBits <= 8 );
    assert( ( size_t )BITS_UCHAR == sizeof( UCHAR ) * 8 );

    /* Calculate byte offset for first byte containing the bit values starting
     * at usBitOffset. */
    usByteOffset = ( USHORT ) ( ( usBitOffset ) / BITS_UCHAR );

    /* How many bits precede our bits to set. */
    usNPreBits = ( USHORT ) ( usBitOffset - usByteOffset * BITS_UCHAR );

    /* Move bit field into position over bits to set */
    usValue <<= usNPreBits;

    /* Prepare a mask for setting the new bits. */
    usMask = ( USHORT ) ( ( 1 << ( USHORT ) usNBits ) - 1 );
    usMask <<= usBitOffset - usByteOffset * BITS_UCHAR;

    /* copy bits into temporary storage. */
    usWordBuf = ucByteBuf[usByteOffset];
    usWordBuf |= ucByteBuf[usByteOffset + 1] << BITS_UCHAR;

    /* Zero out bit field bits and then or value bits into them. */
    usWordBuf = ( usWordBuf & ( ~usMask ) ) | usValue;

    /* move bits back into storage */
    ucByteBuf[usByteOffset] = ( UCHAR ) ( usWordBuf & 0xFF );
    ucByteBuf[usByteOffset + 1] = usWordBuf >> BITS_UCHAR;
}

UCHAR
xMBUtilGetBits( UCHAR *ucByteBuf, USHORT usBitOffset, UCHAR usNBits )
{
    USHORT          usWordBuf;
    USHORT          usMask;
    USHORT          usByteOffset;
    USHORT          usNPreBits;

    /* Calculate byte offset for first byte containing the bit values starting
     * at usBitOffset. */
    usByteOffset = ( USHORT ) ( ( usBitOffset ) / BITS_UCHAR );

    /* How many bits precede our bits to set. */
    usNPreBits = ( USHORT ) ( usBitOffset - usByteOffset * BITS_UCHAR );

    /* Prepare a mask for setting the new bits. */
    usMask = ( USHORT ) ( ( 1 << ( USHORT ) usNBits ) - 1 );

    /* copy bits into temporary storage. */
    usWordBuf = ucByteBuf[usByteOffset];
    usWordBuf |= ucByteBuf[usByteOffset + 1] << BITS_UCHAR;

    /* throw away unneeded bits. */
    usWordBuf >>= usNPreBits;

    /* mask away bits above the requested bitfield. */
    usWordBuf &= usMask;

    return ( UCHAR ) usWordBuf;
}

eMBException
prveMBError2Exception( eMBErrorCode eErrorCode )
{
    eMBException    eStatus;

    switch ( eErrorCode )
    {
    case MB_ENOERR:
        eStatus = MB_EX_NONE;
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

    return eStatus;
}
