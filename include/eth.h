/**
 * @file eth.h
 *
 * @brief Contains constants and function prototypes for eth.c
 */

/* USER CODE BEGIN (1) */
#include "FreeRTOS.h"
#include "os_task.h"
#include "FreeRTOS_UDP_IP.h"

#include "FreeRTOS_Sockets.h"

#include "uart.h"

#include "helpers.h"

/** @brief Use DHCP for IP configuration if set to 1 */
#define USE_DHCP    1

/** @brief Static IP in case DHCP Fails */
//{@
static const uint8_t ucIPAddress[ 4 ] = { 169, 254, 99, 75 };
static const uint8_t ucNetMask[ 4 ] = { 255, 255, 0, 0 };
static const uint8_t ucGatewayAddress[ 4 ] = { 169, 254, 99, 1 };
static const uint8_t ucDNSServerAddress[ 4 ] = { 8, 8, 8, 8 };
//@}

/** @brief Task handle for ethernet */
xTaskHandle EthernetTaskHandle;

extern int special_debug_flag;

/**
 * @brief     Event hook for IP Network Event
 *
 * @param[in] eNetworkEvent A eIPCallbackEvent_t enum identifying success or failure of the network
 *                          initialization event.
 */
void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent);


/**
 * @brief     This function is called by Ethernet task as task entry
 *            function and this function handled the UDP packet send
 *            and receive functionality.
 *
 * @param[in] pvParamters Any information to be passed to the task, NULL if
 *                        not required.
 */
void EthernetTask(void *pvParameters);

