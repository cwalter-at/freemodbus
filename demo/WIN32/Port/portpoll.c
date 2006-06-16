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
 * File: $Id: portpoll.c,v 1.1 2006/06/16 00:13:39 wolti Exp $
 */

#include <windows.h>

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Type definitions ---------------------------------*/
enum ThreadState
{
    STOPPED,
    RUNNING,
    SHUTDOWN
};

/* ----------------------- Static variables ---------------------------------*/
static HANDLE   m_hThread;
static DWORD    m_dwThreadId;
static enum ThreadState m_eState;

/* ----------------------- Static functions ---------------------------------*/
static enum ThreadState prveMBPortGetThreadState(  );
static void     prveMBPortSetThreadState( enum ThreadState );
static DWORD WINAPI prvdwMBPortPollingThread( LPVOID );

/* ----------------------- Begin implementation -----------------------------*/
SHORT
xMBPortStartPoolingThread(  )
{
    eMBErrorCode    eStatus;

    if( prveMBPortGetThreadState(  ) != STOPPED )
    {
        /* already running */
        eStatus = MB_EILLSTATE;
    }
    if( eMBEnable(  ) != MB_ENOERR )
    {
        /* can't start Modbus protocol stack. */
        eStatus = MB_EILLSTATE;
    }
    if( ( m_hThread = CreateThread( NULL, 0, prvdwMBPortPollingThread, NULL, 0, &m_dwThreadId ) ) == NULL )
    {
        /* can't create modbus polling thread. */
        eStatus = MB_ENORES;
    }
    else
    {
        TRACEC( L"CFreeModbusThread::start: waiting for protocol stack to become ready\r\n" );

        eStatus = MB_ENOERR;
    }
    return eStatus;
}

SHORT
xMBPortStopPoolingThread(  )
{
    eMBErrorCode    eStatus;

    if( prveMBPortGetThreadState(  ) != RUNNING )
        eStatus = MB_EILLSTATE;
    else
    {
        prveMBPortSetThreadState( SHUTDOWN );

        TRACEC( L"CFreeModbusThread::start: waiting for protocol to shutdown\r\n" );
        eStatus = MB_ENOERR;
    }
    return eStatus;
}

DWORD           WINAPI
prvdwMBPortPollingThread( LPVOID lpParameter )
{
    prveMBPortSetThreadState( RUNNING );
    TRACEC( L"PoolingThread: signaling that main thread is ready.\r\n" );
    do
    {
        xMBPortSerialPoll(  );
        /* If a frame has been received this is checked in the next call
         * to eMBPoll.
         */
        eMBPoll(  );
        /* We execute again to let the protocol stack generate the anwser.
         * This seems not very logical but is needed because there is an
         * additional step between EV_FRAME_RECEIVED and EV_EXECUTE.
         */
        eMBPoll(  );
    }
    while( prveMBPortGetThreadState(  ) != SHUTDOWN );

    TRACEC( L"PoolingThread: signaling that shutdown is finished.\r\n" );
    return 0;
}

enum ThreadState
prveMBPortGetThreadState(  )
{
    enum ThreadState eState;

    // variable access not locked. make a copy
    eState = m_eState;

    return eState;
}

void
prveMBPortSetThreadState( enum ThreadState eNewState )
{
    // variable access not locked. make a copy
    m_eState = eNewState;
}
