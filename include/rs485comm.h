/*
 * rs485comm.h
 *
 *  Created on: Feb 12, 2019
 *      Author: anil.verma
 */

#ifndef INCLUDE_RS485COMM_H_
#define INCLUDE_RS485COMM_H_

/******************************************************************************
        Includes
******************************************************************************/

/******************************************************************************
        Typedef structures, unions
******************************************************************************/
#define INSTALL_DEBUG


#define COMMAND_INDEX   1
#define STARTADD_INDEX  2

//Supported Modbus Function Codes
#define MODBUS_FUNC_READ_DISCRETE_INPUT            (uint8_t)0x02
#define MODBUS_FUNC_READ_INPUT_REG                 (uint8_t)0x04
#define MODBUS_FUNC_WRITE_SINGLE_COIL              (uint8_t)0x05
#define MODBUS_FUNC_WRITE_MULTIPLE_COIL            (uint8_t)0x0F

// Modbus Exception (error) Codes
#define ILLEGAL_FUNCTION_CODE          (uint8_t)1
#define ILLEGAL_DATA_ADDRESS           (uint8_t)2

// Modbus Slave Address
#define SLAVE_ADDRESS   0x00

// Maximum Number of Register Available Read/Write registers
#define NO_OF_MODBUS_DISCRETE_READ_REG  20
#define NO_OF_MODBUS_ANALOG_READ_REG    20
#define NO_OF_MODBUS_WRITE_REG  46

#define TOTAL_NOOF_DO_REGS 16
#define TOTAL_NOOF_LED_REGS 30

#define MODBUS_FUNC_WRITE_MULTI_HOLDING_REG        (uint8_t)0x10
#define MODBUS_MAX_PACK_SIZE                       (uint8_t)0x40
#define MODBUS_MAX_NUMBER_OF_REGS                  (uint8_t)29
#define SET_MSB_FOR_EXCEPTION_RESP                 (uint8_t)0x80
#define MODBUS_REG_SIZE                            (uint8_t)2
#define MODBUS_SIZE_OF_2_REG                       (uint8_t)4
#define NEXT_BYTE                                  (uint8_t)1
#define PREV_BYTE                                  (uint8_t)1



#define MODBUS          (uint8_t)2
// for testing
#define MODBUS_MAX_OFFSET           (uint8_t)755
#define REG_CNT_POS_FOR_READ_CMD    (uint8_t)4
#define MAX_MODBUS_WOREG_SIZE       (uint8_t)5
#define MAX_MODBUS_RWREG_SIZE       (uint8_t)40

// Default RS-485 and RS-232 configuration
#define DEFAULT_RS485_PROTOCOL          (uint8_t)2
#define DEFAULT_RS485_BAUD              (uint8_t)2
#define DEFAULT_RS485_ADDR              (uint8_t)1
#define DEFAULT_TTL_PROTOCOL            (uint8_t)1
#define DEFAULT_TTL_BAUD                (uint8_t)2
#define DEFAULT_TTL_ADDR                (uint8_t)1



#define SET       (uint8_t)1
#define CLEAR     (uint8_t)0
#define ENABLE    (uint8_t)1
#define DISABLE   (uint8_t)0
#define ERROR     (uint8_t)1
#define SUCCESS   (uint8_t)0

#define PACKET_BEGINNING            (uint8_t)0
#define MODBUS_DATA_ADDR_BYTE1_POS  (uint8_t)2
#define MODBUS_DATA_ADDR_BYTE2_POS  (uint8_t)3
#define HEX_WORD                    (uint8_t)0xFFFF
#define SHIFT_1_BIT                 (uint8_t)1
#define BYTE_MASK_LSB               (uint8_t)0x01
#define LOOP_COUNT_FOR_BYTE         (uint8_t)7
#define GENERATOR_POLYNOMIAL        (uint16_t)0xA001
#define SHIFT_BYTE                  (uint8_t)0x08
#define COMMAND_BYTE_POSITION       (uint8_t)1
#define MODBUS_EXCEPTION_CODE_POSITION      (uint8_t)2
#define MODBUS_EXCEPTION_CODE_POSITION      (uint8_t)2
#define DATA_SIZE_IN_EXCEPTION_RESP         (uint8_t)3

#define SCI2_PORT 1
typedef enum
{
    READ_ACCESS = 0,
    WRITE_ACCESS = 1,
}REG_RW_ACCESS;

typedef enum
{
    REG_SIZE1 = 1,
    REG_SIZE2 = 2,
}REG_SIZE;



typedef union   utIntToChar
{
    uint8_t   ucAttribute[4];
    uint32_t  unFullattribute;
}ut_IntToChar;

typedef struct
{
    void* m_ucVariablePointer;
    uint32_t m_unParamInfo;
}stModbusAddresses;

typedef enum
{
    UNDEFINED = 0,
    READ_ONLY,
    READ_WRITE,
    WRITE_ONLY
}et_AccessTypes;


//Configuration Parameters usually changed from Communication or User Interface
typedef struct
{
    uint8_t   m_ucProtocol485;
    uint8_t   m_ucBaudrate485;
    uint8_t   m_ucAddr485;
    uint8_t   m_ucProtocolTTL;
    uint8_t   m_ucBaudrateTTL;
    uint8_t   m_ucAddrTTL;
}stConfigs;

/******************************************************************************
        Module variables
******************************************************************************/

uint8_t g_Modbus_Recieve_Buffer[65];
extern uint8_t g_ucPacketReceivedOnSCI1;  /* Packet received on SCI 1 */
extern uint8_t g_ucPacketReceivedOnSCI2;  /* Packet received on SCI 2 */
extern uint8_t g_aucDriverReceiveBufferForSCI2[MODBUS_MAX_PACK_SIZE];
extern uint8_t g_aucAppReceiveBufferForSCI2[MODBUS_MAX_PACK_SIZE];
extern uint8_t g_aucResponsePacket[MODBUS_MAX_PACK_SIZE];
extern uint8_t g_aucPacketCounter;
extern uint8_t g_aucAppPacketCounter;
extern uint8_t g_ucAppTxByteCountSCI1;
extern uint8_t g_ucAppTxByteCountSCI2;
extern uint8_t g_ucResponseReadyForSCI1;
extern uint8_t g_ucResponseReadyForSCI2;
stConfigs  g_stConfig;

/******************************************************************************
       Function Prototypes
******************************************************************************/
void ModbusReceive(uint8_t ucRxData, uint8_t ucPortNumber);
void Sci2TimerISR(void);
extern void UARTprintf(const char *pcString, ...);
void CheckPacketReceiveCompletion(void);

extern void rs485RegInit(void);



#endif /* INCLUDE_RS485COMM_H_ */
