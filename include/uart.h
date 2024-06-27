/**
 * @file uart.h
 *
 * @brief Contains constants and function prototypes for uart.c
 */

#include "FreeRTOS.h"
#include "sci.h"
#include "os_semphr.h"

/** @brief A special debug flag that limits the number of serial debug statements */
int special_debug_flag;

/** @brief Semaphore */
SemaphoreHandle_t xRecursiveMutex;

/*
 * @brief Write a message of specified length over UART
 *
 * @param[in] pcBuf Pointer to the character string to be sent over serial
 * @param[in] ui32Len Number of characters to be written
 *
 * @return 0 in case of failure and 1 for success
 */
int UARTwrite(const char *pcBuf, unsigned int ui32Len);
