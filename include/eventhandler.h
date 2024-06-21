/*
 * eventhandler.h
 *
 *  Created on: Feb 12, 2019
 *      Author: anil.verma
 */

#ifndef INCLUDE_EVENTHANDLER_H_
#define INCLUDE_EVENTHANDLER_H_


/******************************************************************************
        Includes
******************************************************************************/

/******************************************************************************
        Typedef structures, unions
******************************************************************************/
typedef enum eventstate
{
    EVENT_NONE = 0,
    EVENT_START,
    EVENT_PROGRESS,
    EVENT_TIMEOUT
}EVENTSTATE;

typedef enum eventnumber
{
    EVENT_MODBUSRECV = 0,
    EVENT_ONESECONDTIMER,
    EVENT_MAX
}EVENTNO;


typedef struct events
{
    EVENTNO ucEventIndex;
    EVENTSTATE ucEventState;
    uint16_t uiEventTime;
}EVENTS;


/******************************************************************************
        Module variables
******************************************************************************/
extern uint16_t g_uiEventTimeOut[EVENT_MAX];
extern EVENTS g_stEvents[EVENT_MAX];

/******************************************************************************
       Function Prototypes
******************************************************************************/
extern void EventHandler(void);



#endif /* INCLUDE_EVENTHANDLER_H_ */
