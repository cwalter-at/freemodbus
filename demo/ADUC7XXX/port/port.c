 /*
  * FreeModbus Libary: ADuC7xxx Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>,
	*								2021 Arend Lammertink <lamare@gmail.com>.
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

/* ----------------------- System includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

/* ----------------------- Variables ----------------------------------------*/
int             IRQ_Temp;
int             FIRQ_Temp;
BOOL 						InCriticalSection = FALSE;

/* ----------------------- Start implementation -----------------------------*/


/*
		Note that to clear an already enabled interrupt source, the user 
		must set the appropriate bit in the IRQCLR register. Clearing an 
		interrupt’s IRQEN bit does not disable the interrupt.
*/

void EnterCriticalSection(  )
{
    IRQ_Temp  = IRQEN;   	/* Save IRQEN */
		FIRQ_Temp = FIQEN;
	
    IRQCLR   = IRQ_Temp;  /* Disable Interrupts */
		FIQCLR	 = FIRQ_Temp;
	
	  InCriticalSection = TRUE;
}



void ExitCriticalSection(  )
{
		IRQEN = IRQ_Temp;    	/* Restore IRQEN */
		FIQEN = FIRQ_Temp;
	
		InCriticalSection = FALSE;
}

