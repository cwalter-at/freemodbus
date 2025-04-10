#include "mb.h"
#include "mbport.h"
#include "port_internal.h"

/* Static variables */
static eMBEventType eQueuedEvent;
static BOOL xEventInQueue;

BOOL xMBPortEventInit(void)
{
    xEventInQueue = FALSE;
    return TRUE;
}

BOOL xMBPortEventPost(eMBEventType eEvent)
{
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    return TRUE;
}

BOOL xMBPortEventGet(eMBEventType *eEvent)
{
    BOOL xEventHappened = FALSE;
    
    if(xEventInQueue)
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    
    return xEventHappened;
}
