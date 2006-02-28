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
 *
 * File: $Id: mbfunc.h,v 1.7 2006/02/28 22:41:37 wolti Exp $
 */

#ifndef _MB_FUNC_H
#define _MB_FUNC_H

#if MB_FUNC_OTHER_REP_SLAVEID_BUF > 0
eMBException    eMBFuncReportSlaveID( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_INPUT_ENABLED > 0
eMBException    eMBFuncReadInputRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_HOLDING_ENABLED > 0
eMBException    eMBFuncReadHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
eMBException    eMBFuncWriteHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
eMBException    eMBFuncWriteMultipleHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_COILS_ENABLED > 0
eMBException    eMBFuncReadCoils( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_COIL_ENABLED > 0
eMBException    eMBFuncWriteCoil( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
eMBException    eMBFuncWriteMultipleCoils( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
eMBException    eMBFuncReadDiscreteInputs( UCHAR * pucFrame, USHORT * usLen );
#endif

#endif
