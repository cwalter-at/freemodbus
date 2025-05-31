/*
 * FreeModbus Libary: BARE Port
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
 * File: $Id$
 */

#ifndef _PORT_H
#define _PORT_H

#include <assert.h>
#include <inttypes.h>

#include <aduc7026.h>

#define 	UCLK 	41779200L		// 32768 * 1275 = 41.78MHz +/- 3%

#define OSCI_HIGHSPEED     
// 
// Clock must be switched to 41.78MHz in main initialization code by:
//
//  	POWKEY1 = 0x01;       // Overwrite protection 
//  	POWCON  = 0x00;       // Switch clockdivider to 41.78MHz
//  	POWKEY2 = 0xF4;      	// Overwrite protection

#ifdef OSCI_HIGHSPEED 			// 41.78MHz clock speed, POWCON = 0x00;
	#define	HCLK	UCLK
#else 											// default 5.22 MHz
	#define	HCLK	(UCLK/8)
#endif // OSCI_HIGHSPEED


#define	INLINE
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }


#define ENTER_CRITICAL_SECTION( )		EnterCriticalSection( )
#define EXIT_CRITICAL_SECTION( )    ExitCriticalSection( )



extern int			IRQ_Temp;			// Temporary copy from IRQEN when in crictical section
extern int			FIRQ_Temp;		// Temporary copy from FIQEN when in crictical section
void            EnterCriticalSection( void );
void            ExitCriticalSection( void );

typedef uint8_t BOOL;
extern BOOL			InCriticalSection;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#endif
