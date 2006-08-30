/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 * Modifcations: Christian Walter <wolti@sil.at>
 *
 * $Id: sys_arch.c,v 1.1 2006/08/30 23:18:07 wolti Exp $
 */

/* ------------------------ System includes ------------------------------- */
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

/* ------------------------ FreeRTOS includes ----------------------------- */
#include "FreeRTOS.h"
#include "task.h"
#include "serial.h"
#include "semphr.h"

/* ------------------------ lwIP includes --------------------------------- */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

/* ------------------------ Defines --------------------------------------- */
/* This is the number of threads that can be started with sys_thead_new() */
#define SYS_MBOX_SIZE               ( 16 )
#define SYS_THREAD_MAX              ( 4 )
#define MS_TO_TICKS( ms )           \
    ( portTickType )( ( portTickType ) ( ms ) / portTICK_RATE_MS )
#define TICKS_TO_MS( ticks )        \
    ( unsigned portLONG )( ( portTickType ) ( ticks ) * portTICK_RATE_MS )
#define THREAD_STACK_SIZE           ( 1024 )
#define THREAD_NAME                 "lwIP"

/* Must hold the string THREAD_NAME + task number + terminating '\0'. */
#define THREAD_NAME_LEN_MAX         6

/* ------------------------ Type definitions ------------------------------ */
typedef struct
{
    struct sys_timeouts timeouts;
    xTaskHandle     pid;
    char            name[THREAD_NAME_LEN_MAX];
} sys_tcb_t;

/* ------------------------ Static functions ------------------------------ */
sys_tcb_t      *sys_thread_current( void );

/* ------------------------ Static variables ------------------------------ */
static sys_tcb_t tasks[SYS_THREAD_MAX];
static int      nextthread = 0;

/* ------------------------ Start implementation -------------------------- */
void
sys_init( void )
{
    int             i;

    /* Initialize the the per-thread sys_timeouts structures  make sure there
     * are no valid pids in the list */
    for( i = 0; i < SYS_THREAD_MAX; i++ )
    {
        tasks[i].timeouts.next = NULL;
        tasks[i].pid = 0;
        snprintf( tasks[i].name, THREAD_NAME_LEN_MAX, "%s%d", THREAD_NAME, i );
    }

    /* keep track of how many threads have been created */
    nextthread = 0;
}

/*
 * This optional function does a "fast" critical region protection and returns
 * the previous protection level. This function is only called during very short
 * critical regions. An embedded system which supports ISR-based drivers might
 * want to implement this function by disabling interrupts. Task-based systems
 * might want to implement this by using a mutex or disabling tasking. This
 * function should support recursive calls from the same task or interrupt. In
 * other words, sys_arch_protect() could be called while already protected. In
 * that case the return value indicates that it is already protected.
 *
 * sys_arch_protect() is only required if your port is supporting an operating
 * system.
 */
sys_prot_t
sys_arch_protect( void )
{
    vPortEnterCritical(  );
    return 1;
}

/*
 * This optional function does a "fast" set of critical region protection to the
 * value specified by pval. See the documentation for sys_arch_protect() for
 * more information. This function is only required if your port is supporting
 * an operating system.
 */
void
sys_arch_unprotect( sys_prot_t pval )
{
    ( void )pval;
    vPortExitCritical(  );
}

/*
 * Prints an assertion messages and aborts execution.
 */
void
sys_assert( const char *szAssertMsg )
{
    ( void )fputs( szAssertMsg, stderr );
    ( void )fputs( "\n\r", stderr );
    exit( EXIT_FAILURE );
}

void
sys_debug( const char *const szFmt, ... )
{
    va_list         ap;

    va_start( ap, szFmt );
    ( void )vprintf( szFmt, ap );
    ( void )putc( '\r', stdout );
    fflush( stdout );
    va_end( ap );
}

/* ------------------------ Start implementation ( Threads ) -------------- */

/*
 * Starts a new thread with priority "prio" that will begin its execution in the
 * function "thread()". The "arg" argument will be passed as an argument to the
 * thread() function. The id of the new thread is returned. Both the id and
 * the priority are system dependent.
 */
sys_thread_t
sys_thread_new( void ( *thread ) ( void *arg ), void *arg, int prio )
{
    xTaskHandle     xCreatedTask;
    sys_prot_t      old_level;

    old_level = sys_arch_protect(  );

    /* We disable the FreeRTOS scheduler because it might be the case that the new
     * tasks gets scheduled inside the xTaskCreate function. To prevent this we
     * disable the scheduling. Note that this can happen although we have interrupts
     * disabled because xTaskCreate contains a call to taskYIELD( ).
     */
    vTaskSuspendAll(  );
    if( xTaskCreate( thread, tasks[nextthread].name, THREAD_STACK_SIZE,
                     arg, prio, &xCreatedTask ) == pdPASS )
    {
        tasks[nextthread++].pid = xCreatedTask;
    }
    else
    {
        xCreatedTask = SYS_THREAD_NULL;
    }
    ( void )xTaskResumeAll(  );

    sys_arch_unprotect( old_level );
    return xCreatedTask;
}

/*
 * Returns the thread control block for the currently active task. In case
 * of an error the functions returns NULL.
 */
sys_tcb_t      *
sys_thread_current( void )
{
    sys_prot_t      old_level;
    sys_tcb_t      *ptask = NULL;
    int             i;
    xTaskHandle     pid = xTaskGetCurrentTaskHandle(  );

    old_level = sys_arch_protect(  );
    for( i = 0; i < nextthread; i++ )
    {
        if( tasks[i].pid == pid )
        {
            ptask = &tasks[i];
        }
    }
    sys_arch_unprotect( old_level );
    return ptask;
}

/*
 * Returns a pointer to the per-thread sys_timeouts structure. In lwIP,
 * each thread has a list of timeouts which is represented as a linked
 * list of sys_timeout structures. The sys_timeouts structure holds a
 * pointer to a linked list of timeouts. This function is called by
 * the lwIP timeout scheduler and must not return a NULL value.
 *
 * In a single threaded sys_arch implementation, this function will
 * simply return a pointer to a global sys_timeouts variable stored in
 * the sys_arch module.
 */
struct sys_timeouts *
sys_arch_timeouts( void )
{
    sys_tcb_t      *ptask;

    ptask = sys_thread_current(  );
    LWIP_ASSERT( "sys_arch_timeouts: ptask != NULL", ptask != NULL );
    return ptask != NULL ? &( ptask->timeouts ) : NULL;
}

/* ------------------------ Start implementation ( Semaphores ) ----------- */

/* Creates and returns a new semaphore. The "count" argument specifies
 * the initial state of the semaphore.
 */
sys_sem_t
sys_sem_new( u8_t count )
{
    sys_prot_t      old_level;
    xSemaphoreHandle xSemaphore;

    old_level = sys_arch_protect(  );

    vSemaphoreCreateBinary( xSemaphore );
    if( xSemaphore != SYS_SEM_NULL )
    {
        if( count == 0 )
        {
            xSemaphoreTake( xSemaphore, 1 );
        }
#ifdef SYS_STATS
        lwip_stats.sys.sem.used++;
        if( lwip_stats.sys.sem.used > lwip_stats.sys.sem.max )
        {
            lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
        }
#endif
    }
    else
    {
        LWIP_ASSERT( "sys_sem_new: xSemaphore == SYS_SEM_NULL\n", xSemaphore != SYS_SEM_NULL );
    }

    sys_arch_unprotect( old_level );
    return xSemaphore;
}

/* Deallocates a semaphore */
void
sys_sem_free( sys_sem_t sem )
{
    LWIP_ASSERT( "sys_sem_free: sem != SYS_SEM_NULL\n", sem != SYS_SEM_NULL );
    if( sem != SYS_SEM_NULL )
    {
#ifdef SYS_STATS
        lwip_stats.sys.sem.used--;
#endif
        vQueueDelete( sem );
    }
}

/* Signals a semaphore */
void
sys_sem_signal( sys_sem_t sem )
{
    LWIP_ASSERT( "sys_sem_signal: sem != SYS_SEM_NULL\n", sem != SYS_SEM_NULL );
    xSemaphoreGive( sem );
}

/*
 * Blocks the thread while waiting for the semaphore to be
 * signaled. If the "timeout" argument is non-zero, the thread should
 * only be blocked for the specified time (measured in
 * milliseconds).
 *
 * If the timeout argument is non-zero, the return value is the number of
 * milliseconds spent waiting for the semaphore to be signaled. If the
 * semaphore wasn't signaled within the specified time, the return value is
 * SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
 * (i.e., it was already signaled), the function may return zero.
 *
 * Notice that lwIP implements a function with a similar name,
 * sys_sem_wait(), that uses the sys_arch_sem_wait() function.
 */
u32_t
sys_arch_sem_wait( sys_sem_t sem, u32_t timeout )
{
    portBASE_TYPE   xStatus;
    portTickType    xTicksStart, xTicksEnd, xTicksElapsed;
    u32_t           timespent;

    LWIP_ASSERT( "sys_arch_sem_wait: sem != SYS_SEM_NULL", sem != SYS_SEM_NULL );
    xTicksStart = xTaskGetTickCount(  );
    if( timeout == 0 )
    {
        do
        {
            xStatus = xSemaphoreTake( sem, MS_TO_TICKS( 100 ) );
        }
        while( xStatus != pdTRUE );
    }
    else
    {
        xStatus = xSemaphoreTake( sem, MS_TO_TICKS( timeout ) );
    }

    /* Semaphore was signaled. */
    if( xStatus == pdTRUE )
    {
        xTicksEnd = xTaskGetTickCount(  );
        xTicksElapsed = xTicksEnd - xTicksStart;
        timespent = TICKS_TO_MS( xTicksElapsed );
    }
    else
    {
        timespent = SYS_ARCH_TIMEOUT;
    }
    return timespent;
}


/* ------------------------ Start implementation ( Mailboxes ) ------------ */

/* Creates an empty mailbox. */
sys_mbox_t
sys_mbox_new( void )
{
    xQueueHandle    mbox;

    mbox = xQueueCreate( SYS_MBOX_SIZE, sizeof( void * ) );
    if( mbox != SYS_MBOX_NULL )
    {
#ifdef SYS_STATS
        lwip_stats.sys.mbox.used++;
        if( lwip_stats.sys.mbox.used > lwip_stats.sys.mbox.max )
        {
            lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
        }
#endif
    }
    return mbox;
}

/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void
sys_mbox_free( sys_mbox_t mbox )
{
    void           *msg;

    LWIP_ASSERT( "sys_mbox_free: mbox != SYS_MBOX_NULL", mbox != SYS_MBOX_NULL );
    if( mbox != SYS_MBOX_NULL )
    {
        while( uxQueueMessagesWaiting( mbox ) != 0 )
        {
            if( sys_arch_mbox_fetch( mbox, &msg, 1 ) != SYS_ARCH_TIMEOUT )
            {
                LWIP_ASSERT( "sys_mbox_free: memory leak (msg != NULL)", msg == NULL );
            }
        }
        vQueueDelete( mbox );
#ifdef SYS_STATS
        lwip_stats.sys.mbox.used--;
#endif
    }
}

/*
 * This function sends a message to a mailbox. It is unusual in that no error
 * return is made. This is because the caller is responsible for ensuring that
 * the mailbox queue will not fail. The caller does this by limiting the number
 * of msg structures which exist for a given mailbox.
 */
void
sys_mbox_post( sys_mbox_t mbox, void *data )
{
    portBASE_TYPE   xQueueSent;

    /* Queue must not be full - Otherwise it is an error. */
    xQueueSent = xQueueSend( mbox, &data, 0 );
    LWIP_ASSERT( "sys_mbox_post: xQueueSent == pdPASS", xQueueSent == pdPASS );
}

/*
 * Blocks the thread until a message arrives in the mailbox, but does
 * not block the thread longer than "timeout" milliseconds (similar to
 * the sys_arch_sem_wait() function). The "msg" argument is a result
 * parameter that is set by the function (i.e., by doing "*msg =
 * ptr"). The "msg" parameter maybe NULL to indicate that the message
 * should be dropped.
 *
 * Note that a function with a similar name, sys_mbox_fetch(), is
 * implemented by lwIP.
 */
u32_t
sys_arch_mbox_fetch( sys_mbox_t mbox, void **msg, u32_t timeout )
{
    void           *ret_msg;
    portBASE_TYPE   xStatus;
    portTickType    xTicksStart, xTicksEnd, xTicksElapsed;
    u32_t           timespent;

    LWIP_ASSERT( "sys_arch_mbox_fetch: mbox != SYS_MBOX_NULL", mbox != SYS_MBOX_NULL );
    xTicksStart = xTaskGetTickCount(  );
    if( timeout == 0 )
    {
        do
        {
            xStatus = xQueueReceive( mbox, &ret_msg, MS_TO_TICKS( 100 ) );
        }
        while( xStatus != pdTRUE );
    }
    else
    {
        xStatus = xQueueReceive( mbox, &ret_msg, MS_TO_TICKS( timeout ) );
    }

    if( xStatus == pdTRUE )
    {
        if( msg )
        {
            *msg = ret_msg;
        }
        xTicksEnd = xTaskGetTickCount(  );
        xTicksElapsed = xTicksEnd - xTicksStart;
        timespent = TICKS_TO_MS( xTicksElapsed );
    }
    else
    {
        if( msg )
        {
            *msg = NULL;
        }
        timespent = SYS_ARCH_TIMEOUT;
    }
    return timespent;
}
