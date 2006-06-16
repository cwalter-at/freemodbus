/*
 * FreeModbus Library: A portable Modbus implementation for Modbus ASCII/RTU.
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
 * File: $Id: mbproto.h,v 1.9 2006/06/16 00:08:52 wolti Exp $
 */

#ifndef _MB_PROTO_H
#define _MB_PROTO_H

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_BEGIN_EXTERN_C
/* *INDENT-ON* */
#endif

/* ----------------------- Defines ------------------------------------------*/
#define MB_ADDRESS_BROADCAST    ( 0 )   /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN          ( 1 )   /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX          ( 247 ) /*! Biggest possible slave address. */
#define MB_FUNC_NONE                        (  0 )
#define MB_FUNC_READ_COILS                  (  1 )
#define MB_FUNC_READ_DISCRETE_INPUTS        (  2 )
#define MB_FUNC_WRITE_SINGLE_COIL           (  5 )
#define MB_FUNC_WRITE_MULTIPLE_COILS        ( 15 )
#define MB_FUNC_READ_HOLDING_REGISTER       (  3 )
#define MB_FUNC_READ_INPUT_REGISTER         (  4 )
#define MB_FUNC_WRITE_REGISTER              (  6 )
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS    ( 16 )
#define MB_FUNC_DIAG_READ_EXCEPTION         (  7 )
#define MB_FUNC_DIAG_DIAGNOSTIC             (  8 )
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT      ( 11 )
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG      ( 12 )
#define MB_FUNC_OTHER_REPORT_SLAVEID        ( 17 )
#define MB_FUNC_ERROR                       ( 128 )

/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    MB_EX_NONE = 0x00,
    MB_EX_ILLEGAL_FUNCTION = 0x01,
    MB_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MB_EX_ILLEGAL_DATA_VALUE = 0x03,
    MB_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MB_EX_ACKNOWLEDGE = 0x05,
    MB_EX_SLAVE_BUSY = 0x06,
    MB_EX_MEMORY_PARITY_ERROR = 0x08,
    MB_EX_GATEWAY_PATH_FAILED = 0x0A,
    MB_EX_GATEWAY_TGT_FAILED = 0x0B,
} eMBException;

typedef         eMBException( *pxMBFunctionHandler ) ( UCHAR * pucFrame, USHORT * usLen );

typedef struct
{
    UCHAR           ucFunctionCode;
    pxMBFunctionHandler pxHandler;
} xMBFunctionHandler;

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_END_EXTERN_C
/* *INDENT-ON* */
#endif

#endif
