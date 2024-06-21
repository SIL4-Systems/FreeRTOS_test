/*
 * File: uart.c
 *
 * Contains functions for serial communication via UART
 */

#include "uart.h"

/*
 * @brief Write a message of specified length over UART
 *
 * @param[in] pcBuf Pointer to the character string to be sent over serial
 * @param[in] ui32Len Number of characters to be written
 *
 * @return 0 in case of failure and 1 for success
 */
int UARTwrite(const char *pcBuf, unsigned int ui32Len) {
    uint8_t ucIdx =0;

    // Send the characters
    if( xRecursiveMutex != NULL ) {
        // The semaphore was created successfully.
        // The semaphore can now be used.
        if( xSemaphoreTakeRecursive( xRecursiveMutex, ( TickType_t ) 10 ) == pdTRUE ) {
            for(ucIdx = 0; ucIdx < ui32Len; ucIdx++) {
                // If the character to the UART is \n, and the previous character was not '\r',
                // then add a \r before it so that \n is translated to \r\n in the output.
                if(pcBuf[ucIdx] == '\n' && !(ucIdx != 0 && pcBuf[ucIdx - 1] == '\r')) {
                    sciSendByte(sciREG, '\r');
                }

                // Send the character to the UART output.
                sciSendByte(sciREG, pcBuf[ucIdx]);
            }
            xSemaphoreGiveRecursive( xRecursiveMutex );
        }
        else {
            // We could not obtain the mutex and can therefore not access
            // the shared resource safely.
            return 0;
        }
    }
    else {
        // The semaphore was not created
        return 0;
    }
    return(ucIdx);
}
