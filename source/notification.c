/** @file notification.c 
*   @brief User Notification Definition File
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file  defines  empty  notification  routines to avoid
*   linker errors, Driver expects user to define the notification. 
*   The user needs to either remove this file and use their custom 
*   notification function or place their code sequence in this file 
*   between the provided USER CODE BEGIN and USER CODE END.
*
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* Include Files */

#include "esm.h"
#include "sys_selftest.h"
#include "adc.h"
#include "can.h"
#include "gio.h"
#include "mibspi.h"
#include "sci.h"
#include "spi.h"
#include "het.h"
#include "dcc.h"
#include "i2c.h"
#include "sys_dma.h"
#include "emac.h" 

/* USER CODE BEGIN (0) */
#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_list.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "notification.h"

/* Buffers to retrieve EMAC data at interrupt level */
/* and forward to the IP stack at task level */
static emac_data_t emacRxData[NUMBER_OF_BUFS];
static int emacSetDataIndex = 0;

/* USER CODE END */
#pragma WEAK(esmGroup1Notification)
void esmGroup1Notification(uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (1) */
/* USER CODE END */
}

/* USER CODE BEGIN (2) */
/* USER CODE END */
#pragma WEAK(esmGroup2Notification)
void esmGroup2Notification(uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (3) */
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
#pragma WEAK(memoryPort0TestFailNotification)
void memoryPort0TestFailNotification(uint32 groupSelect, uint32 dataSelect, uint32 address, uint32 data)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (5) */
/* USER CODE END */
}

/* USER CODE BEGIN (6) */
/* USER CODE END */
#pragma WEAK(memoryPort1TestFailNotification)
void memoryPort1TestFailNotification(uint32 groupSelect, uint32 dataSelect, uint32 address, uint32 data)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (7) */
/* USER CODE END */
}

/* USER CODE BEGIN (8) */
/* USER CODE END */
#pragma WEAK(adcNotification)
void adcNotification(adcBASE_t *adc, uint32 group)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (11) */
/* USER CODE END */
}

/* USER CODE BEGIN (12) */
/* USER CODE END */
#pragma WEAK(canErrorNotification)
void canErrorNotification(canBASE_t *node, uint32 notification)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (13) */
/* USER CODE END */
}

#pragma WEAK(canStatusChangeNotification)
void canStatusChangeNotification(canBASE_t *node, uint32 notification)  
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (14) */
/* USER CODE END */
}

#pragma WEAK(canMessageNotification)
void canMessageNotification(canBASE_t *node, uint32 messageBox)  
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (15) */
/* USER CODE END */
}

/* USER CODE BEGIN (16) */
/* USER CODE END */
#pragma WEAK(dccNotification)
void dccNotification(dccBASE_t  *dcc,uint32 flags)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (17) */
/* USER CODE END */
}

/* USER CODE BEGIN (18) */
/* USER CODE END */
#pragma WEAK(gioNotification)
void gioNotification(gioPORT_t *port, uint32 bit)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (19) */
/* USER CODE END */
}

/* USER CODE BEGIN (20) */
/* USER CODE END */
#pragma WEAK(i2cNotification)
void i2cNotification(i2cBASE_t *i2c, uint32 flags)      
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (21) */
/* USER CODE END */
}

/* USER CODE BEGIN (22) */
/* USER CODE END */
#pragma WEAK(mibspiNotification)
void mibspiNotification(mibspiBASE_t *mibspi, uint32 flags)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (25) */
/* USER CODE END */
}

/* USER CODE BEGIN (26) */
/* USER CODE END */
#pragma WEAK(mibspiGroupNotification)
void mibspiGroupNotification(mibspiBASE_t *mibspi, uint32 group)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (27) */
/* USER CODE END */
}
/* USER CODE BEGIN (28) */
/* USER CODE END */

#pragma WEAK(sciNotification)
void sciNotification(sciBASE_t *sci, uint32 flags)     
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (29) */
/* USER CODE END */
}

/* USER CODE BEGIN (30) */
/* USER CODE END */
#pragma WEAK(spiNotification)
void spiNotification(spiBASE_t *spi, uint32 flags)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (31) */
/* USER CODE END */
}

/* USER CODE BEGIN (32) */
/* USER CODE END */
#pragma WEAK(spiEndNotification)
void spiEndNotification(spiBASE_t *spi)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (33) */
/* USER CODE END */
}

/* USER CODE BEGIN (34) */
/* USER CODE END */

#pragma WEAK(pwmNotification)
void pwmNotification(hetBASE_t * hetREG,uint32 pwm, uint32 notification)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (35) */
/* USER CODE END */
}

/* USER CODE BEGIN (36) */
/* USER CODE END */
#pragma WEAK(edgeNotification)
void edgeNotification(hetBASE_t * hetREG,uint32 edge)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (37) */
/* USER CODE END */
}

/* USER CODE BEGIN (38) */
/* USER CODE END */
#pragma WEAK(hetNotification)
void hetNotification(hetBASE_t *het, uint32 offset)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (39) */
/* USER CODE END */
}

/* USER CODE BEGIN (40) */
/* USER CODE END */


/* USER CODE BEGIN (43) */
/* USER CODE END */


/* USER CODE BEGIN (47) */
/* USER CODE END */


/* USER CODE BEGIN (50) */
/* USER CODE END */


/* USER CODE BEGIN (53) */
/* USER CODE END */

#pragma WEAK(dmaGroupANotification)
void dmaGroupANotification(dmaInterrupt_t inttype, uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (54) */
/* USER CODE END */
}
/* USER CODE BEGIN (55) */
/* USER CODE END */

/* USER CODE BEGIN (56) */
/* USER CODE END */
#pragma WEAK(emacTxNotification)
void emacTxNotification(hdkif_t *hdkif)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (57) */
/* USER CODE END */
}

/* USER CODE BEGIN (58) */
/* USER CODE END */
#pragma WEAK(emacRxNotification)
void emacRxNotification(hdkif_t *hdkif)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (59) */

    /*******************************************************/
    /* This method runs at processor interrupt level */
    /*******************************************************/

    uint32_t localBufferSize = 0;

    uint32_t dataCopied = 0;

    /* Used to copy the data from the EMAC to a statically
     * allocated buffer so a task can put it into a
     * network buffer and send it to the UDP/IP stack.
     *
     */
    uint8_t* pSrcBuf;
    uint8_t* pDestBuf;

    /* Buffer descriptor pointers */
    rxch_t *rxch_int;
    volatile emac_rx_bd_t* curr_bd;
    volatile emac_rx_bd_t* curr_tail;
    volatile emac_rx_bd_t* last_bd;

    /* I used this to send the data buffer pointer to the task queue */
    long queueData;

    /* The receive structure that holds data about a particular receive channel */
    rxch_int = &(hdkif->rxchptr);

    /* Get the buffer descriptors which contain the earliest filled data */
    curr_bd = rxch_int->active_head;
    last_bd = rxch_int->active_tail;

    /*
     * Process the descriptors as long as data is available. When the DMA
     * is receiving data SOP flag will be set.
     */
    while((curr_bd->flags_pktlen & EMAC_BUF_DESC_SOP) == EMAC_BUF_DESC_SOP)
    {
        /* Wait for the BD to be freed by the EMAC - EMAC_BUF_DESC_OWNER bit is cleared */
        if((curr_bd->flags_pktlen & EMAC_BUF_DESC_OWNER) != EMAC_BUF_DESC_OWNER)
        {
            /* This bd chain will be freed after processing */
            rxch_int->free_head = curr_bd;

            /*
             * I copied the data to a temporary buffer first because the
             * stack buffer manager doesn't support getting buffers at ISR
             */

            /* Initialize the destination data buffer pointer */
            pDestBuf = (uint8_t *)(&emacRxData[emacSetDataIndex].emacData[0]);

            /*
             * NOTE: this method is written using static structures to retrieve
             * the data from the EMAC. An assumption is made that a packet is
             * never larger than 2,000 bytes. This should be a safe assumption
             * given that this EMAC is a fast Ethernet (100Mbps) device - i.e.,
             * there is no chance of a jumbo frame being received.
             *
             */
            while((curr_bd->flags_pktlen & EMAC_BUF_DESC_EOP)!= EMAC_BUF_DESC_EOP)
            {
                /* Initialize the source buffer pointer */
                pSrcBuf = (uint8_t *)(curr_bd->bufptr + ((curr_bd->bufoff_len & 0xFFFF0000) >> 16));
                localBufferSize = curr_bd->bufoff_len & 0x0000FFFF;

                /* Copy the data to the destination buffer */
                memcpy(pDestBuf,          /* Destination */
                       pSrcBuf,           /* Source */
                       localBufferSize);  /* Number of bytes */

                /* Track the amount of data copied */
                dataCopied += localBufferSize;

                /* Advance the destination pointer */
                pDestBuf += localBufferSize;

                /*Update the flags for the descriptor again and the length of the buffer*/
                curr_bd->flags_pktlen = (uint32)EMAC_BUF_DESC_OWNER;
                curr_bd->bufoff_len = (uint32)MAX_TRANSFER_UNIT;
                last_bd = curr_bd;
                curr_bd = curr_bd->next;
            }

            /* Process the EOP BD */

            /* Initialize the source buffer pointer */
            pSrcBuf = (uint8_t *)(curr_bd->bufptr + ((curr_bd->bufoff_len & 0xFFFF0000) >> 16));
            localBufferSize = curr_bd->bufoff_len & 0x0000FFFF;

            /* Copy the data to the destination buffer */
            memcpy(pDestBuf,          /* Destination */
                   pSrcBuf,           /* Source */
                   localBufferSize);  /* Number of bytes */

            /* Track the amount of data copied */
            dataCopied += localBufferSize;

            /*Update the flags for the descriptor again and the length of the buffer*/
            curr_bd->flags_pktlen = (uint32)EMAC_BUF_DESC_OWNER;
            curr_bd->bufoff_len = (uint32)MAX_TRANSFER_UNIT;
            last_bd = curr_bd;
            curr_bd = curr_bd->next;

            /* Acknowledge that this packet is processed */
            EMACRxCPWrite(hdkif->emac_base, (uint32)EMAC_CHANNELNUMBER, (uint32)last_bd);

            /* The next buffer descriptor is the new head of the linked list. */
            rxch_int->active_head = curr_bd;

            /* The processed descriptor is now the tail of the linked list. */
            curr_tail = rxch_int->active_tail;
            curr_tail->next = rxch_int->free_head;

            /* The last element in the already processed Rx descriptor chain is now the end of list. */
            last_bd->next = NULL;

            /*
             * Check if the reception has ended. If the EOQ flag is set, the NULL
             * Pointer is taken by the DMA engine. So we need to write the RX HDP
             * with the next descriptor.
             */
            if((curr_tail->flags_pktlen & EMAC_BUF_DESC_EOQ) == EMAC_BUF_DESC_EOQ)
            {
                EMACRxHdrDescPtrWrite(hdkif->emac_base, (uint32)(rxch_int->free_head), (uint32)EMAC_CHANNELNUMBER);
            }
            rxch_int->free_head = curr_bd;
            rxch_int->active_tail = last_bd;

            /* Force the packet size to the amount of data copied */
            emacRxData[emacSetDataIndex].packetSize = dataCopied;
            dataCopied = 0;

            /*
             * Notify the receiving task - I tried to do the cast in one step
             * in the function call, but it didn't work right so I gave up and
             * did it separately
             */
            queueData = (long)(&emacRxData[emacSetDataIndex]);
            xQueueSendToBackFromISR(xEmacRxQueue, &queueData, NULL);

            /* Advance the static receive buffer index */
            emacSetDataIndex = (emacSetDataIndex + 1) % NUMBER_OF_BUFS;

        } /* end if((curr_bd->flags_pktlen & EMAC_BUF_DESC_OWNER) != EMAC_BUF_DESC_OWNER) */

    } /* while((curr_bd->flags_pktlen & EMAC_BUF_DESC_SOP) == EMAC_BUF_DESC_SOP) */

/* USER CODE END */
}

/* USER CODE BEGIN (60) */

/* USER CODE END */
