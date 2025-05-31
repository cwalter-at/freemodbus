/*
 * FreeModbus Libary: STM32 Cmake Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 * Copyright (C) 2025 Arend Lammertink <a.lammertink@ara-industries.nl>
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
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port_internal.h"

// Include BSP package
#if defined(STM32G4xx) || defined(STM32G431xx)
  #include "stm32g4xx_nucleo.h"
#endif

#include <stdio.h>

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4
#define SLAVE_ID        0x0A
#define MB_BAUDRATE     115200
//#define MB_BAUDRATE     9600

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];

/* ----------------------- Static functions ---------------------------------*/

/* --------------------- Forward declarations -------------------------------*/

void Error_Handler(void); // standard defined in main.h

/* ----------------Modbus register callback functions -----------------------*/
eMBErrorCode eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs);
eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode);
eMBErrorCode eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode);
eMBErrorCode eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete);


static uint32_t lastToggleTime; 

/* ----------------------- Public API functions -----------------------------*/
/**
  * @brief  Initialize Modbus functionality
  * @note   Call this once during system initialization
  * @retval None
  */
void modbusInit(void)
{
    printf("STM32 Modbus RTU Slave Example\r\n");

    // Deinitialize COM port (initialized by BSP) for use with Modbus
    HAL_Delay(100); // Wait for message to be sent
    BSP_COM_DeInit(COM1); 

    eMBErrorCode eStatus;
    
    MB_Uart_Init();

    /* Initialize Modbus protocol stack */
    eStatus = eMBInit(MB_RTU, SLAVE_ID, 0, MB_BAUDRATE, MB_PAR_NONE);
    //eStatus = eMBInit(MB_ASCII, SLAVE_ID, 0, MB_BAUDRATE, MB_PAR_EVEN);
    
    /* Initialize register values */
    usRegInputBuf[0] = 0;  /* Initialize counter */
    usRegInputBuf[1] = 0;
    usRegInputBuf[2] = 0;
    usRegInputBuf[3] = 0;
    
    /* Enable Modbus protocol stack */
    eStatus = eMBEnable();

    if( eStatus != MB_ENOERR )
    {
        Error_Handler();
    }

    lastToggleTime = HAL_GetTick(); 
}

/**
  * @brief  Poll Modbus protocol stack
  * @note   Call this regularly in main loop
  * @retval None
  */
void modbusPoll(void)
{
    /* Process Modbus events */
    (void)eMBPoll();
    
    /* Update the first register as a counter */
    usRegInputBuf[0]++;

    /* Toggle LED for visual feedback every 500 ms */
    if (HAL_GetTick() - lastToggleTime >= 500)
    {
#ifndef MB_TIMER_DEBUG
        BSP_LED_Toggle(LED_GREEN);
#endif    
        lastToggleTime = HAL_GetTick();
    }
}



/* ----------------------- Modbus register callback functions ---------------------------------*/
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    UNUSED( pucRegBuffer );
    UNUSED( usAddress );
    UNUSED( usNRegs );
    UNUSED( eMode );

    return MB_ENOREG;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    UNUSED( pucRegBuffer );
    UNUSED( usAddress );
    UNUSED( usNCoils );
    UNUSED( eMode );

    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    UNUSED( pucRegBuffer );
    UNUSED( usAddress );
    UNUSED( usNDiscrete );

    return MB_ENOREG;
}

