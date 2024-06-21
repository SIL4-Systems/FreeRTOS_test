/*******************************************************************************
File Name               :  rs485comm.c
Revision/Version        :  1.0
Purpose/Description     :  This file contains the high level functions of
                           Modbus protocol data process.
Author                  :  Anil
Created Date            :  Feb 12, 2019
******************************************************************************/

/******************************************************************************
        Includes
******************************************************************************/
#include <string.h>
#include "sys_common.h"
#include "rs485comm.h"
#include "sci.h"

/******************************************************************************
        Module Variables
******************************************************************************/
//static uint8_t *stat_pucDriverLevelRxBuffer[NUM_SERIAL_COMM_PORT];
uint8_t g_aucDriverReceiveBufferForSCI2[MODBUS_MAX_PACK_SIZE];
uint8_t g_aucAppReceiveBufferForSCI1[MODBUS_MAX_PACK_SIZE];
uint8_t g_aucResponsePacket[MODBUS_MAX_PACK_SIZE];
uint8_t g_ucPacketReceivedOnSCI1 = CLEAR;
uint8_t g_ucPacketReceivedOnSCI2 = CLEAR;
uint8_t g_aucPacketCounter = CLEAR;
uint8_t g_aucAppPacketCounter = CLEAR;
uint8_t g_ucAppTxByteCountSCI1 = CLEAR;
uint8_t g_ucAppTxByteCountSCI2 = CLEAR;
uint8_t g_ucResponseReadyForSCI1 = CLEAR;
uint8_t g_ucResponseReadyForSCI2 = CLEAR;
uint8_t g_aucAppReceiveBufferForSCI2[MODBUS_MAX_PACK_SIZE];

typedef union
{
    uint16_t m_uiDigitalInput;
    struct
    {
        uint16_t DI0:1;
        uint16_t DI1:1;
        uint16_t DI2:1;
        uint16_t DI3:1;
        uint16_t DI4:1;
        uint16_t DI5:1;
        uint16_t DI6:1;
        uint16_t DI7:1;
        uint16_t DI8:1;
        uint16_t DI9:1;
        uint16_t DI10:1;
        uint16_t DI11:1;
        uint16_t DI12:1;
        uint16_t DI13:1;
        uint16_t DI14:1;
        uint16_t DI15:1;
    }DIBits;
}REG_DES_DI;

typedef union
{
    uint16_t m_uiDigitalOutput;
    struct
    {
        uint16_t DO0:1;
        uint16_t DO1:1;
        uint16_t DO2:1;
        uint16_t DO3:1;
        uint16_t DO4:1;
        uint16_t DO5:1;
        uint16_t DO6:1;
        uint16_t DO7:1;
        uint16_t DO8:1;
        uint16_t DO9:1;
        uint16_t DO10:1;
        uint16_t DO11:1;
        uint16_t DO12:1;
        uint16_t DO13:1;
        uint16_t DO14:1;
        uint16_t DO15:1;
    }DOBits;
}REG_DES_DO;


// Modbus Discrete Input Registers Memory Mapping
typedef union
{
    uint32_t ulDigitalInput;
    struct
    {
        uint32_t DI0:1;
        uint32_t DI1:1;
        uint32_t DI2:1;
        uint32_t DI3:1;
        uint32_t DI4:1;
        uint32_t DI5:1;
        uint32_t DI6:1;
        uint32_t DI7:1;
        uint32_t DI8:1;
        uint32_t DI9:1;
        uint32_t DI10:1;
        uint32_t DI11:1;
        uint32_t DI12:1;
        uint32_t DI13:1;
        uint32_t DI14:1;
        uint32_t DI15:1;
        uint32_t DI16:1;
        uint32_t DI17:1;
        uint32_t DI18:1;
        uint32_t DI19:1;
        uint32_t DIUnused:12;
    }DIBits;
}DISCRETE_INPUT_REG;

DISCRETE_INPUT_REG  g_stDisInReg;


typedef struct
{
    uint8_t DO1;
    uint8_t DO2;
    uint8_t DO3;
    uint8_t DO4;
    uint8_t DO5;
    uint8_t DO6;
    uint8_t DO7;
    uint8_t DO8;
    uint8_t DO9;
    uint8_t DO10;
    uint8_t DO11;
    uint8_t DO12;
    uint8_t DO13;
    uint8_t DO14;
    uint8_t DO15;
    uint8_t DO16;
    uint8_t LED1;
    uint8_t LED2;
    uint8_t LED3;
    uint8_t LED4;
    uint8_t LED5;
    uint8_t LED6;
    uint8_t LED7;
    uint8_t LED8;
    uint8_t LED9;
    uint8_t LED10;
    uint8_t LED11;
    uint8_t LED12;
    uint8_t LED13;
    uint8_t LED14;
    uint8_t LED15;
    uint8_t LED16;
    uint8_t LED17;
    uint8_t LED18;
    uint8_t LED19;
    uint8_t LED20;
    uint8_t LED21;
    uint8_t LED22;
    uint8_t LED23;
    uint8_t LED24;
    uint8_t LED25;
    uint8_t LED26;
    uint8_t LED27;
    uint8_t LED28;
    uint8_t LED29;
    uint8_t LED30;

}DISCRETE_OUTPUT_REG;

DISCRETE_OUTPUT_REG  g_stDisOutReg;


typedef struct
{
    uint16_t m_uiOffset;
    uint32_t *m_pulRegAddress;
    REG_SIZE m_ucRegSize;
    REG_RW_ACCESS m_ucRegAccessRights;
}MODBUS_REGMAP;

typedef struct
{
    uint16_t m_uiOffset;
    uint16_t *m_puiRegAddress;
    REG_SIZE m_ucRegSize;
    REG_RW_ACCESS m_ucRegAccessRights;
}MODBUS_REGMAP1;

typedef struct
{
    uint16_t m_uiOffset;
    uint8_t *m_pucRegAddress;
    REG_SIZE m_ucRegSize;
    REG_RW_ACCESS m_ucRegAccessRights;
}MODBUS_REGMAP2;


// This follow the Read Digital Input command (0x02) - Discrete access with Read Only Access
// Address Range starts with 10001 - 19999
MODBUS_REGMAP g_stModbusDiscreteDigitalInputReg[NO_OF_MODBUS_DISCRETE_READ_REG] =
{
    {   0x0000, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0001, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0002, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0003, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0004, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0005, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0006, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0007, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0008, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0009, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x000A, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x000B, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x000C, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x000D, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x000E, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x000F, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0010, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0011, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0012, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS},
    {   0x0013, &g_stDisInReg.ulDigitalInput, REG_SIZE1, READ_ACCESS}
};


typedef struct
{

    uint16_t m_uiAnalogIn1;
    uint16_t m_uiAnalogIn2;
    uint16_t m_uiAnalogIn3;
    uint16_t m_uiAnalogIn4;

    REG_DES_DI m_uiDigitalInput1;
    REG_DES_DI m_uiDigitalInput2;

    uint16_t m_uiTachInput1;
    uint16_t m_uiTachInput2;
    uint16_t m_uiTachInput3;
    uint16_t m_uiTachInput4;

    uint16_t m_uiPulseInput1;
    uint16_t m_uiPulseInput2X_1;
    uint16_t m_uiPulseInput4X_1;

    uint16_t m_uiPulseInput2;
    uint16_t m_uiPulseInput2X_2;
    uint16_t m_uiPulseInput4X_2;

    uint16_t m_uiPulseInput3;
    uint16_t m_uiPulseInput2X_3;
    uint16_t m_uiPulseInput4X_3;

    uint16_t m_uiPulseInput4;
    uint16_t m_uiPulseInput2X_4;
    uint16_t m_uiPulseInput4X_4;

}ANALOG_INPUTREG;

ANALOG_INPUTREG g_stAnalogInputReg;

// This follow the Read Analog Input command (0x04) - 16 bit access with Read Only Access
// Address Range starts with 40001 - 49999
MODBUS_REGMAP1 g_stModbusAnalogInputReg[NO_OF_MODBUS_ANALOG_READ_REG] =
{
    {   0x0000, &g_stAnalogInputReg.m_uiAnalogIn1, REG_SIZE2, READ_ACCESS},
    {   0x0002, &g_stAnalogInputReg.m_uiAnalogIn2, REG_SIZE2, READ_ACCESS},
    {   0x0004, &g_stAnalogInputReg.m_uiAnalogIn3, REG_SIZE2, READ_ACCESS},
    {   0x0006, &g_stAnalogInputReg.m_uiAnalogIn4, REG_SIZE2, READ_ACCESS},
    {   0x0008, &g_stAnalogInputReg.m_uiTachInput1, REG_SIZE2, READ_ACCESS},
    {   0x000A, &g_stAnalogInputReg.m_uiTachInput2, REG_SIZE2, READ_ACCESS},
    {   0x000C, &g_stAnalogInputReg.m_uiTachInput3, REG_SIZE2, READ_ACCESS},
    {   0x000E, &g_stAnalogInputReg.m_uiTachInput4, REG_SIZE2, READ_ACCESS},
    {   0x0010, &g_stAnalogInputReg.m_uiPulseInput1, REG_SIZE2, READ_ACCESS},
    {   0x0012, &g_stAnalogInputReg.m_uiPulseInput2X_1, REG_SIZE2, READ_ACCESS},
    {   0x0014, &g_stAnalogInputReg.m_uiPulseInput4X_1, REG_SIZE2, READ_ACCESS},
    {   0x0016, &g_stAnalogInputReg.m_uiPulseInput2, REG_SIZE2, READ_ACCESS},
    {   0x0018, &g_stAnalogInputReg.m_uiPulseInput2X_2, REG_SIZE2, READ_ACCESS},
    {   0x001A, &g_stAnalogInputReg.m_uiPulseInput4X_2, REG_SIZE2, READ_ACCESS},
    {   0x001C, &g_stAnalogInputReg.m_uiPulseInput3, REG_SIZE2, READ_ACCESS},
    {   0x001E, &g_stAnalogInputReg.m_uiPulseInput2X_3, REG_SIZE2, READ_ACCESS},
    {   0x0020, &g_stAnalogInputReg.m_uiPulseInput4X_3, REG_SIZE2, READ_ACCESS},
    {   0x0022, &g_stAnalogInputReg.m_uiPulseInput4, REG_SIZE2, READ_ACCESS},
    {   0x0024, &g_stAnalogInputReg.m_uiPulseInput2X_4, REG_SIZE2, READ_ACCESS},
    {   0x0026, &g_stAnalogInputReg.m_uiPulseInput4X_4, REG_SIZE2, READ_ACCESS}
};

// This follow the Write Single Coil command (0x05)/ Write Multiple Coil Command (0x15) - Discrete access with Write Only Access
// Address Range starts with 0001 - 9999
MODBUS_REGMAP2 g_stModbusDiscreteDigitalOutputReg[NO_OF_MODBUS_WRITE_REG] =
{
    {   0x0000, &g_stDisOutReg.DO1, REG_SIZE1, READ_ACCESS},
    {   0x0001, &g_stDisOutReg.DO2, REG_SIZE1, READ_ACCESS},
    {   0x0002, &g_stDisOutReg.DO3, REG_SIZE1, READ_ACCESS},
    {   0x0003, &g_stDisOutReg.DO4, REG_SIZE1, READ_ACCESS},
    {   0x0004, &g_stDisOutReg.DO5, REG_SIZE1, READ_ACCESS},
    {   0x0005, &g_stDisOutReg.DO6, REG_SIZE1, READ_ACCESS},
    {   0x0006, &g_stDisOutReg.DO7, REG_SIZE1, READ_ACCESS},
    {   0x0007, &g_stDisOutReg.DO8, REG_SIZE1, READ_ACCESS},
    {   0x0008, &g_stDisOutReg.DO9, REG_SIZE1, READ_ACCESS},
    {   0x0009, &g_stDisOutReg.DO10, REG_SIZE1, READ_ACCESS},
    {   0x000A, &g_stDisOutReg.DO11, REG_SIZE1, READ_ACCESS},
    {   0x000B, &g_stDisOutReg.DO12, REG_SIZE1, READ_ACCESS},
    {   0x000C, &g_stDisOutReg.DO13, REG_SIZE1, READ_ACCESS},
    {   0x000D, &g_stDisOutReg.DO14, REG_SIZE1, READ_ACCESS},
    {   0x000E, &g_stDisOutReg.DO15, REG_SIZE1, READ_ACCESS},
    {   0x000F, &g_stDisOutReg.DO16, REG_SIZE1, READ_ACCESS},
    {   0x0010, &g_stDisOutReg.LED1, REG_SIZE1, READ_ACCESS},
    {   0x0011, &g_stDisOutReg.LED2, REG_SIZE1, READ_ACCESS},
    {   0x0012, &g_stDisOutReg.LED3, REG_SIZE1, READ_ACCESS},
    {   0x0013, &g_stDisOutReg.LED4, REG_SIZE1, READ_ACCESS},
    {   0x0014, &g_stDisOutReg.LED5, REG_SIZE1, READ_ACCESS},
    {   0x0015, &g_stDisOutReg.LED6, REG_SIZE1, READ_ACCESS},
    {   0x0016, &g_stDisOutReg.LED7, REG_SIZE1, READ_ACCESS},
    {   0x0017, &g_stDisOutReg.LED8, REG_SIZE1, READ_ACCESS},
    {   0x0018, &g_stDisOutReg.LED9, REG_SIZE1, READ_ACCESS},
    {   0x0019, &g_stDisOutReg.LED10, REG_SIZE1, READ_ACCESS},
    {   0x001A, &g_stDisOutReg.LED11, REG_SIZE1, READ_ACCESS},
    {   0x001B, &g_stDisOutReg.LED12, REG_SIZE1, READ_ACCESS},
    {   0x001C, &g_stDisOutReg.LED13, REG_SIZE1, READ_ACCESS},
    {   0x001D, &g_stDisOutReg.LED14, REG_SIZE1, READ_ACCESS},
    {   0x001E, &g_stDisOutReg.LED15, REG_SIZE1, READ_ACCESS},
    {   0x001F, &g_stDisOutReg.LED16, REG_SIZE1, READ_ACCESS},
    {   0x0020, &g_stDisOutReg.LED17, REG_SIZE1, READ_ACCESS},
    {   0x0021, &g_stDisOutReg.LED18, REG_SIZE1, READ_ACCESS},
    {   0x0022, &g_stDisOutReg.LED19, REG_SIZE1, READ_ACCESS},
    {   0x0023, &g_stDisOutReg.LED20, REG_SIZE1, READ_ACCESS},
    {   0x0024, &g_stDisOutReg.LED21, REG_SIZE1, READ_ACCESS},
    {   0x0025, &g_stDisOutReg.LED22, REG_SIZE1, READ_ACCESS},
    {   0x0026, &g_stDisOutReg.LED23, REG_SIZE1, READ_ACCESS},
    {   0x0027, &g_stDisOutReg.LED24, REG_SIZE1, READ_ACCESS},
    {   0x0028, &g_stDisOutReg.LED25, REG_SIZE1, READ_ACCESS},
    {   0x0029, &g_stDisOutReg.LED26, REG_SIZE1, READ_ACCESS},
    {   0x002A, &g_stDisOutReg.LED27, REG_SIZE1, READ_ACCESS},
    {   0x002B, &g_stDisOutReg.LED28, REG_SIZE1, READ_ACCESS},
    {   0x002C, &g_stDisOutReg.LED29, REG_SIZE1, READ_ACCESS},
    {   0x002D, &g_stDisOutReg.LED30, REG_SIZE1, READ_ACCESS}

};

/******************************************************************************
       Local Function Prototypes
******************************************************************************/
//static void CheckPacketReceiveCompletion(void);
//static uint16_t ModbusCRC(const uint8_t* pucMessagePointer, uint8_t ucLengthOfPacket);
//static uint16_t Merge(uint8_t ucByte_H, uint8_t ucByte_L);
//static uint8_t ModbusExceptionResponse(uint8_t ucErrorCode,uint8_t* pucPacket);
//static bool Validate_Modbus_Writereg(uint16_t usnRegnum, uint16_t usnRegcount,uint8_t usnFunctioncode);
//static uint8_t ModbusPacketize(uint8_t* pucPacket,uint8_t ucPacketLength);
void rs485RegInit(void);

void rs485RegInit(void)
{
    g_stDisInReg.ulDigitalInput = 0x55555555;

    g_stAnalogInputReg.m_uiAnalogIn1=100;
    g_stAnalogInputReg.m_uiAnalogIn2=150;
    g_stAnalogInputReg.m_uiAnalogIn3=200;
    g_stAnalogInputReg.m_uiAnalogIn4=250;
    g_stAnalogInputReg.m_uiTachInput1=25;
    g_stAnalogInputReg.m_uiTachInput2=50;
    g_stAnalogInputReg.m_uiTachInput3=75;
    g_stAnalogInputReg.m_uiTachInput4=100;
    g_stAnalogInputReg.m_uiPulseInput1=10;
    g_stAnalogInputReg.m_uiPulseInput2X_1=20;
    g_stAnalogInputReg.m_uiPulseInput4X_1=30;
    g_stAnalogInputReg.m_uiPulseInput2=40;
    g_stAnalogInputReg.m_uiPulseInput2X_2=50;
    g_stAnalogInputReg.m_uiPulseInput4X_2=60;
    g_stAnalogInputReg.m_uiPulseInput3=10;
    g_stAnalogInputReg.m_uiPulseInput2X_3=20;
    g_stAnalogInputReg.m_uiPulseInput4X_3=30;
    g_stAnalogInputReg.m_uiPulseInput4=40;
    g_stAnalogInputReg.m_uiPulseInput2X_4=50;
    g_stAnalogInputReg.m_uiPulseInput4X_4=60;

}

static void ModbusResponse(uint8_t* pucReceivedPacket,uint8_t ucPacketLength);


uint16_t CRC16 (uint8_t *nData, uint16_t wLength)
{
    static const uint16_t wCRCTable[] = {
    0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };

    uint8_t nTemp;
    uint16_t wCRCWord = 0xFFFF;

   while (wLength--)
   {
      nTemp = *nData++ ^ wCRCWord;
      wCRCWord >>= 8;
      wCRCWord ^= wCRCTable[nTemp];
   }
   return wCRCWord;

}



void CheckPacketReceiveCompletion(void)
{
    /* Monitor if Packet has been received in  SCI Port */
    if(SET == g_ucPacketReceivedOnSCI2)
    {
#ifdef INSTALL_DEBUG
        UARTwrite("\r\v Packet received on SCI 2");
#endif
        g_ucPacketReceivedOnSCI2 = CLEAR;
        /* check which protocol is set for port 2 and call the corresponding
        response function */
        ModbusResponse(&g_aucAppReceiveBufferForSCI2[PACKET_BEGINNING],
                       g_aucAppPacketCounter);
    }

    /* If Response is ready then start transmitting it on the respective port */
    if(SET == g_ucResponseReadyForSCI2)
    {

        memset(g_aucAppReceiveBufferForSCI2, 0x00, g_aucAppPacketCounter);
#ifdef INSTALL_DEBUG
        UARTwrite("\r\v starting transmission on SCI 2");
#endif

        g_ucResponseReadyForSCI2 = CLEAR;
        sciSend(scilinREG,g_ucAppTxByteCountSCI2,(uint8*)&g_aucResponsePacket[PACKET_BEGINNING]);
    }
}

uint8_t ModbusWriteMultipleCoils(uint8_t * pucReceivedPacket)
{
    uint8_t ucReturnVal = CLEAR;
    uint8_t ucAddressMatchFound = CLEAR;

    uint16_t uiStartAddress;
    uint16_t uiNoOfRegisters;
    uint16_t uiTotalNoOfRegisters;

    uint8_t ucCount;
    uint8_t ucIndex;
    uint8_t ucIndex1;
    uint8_t ucData;

    uint8_t ucQuotient;

    uint8_t ucDataLength;

    uint8_t ucNoOfBytesToBeWrite;

    uint8_t *pucReceivePayloadPointer;



    pucReceivePayloadPointer = pucReceivedPacket + STARTADD_INDEX;

    uiStartAddress = (((uint16_t )*pucReceivePayloadPointer++ << 8) & 0xFF00) + ((uint16_t)*pucReceivePayloadPointer++ & 0x00FF);

    // Validate Address Index is matching with the table
    for(ucCount = 0; ucCount < NO_OF_MODBUS_WRITE_REG; ucCount++)
    {
        if(uiStartAddress == g_stModbusDiscreteDigitalOutputReg[ucCount].m_uiOffset)
        {
            ucAddressMatchFound = SET;
            break;
        }
    }
    if(ucAddressMatchFound != SET)
    {
        return ucReturnVal;
    }

    // Validate No of Registers to be read
    uiTotalNoOfRegisters = (((uint16_t )*pucReceivePayloadPointer++ << 8) & 0xFF00) + ((uint16_t)*pucReceivePayloadPointer++ & 0x00FF);
    uiNoOfRegisters = uiTotalNoOfRegisters;
    if((NO_OF_MODBUS_WRITE_REG - ucCount) < uiNoOfRegisters)
    {
        return ucReturnVal;
    }

    // Read Number of Bytes to be copied
    ucNoOfBytesToBeWrite = (uint8_t )*pucReceivePayloadPointer++;

    // Compute Number of Digital Output and LED Output Registers to be written
    ucData = (uint8_t )*pucReceivePayloadPointer++;

    ucQuotient = uiNoOfRegisters / 8;


    if(ucQuotient > 0)
    {
        for(ucIndex = 0; ucIndex < ucQuotient; ucIndex++)
        {
            ucDataLength = (uiNoOfRegisters >= 8)? 8: uiNoOfRegisters;
            for(ucIndex1 = 0; ucIndex1 < ucDataLength; ucIndex1++)
            {
                if(ucData & (1 << ucIndex1))
                {
                    *(g_stModbusDiscreteDigitalOutputReg[ucCount].m_pucRegAddress) = 0x01;
                }
                else
                {
                    *(g_stModbusDiscreteDigitalOutputReg[ucCount].m_pucRegAddress) = 0x00;
                }
                // Get the Next Address
                ucCount++;
            }
            // Read Next Byte
            ucData = (uint8_t )*pucReceivePayloadPointer++;
            uiNoOfRegisters = uiNoOfRegisters - ucDataLength;
        }
    }

    for(ucIndex = 0; ucIndex < uiNoOfRegisters; ucIndex++)
    {
        if(ucData & (1 << ucIndex))
        {
            *(g_stModbusDiscreteDigitalOutputReg[ucCount].m_pucRegAddress) = 0x01;
        }
        else
        {
            *(g_stModbusDiscreteDigitalOutputReg[ucCount].m_pucRegAddress) = 0x00;
        }
        // Get the Next Address
        ucCount++;
    }

    // Copy Slave Address and Function Code Information
    g_ucAppTxByteCountSCI2 = 0;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = SLAVE_ADDRESS;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = *(pucReceivedPacket + COMMAND_INDEX);

    // Copy Start Address
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)((uiStartAddress >> 8) & 0x00FF);
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(uiStartAddress & 0x00FF);

    // Copy Data
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)((uiTotalNoOfRegisters >> 8) & 0x00FF);
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(uiTotalNoOfRegisters & 0x00FF);

    ucReturnVal = SET;

    return ucReturnVal;
}

uint8_t ModbusWriteSingleCoils(uint8_t * pucReceivedPacket)
{
    uint8_t ucReturnVal = CLEAR;
    uint8_t ucAddressMatchFound = CLEAR;

    uint16_t uiStartAddress;
    uint8_t ucCount;
    uint16_t uiData;
    uint8_t *pucReceivePayloadPointer;

    pucReceivePayloadPointer = pucReceivedPacket + STARTADD_INDEX;

    uiStartAddress = (((uint16_t )*pucReceivePayloadPointer++ << 8) & 0xFF00) + ((uint16_t)*pucReceivePayloadPointer++ & 0x00FF);

    // Validate Address Index is matching with the table
    for(ucCount = 0; ucCount < NO_OF_MODBUS_ANALOG_READ_REG; ucCount++)
    {
        if(uiStartAddress == g_stModbusDiscreteDigitalOutputReg[ucCount].m_uiOffset)
        {
            ucAddressMatchFound = SET;
            break;
        }
    }
    if(ucAddressMatchFound != SET)
    {
        return ucReturnVal;
    }

    // Get the Data
    uiData = (((uint16_t )*pucReceivePayloadPointer++ << 8) & 0xFF00) + ((uint16_t)*pucReceivePayloadPointer++ & 0x00FF);

    if(uiData != 0)
    {
        *(g_stModbusDiscreteDigitalOutputReg[ucCount].m_pucRegAddress) = 0x01;
    }
    else
    {
        *(g_stModbusDiscreteDigitalOutputReg[ucCount].m_pucRegAddress) = 0x00;
    }

    // Copy Slave Address and Function Code Information
    g_ucAppTxByteCountSCI2 = 0;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = SLAVE_ADDRESS;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = *(pucReceivedPacket + COMMAND_INDEX);

    // Copy Start Address
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)((uiStartAddress >> 8) & 0x00FF);
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(uiStartAddress & 0x00FF);

    // Copy Data
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)((uiData >> 8) & 0x00FF);
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(uiData & 0x00FF);

    ucReturnVal = SET;

    return ucReturnVal;
}

uint8_t ModbusReadAnalogInputs(uint8_t * pucReceivedPacket)
{
    uint8_t ucReturnVal = CLEAR;
    uint8_t ucAddressMatchFound = CLEAR;

    uint16_t uiStartAddress;
    uint16_t uiNoOfRegisters;

    uint8_t ucCount;
    uint8_t ucIndex;

    uint16_t *puiRegAddress;

    uint8_t *pucReceivePayloadPointer;

    pucReceivePayloadPointer = pucReceivedPacket + STARTADD_INDEX;

    uiStartAddress = (((uint16_t )*pucReceivePayloadPointer++ << 8) & 0xFF00) + ((uint16_t)*pucReceivePayloadPointer++ & 0x00FF);

    // Validate Address Index is matching with the table
    for(ucCount = 0; ucCount < NO_OF_MODBUS_ANALOG_READ_REG; ucCount++)
    {
        if(uiStartAddress == g_stModbusAnalogInputReg[ucCount].m_uiOffset)
        {
            ucAddressMatchFound = SET;
            break;
        }
    }
    if(ucAddressMatchFound != SET)
    {
        return ucReturnVal;
    }

    // Validate No of Registers to be read
    uiNoOfRegisters = (((uint16_t )*pucReceivePayloadPointer++ << 8) & 0xFF00) + ((uint16_t)*pucReceivePayloadPointer++ & 0x00FF);
    if((NO_OF_MODBUS_ANALOG_READ_REG - ucCount) < uiNoOfRegisters)
    {
        return ucReturnVal;
    }

    // Copy Slave Address and Function Code Information
    g_ucAppTxByteCountSCI2 = 0;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = SLAVE_ADDRESS;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = *(pucReceivedPacket + COMMAND_INDEX);

    // Copy Number of Bytes More
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = uiNoOfRegisters * 2;;

    // Copy Data
    for(ucIndex = 0; ucIndex < uiNoOfRegisters; ucIndex++)
    {
        puiRegAddress = g_stModbusAnalogInputReg[ucCount].m_puiRegAddress;

        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)((*puiRegAddress >> 8) & 0x00FF);
        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)((*puiRegAddress) & 0x00FF);

        ucCount++;
    }

    ucReturnVal = SET;

    return ucReturnVal;
}

uint8_t ModbusReadDiscreteInputs(uint8_t * pucReceivedPacket)
{
    uint8_t ucReturnVal = CLEAR;
    uint8_t ucAddressMatchFound = CLEAR;

    uint16_t uiStartAddress;
    uint16_t uiNoOfRegisters;

    uint8_t ucIndex;
    uint8_t ucCount;
    uint8_t ucByteRead;
    uint32_t ulDataToBeCopied;
    uint8_t *pucReceivePayloadPointer;

    pucReceivePayloadPointer = pucReceivedPacket + STARTADD_INDEX;

    uiStartAddress = (((uint16_t )*pucReceivePayloadPointer++ << 8) & 0xFF00) + ((uint16_t)*pucReceivePayloadPointer++ & 0x00FF);

    // Validate Address Index is matching with the table
    for(ucCount = 0; ucCount < NO_OF_MODBUS_DISCRETE_READ_REG; ucCount++)
    {
        if(uiStartAddress == g_stModbusDiscreteDigitalInputReg[ucCount].m_uiOffset)
        {
            ucAddressMatchFound = SET;
            break;
        }
    }
    if(ucAddressMatchFound != SET)
    {
        return ucReturnVal;
    }

    // Validate No of Registers to be read
    uiNoOfRegisters = (((uint16_t )*pucReceivePayloadPointer++ << 8) & 0xFF00) + ((uint16_t)*pucReceivePayloadPointer++ & 0x00FF);
    if((NO_OF_MODBUS_DISCRETE_READ_REG - ucCount) < uiNoOfRegisters)
    {
        return ucReturnVal;
    }

    // Copy Slave Address and Function Code Information
    g_ucAppTxByteCountSCI2 = 0;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = SLAVE_ADDRESS;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = *(pucReceivedPacket + COMMAND_INDEX);

    // Copy Number of Bytes More
    ucByteRead = (uiNoOfRegisters/8) + ((uiNoOfRegisters%8)?1:0);
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = ucByteRead;

    // Copy Data
    ulDataToBeCopied = g_stDisInReg.ulDigitalInput >> (uint32_t)ucCount;

    for(ucIndex = 0; ucIndex < ucByteRead; ucIndex++)
    {
        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = ulDataToBeCopied >> (ucIndex * 8);
    }

    ucReturnVal = SET;

    return ucReturnVal;
}

static void ModbusIllegalDataResponse(const uint8_t* pucReceivedPacket)
{
    uint16_t CRCValue;
    // Illegal Data Address Code
    g_ucAppTxByteCountSCI2 = 0;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = SLAVE_ADDRESS;
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = 0x80 + *(pucReceivedPacket + COMMAND_INDEX);
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = ILLEGAL_DATA_ADDRESS;
    // Compute CRC for this packet and Append with the message
    CRCValue = CRC16((uint8_t *)&g_aucResponsePacket, g_ucAppTxByteCountSCI2);
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(CRCValue >> 8);
    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)CRCValue;
    g_ucResponseReadyForSCI2 = SET;
}

static void ModbusResponse(uint8_t* pucReceivedPacket,uint8_t ucPacketLength)
{
    uint16_t CRCValue;
    uint16_t CRCValueReceived;

    // if slave address matches then proceed further validation else discard the message
    if(SLAVE_ADDRESS == *pucReceivedPacket)
    {
        CRCValueReceived = ((uint16_t)*(pucReceivedPacket + (ucPacketLength - 2)) << 8) | ((uint16_t)*(pucReceivedPacket + (ucPacketLength - 1)));
        // Compute CRC for this received packet
        CRCValue = CRC16(pucReceivedPacket, (ucPacketLength -2));
        // CRC matches then proceed further else send error code saying that
        if(CRCValueReceived == CRCValue)
        {
            switch(*(pucReceivedPacket + COMMAND_INDEX))
            {
                case MODBUS_FUNC_READ_DISCRETE_INPUT:
                {
                    // Pass the pointer which points Start Address and Get the Response Data Filled
                    if(ModbusReadDiscreteInputs(pucReceivedPacket))
                    {
                        // Compute CRC for this packet and Append with the message
                        CRCValue = CRC16((uint8_t *)&g_aucResponsePacket, g_ucAppTxByteCountSCI2);
                        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(CRCValue >> 8);
                        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)CRCValue;
                        g_ucResponseReadyForSCI2 = SET;
                    }
                    else
                    {
                        ModbusIllegalDataResponse(pucReceivedPacket);
                        g_ucResponseReadyForSCI2 = SET;
                    }
                }
                break;
                case MODBUS_FUNC_READ_INPUT_REG:
                {
                    // Pass the pointer which points Start Address
                    if(ModbusReadAnalogInputs(pucReceivedPacket))
                    {
                        // Compute CRC for this packet and Append with the message
                        CRCValue = CRC16((uint8_t *)&g_aucResponsePacket, g_ucAppTxByteCountSCI2);
                        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(CRCValue >> 8);
                        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)CRCValue;
                        g_ucResponseReadyForSCI2 = SET;
                    }
                    else
                    {
                        ModbusIllegalDataResponse(pucReceivedPacket);
                        g_ucResponseReadyForSCI2 = SET;
                    }
                }
                break;
                case MODBUS_FUNC_WRITE_SINGLE_COIL:
                    // Pass the pointer which points Start Address
                    if(ModbusWriteSingleCoils(pucReceivedPacket))
                    {
                        // Compute CRC for this packet and Append with the message
                        CRCValue = CRC16((uint8_t *)&g_aucResponsePacket, g_ucAppTxByteCountSCI2);
                        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(CRCValue >> 8);
                        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)CRCValue;
                        g_ucResponseReadyForSCI2 = SET;
                    }
                    else
                    {
                        ModbusIllegalDataResponse(pucReceivedPacket);
                        g_ucResponseReadyForSCI2 = SET;
                    }
                break;
                case MODBUS_FUNC_WRITE_MULTIPLE_COIL:
                    // Pass the pointer which points Start Address
                    if(ModbusWriteMultipleCoils(pucReceivedPacket))
                    {
                        // Compute CRC for this packet and Append with the message
                        CRCValue = CRC16((uint8_t *)&g_aucResponsePacket, g_ucAppTxByteCountSCI2);
                        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(CRCValue >> 8);
                        g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)CRCValue;
                        g_ucResponseReadyForSCI2 = SET;
                    }
                    else
                    {
                        ModbusIllegalDataResponse(pucReceivedPacket);
                        g_ucResponseReadyForSCI2 = SET;
                    }
                break;
                default:
                    // Unsupported Code
                    g_ucAppTxByteCountSCI2 = 0;
                    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = SLAVE_ADDRESS;
                    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = 0x80 + *(pucReceivedPacket + COMMAND_INDEX);
                    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = ILLEGAL_FUNCTION_CODE;

                    // Compute CRC for this packet and Append with the message
                    CRCValue = CRC16((uint8_t *)&g_aucResponsePacket, g_ucAppTxByteCountSCI2);
                    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)(CRCValue >> 8);
                    g_aucResponsePacket[g_ucAppTxByteCountSCI2++] = (uint8_t)CRCValue;
                    g_ucResponseReadyForSCI2 = SET;

                    g_ucResponseReadyForSCI2 = SET;
                break;
            }
        }
        else
        {
            // if CRC does not match, Discard the message
        }
    }
    else
    {
        // if Slave Address does not match, Discard the message

    }
}




