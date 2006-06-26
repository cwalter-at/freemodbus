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
 * File: $Id: mbascii.h,v 1.7 2006/06/18 09:57:03 wolti Exp $
 */

#ifndef _MB_ASCII_H
#define _MB_ASCII_H

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_BEGIN_EXTERN_C
/* *INDENT-ON* */
#endif

#if MB_ASCII_ENABLED > 0
eMBErrorCode    eMBASCIIInit( UCHAR slaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity );
void            eMBASCIIStart( void );
void            eMBASCIIStop( void );

eMBErrorCode    eMBASCIIReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength );
eMBErrorCode    eMBASCIISend( UCHAR slaveAddress, const UCHAR * pucFrame, USHORT usLength );
BOOL            xMBASCIIReceiveFSM( void );
BOOL            xMBASCIITransmitFSM( void );
BOOL            xMBASCIITimerT1SExpired( void );
#endif

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_END_EXTERN_C
/* *INDENT-ON* */
#endif

#endif
