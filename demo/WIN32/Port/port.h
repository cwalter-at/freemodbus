/*
 * FreeModbus Library: Win32 Port
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
 * File: $Id: port.h,v 1.2 2006/06/17 00:18:19 wolti Exp $
 */

#ifndef _PORT_H
#define _PORT_H

#include <windows.h>
#include <tchar.h>
#include <assert.h>

#define INLINE
#define PR_BEGIN_EXTERN_C           extern "C" {
#define PR_END_EXTERN_C             }

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_BEGIN_EXTERN_C
/* *INDENT-ON* */
#endif
#define ENTER_CRITICAL_SECTION( )
#define EXIT_CRITICAL_SECTION( )
#define MB_PORT_HAS_CLOSE   1
#ifndef TRUE
#define TRUE            1
#endif
#ifndef FALSE
#define FALSE           0
#endif
#ifdef _DEBUG
void            TRACEC( const TCHAR * pcFmt, ... );
void            ERRORC( const TCHAR * pcFmt, DWORD dwError, ... );
#else
#define         TRACEC( pcFmt, ... )
#define         ERRORC( pcFmt, dwError, ... )
#endif
void            vMBPortTimerPoll(  );
BOOL            xMBPortSerialPoll(  );
BOOL            xMBPortSerialSetTimeout( DWORD dwTimeoutMs );
SHORT           xMBPortStartPoolingThread(  );
SHORT           xMBPortStopPoolingThread(  );

#ifdef __cplusplus
/* *INDENT-OFF* */
PR_END_EXTERN_C
/* *INDENT-ON* */
#endif

#endif
