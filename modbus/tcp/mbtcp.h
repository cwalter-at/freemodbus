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
 * File: $Id: mbtcp.h,v 1.1 2006/06/25 00:06:07 wolti Exp $
 */

#ifndef _MB_TCP_H
#define _MB_TCP_H

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_BEGIN_EXTERN_C
/* *INDENT-ON* */
#endif

/* ----------------------- Defines ------------------------------------------*/
#define MB_TCP_PSEUDO_ADDRESS   255

/* ----------------------- Function prototypes ------------------------------*/
eMBErrorCode    eMBTCPDoInit( USHORT ucTCPPort );
void            eMBTCPStart( void );
void            eMBTCPStop( void );
eMBErrorCode    eMBTCPReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength );
eMBErrorCode    eMBTCPSend( UCHAR _unused, const UCHAR * pucFrame, USHORT usLength );

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_END_EXTERN_C
/* *INDENT-ON* */
#endif

#endif
