/**
 * @file  eth.c
 *
 * @brief File containing all network specific tasks, helper function definitions and event hooks
 */

#include "eth.h"

/**
 * @brief     Event hook for IP Network Event
 *
 * @param[in] eNetworkEvent A eIPCallbackEvent_t enum identifying success or failure of the network
 *                          initialization event.
 */
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
    } else {
//        UARTWrite("IP Task not created!\n", strlen("IP Task not created!\n"));
    }
}


/**
 * @brief     This function is called by Ethernet task as task entry
 *            function and this function handled the UDP packet send
 *            and receive functionality.
 *
 * @param[in] pvParamters Any information to be passed to the task, NULL if
 *                        not required.
 */
void EthernetTask(void *pvParameters) {
    UARTwrite("Ethernet Task started...\n", strlen("Ethernet Task started...\n"));
    xSocket_t xSocket;
    struct freertos_sockaddr xBindAddress;
    struct freertos_sockaddr xSourceAddress;
    uint8_t ucRecvBuffer[ 100 ];
    int32_t lRecvBytes;
    const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 200 );
    socklen_t xSourceAddressLength = sizeof( xSourceAddress );
    UARTwrite("Socket...\n", strlen("Socket...\n"));

    // Create a UDP socket
    special_debug_flag = 2786;
    xSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP );
    special_debug_flag = 0;
    if (xSocket == FREERTOS_INVALID_SOCKET)
    {
        UARTwrite("Failed to create socket\n", strlen("Failed to create socket\n"));
        while (1);
    }
    else
    {
        UARTwrite("Socket created successfully\n", strlen("Socket created successfully\n"));
    }


    UARTwrite("Socket...\n", strlen("Socket...\n"));

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
