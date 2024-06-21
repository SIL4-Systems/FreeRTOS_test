/*******************************************************************************
File Name               :  eventhandler.c
Revision/Version        :  1.0
Purpose/Description     :  This file contains the event handler for different
                           task (Modbus receive, 1 sec. timer etc)
Author                  :  Chinnasamy
Created Date            :  Feb 12, 2019
******************************************************************************/



/******************************************************************************
        Includes
******************************************************************************/
#include "sys_common.h"
#include "eventhandler.h"
#include "rs485comm.h"
#include "sci.h"
/******************************************************************************
 *
 *
        Module Variables
******************************************************************************/
uint16_t g_uiEventTimeOut[EVENT_MAX] ={3, 200};
EVENTS g_stEvents[EVENT_MAX];
extern void CheckPacketReceiveCompletion(void);

/******************************************************************************
       Local Function Prototypes
******************************************************************************/



/******************************************************************************
FUNCTION NAME           : EventHandler
PURPOSE & DESCRIPTION   : This function is executed at regular interval of 0.5mSec.
                          It generates sub time bases of 100mSec, 1Sec, 1Min etc
INPUTS                  :
  Parameters            : None
 Module Data Referenced : None
 Global Data Referenced :
OUTPUTS                 :
 Return Value           : None
 Module Data Modified   : None
 Caller Data Modified   : None
 Global Data Modified   :

FUNCTION CALLED BY      :
FUNCTION CALLING        :
******************************************************************************/
void EventHandler(void)
{
    EVENTNO ucEventIndex = EVENT_MODBUSRECV;

    for(ucEventIndex = EVENT_MODBUSRECV; ucEventIndex < EVENT_MAX; ucEventIndex++)
    {

        if((g_stEvents[ucEventIndex].ucEventState == EVENT_PROGRESS) && (g_stEvents[ucEventIndex].uiEventTime == 0))
        {
            g_stEvents[ucEventIndex].ucEventState = EVENT_TIMEOUT;
        }
        if((g_stEvents[ucEventIndex].ucEventState == EVENT_PROGRESS)  && (g_stEvents[ucEventIndex].uiEventTime != 0))
        {
            g_stEvents[ucEventIndex].uiEventTime--;
            g_stEvents[ucEventIndex].ucEventState = EVENT_PROGRESS;
        }
        if(g_stEvents[ucEventIndex].ucEventState == EVENT_START)
        {
            g_stEvents[ucEventIndex].uiEventTime = g_uiEventTimeOut[ucEventIndex];
            g_stEvents[ucEventIndex].ucEventState = EVENT_PROGRESS;
        }
    }


    if(g_stEvents[EVENT_MODBUSRECV].ucEventState == EVENT_TIMEOUT)
    {
        g_stEvents[EVENT_MODBUSRECV].ucEventState = EVENT_NONE;
        // Call RS485 Receive Notify function to disable the receive interrupt
        sciNotification(scilinREG, (uint32)SCI_RX_INT);

        // Also Indicate to application to read the received data via RS485
    }
}



