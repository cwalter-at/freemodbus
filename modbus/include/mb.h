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
 * File: $Id: mb.h,v 1.6 2006/02/28 00:17:50 wolti Exp $
 */

#ifndef _MB_H
#define _MB_H

#include "port.h"
#include "mbport.h"

/*! \defgroup modbus Modbus
 * \code #include "mb.h" \endcode
 *
 * This module defines the interface for the application. It contains
 * the basic functions and types required to use the Modbus protocol stack.
 * A typical application will want to call eMBInit() first. If the device
 * is ready to answer network requests it must then call eMBEnable() to activate
 * the protocol stack. In the main loop the functin eMBPool() must be called
 * periodically. The time intervall between pooling depends on the configured
 * Modbus timeout. If an RTOS is available a seperate task should be created
 * and the task should always call the function eMBPool().
 *
 * \code
 * // Initialize protocol stack in RTU mode for a slave with address 10 = 0x0A
 * eMBInit( MB_RTU, 0x0A, 38400, MB_PAR_EVEN );
 * // Enable the Modbus Protocol Stack.
 * eMBEnable(  );
 * for( ;; )
 * {
 *     // Call the main polling loop of the Modbus protocol stack.
 *     eMBPool(  );
 *     ...
 * }
 * \endcode
 */

/* ----------------------- Type definitions ---------------------------------*/

/*! \ingroup modbus
 * \brief Modbus serial transmission modes (RTU/ASCII).
 *
 * Modbus serial supports two transmission modes. Either ASCII or RTU. RTU
 * is faster but has more hardware requirements and requires a network with
 * a low jitter. ASCII is slower and more reliable on slower links (E.g. modems)
 */
typedef enum
{
    MB_RTU,                     /*!< RTU transmission mode. */
    MB_ASCII                    /*!< ASCII transmission mode. */
} eMBMode;

/*! \ingroup modbus
 * \brief If register should be written or read.
 *
 * This value is passed to the callback functions which support either
 * reading or writing register values. Writing means that the application
 * registers should be updated and reading means that the modbus protocol
 * stack needs to know the current register values.
 *
 * \see eMBRegHoldingCB( ) and eMBRegCoilsCB().
 */
typedef enum
{
    MB_REG_READ,                /*!< Read register values and pass to protocol stack. */
    MB_REG_WRITE                /*!< Update register values. */
} eMBRegisterMode;

/*! \ingroup modbus
 * \brief Errorcodes used by all function in the protocol stack.
 */
typedef enum
{
    MB_ENOERR,                  /*!< no error. */
    MB_ENOREG,                  /*!< illegal register addresss. */
    MB_EINVAL,                  /*!< illegal argument. */
    MB_EPORTERR,                /*!< porting layer error. */
    MB_ENORES,                  /*!< insufficient resources. */
    MB_EIO,                     /*!< I/O error. */
    MB_EILLSTATE,               /*!< protocol stack in illegal state. */
    MB_ETIMEDOUT                /*!< timeout error occured. */
} eMBErrorCode;

/* ----------------------- Function prototypes ------------------------------*/

/*! \ingroup modbus
 * \brief Initialize the Modbus protocol stack.
 *
 * This functions initializes the ASCII or RTU module and calls the
 * init functions of the porting layer to prepare the hardware. Please
 * note that the receiver is still disabled and no Modbus frames are
 * processed until eMBEnable() has been called.
 *
 * \param eMode If ASCII or RTU mode should be used.
 * \param ucSlaveAddress The slave address. Only frames sent to this
 *   address or to the broadcast address are processed.
 * \param ulBaudRate The baudrate. E.g. 19200. Supported baudrates depend
 *   on the porting layer.
 * \param eParity Parity used for serial transmission.
 *
 * \return If no error occurs the function returns eMBErrorCode::MB_ENOERR.
 *   Otherwise one of the following error codes is returned:
 *    - eMBErrorCode::MB_EINVAL If the slave address was not valid. Valid
 *        slave addresses are in the range 1 - 247.
 *    - eMBErrorCode::MB_EPORTERR IF the porting layer returned an error.
 */
eMBErrorCode    eMBInit( eMBMode eMode, UCHAR ucSlaveAddress, ULONG ulBaudRate, eMBParity eParity );

/*! \ingroup modbus
 * \brief Enable the Modbus protocol stack.
 *
 * This function enabled processing of Modbus frames. eMBInit() must be
 * called before.
 *
 * \return If no error occurs the function returns eMBErrorCode::MB_ENOERR. If
 *   eMBInit() was not called before ir returns eMBErrorCode::MB_EILLSTATE.
 */
eMBErrorCode    eMBEnable( void );

/*! \ingroup modbus
 * \brief The main pooling loop of the Modbus protocol stack.
 *
 * This function must be called periodically. The timer intervall required
 * is given by the configured Modbus slave timeout. Internally the function
 * calls xMBPortEventGet() and waits for an event from the receiver or
 * transmitter state machines. Depending upon the port dependent implementation
 * of xMBPortEventGet() other processing might take place in the meantime.
 *
 * \return If no error occured the function returns eMBErrorCode::MB_ENOERR.
 */
eMBErrorCode    eMBPool( void );

/*! \ingroup modbus
 * \brief Configure the slave id of the device.
 *
 * This function should be called when the Modbus function <em>Report Slave ID</em>
 * is enabled ( By defining MB_FUNC_OTHER_REP_SLAVEID_ENABLED in mbconfig.h ).
 *
 * \param ucSlaveID Values is returned in the <em>Slave ID</em> byte of the
 *   <em>Report Slave ID</em> response.
 * \param xIsRunning If TRUE the <em>Run Indicator Status</em> byte is set to 0xFF.
 *   otherwise the <em>Run Indicator Status</em> is 0x00.
 * \param pucAdditional Values which should be returned in the <em>Additonal</em>
 *   bytes of the <em> Report Slave ID</em> response.
 * \param usAdditionalLen Length of the buffer <code>pucAdditonal</code>.
 *
 * \return
 */
eMBErrorCode    eMBSetSlaveID( UCHAR ucSlaveID, BOOL xIsRunning, UCHAR const *pucAdditional, USHORT usAdditionalLen );

/* ----------------------- Callback -----------------------------------------*/

/*! \defgroup modbus_registers Modbus Registers
 * \code #include "mb.h" \endcode
 * The protocol stack does not internally allocate any memory for the
 * register. This makes the protocol stack very small and also usable on
 * low end targets. In addition the values don't have to be in the memory
 * and could be stored in a serial flash for example.<br>
 * Whenever the protocol stack requires a value if calls one of the callback
 * function with the register address and the number of registers to read
 * as an argument. The application should then read the actual register values
 * (for example the ADC voltage) and should store the result in the supplied
 * buffer.<br>
 * If the protocol stack wants to update a register value because a write
 * register function was received a buffer with the new register values is
 * passed to the callback function. The function should then use these values
 * to update the register values.
 */

/*! \ingroup modbus_registers
 * \brief Callback function used if the value of a <em>Input Register</em>
 *   is required by the protocol stack.
 *
 * \param pucRegBuffer The actual values of the registers starting at
 *   <code>usAddress</code> should be written to this buffer.
 * \param usAddress The starting address of the register.
 * \param usNRegs Number of registers values requested. I.e. first register
 *   is given by <code>usAddress</code> and last by
 *   <code>usAddress + usNRegs</code>
 * \return The function must return one of the following error codes:
 *   - eMBErrorCode::MB_ENOERR If no error occured. In this case a normal
 *       Modbus response is sent.
 *   - eMBErrorCode::MB_ENOREG If no register on this address is available.
 *       In this case a <b>ILLEGAL DATA ADDRESS</b> is sent as a response.
 *   - eMBErrorCode::MB_ETIMEDOUT If the requested register block is
 *       currently not available and the application dependent response
 *       timeout would be violated. In this case a <b>SLAVE DEVICE BUSY</b>
 *       exception is sent as a response.
 *   - eMBErrorCode::MB_EIO If an unrecoverable error occured. In this case
 *       a <b>SLAVE DEVICE FAILURE</b> exception is sent as a response.
 */
eMBErrorCode    eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs );

/*! \ingroup modbus_registers
 * \brief Callback function used if a <em>Holding Register</em> value is
 *   read or written by the protocol stack.
 *
 * \param pucRegBuffer The actual values of the registers starting at
 *   <code>usAddress</code> should be written to this buffer.
 * \param usAddress The starting address of the register.
 * \param usNRegs Number of registers values requested. I.e. first register
 *   is given by <code>usAddress</code> and last by
 *   <code>usAddress + usNRegs</code>
 * \param eMode If eMBRegisterMode::MB_REG_WRITE the register value should be
 *   updated. In this case the application would read the new register values
 *   from the buffer <code>pucRegBuffer</code>. If
 *   eMBRegisterMode::MB_REG_READ the applicatin should store the current
 *   values in the buffer <code>pucRegBuffer</code>.
 * \return The function must return one of the following error codes:
 *   - eMBErrorCode::MB_ENOERR If no error occured. In this case a normal
 *       Modbus response is sent.
 *   - eMBErrorCode::MB_ENOREG If no register on this address is available.
 *       In this case a <b>ILLEGAL DATA ADDRESS</b> is sent as a response.
 *   - eMBErrorCode::MB_ETIMEDOUT If the requested register block is
 *       currently not available and the application dependent response
 *       timeout would be violated. In this case a <b>SLAVE DEVICE BUSY</b>
 *       exception is sent as a response.
 *   - eMBErrorCode::MB_EIO If an unrecoverable error occured. In this case
 *       a <b>SLAVE DEVICE FAILURE</b> exception is sent as a response.
 */
eMBErrorCode    eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode );

/*! \ingroup modbus_registers
 * \brief Callback function used if a <em>Coil Register</em> value is
 *   read or written by the protocol stack.
 *
 * \param pucRegBuffer The bits are packed in bytes where the first coil
 *   starting at address <code>usAddress</code> is stored at the LSB of the
 *   first byte in the buffer <code>pucRegBuffer</code>.
 *   If the buffer should be written by the callback function unused
 *   coil values (I.e. if not a multiple of eight coils is used) should be set
 *   to zero.
 * \param usAddress The starting address of the register.
 * \param usNCoils Number of coil registers values requested. I.e. first register
 *   is given by <code>usAddress</code> and last by
 *   <code>usAddress + usNRegs</code>.
 * \param eMode If eMBRegisterMode::MB_REG_WRITE the register value should be
 *   updated. In this case the application would read the new register values
 *   from the buffer <code>pucRegBuffer</code>. If
 *   eMBRegisterMode::MB_REG_READ the applicatin should store the current
 *   values in the buffer <code>pucRegBuffer</code>.
 * \return The function must return one of the following error codes:
 *   - eMBErrorCode::MB_ENOERR If no error occured. In this case a normal
 *       Modbus response is sent.
 *   - eMBErrorCode::MB_ENOREG If no register on this address is available.
 *       In this case a <b>ILLEGAL DATA ADDRESS</b> is sent as a response.
 *   - eMBErrorCode::MB_ETIMEDOUT If the requested register block is
 *       currently not available and the application dependent response
 *       timeout would be violated. In this case a <b>SLAVE DEVICE BUSY</b>
 *       exception is sent as a response.
 *   - eMBErrorCode::MB_EIO If an unrecoverable error occured. In this case
 *       a <b>SLAVE DEVICE FAILURE</b> exception is sent as a response.
 */
eMBErrorCode    eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode );

eMBErrorCode    eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete );
#endif
