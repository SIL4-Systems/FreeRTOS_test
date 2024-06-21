/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
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


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
#include "FreeRTOS.h"
#include "os_task.h"
//#include "led_demo.h"
#include "os_queue.h"
#include "os_timer.h"
#include "sys_core.h"
#include "FreeRTOS_UDP_IP.h"
#include "sci.h"
#include "os_mpu_wrappers.h"

#include "stdlib.h"
#include "stdio.h"
#include "gio.h"
#include "FreeRTOSConfig.h"
#include "os_semphr.h"
#include "string.h"
//#include "het.h"
#include "etpwm.h"
#include "rs485comm.h"
//#include "C:/Users/Adam/CodeComposerWorkspace/RM-VMP-Test/SDCARD/sd_card.h"
#include "emif.h"
#include "spi.h"
#include "os_portable.h"
#include "reg_system.h"
#include "nandflash.h"
//#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_Sockets.h"

// Use DHCP for IP address assignment if this is set to 1
#define USE_DHCP 1

/* FOr the LED tasks to randomize the blinking */
unsigned int SubTask_Number = 0;
unsigned int Task_Number = 9;
unsigned int task_data = 0xff00;

/* Task queues */
xQueueHandle xTask1Queue;
xQueueHandle xTask2Queue;

// Define Task Handles
xTaskHandle EthernetTaskHandle;
xTaskHandle RS485TaskHandle;
xTaskHandle RTCTaskHandle;
xTaskHandle SDCardTaskHandle;
xTaskHandle EMIFTaskHandle;

xSemaphoreHandle xMutex = NULL;
SemaphoreHandle_t xRecursiveMutex;

unsigned short read_result[TOTAL_WORDS_PER_PAGE];

struct DateTime
{
    unsigned char Year;
    unsigned char Month;
    unsigned char Day;
    unsigned char Hour;
    unsigned char Minute;
    unsigned char Second;
    unsigned char Millisecond;
    unsigned char Reserved;
};

typedef enum {
    EMIF_NAND,
    EMIF_AIN1,
    EMIF_AIN2,
    EMIF_AIN3,
    EMIF_AIN4,
    EMIF_LED1,
    EMIF_LED2,
    EMIF_TACH1,
    EMIF_TACH2,
    EMIF_OUT,
    EMIF_DIN1,
    EMIF_DIN2,
    EMIF_SCRATCH,
    EMIF_VERSION,
    EMIF_TACH3,
    EMIF_TACH4,
    EMIF_TIMER
} EMIF_PARAM;

typedef enum {
    EMIF_MANUAL_OFF,
    EMIF_MANUAL_ON,
    EMIF_APPLY_ON,
    EMIF_APPLY_OFF,
    EMIF_RELEASE_ON,
    EMIF_RELEASE_OFF,
    EMIF_ADUMP_ON,
    EMIF_ADUMP_OFF,
    EMIF_BDUMP_ON,
    EMIF_BDUMP_OFF,
    EMIF_WSS_ON,
    EMIF_WSS_OFF,
    EMIF_SCO_ON,
    EMIF_SCO_OFF,
    EMIF_FAULT_ON,
    EMIF_FAULT_OFF
} EMIF_OUTPUT_CONTROL;


static void InitializePeripherals(void);
static InitializeTimer(void);
static void EthernetTask();
static void RS485Task( void *pvParameters );
static void SDCardTask( void *pvParameters );
static void RTCTask( void *pvParameters );
static void EMIFTask( void *pvParameters );
int UARTwrite(const char *pcBuf, unsigned int ui32Len);
//int initEMIF(void);
uint16_t emifSetOutput(unsigned char command);

uint16_t ethReadEmifLocation(char memSuffix);
unsigned long emifFieldToAddr(unsigned char emifParameter);
void EMIFTaskCheckNotifications(void);

void ethSetManualControl(unsigned char command);

/* Static IP in case DHCP Fails */
//static const uint8_t ucIPAddress[ 4 ] = { 10, 1, 10, 98 };
//static const uint8_t ucNetMask[ 4 ] = { 255, 0, 0, 0 };
//static const uint8_t ucGatewayAddress[ 4 ] = { 10, 1, 10, 1 };

static const uint8_t ucIPAddress[ 4 ] = { 169, 254, 99, 75 };
static const uint8_t ucNetMask[ 4 ] = { 255, 255, 0, 0 };
static const uint8_t ucGatewayAddress[ 4 ] = { 169, 254, 99, 1 };

//static const uint8_t ucStaticIPAddress[4] = {169, 254, 99, 75};
//static const uint8_t ucNetMask[4] = {255, 255, 0, 0};
//static const uint8_t ucGatewayAddress[4] = {169, 254, 99, 1};
//static const uint8_t ucDNSServerAddress[4] = {8, 8, 8, 8};

/* The following is the address of an OpenDNS server. */
static const uint8_t ucDNSServerAddress[ 4 ] = { 8, 8, 8, 8 };

/* Forward declarations for the LED blinky tasks */
//void Running_LED_demo(void);
//void run_LED_StartUp(void);

static void tmpWait(int up)
{
    int i = 0;
    while(i<=up)
        i++;
}

/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

//uint8	emacAddress[6U] = 	{0x00U, 0x08U, 0xEEU, 0x03U, 0xA6U, 0x6CU};
uint8   emacAddress[6U] =   {0xffU, 0xffU, 0xffU, 0xffU, 0xffU, 0xffU};
uint32 	emacPhyAddress	=	1U;

int main(void)
{
/* USER CODE BEGIN (3) */
    BaseType_t returnVal = pdFALSE;;
    char *message = "hello\n";
    char testMessage[32];

    // Initialize the peripherals
    InitializePeripherals();
    //read_result = (unsigned short*)pvPortMalloc(TOTAL_BYTES_PER_PAGE+2); //attempt to allocate memory for reading data from NANDz
    //read_result =
//    _enable_interrupts();
    xRecursiveMutex = xSemaphoreCreateRecursiveMutex();
    UARTwrite(message,strlen(message));
//    free(message);

    /* Create Tasks */
//    xTask1Queue = xQueueCreate(1, sizeof(long));
//    xTask2Queue = xQueueCreate(1, sizeof(long));
//
//    SubTask_Number = 0;
//    long xmitVal = 1;
//    xQueueSendToBack(xTask2Queue, &xmitVal, portMAX_DELAY);

    //initEMIF();

    //Create RS-485 Task
//    if (xTaskCreate(RS485Task,"RS485Task", configMINIMAL_STACK_SIZE, NULL, 2, &RS485TaskHandle) != pdTRUE)
//    {
//        message = "Could not create RS485Task\n";
//        UARTwrite(message,strlen(message));
//        free(message);
//        while(1);
//    }

//    UARTwrite("here1\n",strlen("here1\n"));

    //Create SD Card Task
    /*
    if (xTaskCreate(SDCardTask,"SDCardTask", configMINIMAL_STACK_SIZE, NULL, 6, &SDCardTaskTaskHandle) != pdTRUE)
    {
        message = "Could not create SDCardTask\n";
        UARTwrite(message,strlen(message));
        free(message);
        while(1);
    }
*/
    //Create RTC Task
//    if (xTaskCreate(RTCTask,"RTCTask", configMINIMAL_STACK_SIZE, NULL,3 , &RTCTaskHandle) != pdTRUE)
//    {
//        message = "Could not create RTCTask\n";
//        UARTwrite(message,strlen(message));
//        free(message);
//        while(1);
//    }
//    UARTwrite("here2\n",strlen("here2\n"));


    returnVal = FreeRTOS_IPInit( ucIPAddress,
                                 ucNetMask,
                                 NULL,
                                 ucDNSServerAddress,
                                 emacAddress );

    EthernetTask();

//    if (xTaskCreate(EthernetTask,"EthernetTask", configMINIMAL_STACK_SIZE, NULL,3 , &EthernetTaskHandle) != pdTRUE) {
//        message = "Could not create EthernetTask\n";
//        UARTwrite(message,strlen(message));
//        free(message);
//        while(1);
//    } else {
//        message = "Created EthernetTask\n";
//        UARTwrite(message,strlen(message));
//    }



    UARTwrite("here3\n",strlen("here3\n"));
    //Create EMIF Task
//    static portSTACK_TYPE xTaskStack[ 512 ] __attribute__((aligned(512*4)));
//
//
//    TaskParameters_t task_params = {
//        EMIFTask,
//        "EMIFTask",
//        configMINIMAL_STACK_SIZE,
//        NULL,
//        4,
//        xTaskStack,
//        /* xRegions - In this case only one of the three user definable regions is
//        actually used.  The parameters are used to set the region to read only. */
//        {
//            /* Base address   Length                    Parameters */
//            { (char*)0x60000000,    0x20000,                portMPU_REGION_READ_WRITE }, //was length = 0x2000
//            { (char*)read_result,   TOTAL_BYTES_PER_PAGE+2,   portMPU_REGION_READ_WRITE },
//            { 0, 0, 0 },
//        }
//    };
//
//    if (xTaskCreateRestricted(&task_params, &EMIFTaskHandle) != pdTRUE)
//    {
//        message = "Could not create EMIFTask\n";
//        UARTwrite(message,strlen(message));
//        free(message);
//        while(1);
//    }

//#if USE_DHCP
//    returnVal = FreeRTOS_IPInit(NULL, NULL, NULL, NULL, NULL);
//#else

//    FreeRTOS_IPInit(ucStaticIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress);
//#endif
//    if(pdFAIL == returnVal)
//    {
//        /* Do Something */
//        UARTwrite("IP assignment failed\n",strlen("IP assignment failed\n"));
//    } else {
//        UARTwrite("IP assignment success\n",strlen("IP assignment success\n"));
//    }

    UARTwrite("Starting scheduler...",strlen("Starting scheduler..."));

    /* Start the OS scheduler */
    vTaskStartScheduler();

    UARTwrite("Starting the indefinite loop now", strlen("Starting the indefinite loop now"));

    /* Shouldn't get here unless the idle task couldn't be created */
    for(;;);

/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
/******************************************************************************
FUNCTION NAME           : InitializePeripherals
PURPOSE & DESCRIPTION   : Initialize all the hardware peripherals in VMP system.
INPUTS                  :
  Parameters            : None
 Module Data Referenced : None
 Global Data Referenced :
OUTPUTS                 :
 Return Value           : None
 Module Data Modified   : None
 Caller Data Modified   : None
 Global Data Modified   :

FUNCTION CALLED BY      : main()
FUNCTION CALLING        : sciInit(),
******************************************************************************/
static void InitializePeripherals()
{
    //uint8 ip_addr[4] = { 10, 1, 10, 96  };
    //uint8 netmask[4] = { 255, 0, 0, 0 };
    //uint8 gateway[4] = { 10, 1, 10, 1 };

    //initialize serial communication (Console and RS-485)
    sciInit();
    // GPIO initialized
    gioInit();

    gioSetDirection(gioPORTA, 0b00001000); //enable output on 3rd bit

   // gioSetDirection(gioPORTB, 0b10000000); //enable output on 7th bit
    gioSetDirection(gioPORTB, 0b00010000); //enable output to 4th bit (GPIO_LED3)
    //gioSetDirection(gioPORTA, 0b00001000);
    gioPORTB->PSL = gioPORTB->PSL | 0b10000000; //pullup/down select
    gioPORTB->PULDIS = gioPORTB->PULDIS | 0b10000000; //pullup/down enable
    /*
    while(1)
    {
        gioSetBit(gioPORTB,0,1);
        gioSetBit(gioPORTB,1,1);
        gioSetBit(gioPORTB,2,1);
        gioSetBit(gioPORTB,3,1);
        gioSetBit(gioPORTB,4,1);
        tmpWait(10000);
        //vTaskDelay(100);
        gioSetBit(gioPORTB,0,0);
        gioSetBit(gioPORTB,1,0);
        gioSetBit(gioPORTB,2,0);
        gioSetBit(gioPORTB,3,0);
        gioSetBit(gioPORTB,4,0);
        tmpWait(10000);
        //vTaskDelay(100);
    }
    */

    gioPORTA->PSL = gioPORTA->PSL | 0b00001000; //pullup/down select
    gioPORTA->PULDIS = gioPORTA->PULDIS | 0b00000000; //pullup/down enable
    //gioPORTA->PDR = gioPORTA->PDR | 0b00000000; // open drain
    gioSetBit(gioPORTB,3,1);
    // SPI initialized for SD card
    //power_on();
    /*
    gioSetBit(spiPORT4,0,0);
    gioSetBit(spiPORT4,1,0);
    gioSetBit(spiPORT4,2,0);
    gioSetBit(spiPORT4,3,0);
    gioSetBit(spiPORT4,4,0);
    gioSetBit(spiPORT4,5,0);
    gioSetBit(spiPORT4,6,0);
    gioSetBit(spiPORT4,7,0);
    */
    // Enable IRQ for all Peripherals
    _enable_IRQ();
    // Initialized the free running timer at 0.5 mSec interval
    InitializeTimer();
    // Activate SCI-2 Interrupt based on TI requirement
    sciSend(scilinREG,0,g_ucSciLinTxReg);
    sciReceive(scilinREG, 0, g_ucSciLinRxReg);

    rs485RegInit();
//#if 0
//   EMAC_LwIP_Main(emacAddress ,ip_addr, netmask,gateway);
//#endif
}

/******************************************************************************
FUNCTION NAME           : InitializeTimer
PURPOSE & DESCRIPTION   : This function is called by Ethernet task as task entry
                          function and this function handled the UDP packet send
                          and receive functionality.
INPUTS                  :
  Parameters            : None
 Module Data Referenced : None
 Global Data Referenced :
OUTPUTS                 :
 Return Value           : None
 Module Data Modified   : None
 Caller Data Modified   : None
 Global Data Modified   :

FUNCTION CALLED BY      : InitializePeripherals()
FUNCTION CALLING        :etpwmSetClkDiv(),etpwmSetTimebasePeriod(),etpwmSetCmpA(),
                         etpwmSetCount(),etpwmEnableInterrupt()
******************************************************************************/
static InitializeTimer()
{
#if 1
        _enable_interrupt_();

        /* Configure ETPWM1 for 0.5ms */
        /* Set the TBCLK frequency =  VCLK4 frequency = 90MHz */
        etpwmSetClkDiv(etpwmREG1, ClkDiv_by_1, HspClkDiv_by_1);
        /* Set the time period as 500mS (Divider value = (500ms * (90MHz/(64*12)) - 1 = 89)*/
        etpwmSetTimebasePeriod(etpwmREG1, 44999);
        /* Configure Compare A value as half the time period */
        etpwmSetCmpA(etpwmREG1, 22500);
        /* Configure the module to set PWMA value as 1 when CTR=0 and as 0 when CTR=CmpA  */
        etpwmActionQualConfig_t configPWMA;
        configPWMA.CtrEqZero_Action = ActionQual_Set;
        configPWMA.CtrEqCmpAUp_Action = ActionQual_Clear;
        configPWMA.CtrEqPeriod_Action = ActionQual_Disabled;
        configPWMA.CtrEqCmpADown_Action = ActionQual_Disabled;
        configPWMA.CtrEqCmpBUp_Action = ActionQual_Disabled;
        configPWMA.CtrEqCmpBDown_Action = ActionQual_Disabled;
        etpwmSetActionQualPwmA(etpwmREG1, configPWMA);
        /* Start counter in CountUp mode */
        etpwmSetCount(etpwmREG1, 0);
        etpwmSetCounterMode(etpwmREG1, CounterMode_Up);
        etpwmStartTBCLK();
        // Enable Interrupt
        etpwmEnableInterrupt(etpwmREG1, CTR_ZERO, EventPeriod_FirstEvent);
#endif
}

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* Check this was a network up event, as opposed to a network down event. */
    if( eNetworkEvent == eNetworkUp )
    {
        /* Create the tasks that use the IP stack if they have not already been
        created. */
        if( xTasksAlreadyCreated == pdFALSE )
        {
            /*
             * For convenience, tasks that use FreeRTOS+UDP can be created here
             * to ensure they are not created before the network is usable.
             */

            //Create Ethernet Task
                    if (xTaskCreate(EthernetTask,"EthernetTask", 16384, NULL, 1 | portPRIVILEGE_BIT, &EthernetTaskHandle) != pdTRUE)
                    {
                    }


            //Create EMIF Task
/*
                    static portSTACK_TYPE xTaskStack[ 512 ] __attribute__((aligned(512*4)));


                    TaskParameters_t task_params = {
                        EthernetTask,
                        "EthernetTask",
                        //configMINIMAL_STACK_SIZE,
                        16384,
                        NULL,
                        1,
                        xTaskStack,
                        // xRegions - In this case only one of the three user definable regions is
                        //actually used.  The parameters are used to set the region to read only.
                        {
                            // Base address   Length                    Parameters
                         {(char*)0x60000000, 0x2000, portMPU_REGION_READ_WRITE },
                            { 0,              0,                        0                        },
                            { 0,              0,                        0                        },
                        }
                    };

                    if (xTaskCreateRestricted(&task_params, &EthernetTaskHandle) != pdTRUE)
                    {
                    }
*/

            xTasksAlreadyCreated = pdTRUE;
        }
    }
}
void vApplicationMallocFailedHook(void){
    UARTwrite("!!!!!!!!!!!!\n",strlen("!!!!!!!!!!!!\n"));
}


static void EthernetTask(  )
{
    UARTwrite("Ethernet Task started...\n", strlen("Ethernet Task started...\n"));
    xSocket_t xSocket;
    struct freertos_sockaddr xBindAddress;
    struct freertos_sockaddr xSourceAddress;
    uint8_t ucRecvBuffer[ 100 ];
    int32_t lRecvBytes;
    const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 200 );
    socklen_t xSourceAddressLength = sizeof( xSourceAddress );

    // Create a UDP socket
    xSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );
    configASSERT( xSocket != FREERTOS_INVALID_SOCKET );

    UARTwrite("Socket...\n", strlen("Socket...\n"));

    // Set a time out so the socket does not block indefinitely
    FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );

    // Bind the socket to port 2010
    xBindAddress.sin_port = FreeRTOS_htons( 2010 );
    FreeRTOS_bind( xSocket, &xBindAddress, sizeof( xBindAddress ) );

    UARTwrite( "Ethernet Ready...\n", strlen( "Ethernet Ready...\n" ) );

    for( ;; )
    {
        // Wait for incoming data
        lRecvBytes = FreeRTOS_recvfrom( xSocket, ucRecvBuffer, sizeof( ucRecvBuffer ), 0, &xSourceAddress, &xSourceAddressLength );

        if( lRecvBytes > 0 )
        {
            // Data received, print it to UART
            UARTwrite( "Data received via Ethernet\n", strlen( "Data received via Ethernet\n" ) );
            UARTwrite( "Received data: ", strlen( "Received data: " ) );
            UARTwrite( ( char * ) ucRecvBuffer, lRecvBytes );
            UARTwrite( "\n", 1 );

            // Echo the received data back
            FreeRTOS_sendto( xSocket, ucRecvBuffer, lRecvBytes, 0, &xSourceAddress, xSourceAddressLength );
        }
        else
        {
            UARTwrite( "No data received\n", strlen( "No data received\n" ) );
        }

        // Delay to avoid hogging the CPU
        vTaskDelay( pdMS_TO_TICKS( 10 ) );
    }
}


/******************************************************************************
FUNCTION NAME           : EthernetTask
PURPOSE & DESCRIPTION   : This function is called by Ethernet task as task entry
                          function and this function handled the UDP packet send
                          and receive functionality.
INPUTS                  :
  Parameters            : None
 Module Data Referenced : None
 Global Data Referenced :
OUTPUTS                 :
 Return Value           : None
 Module Data Modified   : None
 Caller Data Modified   : None
 Global Data Modified   :

FUNCTION CALLED BY      : xTaskCreate()
FUNCTION CALLING        :
******************************************************************************/
//static void EthernetTask(  )
//{
//    UARTwrite("Ethernet starting...\n",strlen("Ethernet starting...\n"));
//    int i = 0;
//    char tempString[10];
//    xSocket_t sock; //recv sock
//    xSocket_t sendSock;
//    struct freertos_sockaddr addr; //recv sock
//    struct freertos_sockaddr sourceAddr;
//    struct freertos_sockaddr sendAddr; //send to sock
//    uint16_t tempData = 0;
//    uint16_t output_length = 0;
//    unsigned short port = 2010;
//    unsigned char inBuf[100];
//    unsigned char outBuf[400];
//    int32_t iReturned;
//    const TickType_t xRecvTimeOut = 20 / portTICK_PERIOD_MS;
//
//    //BaseType_t xRunningPrivileged = prvRaisePrivilege();
//
//    sock = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP);
//
//    configASSERT(sock != FREERTOS_INVALID_SOCKET);
//
//    FreeRTOS_setsockopt(sock,0,FREERTOS_SO_RCVTIMEO,&xRecvTimeOut,sizeof(xRecvTimeOut));
//
//    addr.sin_port = FreeRTOS_htons(port);      /* Broadcast port */
//
//    sendAddr.sin_addr = FreeRTOS_inet_addr_quick( 255, 255, 255, 255 );
//    //sendAddr.sin_addr = FreeRTOS_inet_addr_quick( 192, 168, 2, 113 );
//    //sendAddr.sin_addr = 0;
//    sendAddr.sin_port = FreeRTOS_htons( 2012 );
//
//    FreeRTOS_bind(sock, &addr, sizeof(addr));
//    UARTwrite("Ethernet Ready...\n",strlen("Ethernet Ready...\n"));
//    for( ;; )
//    {
//        //UARTwrite(pcTaskName,strlen(pcTaskName));
//        //Delay for a period
//        memset(inBuf,'\0',100);
//        memset(outBuf,'\0',400);
//
//        if ((iReturned = FreeRTOS_recvfrom(sock, inBuf, 100, 0, &sourceAddr, sizeof(sourceAddr))) < 0){
//            UARTwrite("No data received\n", strlen("No data received\n"));
//        }else{
//            //UARTwrite("error\n",6);
//            UARTwrite("Data received via Ethernet\n", strlen("Data received via Ethernet\n"));
//            UARTwrite(strcat("Read something via ethernet: ", inBuf), strlen("Read something via ethernet: ") + strlen(inBuf));
//            switch(inBuf[0]){
//            case 0:
//                //Reset
//                UARTwrite("SW Reset...\n",strlen("SW Reset...\n"));
//                //*(uint32_t*)(0xFFFFFFE0) |= (1 << 15);
//                break;
//            case 1:
//                //Set IP address
//                UARTwrite("got a 1 here...\r\n", strlen("got a 1 here...\r\n"));
//                break;
//            case 10:
//                //Request firmware upgrade
//                UARTwrite("got a 10 here...\r\n", strlen("got a 10 here...\r\n"));
//                break;
//            case 11:
//                //Dump debug info
//                UARTwrite("got an 11 here...\r\n", strlen("got an 11 here...\r\n"));
//                break;
//            case 14:
//                UARTwrite("got a 14 here...\r\n", strlen("got a 14 here...\r\n"));
//                //NAND read test
//                break;
//            case 15:
//                UARTwrite("got a 15 here...\r\n", strlen("got a 15 here...\r\n"));
//                //NAND block erase
//                break;
//            case 16:
//                //NAND write test
//                break;
//            case 24:
//                //SPI read test
//                break;
//            case 25:
//                //Dump current data
//                //UARTwrite("Building Packet\n",strlen("Building Packet\n"));
//                sprintf(outBuf, "Decoded MCL: Not implemented\n");
//                sprintf(outBuf+strlen(outBuf), "Calculated Target BC Voltage: Not implemented\n");
//                sprintf(outBuf+strlen(outBuf), "Firmware version: 2.03\n");
//                sprintf(outBuf+strlen(outBuf), "Input 1: %X\n", ethReadEmifLocation(EMIF_DIN1));
//                sprintf(outBuf+strlen(outBuf), "Input 2: %X\n", ethReadEmifLocation(EMIF_DIN2));
//                sprintf(outBuf+strlen(outBuf), "Output: %X\n", ethReadEmifLocation(EMIF_OUT));
//                sprintf(outBuf+strlen(outBuf), "Tach 1 (Hz): %d\n", ethReadEmifLocation(EMIF_TACH1));
//                sprintf(outBuf+strlen(outBuf), "Tach 2 (Hz): %d\n", ethReadEmifLocation(EMIF_TACH2));
//                sprintf(outBuf+strlen(outBuf), "Tach 3 (Hz): %d\n", ethReadEmifLocation(EMIF_TACH3));
//                sprintf(outBuf+strlen(outBuf), "Tach 4 (Hz): %d\n", ethReadEmifLocation(EMIF_TACH4));
//                sprintf(outBuf+strlen(outBuf), "FPGA Version: %X\n", ethReadEmifLocation(EMIF_VERSION));
//                sprintf(outBuf+strlen(outBuf), "LED 1: %X\n", ethReadEmifLocation(EMIF_LED1));
//                sprintf(outBuf+strlen(outBuf), "LED 2: %X\n", ethReadEmifLocation(EMIF_LED2));
//                sprintf(outBuf+strlen(outBuf), "AIN 1: %X\n", ethReadEmifLocation(EMIF_AIN1));
//                //sprintf(outBuf+strlen(outBuf), "AIN 2: %X\n", ethReadEmifLocation(EMIF_AIN2));
//                sprintf(outBuf+strlen(outBuf), "AIN 2: %X\n", ethReadEmifLocation(EMIF_AIN4));
//                sprintf(outBuf+strlen(outBuf), "AIN 3: %X\n", ethReadEmifLocation(EMIF_AIN3));
//                //sprintf(outBuf+strlen(outBuf), "AIN 4: %X\n", ethReadEmifLocation(EMIF_AIN4));
//                sprintf(outBuf+strlen(outBuf), "AIN 4: %X\n", ethReadEmifLocation(EMIF_AIN2));
//                sprintf(outBuf+strlen(outBuf), "Scratch: %X\n", ethReadEmifLocation(EMIF_SCRATCH));
//                sprintf(outBuf+strlen(outBuf), "Timer: %d\n", ethReadEmifLocation(EMIF_TIMER));
//                //UARTwrite("Sending\n",strlen("Sending\n"));
//                FreeRTOS_sendto(sock,outBuf,strlen(outBuf),0,&sendAddr,sizeof(sendAddr));
//                //UARTwrite("Sent\n",strlen("Sent\n"));
//                break;
//            case 28: //set output reg val
//
//                break;
//            case 35: //live data
//                while(inBuf[0] != 36){
//                    //make delay of 10 ms between sending data
//                    output_length = sprintf(outBuf, "%c%c%c%c%c%c%c", 1,1,1,1,1,1,1);
//                    tempData = ethReadEmifLocation(EMIF_OUT);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[1], ((char*)(&tempData))[0]);
//                    tempData = ethReadEmifLocation(EMIF_TACH1);
//                    //tempData = (tempData/100);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[1], ((char*)(&tempData))[0]);
//                    tempData = ethReadEmifLocation(EMIF_TACH2);
//                    //tempData = (tempData/100);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[1], ((char*)(&tempData))[0]);
//                    output_length += sprintf(outBuf + output_length, "111122223333");
//                    tempData = ethReadEmifLocation(EMIF_DIN1);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[1], ((char*)(&tempData))[0]);
//                    tempData = ethReadEmifLocation(EMIF_DIN2);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[1], ((char*)(&tempData))[0]);
//                    tempData = ethReadEmifLocation(EMIF_AIN1);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[1], ((char*)(&tempData))[0]);
//                    tempData = ethReadEmifLocation(EMIF_AIN4);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[1], ((char*)(&tempData))[0]);
//                    tempData = ethReadEmifLocation(EMIF_AIN3);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[1], ((char*)(&tempData))[0]);
//                    tempData = ethReadEmifLocation(EMIF_AIN2);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[1], ((char*)(&tempData))[0]);
//                    output_length += sprintf(outBuf + output_length, "111");
//                    output_length += sprintf(outBuf + output_length, "VV"); //voltage count
//                    output_length += sprintf(outBuf + output_length, "1"); //whatever 0x30005000[0] is on the coldfire board
//                    output_length += sprintf(outBuf + output_length, "11111111111111");
//                    tempData = ethReadEmifLocation(EMIF_TACH3);
//                    //tempData = (tempData/100);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[0], ((char*)(&tempData))[1]);
//                    tempData = ethReadEmifLocation(EMIF_TACH4);
//                    //tempData = (tempData/100);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[0], ((char*)(&tempData))[1]);
//                    tempData = ethReadEmifLocation(EMIF_AIN4);
//                    output_length += sprintf(outBuf + output_length, "%c%c", ((char*)(&tempData))[0], ((char*)(&tempData))[1]);
//                    FreeRTOS_sendto(sock,outBuf,output_length,0,&sendAddr,sizeof(sendAddr));
//                    if ((iReturned = FreeRTOS_recvfrom(sock, inBuf, 100, 0, &sourceAddr, sizeof(sourceAddr))) < 0){
//
//                    }else{
//                        if(inBuf[0] > 0 && inBuf[0] < 20){
//                            //activate manual control of outputs (overriding brake control algo)
//                            ethSetManualControl(EMIF_MANUAL_ON);
//                        }else{
//                            //deactivate manual control of outputs
//                            ethSetManualControl(EMIF_MANUAL_OFF);
//                        }
//
//                        switch(inBuf[0]){
//                            case 1:
//                                ethSetManualControl(EMIF_APPLY_ON);
//                                break;
//                            case 2:
//                                ethSetManualControl(EMIF_APPLY_ON);
//                                break;
//                            case 3:
//                                ethSetManualControl(EMIF_RELEASE_ON);
//                                break;
//                            case 4:
//                                ethSetManualControl(EMIF_RELEASE_ON);
//                                break;
//                            case 5:
//                                ethSetManualControl(EMIF_ADUMP_ON);
//                                break;
//                            case 6:
//                                ethSetManualControl(EMIF_ADUMP_ON);
//                                break;
//                            case 7:
//                                ethSetManualControl(EMIF_BDUMP_ON);
//                                break;
//                            case 8:
//                                ethSetManualControl(EMIF_BDUMP_ON);
//                                break;
//                            case 9:
//                                ethSetManualControl(EMIF_WSS_ON);
//                                break;
//                            case 10:
//                                ethSetManualControl(EMIF_WSS_ON);
//                                break;
//                            case 11:
//                                ethSetManualControl(EMIF_APPLY_OFF);
//                                break;
//                            case 12:
//                                ethSetManualControl(EMIF_RELEASE_OFF);
//                                break;
//                            case 13:
//                                ethSetManualControl(EMIF_ADUMP_OFF);
//                                break;
//                            case 14:
//                                ethSetManualControl(EMIF_BDUMP_OFF);
//                                break;
//                            case 15:
//                                ethSetManualControl(EMIF_WSS_OFF);
//                                break;
//                            case 16:
//                                ethSetManualControl(EMIF_SCO_ON);
//                                break;
//                            case 17:
//                                ethSetManualControl(EMIF_SCO_OFF);
//                                break;
//                            case 18:
//                                ethSetManualControl(EMIF_FAULT_ON);
//                                break;
//                            case 19:
//                                ethSetManualControl(EMIF_FAULT_OFF);
//                                break;
//                        }
//
//                        if(inBuf[0] != 19){
//                            //energize brake fault output
//                            ethSetManualControl(EMIF_FAULT_ON);
//                        }
//                    }
//                }
//                break;
//            case 148: //used for auto test in BTU
//                sprintf(outBuf, "Firmware Version: 2.03 \n");
//                sprintf(outBuf+strlen(outBuf), "FPGA Version Contents: %X \n", ethReadEmifLocation(EMIF_VERSION));
//                sprintf(outBuf+strlen(outBuf), "NandSignature(High,Low)= (0,0) \n"); //no idea what this means
//                sprintf(outBuf+strlen(outBuf), "Timestamp1: 1/1/1, 1:1:1.1 \n");
//                sprintf(outBuf+strlen(outBuf), "IP Address: 1:1:1:1");
//                FreeRTOS_sendto(sock,outBuf,strlen(outBuf),0,&sendAddr,sizeof(sendAddr));
//                break;
//            case 175:
//                //pause faults
//
//                break;
//            default:
//                break;
//            }
//
//
//            /*
//            //FreeRTOS_sendto(sock,buf,10,0,&sourceAddr,sizeof(sourceAddr));
//            if(FreeRTOS_sendto(sock,"test\n",sizeof("test\n"),0,&sendAddr,sizeof(sendAddr))< 0){
//            //if(FreeRTOS_sendto(sock,"test\n",sizeof("test\n"),0,NULL,0)< 0){
//                UARTwrite("error\n",6);
//            }else{
//                UARTwrite("send-OK\n",8);
//            }
//            for(i = 0; i < 10; i++){
//                sprintf(tempString, "%d:%d\n\r", i, inBuf[i]);
//                UARTwrite(tempString,strlen(tempString));
//            }
//            */
//            //UARTwrite(buf,strlen(buf));
//        }
//
//        //UARTwrite("in eth\n",strlen("in eth\n"));
//        vTaskDelay(10);
//    }
//
//
//    for( ;; )
//    {
//        vTaskDelay(1000);
//    }
//    //portRESET_PRIVILEGE( xRunningPrivileged );
//}

uint16_t ethReadEmifLocation(char memSuffix){
    uint16_t data;
    uint32_t messageSend;
    uint32_t messageRecv;
    messageSend = 0x10000000; //First 4 bits: val of 1 = read
    messageSend |= memSuffix << 23; //Next 8 bits: emif location for MCU
    if(xTaskNotify(EMIFTaskHandle, messageSend, eSetValueWithoutOverwrite) == pdFALSE){
        UARTwrite("EMIF read notification failed.\n",strlen("EMIF read notification failed.\n"));
        data = 0xFFFF; //ffff for failure
    }else{
        if(xTaskNotifyWait(0x0, 0xFFFFFFFF, &messageRecv, pdMS_TO_TICKS(1000)) == pdFALSE){
            data = 0xFFFF;
            UARTwrite("EMIF read notification failed.\n",strlen("EMIF read notification failed.\n"));
        }else{
            data = (uint16_t)(messageRecv >> 8);
        }
    }

    return data;
}

uint16_t ethWriteEmifLocation(char memSuffix, uint16_t val){
    uint16_t data;
    uint32_t messageSend;
    uint32_t messageRecv;
    messageSend = 0x10000000; //First 4 bits: val of 1 = read
    messageSend |= memSuffix << 24; //Next 8 bits: emif location for MCU


    return 0;
}

void ethSetManualControl(unsigned char command){
    uint32_t messageSend;
    messageSend = 0x20000000; //First 4 bits: val of 2 = manual control mode
    messageSend |= command << 24;

    //if(xTaskNotify(EMIFTaskHandle, messageSend, eSetValueWithoutOverwrite) == pdFALSE){
    if(xTaskNotify(EMIFTaskHandle, messageSend, eSetValueWithoutOverwrite) == pdFALSE){
        UARTwrite("EMIF set output notification failed.\n",strlen("EMIF set output notification failed.\n"));
    }else{ //just wait for the emif task to clear its notifications
        if(xTaskNotifyWait(0x0, 0xFFFFFFFF, &messageSend, pdMS_TO_TICKS(1000)) == pdFALSE){
            UARTwrite("EMIF never responded.\n",strlen("EMIF never responded.\n"));
        }else{
        }
    }
}

void EMIFTaskCheckNotifications(void){
    uint32_t messageRecv = 0;
    uint32_t messageSend = 0;
    unsigned long emifAddr = 0;
    unsigned char emifParameter;
    if(xTaskNotifyWait(0x0, 0xFFFFFFFF, &messageRecv, pdMS_TO_TICKS(1)) == pdTRUE){
        if((messageRecv & 0xF0000000) == 0x10000000){ //if command = ethernet request to read emif
            emifParameter = (messageRecv & 0x0F800000) >> 23;
            emifAddr = emifFieldToAddr(emifParameter);
            messageRecv &= 0xFF000000;
            messageSend = messageRecv | (*(unsigned short*)emifAddr) << 8;
            if(xTaskNotify(EthernetTaskHandle, messageSend, eSetValueWithoutOverwrite) == pdFALSE){
                UARTwrite("EMIF task could not respond.\n",strlen("EMIF task could not respond.\n"));
            }
        } else if((messageRecv & 0xF0000000) == 0x20000000){ //if command = ethernet request to set outputs
            emifParameter = (messageRecv & 0x0F000000) >> 24;
            emifSetOutput(emifParameter);
            messageSend = messageRecv;
            if(xTaskNotify(EthernetTaskHandle, messageSend, eSetValueWithoutOverwrite) == pdFALSE){
                UARTwrite("EMIF task could not respond.\n",strlen("EMIF task could not respond.\n"));
            }
        }
    }
    return;
}

unsigned long emifFieldToAddr(unsigned char emifParameter){
    unsigned long emifAddr = 0;
    switch(emifParameter){
        case EMIF_NAND:
            emifAddr = 0x60000000;
            break;
        case EMIF_AIN1:
            emifAddr = 0x60000200;
            break;
        case EMIF_AIN2:
            emifAddr = 0x60000400;
            break;
        case EMIF_AIN3:
            emifAddr = 0x60000600;
            break;
        case EMIF_AIN4:
            emifAddr = 0x60000800;
            break;
        case EMIF_LED1:
            emifAddr = 0x60000a00;
            break;
        case EMIF_LED2:
            emifAddr = 0x60000c00;
            break;
        case EMIF_TACH1:
            emifAddr = 0x60000e00;
            break;
        case EMIF_TACH2:
            emifAddr = 0x60001000;
            break;
        case EMIF_OUT:
            emifAddr = 0x60001200;
            break;
        case EMIF_DIN1:
            emifAddr = 0x60001400;
            break;
        case EMIF_DIN2:
            emifAddr = 0x60001600;
            break;
        case EMIF_SCRATCH:
            emifAddr = 0x60001800;
            break;
        case EMIF_VERSION:
            emifAddr = 0x60001a00;
            break;
        case EMIF_TACH3:
            emifAddr = 0x60002a00;
            break;
        case EMIF_TACH4:
            emifAddr = 0x60002c00;
            break;
        case EMIF_TIMER:
            emifAddr = 0x60002800;
            break;
        default:
            emifAddr = 0x60000000;
            break;
        }
    return emifAddr;
}

uint16_t emifSetOutput(unsigned char command){
    uint16_t regVal = *(uint16_t*)0x60001200;
    uint16_t offset = 0x20;
    switch(command){
        case EMIF_MANUAL_OFF:
            //not implemented
            break;
        case EMIF_MANUAL_ON:
            //not implemented
            break;
        case EMIF_APPLY_ON:
            regVal |= 0x1*offset;
            break;
        case EMIF_APPLY_OFF:
            regVal &= ~0x1*offset;
            break;
        case EMIF_RELEASE_ON:
            regVal |= 0x2*offset;
            break;
        case EMIF_RELEASE_OFF:
            regVal &= ~0x2*offset;
            break;
        case EMIF_ADUMP_ON:
            regVal |= 0x4*offset;
            break;
        case EMIF_ADUMP_OFF:
            regVal &= ~0x4*offset;
            break;
        case EMIF_BDUMP_ON:
            regVal |= 0x8*offset;
            break;
        case EMIF_BDUMP_OFF:
            regVal &= ~0x8*offset;
            break;
        case EMIF_WSS_ON:
            regVal |= 0x10*offset;
            break;
        case EMIF_WSS_OFF:
            regVal &= ~0x10*offset;
            break;
        case EMIF_SCO_ON:
            regVal |= 0x20*offset;
            break;
        case EMIF_SCO_OFF:
            regVal &= ~0x20*offset;
            break;
        case EMIF_FAULT_ON:
            regVal |= 0x40*offset;
            break;
        case EMIF_FAULT_OFF:
            regVal &= ~0x40*offset;
            break;
        default:
            //should not happen
            break;
    }

    *(uint16_t*)0x60001200 = regVal;

    return regVal;
}

uint16_t emifSetOutputBlah(unsigned char command){
    uint16_t regVal = *(uint16_t*)0x60001200;
    uint16_t offset = 0x20;
    switch(command){
        case EMIF_MANUAL_OFF:
            //not implemented
            break;
        case EMIF_MANUAL_ON:
            //not implemented
            break;
        case EMIF_APPLY_ON:
            regVal |= 0x1*offset;
            break;
        case EMIF_APPLY_OFF:
            regVal &= ~0x1*offset;
            break;
        case EMIF_RELEASE_ON:
            regVal |= 0x2;
            break;
        case EMIF_RELEASE_OFF:
            regVal &= ~0x2;
            break;
        case EMIF_ADUMP_ON:
            regVal |= 0x4;
            break;
        case EMIF_ADUMP_OFF:
            regVal &= ~0x4;
            break;
        case EMIF_BDUMP_ON:
            regVal |= 0x8;
            break;
        case EMIF_BDUMP_OFF:
            regVal &= ~0x8;
            break;
        case EMIF_WSS_ON:
            regVal |= 0x10;
            break;
        case EMIF_WSS_OFF:
            regVal &= ~0x10;
            break;
        case EMIF_SCO_ON:
            regVal |= 0x20;
            break;
        case EMIF_SCO_OFF:
            regVal &= ~0x20;
            break;
        case EMIF_FAULT_ON:
            regVal |= 0x40;
            break;
        case EMIF_FAULT_OFF:
            regVal &= ~0x40;
            break;
        default:
            //should not happen
            break;
    }

    *(uint16_t*)0x60001200 = regVal;

    return regVal;
}

uint16_t emifSetOutputOld(unsigned char command){
    uint16_t regVal = *(uint16_t*)0x60001200;

    switch(command){
        case EMIF_MANUAL_OFF:
            //not implemented
            break;
        case EMIF_MANUAL_ON:
            //not implemented
            break;
        case EMIF_APPLY_ON:
            regVal |= 0x100000;
            break;
        case EMIF_APPLY_OFF:
            regVal &= ~0x100000;
            break;
        case EMIF_RELEASE_ON:
            regVal |= 0x1000000;
            break;
        case EMIF_RELEASE_OFF:
            regVal &= ~0x1000000;
            break;
        case EMIF_ADUMP_ON:
            regVal |= 0x80;
            break;
        case EMIF_ADUMP_OFF:
            regVal &= ~0x80;
            break;
        case EMIF_BDUMP_ON:
            regVal |= 0x100;
            break;
        case EMIF_BDUMP_OFF:
            regVal &= ~0x100;
            break;
        case EMIF_WSS_ON:
            regVal |= 0x8000;
            break;
        case EMIF_WSS_OFF:
            regVal &= ~0x8000;
            break;
        case EMIF_SCO_ON:
            regVal |= 0x400;
            break;
        case EMIF_SCO_OFF:
            regVal &= ~0x400;
            break;
        case EMIF_FAULT_ON:
            regVal |= 0x800;
            break;
        case EMIF_FAULT_OFF:
            regVal &= ~0x800;
            break;
        default:
            //should not happen
            break;
    }

    *(uint16_t*)0x60001200 = regVal;

    return regVal;
}

/******************************************************************************
FUNCTION NAME           : RS485Task
PURPOSE & DESCRIPTION   : This function is called by RS-48t task as task entry
                          function and this function handled the Modbus packet
                          transmit and receive functionality.
INPUTS                  :
  Parameters            : None
 Module Data Referenced : None
 Global Data Referenced :
OUTPUTS                 :
 Return Value           : None
 Module Data Modified   : None
 Caller Data Modified   : None
 Global Data Modified   :

FUNCTION CALLED BY      : xTaskCreate()
FUNCTION CALLING        :
******************************************************************************/
void RS485Task( void *pvParameters )
{
    const char *pcTaskName = "Task: RS-485 Modbus is running\r\n";

    for( ;; )
    {
        //UARTwrite(pcTaskName,strlen(pcTaskName));
        CheckPacketReceiveCompletion();
        //Delay for a period
        vTaskDelay(110);
    }
}

/******************************************************************************
FUNCTION NAME           : SDCardTask
PURPOSE & DESCRIPTION   : This function is called by SD card task as task entry
                          function and this function handled the read and write
                          data on SD card.
INPUTS                  :
  Parameters            : None
 Module Data Referenced : None
 Global Data Referenced :
OUTPUTS                 :
 Return Value           : None
 Module Data Modified   : None
 Caller Data Modified   : None
 Global Data Modified   :

FUNCTION CALLED BY      : xTaskCreate()
FUNCTION CALLING        :
******************************************************************************/
static void SDCardTask( void *pvParameters )
{
    const char *pcTaskName = "Task: SD is running\r\n";
    for( ;; )
    {
        //UARTwrite(pcTaskName,strlen(pcTaskName));
        //SD_Card_Read(); // Need to address semaphores-conflicts with serial port
        //Delay for a period
        vTaskDelay(1000);
    }
}


/******************************************************************************
FUNCTION NAME           : RTCTask
PURPOSE & DESCRIPTION   : This function is called byRTC task as task entry
                          function and this function handled the read and write
                          Date & Time from/to external RTC via SPI interface.
INPUTS                  :
  Parameters            : None
 Module Data Referenced : None
 Global Data Referenced :
OUTPUTS                 :
 Return Value           : None
 Module Data Modified   : None
 Caller Data Modified   : None
 Global Data Modified   :

FUNCTION CALLED BY      : xTaskCreate()
FUNCTION CALLING        :UARTwrite();
******************************************************************************/
static void RTCTask( void *pvParameters )
{
    const char *pcTaskName = "Task: RTC is running\r\n";
    //initRTCWatchdog();

    for( ;; )
    {
        //UARTwrite(pcTaskName,strlen(pcTaskName));
        //Delay for a period
        vTaskDelay(120);
    }
}


/******************************************************************************
FUNCTION NAME           : EMIFTask
PURPOSE & DESCRIPTION   : This function is called by EMIF task as task entry
                          function and this function handled the read and write
                          FPGA.
INPUTS                  :
  Parameters            : None
 Module Data Referenced : None
 Global Data Referenced :
OUTPUTS                 :
 Return Value           : None
 Module Data Modified   : None
 Caller Data Modified   : None
 Global Data Modified   :

FUNCTION CALLED BY      : xTaskCreate()
FUNCTION CALLING        :
******************************************************************************/
static void EMIFTask( void *pvParameters )
{
    const char *pcTaskName = "Task: EMIF is running\r\n";
    char testDataStr[64];
    unsigned short data = 0;
    int led_on = 0;
    int i = 0;
    //char write_data[TOTAL_BYTES_PER_PAGE];
    //sprintf(write_data,"This is a test of the NAND...\n");
    /*
    static const MemoryRegion_t mem[1] =
        {
             { (void *)(0x60000000), (unsigned long)(0x00002000), portMPU_REGION_READ_WRITE }
        };

    vTaskAllocateMPURegions(EMIFTaskTaskHandle, mem);
    */

    FlashReset();
    vTaskDelay(1000);
    sprintf(testDataStr, "NAND High: %X\n", ReadNandHigh());
    UARTwrite(testDataStr,strlen(testDataStr));
    sprintf(testDataStr, "NAND Low: %X\n", ReadNandLow());
    UARTwrite(testDataStr,strlen(testDataStr));
    sprintf(testDataStr, "NAND Debug: %X\n", ReadNandDebug());
    UARTwrite(testDataStr,strlen(testDataStr));
    sprintf(testDataStr, "NAND Flash Status: %X\n", FlashStatus());
    UARTwrite(testDataStr,strlen(testDataStr));
    sprintf(testDataStr, "NAND FIFO Status: %X\n", FifoStatus());
    UARTwrite(testDataStr,strlen(testDataStr));
    EraseBlock(123);
    if(ValidateErase(123, read_result)){
        sprintf(testDataStr, "NAND Erase appears to be successful.\n");
    }else{
        sprintf(testDataStr, "NAND Erase failed.\n");
    }
    UARTwrite(testDataStr,strlen(testDataStr));


    //for(i = 0; i < TOTAL_WORDS_PER_PAGE; i++ ){
        //sprintf(testDataStr, "Data[%d]: %x\n", i, read_result[i]);
        //UARTwrite(testDataStr,strlen(testDataStr));
    //    read_result[i] = (unsigned short)i;
    //}
    //

    //WritePage(read_result, 0, 123); //write test data to block 123, page 0
    //vTaskDelay(1000);
    //for(i = 0; i < TOTAL_WORDS_PER_PAGE; i++ ){
            //sprintf(testDataStr, "Data[%d]: %x\n", i, read_result[i]);
            //UARTwrite(testDataStr,strlen(testDataStr));
    //        read_result[i] = (unsigned short)0;
    //    }
    //NandPageRead(123,0,read_result);
    //UARTwrite(((char*)NandPageRead(123,0)),strlen(((char*)NandPageRead(123,0))));

    for( ;; )
    {
        //UARTwrite(pcTaskName,strlen(pcTaskName));
        //BaseType_t xRunningPrivileged = prvRaisePrivilege();
        //*(unsigned short*)0x60000a00 = 0xFFFF;
        //*(unsigned short*)0x60000c00 = 0x0;
        //vTaskDelay(10);
        //*(unsigned short*)0x60000a00 = 0x00;
        //*(unsigned short*)0x60000c00 = 0xFFFF;
        //portRESET_PRIVILEGE( xRunningPrivileged );


        if(led_on){
            //gioSetBit(gioPORTB,7,0);
            //gioSetBit(gioPORTA,3,0);
            //*(uint16_t*)0x60001200 = 0x0;
        }else{
            //gioSetBit(gioPORTB,7,1);
            //gioSetBit(gioPORTA,3,1);
            //*(uint16_t*)0x60001200 = 0xFFFF;
        }

        //*(uint16_t*)0x60001200 *= 2;
        //*(uint16_t*)0x60000a00 = *(uint16_t*)0x60001200;
        //*(uint16_t*)0x60000c00 = *(uint16_t*)0x60001200;
        //sprintf(testDataStr, "Tach1: %d\n", *(uint16_t*)emifFieldToAddr(EMIF_TACH3));
        //sprintf(testDataStr, "AIN1: %d\n", *(uint16_t*)emifFieldToAddr(EMIF_AIN1));
        //UARTwrite(testDataStr,strlen(testDataStr));
        //if(*(uint16_t*)0x60001200 == 0
        //    *(uint16_t*)0x60001200 = 1;
        //}
        //led_on = !led_on;
        EMIFTaskCheckNotifications();
        //sprintf(testDataStr, "output REG: %X\n", *(uint16_t*)0x60001200);
        //UARTwrite(testDataStr,strlen(testDataStr));
        vTaskDelay(1);
    }
}

//not currently working
int initRTCWatchdog(void)
{
    //Init SPI2
    uint16 dBuff[20];
    char data[20];
    spiDAT1_t dataRegConf;
    dataRegConf.CS_HOLD = true;
    dataRegConf.WDEL = false;
    dataRegConf.DFSEL = SPI_FMT_0;
    dataRegConf.CSNR = 0;
    struct DateTime date;
    SpiDataStatus_t tX;
    SpiDataStatus_t rX;
    spiInit();
    //spiREG2->PC4 |= SPI_PIN_ENA;
    spiREG2->PC3 = spiREG2->PC3 | 1;
    vTaskDelay(1000);
    spiREG2->PC3 = spiREG2->PC3 & 0xFFFFFFFE;
    //spiREG2->PC5 |= SPI_PIN_ENA;
    vTaskDelay(1000);
    //spiREG2->PC4 |= SPI_PIN_ENA;
    tX = SpiTxStatus(spiREG2);
    rX = SpiRxStatus(spiREG2);
    int i = 0;
    for(i = 0; i < 20; i++){
        spiGetData(spiREG2, &dataRegConf, 20, dBuff); //20 bytes???
        data[i] = dBuff[0];
    }

    date.Millisecond = ((((dBuff[1] & 0xF0) >> 4)*10) + ((dBuff[1] & 0x0F)*1));
    date.Second = (((dBuff[2] & 0xF0) >> 4)*10) + (dBuff[2] & 0x0F);
    date.Minute = (((dBuff[3] & 0xF0) >> 4)*10) + (dBuff[3] & 0x0F);
    date.Hour = (((dBuff[4] & 0xF0) >> 4)*10) + (dBuff[4] & 0x0F);
    date.Day = (((dBuff[6] & 0xF0) >> 4)*10) + (dBuff[6] & 0x0F);
    date.Month = (((dBuff[7] & 0xF0) >> 4)*10) + (dBuff[7] & 0x0F);
    date.Year = (((dBuff[8] & 0xF0) >> 4)*10) + (dBuff[8] & 0x0F);

    return 0;
}

int UARTwrite(const char *pcBuf, unsigned int ui32Len)
{
    uint8_t ucIdx =0;

    // Send the characters
    if( xRecursiveMutex != NULL )
    {
        // The semaphore was created successfully.
        // The semaphore can now be used.
        if( xSemaphoreTakeRecursive( xRecursiveMutex, ( TickType_t ) 10 ) == pdTRUE )
        {
            for(ucIdx = 0; ucIdx < ui32Len; ucIdx++)
            {
                //
                // If the character to the UART is \n, then add a \r before it so that
                // \n is translated to \n\r in the output.
                //
                if(pcBuf[ucIdx] == '\n')
                {
                    sciSendByte(sciREG, '\r');
                }

                //
                // Send the character to the UART output.
                //
                sciSendByte(sciREG, pcBuf[ucIdx]);
            }
            xSemaphoreGiveRecursive( xRecursiveMutex );
        }
        else
        {
            // We could not obtain the mutex and can therefore not access
            // the shared resource safely.
            return 0;
        }
    }
    else
    {
        // The semaphore was not created
        return 0;
    }
    return(ucIdx);
}
/* USER CODE END */
