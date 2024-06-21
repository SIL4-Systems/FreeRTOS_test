/*
 * Created a header file so I could share data structure definitions
 *
 */

#ifndef __NOTIFICATION_H__
#define __NOTIFICATION_H__

#define NUMBER_OF_BUFS 16
#define BUFFER_SIZE 2000

typedef struct emacData
{
    uint32_t  packetSize;
    uint8_t   emacData[BUFFER_SIZE];
} emac_data_t;

xQueueHandle xEmacRxQueue;

#endif /* __NOTIFICATION_H__ */
