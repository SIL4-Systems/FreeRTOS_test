/*
 * nandflash.c
 *
 *  Created on: Dec 19, 2019
 *      Author: Adam
 */

#include "nandflash.h"
/*
#include "stddef.h"
#include "FreeRTOS.h"
#include "os_semphr.h"
#include "reg_sci.h"

char testDataStr[64];
xSemaphoreHandle xMutex = NULL;
SemaphoreHandle_t xRecursiveMutex;
*/
unsigned short FlashStatus(void){
    return *(unsigned short*)(EMIF_NAND_ADDR+FLEX_NAND_FLASHSTATUS_OFFSET);
}

unsigned short FifoStatus(void){
    return *(unsigned short*)(EMIF_NAND_ADDR+FLEX_NAND_FIFOSTATUS_OFFSET);
}

void FlashReset(void){
    unsigned int address = EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET;
    *(unsigned short*)(address) = NAND_RESET;
}

unsigned short ReadNandHigh(void){
    return *(unsigned short*)(EMIF_NAND_ADDR+FLEX_NAND_SIGHIGH_OFFSET);
}

unsigned short ReadNandLow(void){
    return *(unsigned short*)(EMIF_NAND_ADDR+FLEX_NAND_SIGLOW_OFFSET);
}

unsigned short ReadNandDebug(void){
    return *(unsigned short*)(EMIF_NAND_ADDR+FLEX_NAND_DEBUG_OFFSET);
}

unsigned short WritePage(unsigned short* data, unsigned short page_address, unsigned short block_address){
    int x;
    bool read_error;
    unsigned short write_crc;
    unsigned short status;

    write_crc = Crc16((char*)data, ALLOWED_STORAGE_BYTES-1);
    data[DATA_WORDS_PER_PAGE-2] = write_crc;

    *(unsigned short*)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = ASSERT_FLASH_CS;
    //fill the write fifo in the FPGA
    for(x=0;x<TOTAL_WORDS_PER_PAGE;x++)
    {
        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_WRITEFIFO_OFFSET) = data[x]; //write fifo
    }

    //flash write
    *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_WRITECOL_OFFSET) = 0;  //write column address (12 bits)
    *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_WRITEPAGE_OFFSET) = (page_address & 127);  //write page address (6 bits)
    *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_WRITEBLOCK_OFFSET) = (block_address & 4095);  //write block address (13 bits)
    *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_WRITESIZE_OFFSET) = TOTAL_BYTES_PER_PAGE;//((size*(unsigned short)2)&8191);  //write page size (#bytes to be written) (13 bits)

    *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = NAND_WRITE;  //send write command

    status = FlashStatus();
    x = 0;
    read_error = false;

    //wait
    while(((status & 0xFF00) != 0xE000) && !read_error){
        status = FlashStatus();
        if(x>MAX_PROGRAM_TIME)
        {
            read_error = true;
        }
        x++;
    }
    *(unsigned short*)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = NEGATE_FLASH_CS;

    return write_crc;
}

unsigned short* NandPageRead(unsigned short block_address, unsigned short page_address, unsigned short* read_array)
{
    int x;
    unsigned short crc;
    bool read_error;
    unsigned short dummy_read;
    unsigned short status;
    bool still_offset = true;
    unsigned short temp_read = 0;
    int i = 0;


    if(read_array != NULL)
    {

        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = ASSERT_FLASH_CS;//assert flash cs

        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READCOL_OFFSET) = 0;  //read column address (12 bits)
        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READPAGE_OFFSET) = (page_address & 127);  //read page address (6 bits)
        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READBLOCK_OFFSET) = (block_address & 4095);  //read block address (13 bits)
        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READSIZE_OFFSET) = TOTAL_BYTES_PER_PAGE;//(((size+1)*(unsigned short)2) & 8191);  //read page size (#bytes to be read) (13 bits)

        //flush the read fifo
        for(x=0;x<FIFO_DEPTH_WORDS;x++) //FIFO depth is 4096
        {
            dummy_read = *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READFIFO_OFFSET);
        }

        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = NAND_READ;  //send read command


        status = 0;
        x=0;
        read_error = false;

        while(((status & 0xFF00) != 0xE000)  && !read_error)
        {
            status = FlashStatus();
            //wait
            if(x>20000)
            {
                read_error = true;
            }
            x++;
        }

        if(read_error == true)
        {
            //No action
        }

        do
        {
            read_array[0] = *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READFIFO_OFFSET);
            i++;
        }
        while (read_array[0] == 65535 && i < 300); //Restrict this to 150 cycles. Otherwise, will loop forever and trigger the watchdog.

        if(read_array != NULL)
        {

            for(x=1;x<TOTAL_WORDS_PER_PAGE;x++)
            {
                read_array[x] = *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READFIFO_OFFSET);
            }

            *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = NEGATE_FLASH_CS;//negate flash cs

            crc = Crc16((char*)read_array, ALLOWED_STORAGE_BYTES-1);
            read_array[DATA_WORDS_PER_PAGE+1] = crc;

        }
    }
    else
    {
        printf("Could not allocate memory.\n");
    }

    return 0;
}

bool IsPageBlank(unsigned short *page_data)
{
    int x;
    int counter;

    counter = 0;
    for(x=1;x<DATA_WORDS_PER_PAGE-1;x++)
    {
        if(page_data[x] == 0xFFFF)
        {
            counter++; //counter number of blank words
        }
    }
    if(counter>(DATA_WORDS_PER_PAGE-5))
    {
        return true; //page blank
    }
    else
    {
        return false; //page not blank
    }
}

void EraseBlock(unsigned short block_address)
{
    bool read_error;
    int x;
    unsigned short status;

    *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = ASSERT_FLASH_CS;//assert flash cs

    *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_ERASEBLOCK_OFFSET) = (block_address & 8191);  //write (also erase) block address (13 bits)
    *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = NAND_ERASE;  //send erase command

    status = FlashStatus();
    x=0;
    read_error = false;
    while(((status & 0xFF00) != 0xE000)  && !read_error)
    {
        status = FlashStatus();
        //wait
        if(x>MAX_ERASE_TIME)
        {
            read_error = true;
        }
        x++;
    }

    *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = NEGATE_FLASH_CS;//negate flash cs
}

bool ValidateErase(unsigned short block_address, unsigned short* read_result)
{
    //unsigned short *read_result;
    int x;
    bool read_error;
    unsigned short dummy_read;
    bool good_erase = false;
    unsigned short status;

    //read_result = (unsigned short*)pvPortMalloc(TOTAL_BYTES_PER_PAGE); //dies here.
    //read_result = (unsigned short*)pvPortMalloc(16); //dies here.

    //flush the read fifo0
    if(read_result != 0){
        for(x=0;x<FIFO_DEPTH_WORDS;x++) //FIFO depth is 4096
        {
            dummy_read = *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READFIFO_OFFSET);
        }

        *(unsigned short *)EMIF_NAND_ADDR = 0;//assert flash cs

        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READCOL_OFFSET) = 0;  //read column address (12 bits)
        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READPAGE_OFFSET) = 0;  //read page address (6 bits)
        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READBLOCK_OFFSET) = (block_address & 4095);  //read block address (13 bits)
        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READSIZE_OFFSET) = TOTAL_BYTES_PER_PAGE;//(((size+1)*(unsigned short)2) & 8191);  //read page size (#bytes to be read) (13 bits)

        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = NAND_READ;  //send read command

        status = FlashStatus();
        x=0;
        read_error = false;
        while(((status & 0xFF00) != 0xE000)  && !read_error)
        {
            status = FlashStatus();
            //wait
            if(x>MAX_READ_TIME)
            {
                read_error = true;
            }
            x++;
        }

        read_result[0] = *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READFIFO_OFFSET);

        for(x=0;x<TOTAL_WORDS_PER_PAGE;x++)
        {
            read_result[x] = *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_READFIFO_OFFSET);
        }
        *(unsigned short *)(EMIF_NAND_ADDR+FLEX_NAND_COMMAND_OFFSET) = NEGATE_FLASH_CS;//negate flash cs

        if(IsPageBlank(read_result))
        {
            //printf("Erase of block %d passed.\n", block_address);
            good_erase=true;
        }
        else
        {
            //printf("Erase of block %d FAILED.\n", block_address);
            good_erase=false;
        }

        //vPortFree(read_result);
    }

    return good_erase;
}

bool CheckCrc(unsigned short *page_data)
{
    unsigned short crc;
    bool good_crc;

    crc = Crc16((char*)page_data, ALLOWED_STORAGE_BYTES-1);

    if(crc!=page_data[DATA_WORDS_PER_PAGE-2])
    {
        good_crc=false;
    }
    else
    {
        good_crc=true;
    }

    return good_crc;
}

unsigned short Crc16(char *data_p, unsigned short length)
{
    unsigned char i;
    unsigned int data;
    unsigned int crc = 0xffff;

    if (length == 0)
        return (unsigned short)(~crc);

    do
    {
        for (i=0, data=(unsigned int)0xff & *data_p++;
                i < 8;
                i++, data >>= 1)
        {
            if ((crc & 0x0001) ^ (data & 0x0001))
            {
                crc = (crc >> 1) ^ POLY;
            }
            else
            {
                crc >>= 1;
            }
        }
    } while (--length);

    crc = ~crc;
    data = crc;
    crc = (crc << 8) | (data >> 8 & 0xff);

    return (unsigned short)(crc);
}
