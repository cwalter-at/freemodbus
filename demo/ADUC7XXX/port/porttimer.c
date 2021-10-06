/*
 * FreeModbus Libary: ADuC7xxx Port
 * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>,
 *							 2021 Arend Lammertink <lamare@gmail.com>.
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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

static uint32_t timer1_LD = 0;  // Load value for timer 1, T1LD register

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	
		//T1LD = 131 * usTim1Timerout50us;    // Interval of (50us * usTim1Timerout50us) for prescale HCLK/16
		timer1_LD = 131 * usTim1Timerout50us; // Interval of (50us * usTim1Timerout50us) for prescale HCLK/16

		T1CON 		= 0x44;											// Disabled,Periodic,Binary and HCLK/16

		IRQ 			= prvvTIMERExpiredISR;			// Use normal interrupt for timer1.
 
//		GP4DAT 	 &= ~0x00040000;							// Reset P4.2

    return TRUE;
}


void
vMBPortTimersEnable(  )
{
		T1LD 	 = timer1_LD;
	  T1CON |= 0x80;            		// Enable Timer

		if ( InCriticalSection )
			IRQ_Temp |= GP_TIMER_BIT;				// Enable Timer1 IRQ when leaving critical section
		else
			IRQEN 	 |= GP_TIMER_BIT;				// Enable Timer1 IRQ
	
//		GP4DAT |= 0x00040000;				// Set P4.2
}

void
vMBPortTimersDisable()
{

	  T1CON &= ~0x80;            		// Disable Timer
	
		IRQ_Temp &= ~GP_TIMER_BIT;		// Disable Timer1 IRQ when leaving critical section
		IRQEN 	 &= ~GP_TIMER_BIT;		// Disable Timer1 IRQ
    IRQCLR   	= GP_TIMER_BIT;  		// Disable Timer1 IRQ 
		T1CLRI 	  = 0;								// Clear Timer IRQ
	
//    GP4DAT &= ~0x00040000;				// Reset P4.2
}

static void prvvTIMERExpiredISR( void )
{
//		GP4DAT |= 0x00080000;				// Set P4.3

		( void )pxMBPortCBTimerExpired(  );
	
//    GP4DAT &= ~0x00080000;				// Reset P4.3
	
		vMBPortTimersDisable();
}
