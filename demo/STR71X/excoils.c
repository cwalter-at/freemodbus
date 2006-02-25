/*
 * FreeRTOS Modbus Library demo application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: excoils.c,v 1.2 2006/02/25 18:34:08 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "assert.h"

/* ----------------------- Platform includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ----------------------- STR71X includes ----------------------------------*/
#include "eic.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_COILS_START     1000
#define REG_COILS_SIZE      16

/* ----------------------- Static variables ---------------------------------*/
static unsigned portSHORT usRegCoilsStart = REG_COILS_START;
static unsigned portCHAR ucRegCoilsBuf[REG_COILS_SIZE] = { 0xA5, 0x5A };

/* ----------------------- Static functions ---------------------------------*/
static void     vModbusTask( void *pvParameters );
static unsigned portCHAR prvucGetCoilValue( unsigned portSHORT usAddress );

/* ----------------------- Start implementation -----------------------------*/
int
main( void )
{
    EIC_Init(  );
    EIC_IRQConfig( ENABLE );

    ( void )xTaskCreate( vModbusTask, NULL, configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

    vTaskStartScheduler(  );
    return 0;
}

static void
vModbusTask( void *pvParameters )
{
    int             i;
    portTickType    xLastWakeTime;

    /* Select either ASCII or RTU Mode. */
    eMBInit( MB_RTU, 0x0A, 9600, MB_PAR_EVEN );

    /* Enable the Modbus Protocol Stack. */
    eMBEnable(  );
    for( ;; )
    {
        /* Call the main polling loop of the Modbus protocol stack. */
        eMBPool(  );
    }
}

unsigned        portCHAR
prvucGetCoilValue( unsigned portSHORT usAddress )
{
    unsigned portCHAR ucValue = 0;
    int             iCoilIndex;
    int             iCoilOffset;

    iCoilIndex = ( usAddress - usRegCoilsStart ) / 8;
    iCoilOffset = ( usAddress - usRegCoilsStart ) % 8;

    ucValue = ucRegCoilsBuf[iCoilIndex] >> iCoilOffset;

    /* If address is not a multiple of eight some other values might
     * be stored in the next coil buffer register. */
    if( ( iCoilOffset != 0 ) && ( iCoilIndex < REG_COILS_SIZE / 8 ) )
    {
        /* Add the values from the next coil buffer register. */
        ucValue |= ucRegCoilsBuf[iCoilIndex + 1] << ( 8 - iCoilOffset );
    }
    return ucValue;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iNCoils = usNCoils;

    unsigned portSHORT usEndAddress = usAddress + usNCoils;

    if( ( usAddress >= REG_COILS_START ) && ( usEndAddress <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        while( usAddress < usEndAddress )
        {
            *pucRegBuffer++ = prvucGetCoilValue( usAddress );
            usAddress += 8;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegInputCB( UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

void
__assert( const char *pcFile, const char *pcLine, int iLineNumber )
{
    portENTER_CRITICAL(  );
    while( 1 );
}
