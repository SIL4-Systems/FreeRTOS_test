/*
 * FreeRTOS+UDP V1.0.4 (C) 2014 Real Time Engineers ltd.
 * All rights reserved
 *
 * This file is part of the FreeRTOS+UDP distribution.  The FreeRTOS+UDP license
 * terms are different to the FreeRTOS license terms.
 *
 * FreeRTOS+UDP uses a dual license model that allows the software to be used
 * under a standard GPL open source license, or a commercial license.  The
 * standard GPL license (unlike the modified GPL license under which FreeRTOS
 * itself is distributed) requires that all software statically linked with
 * FreeRTOS+UDP is also distributed under the same GPL V2 license terms.
 * Details of both license options follow:
 *
 * - Open source licensing -
 * FreeRTOS+UDP is a free download and may be used, modified, evaluated and
 * distributed without charge provided the user adheres to version two of the
 * GNU General Public License (GPL) and does not remove the copyright notice or
 * this text.  The GPL V2 text is available on the gnu.org web site, and on the
 * following URL: http://www.FreeRTOS.org/gpl-2.0.txt.
 *
 * - Commercial licensing -
 * Businesses and individuals that for commercial or other reasons cannot comply
 * with the terms of the GPL V2 license must obtain a commercial license before
 * incorporating FreeRTOS+UDP into proprietary software for distribution in any
 * form.  Commercial licenses can be purchased from http://shop.freertos.org/udp
 * and do not require any source files to be changed.
 *
 * FreeRTOS+UDP is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+UDP unless you agree that you use the software 'as is'.
 * FreeRTOS+UDP is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/udp
 *
 */

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include "os_semphr.h"

/* FreeRTOS+UDP includes. */
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "NetworkBufferManagement.h"
#include "notification.h"

/* Driver includes. */
#include "emac.h"
#include "mdio.h"

/* Demo includes. */
#include "NetworkInterface.h"

#if ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES != 1
    #define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eProcessBuffer
#else
    #define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eConsiderFrameForProcessing( ( pucEthernetBuffer ) )
#endif

/* When a packet is ready to be sent, if it cannot be sent immediately then the
task performing the transmit will block for niTX_BUFFER_FREE_WAIT
milliseconds.  It will do this a maximum of niMAX_TX_ATTEMPTS before giving
up. */
#define niTX_BUFFER_FREE_WAIT   ( ( TickType_t ) 2UL / portTICK_RATE_MS )
#define niMAX_TX_ATTEMPTS       ( 5 )

/* The length of the queue used to send interrupt status words from the
interrupt handler to the deferred handler task. */
#define niINTERRUPT_QUEUE_LENGTH    ( 10 )

/*
 * A deferred interrupt handler task that processes
 */
static void prvEMACDeferredInterruptHandlerTask( void *pvParameters );

static void xGetPbuf(struct pbuf_struct** aPbuf);

/* The queue used to communicate Ethernet events with the IP task. */
extern xQueueHandle xNetworkEventQueue;

/* Semaphore to protect access to the pbuf array */
static xSemaphoreHandle xPBUFArraySemaphore = NULL;

extern uint8 emacAddress[6U];
extern hdkif_t hdkif_data[MAX_EMAC_INSTANCE];
static hdkif_t* pEthDevDrv = 0;

#define NUMBER_OF_PBUFS 10
static pbuf_t pbufs[NUMBER_OF_PBUFS];
static int pbufIndex = 0;

static boolean networkInitialized = false;

extern uint32 EMACHWInit(uint8_t macaddr[6U]);

BaseType_t xNetworkInterfaceInitialise( void )
{
    if(false == networkInitialized)
    {
        /* Initialize once */
        networkInitialized = true;

        /* Initialize semaphore */
        xPBUFArraySemaphore = xSemaphoreCreateMutex();

        /* Initialize the receive queue */
        xEmacRxQueue = xQueueCreate(16, sizeof(long));

        /* The handler task is created at the highest possible priority to
           ensure the interrupt handler can return directly to it. */
        xTaskCreate( prvEMACDeferredInterruptHandlerTask,
                    "EMAC",
                     configMINIMAL_STACK_SIZE,
                     NULL,
                     (configMAX_PRIORITIES - 1) | portPRIVILEGE_BIT,
                     NULL );

        /* Point to the Ethernet device driver */
        pEthDevDrv = &hdkif_data[0U];

        /* Initialize the Ethernet hardware - do this last */
        (void) EMACHWInit(emacAddress);
    }
    else
    {
        /* Network is down, reset the EMAC */
        (void) EMACHWInit(emacAddress);
    }

    return pdPASS;
}


static void xGetPbuf(struct pbuf_struct** aPbuf)
{
    /* Get a buffer - wait forever */
    xSemaphoreTake( xPBUFArraySemaphore, portMAX_DELAY );
    *aPbuf = &pbufs[pbufIndex];
    pbufIndex = (pbufIndex + 1) % NUMBER_OF_PBUFS;
    xSemaphoreGive(xPBUFArraySemaphore);

}


BaseType_t xNetworkInterfaceOutput( xNetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    BaseType_t returnVal = pdFALSE;

    struct pbuf_struct* pbuf = 0;

    /* Get a buffer and initialize it */
    xGetPbuf(&pbuf);
    pbuf->next = 0; /* No linked buffer chain */
    pbuf->payload = pxNetworkBuffer->pucEthernetBuffer;
    pbuf->tot_len = (uint16)(pxNetworkBuffer->xDataLength & 0xFFFF);
    pbuf->len = pbuf->tot_len;  /* no linked buffer chain */

    /* Send the buffer to the Ethernet device driver */
    returnVal = EMACTransmit( pEthDevDrv, pbuf );
    if(TRUE == returnVal)
    {
        /* Call the standard trace macro to log the send event. */
        iptraceNETWORK_INTERFACE_TRANSMIT();

        returnVal = pdTRUE;
    }

    /* Free the network buffer descriptor */
    vNetworkBufferRelease( pxNetworkBuffer );

    return returnVal;
}


/* The deferred interrupt handler is a standard RTOS task. */
static void prvEMACDeferredInterruptHandlerTask( void *pvParameters )
{
    xNetworkBufferDescriptor_t *pxNetworkBuffer;
    xIPStackEvent_t xRxEvent;
    emac_data_t* pRxData;

    long queueData;

    for( ;; )
    {
        /* Get a pointer to the data from the ISR */
        xQueueReceive(xEmacRxQueue, &queueData, portMAX_DELAY);
        pRxData = (emac_data_t *)(queueData);

        /* Make sure the pointer is not NULL */
        if( NULL != pRxData )
        {
            /* Allocate a network buffer descriptor that references an Ethernet
               buffer large enough to hold the received data. */
            pxNetworkBuffer = pxNetworkBufferGet( pRxData->packetSize, 0 );
            if( NULL != pxNetworkBuffer )
            {
                /* Copy the data to the buffer */
                memcpy(pxNetworkBuffer->pucEthernetBuffer, /* Destination */
                      (uint8_t*)(&pRxData->emacData[0]),   /* Source */
                       pRxData->packetSize);               /* Number of bytes */

                /* See if the data contained in the received Ethernet frame
                   needs to be processed. */
                if( eConsiderFrameForProcessing( pxNetworkBuffer->pucEthernetBuffer )
                                                                  == eProcessBuffer )
                {
                    /* The event about to be sent to the IP stack is an Rx event. */
                    xRxEvent.eEventType = eEthernetRxEvent;

                    /* pvData is used to point to the network buffer descriptor
                       that references the received data. */
                    xRxEvent.pvData = ( void * ) pxNetworkBuffer;

                    /* Send the data to the IP stack. */
                    if( xQueueSendToBack( xNetworkEventQueue, &xRxEvent, 0 )
                                                                == pdFALSE )
                    {
                        /* The buffer could not be sent to the IP task so the
                           buffer must be released. */
                        vNetworkBufferRelease( pxNetworkBuffer );

                        /* Make a call to the standard trace macro to log the
                           occurrence. */
                        iptraceETHERNET_RX_EVENT_LOST();
                    }
                    else
                    {
                        /* The message was successfully sent to the IP stack.
                           Call the standard trace macro to log the occurrence. */
                        iptraceNETWORK_INTERFACE_RECEIVE();
                    }
                }
                else
                {
                    /* The Ethernet frame can be dropped, but the Ethernet
                       buffer must be released. */
                    vNetworkBufferRelease( pxNetworkBuffer );
                }

            } /* end if( NULL != pxNetworkBuffer ) */

            else
            {
                /* The event was lost because a network buffer was not
                   available. Call the standard trace macro to log the
                   occurrence. */
                iptraceETHERNET_RX_EVENT_LOST();
            }

        } /* end if( NULL != pRxData ) */

    } /* end for( ;; ) */

} /* end static void prvEMACDeferredInterruptHandlerTask( void *pvParameters ) */
