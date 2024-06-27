#include "helpers.h"


/**
 * @brief Helper used by custom itoa
 * @see `itoa`
 */
void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}


/**
 * @brief Custom itoa function to convert int to a string
 *
 * Utilizes the `reverse` function helper
 * @see `reverse`
 *
 * @param[in] num The integer to be converted
 * @param[in] base The base of the number
 * @param[out] str The destination char pointer
 *
 * @return Pointer to the destination string `str`
 */
char* itoa(int num, char* str, int base) {
    int i = 0;
    int isNegative = 0;

    // Handle 0 explicitly, otherwise empty string is printed for 0
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with base 10.
    // Otherwise, numbers are considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}


/**
 * @brief Custom uint8 to string conversion
 */
void uint8_to_string(uint8_t value, char* buffer, int base)
{
    char *ptr = buffer;
    char *ptr1 = buffer;
    char tmp_char;
    int tmp_value;

    // If base is not valid, set to 10 (decimal)
    if (base < 2 || base > 36)
    {
        base = 10;
    }

    // Conversion
    do
    {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[tmp_value % base];
    } while (value);

    // Null-terminate the string
    *ptr-- = '\0';

    // Reverse the string
    while (ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr = *ptr1;
        *ptr1 = tmp_char;
        ptr--;
        ptr1++;
    }
}


/**
 * @brief Uses custom itoa to print the number over serial interface
 * @see `itoa`
 * @see `UARTwrite`
 *
 * @param[in] num The integer to print
 */
void printNumber(int num) {
    char buffer[20];
    itoa(num, buffer, 10);
    UARTwrite(buffer, strlen(buffer));
}


/**
 * @brief Uses `itoa` to print Free heap size
 * @see `itoa`
 * @see `UARTwrite`
 * @see `xPortGetFreeHeap`
 */
void printFreeHeapSize() {
    UARTwrite("Free Heap: ", strlen("Free Heap: "));
    printNumber(xPortGetFreeHeapSize());
    UARTwrite("\n", 1);
}


/**
 * @brief Print the MAC address of the board over serial
 */
void printMACAddress() {
    uint8_t *ucMACAddress;
    char cBuffer[5];

    // Retrieve the MAC address
    ucMACAddress = FreeRTOS_GetMACAddress();

    // Print MAC address to UART
    UARTwrite("Board MAC Address: ", strlen("Board MAC Address: "));

    // Write MAC address byte by byte
    int i = 0;
    while (i < 6) {
        uint8_to_string(ucMACAddress[i], cBuffer, 16);
        UARTwrite(cBuffer, strlen(cBuffer));

        if (i < 5) UARTwrite(":", 1);

        i++;
    }

    UARTwrite("\n", 1);
}


/**
 * @brief Print the IP Address of the board
 */
void printIPAddress()
{
//    uint32_t ulIPAddress;
//    ulIPAddress = FreeRTOS_GetIPAddress();
//
//    // Convert IP address to string
//    char cBuffer[16];
//    FreeRTOS_inet_ntoa(ulIPAddress, cBuffer);
//
//    // Print IP address to UART
//    UARTwrite("Board IP Address: ", strlen("Board IP Address: "));
//    UARTwrite(cBuffer, strlen(cBuffer));
//    UARTwrite("\n", 1);
}
