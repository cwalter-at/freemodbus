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
 * File: $Id: mbrtu.h,v 1.8 2006/11/01 12:08:20 wolti Exp $
 */

#ifndef _MB_RTU_H
#define _MB_RTU_H

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_BEGIN_EXTERN_C
/* *INDENT-ON* */
#endif

eMBErrorCode    eMBRTUInit( UCHAR slaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity );
void            eMBRTUStart( void );
void            eMBRTUStop( void );
eMBErrorCode    eMBRTUReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength );
eMBErrorCode    eMBRTUSend( UCHAR slaveAddress, const UCHAR * pucFrame, USHORT usLength );
BOOL            xMBRTUReceiveFSM( void );
BOOL            xMBRTUTransmitFSM( void );
BOOL            xMBRTUTimerT15Expired( void );
BOOL            xMBRTUTimerT35Expired( void );

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_END_EXTERN_C
/* *INDENT-ON* */
#endif

#endif
