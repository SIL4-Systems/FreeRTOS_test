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
#include "emif.h"
#include "spi.h"
#include "os_portable.h"
#include "reg_system.h"
#include "nandflash.h"
#include "uart.h"
#include "eth.h"

#include "FreeRTOS_Sockets.h"

#include "helpers.h"
#include "emac.h"
#include "mdio.h"

/* FOr the LED tasks to randomize the blinking */
unsigned int SubTask_Number = 0;
unsigned int Task_Number = 9;
unsigned int task_data = 0xff00;

/* Task queues */
xQueueHandle xTask1Queue;
xQueueHandle xTask2Queue;

// Define Task Handles
extern xTaskHandle EthernetTaskHandle;
xTaskHandle RS485TaskHandle;
xTaskHandle RTCTaskHandle;
xTaskHandle SDCardTaskHandle;
xTaskHandle EMIFTaskHandle;

xSemaphoreHandle xMutex = NULL;

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
static void RS485Task( void *pvParameters );
static void SDCardTask( void *pvParameters );
static void RTCTask( void *pvParameters );
static void EMIFTask( void *pvParameters );
//int initEMIF(void);
uint16_t emifSetOutput(unsigned char command);

uint16_t ethReadEmifLocation(char memSuffix);
unsigned long emifFieldToAddr(unsigned char emifParameter);
void EMIFTaskCheckNotifications(void);

void ethSetManualControl(unsigned char command);
void initPHY(void);

// PHY Registers (DP83640)
#define PHY_BCR             0x00    // Basic Control Register
#define PHY_BSR             0x01    // Basic Status Register
#define PHY_IDR1            0x02    // PHY Identifier Register 1
#define PHY_IDR2            0x03    // PHY Identifier Register 2
#define PHY_ANAR            0x04    // Auto-Negotiation Advertisement Register
#define PHY_ANLPAR          0x05    // Auto-Negotiation Link Partner Ability Register

// Basic Status Register (BSR) bits
#define BSR_LINK_STATUS     0x0004  // Link status bit

// Function to read a PHY register
static uint16_t readPHYRegister(uint32_t mdioBaseAddr, uint32_t phyAddr, uint32_t regNum)
{
    uint16_t data;
    MDIOPhyRegRead(mdioBaseAddr, phyAddr, regNum, &data);
    return data;
}

// Function to check PHY link status
int checkPHYLinkStatus(uint32_t mdioBaseAddr, uint32_t phyAddr)
{
    uint16_t bsr = readPHYRegister(mdioBaseAddr, phyAddr, PHY_BSR);

    if (bsr & BSR_LINK_STATUS)
    {
        UARTwrite("PHY Link Status: UP\n", strlen("PHY Link Status: UP\n"));
        return 1;
    }
    else
    {
        UARTwrite("PHY Link Status: DOWN\n", strlen("PHY Link Status: DOWN\n"));
        return 0;
    }
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

uint8	emacAddress[6U] = 	{0x00U, 0x08U, 0xEEU, 0x03U, 0xA6U, 0x6CU};
uint32 	emacPhyAddress	=	1U;

int main(void)
{
/* USER CODE BEGIN (3) */
    special_debug_flag = 0;

    BaseType_t returnVal = pdFALSE;;
    char *message = "hello\n";

    _enable_interrupts();
    xRecursiveMutex = xSemaphoreCreateRecursiveMutex();

    // Initialize the peripherals
    InitializePeripherals();

    if (EMACHWInit(emacAddress) != EMAC_ERR_CONNECT) {
        UARTwrite("EMAC HW initialization successful\n", strlen("EMAC HW initialization successful\n"));
    } else {
        UARTwrite("EMAC HW initialization failed!\n", strlen("EMAC HW initialization failed!\n"));
    }

//    EMAC_LwIP_Main(emacAddress);

    /* Create Tasks */
    xTask1Queue = xQueueCreate(3, sizeof(long));
    xTask2Queue = xQueueCreate(3, sizeof(long));

    SubTask_Number = 0;
    long xmitVal = 1;
    xQueueSendToBack(xTask2Queue, &xmitVal, portMAX_DELAY);

    UARTwrite("Performing IP INIT\n", strlen("Performing IP INIT\n"));

    if (FreeRTOS_IPInit( ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, emacAddress ) == pdFAIL) {
        UARTwrite("IP INIT failure!\n", strlen("IP INIT failure!\n"));
    } else {
        UARTwrite("IP assignment success\n",strlen("IP assignment success\n"));
    }

//    checkPHYLinkStatus(MDIO_0_BASE, EMAC_PHYADDRESS);

    EthernetTask(NULL);

    // Create Ethernet task
//    configASSERT(xTaskCreate(EthernetTask, "EthernetTask", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, NULL) == pdTRUE);

//    if (xTaskCreate(EthernetTask,"EthernetTask", configMINIMAL_STACK_SIZE + 100, NULL,3 , &EthernetTaskHandle) != pdTRUE) {
//        message = "Could not create EthernetTask\n";
//        UARTwrite(message, strlen(message));
//        free(message);
//        while(1);
//    } else {
//        message = "Created EthernetTask\n";
//        UARTwrite(message,strlen(message));
//    }

//    UARTwrite("here3\n", strlen("here3\n"));
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

//    special_debug_flag = 2786;
    /* Start the OS scheduler */
    vTaskStartScheduler();

    UARTwrite("Starting the indefinite loop now", strlen("Starting the indefinite loop now"));

    vPortFree(message);
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
static void InitializePeripherals() {
    //uint8 ip_addr[4] = { 10, 1, 10, 96  };
    //uint8 netmask[4] = { 255, 0, 0, 0 };
    //uint8 gateway[4] = { 10, 1, 10, 1 };

    //initialize serial communication (Console and RS-485)
    sciInit();
    // GPIO initialized
    gioInit();

//    initPHY();

    // Wait for the PHY to initialize (e.g., another 100 ms)
//    vTaskDelay(pdMS_TO_TICKS(100));

//    gioSetDirection(gioPORTA, 0b00001000); //enable output on 3rd bit

   // gioSetDirection(gioPORTB, 0b10000000); //enable output on 7th bit
//    gioSetDirection(gioPORTB, 0b00010000); //enable output to 4th bit (GPIO_LED3)
    //gioSetDirection(gioPORTA, 0b00001000);
//    gioPORTB->PSL = gioPORTB->PSL | 0b10000000; //pullup/down select
//    gioPORTB->PULDIS = gioPORTB->PULDIS | 0b10000000; //pullup/down enable

    // Enable IRQ for all Peripherals
    _enable_IRQ();
    // Initialized the free running timer at 0.5 mSec interval
    InitializeTimer();
    // Activate SCI-2 Interrupt based on TI requirement
    sciSend(scilinREG,0,g_ucSciLinTxReg);
    sciReceive(scilinREG, 0, g_ucSciLinRxReg);
//
//    rs485RegInit();

    /** eth stuff */
//    UARTwrite("Starting ETH config!", strlen("Starting ETH config!"));
//
//    MDIOInit(MDIO_0_BASE, configCPU_CLOCK_HZ, 2500000);
//
//    UARTwrite("Starting EMAC config!", strlen("Starting EMAC config!"));
////
////    UARTwrite("Starting PHY config!", strlen("Starting PHY config!"));
////
//    hdkif_t *hdkif = pvPortMalloc(sizeof(hdkif_t *));
//    hdkif->emac_base = EMAC_0_BASE;
//    hdkif->phy_addr = EMAC_PHYADDRESS;
//    hdkif->mdio_base = MDIO_0_BASE;
//
//    // Initialize and configure the DP83640 PHY and setup the link
//    if (EMACLinkSetup(hdkif) != EMAC_ERR_OK) {
//        UARTwrite("PHY link setup failed\n", strlen("PHY link setup failed\n"));
//        while (1);
//    } else {
//        UARTwrite("PHY link setup successfully\n", strlen("PHY link setup successfully\n"));
//    }
//
//    while (checkPHYLinkStatus(MDIO_0_BASE, EMAC_PHYADDRESS) == 0);
//
//    // Example: Configure GIO port A pin 0 as output for PHY reset
//    gioSetDirection(gioPORTA, 0x00000001);

    // Assert PHY reset (set the pin low)
//    gioSetBit(gioPORTA, 0, 0);

    // Wait for a short duration (e.g., 100 ms)
//    vTaskDelay(pdMS_TO_TICKS(100));

    // Deassert PHY reset (set the pin high)
//    gioSetBit(gioPORTA, 0, 1);

//    EMACInit(EMAC_CTRL_0_BASE, EMAC_0_BASE);
//    EMACMACSrcAddrSet(EMAC_CTRL_0_BASE, emacAddress);

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

void vApplicationMallocFailedHook(void){
    UARTwrite("!!!!!!!!!!!!\n",strlen("!!!!!!!!!!!!\n"));
}

// This function will be called if a stack overflow is detected
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    // Print out the name of the task that overflowed its stack
    UARTwrite("Stack overflow in task: ", strlen("Stack overflow in task: "));
    UARTwrite(pcTaskName, strlen(pcTaskName));
    UARTwrite("\n", 1);

    // Optionally, you can add code here to handle the stack overflow
    // For example, you can log the error, reset the system, etc.

    // Enter an infinite loop to halt the system
    while (1);
}

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
/* USER CODE END */
