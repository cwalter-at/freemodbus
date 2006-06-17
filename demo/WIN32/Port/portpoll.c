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
 * File: $Id: portpoll.c,v 1.2 2006/06/17 00:16:31 wolti Exp $
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
static CRITICAL_SECTION hLock;
static HANDLE   hEvents;
static HANDLE   hThread;
static DWORD    dwThreadId;
static enum ThreadState eState;

/* ----------------------- Static functions ---------------------------------*/
static enum ThreadState prveMBPortGetThreadState(  );
static void     prveMBPortSetThreadState( enum ThreadState );
static DWORD WINAPI prvdwMBPortPollingThread( LPVOID );

/* ----------------------- Begin implementation -----------------------------*/
SHORT
xMBPortStartPoolingThread(  )
{
    eMBErrorCode    eStatus;
    DWORD           dwWaitResult;

    InitializeCriticalSection( &hLock );

    if( prveMBPortGetThreadState(  ) != STOPPED )
    {
        eStatus = MB_EILLSTATE;
    }
    else if( ( hEvents = CreateEvent( NULL, FALSE, FALSE, NULL ) ) == NULL )
    {
        eStatus = MB_ENORES;
    }
    else if( ( hThread = CreateThread( NULL, 0, prvdwMBPortPollingThread, NULL, 0, &dwThreadId ) ) == NULL )
    {
        eStatus = MB_ENORES;
    }
    else
    {
        TRACEC( _T( "CFreeModbusThread::start: waiting for protocol stack to become ready\r\n" ) );

        dwWaitResult = WaitForSingleObject( hEvents, INFINITE );
        assert( dwWaitResult == WAIT_OBJECT_0 );

        eStatus = MB_ENOERR;
    }

    if( eStatus != MB_ENOERR )
    {
        ( void )CloseHandle( hEvents );
        DeleteCriticalSection( &hLock );
    }
    return eStatus;
}

SHORT
xMBPortStopPoolingThread(  )
{
    eMBErrorCode    eStatus;
    DWORD           dwWaitResult;

    if( prveMBPortGetThreadState(  ) != RUNNING )
        eStatus = MB_EILLSTATE;
    else
    {
        prveMBPortSetThreadState( SHUTDOWN );

        TRACEC( _T( "CFreeModbusThread::start: waiting for protocol to shutdown\r\n" ) );
        dwWaitResult = WaitForSingleObject( hEvents, INFINITE );
        assert( dwWaitResult == WAIT_OBJECT_0 );

        DeleteCriticalSection( &hLock );
        ( void )CloseHandle( hEvents );
        hEvents = INVALID_HANDLE_VALUE;
        hThread = INVALID_HANDLE_VALUE;
        dwThreadId = -1;

        eStatus = MB_ENOERR;
    }
    return eStatus;
}

DWORD           WINAPI
prvdwMBPortPollingThread( LPVOID lpParameter )
{
    prveMBPortSetThreadState( RUNNING );
    TRACEC( _T( "PoolingThread: signaling that main thread is ready.\r\n" ) );
    SetEvent( hEvents );

    if( eMBEnable(  ) == MB_ENOERR )
    {
        do
        {
            /* Poll the serial device. The serial device timeouts if no
             * characters have been received within for t3.5 during an
             * active transmission or if nothing happens within a specified
             * amount of time. Both timeouts are configured from the timer
             * init functions.
             */
            if( !xMBPortSerialPoll(  ) )
                break;

            /* Check if any of the timers have expired. */
            vMBPortTimerPoll(  );

            /* If a frame has been received this is checked in the next call
             * to eMBPoll.
             */
            if( eMBPoll(  ) != MB_ENOERR )
                break;

            /* We execute again to let the protocol stack generate the anwser.
             * This seems not very logical but is needed because there is an
             * additional step between EV_FRAME_RECEIVED and EV_EXECUTE.
             */
            if( eMBPoll(  ) != MB_ENOERR )
                break;
        }
        while( prveMBPortGetThreadState(  ) != SHUTDOWN );
    }
    ( void )eMBDisable(  );

    prveMBPortSetThreadState( STOPPED );

    TRACEC( _T( "PoolingThread: signaling that shutdown is finished.\r\n" ) );
    SetEvent( hEvents );
    return 0;
}

enum ThreadState
prveMBPortGetThreadState(  )
{
    enum ThreadState eCurState;

    EnterCriticalSection( &hLock );
    eCurState = eState;
    LeaveCriticalSection( &hLock );

    return eCurState;
}

void
prveMBPortSetThreadState( enum ThreadState eNewState )
{
    EnterCriticalSection( &hLock );
    eState = eNewState;
    LeaveCriticalSection( &hLock );
}
