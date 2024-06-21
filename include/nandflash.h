/*
 * nandflash.h
 *
 *  Created on: Dec 19, 2019
 *      Author: Adam
 */

#ifndef INCLUDE_NANDFLASH_H_
#define INCLUDE_NANDFLASH_H_

#include "stdbool.h"
#include "stdio.h"
#include "string.h"

#define TOTAL_BLOCKS            (2048-1)
#define PAGES_PER_BLOCK         128

#define FAULT_BUFFER_COUNT      32
#define TOTAL_BYTES_PER_PAGE    4320
#define FIFO_DEPTH_WORDS        4096
#define DATA_BYTES_PER_PAGE     4096
#define RESERVED_DATA           64
#define ALLOWED_STORAGE_BYTES   (DATA_BYTES_PER_PAGE - RESERVED_DATA)
#define NUM_RESERVED_BLOCKS     16
#define TOTAL_WORDS_PER_PAGE    (TOTAL_BYTES_PER_PAGE/2)
#define DATA_WORDS_PER_PAGE     (DATA_BYTES_PER_PAGE/2)
#define ALLOWED_STORAGE_WORDS   (ALLOWED_STORAGE_BYTES/2)
#define WORDS_PER_RECORD        21
#define RECORDS_PER_PAGE        (ALLOWED_STORAGE_WORDS/WORDS_PER_RECORD)
#define RECORD_DELAY            10 //10ms delay between records

#define PAGE_BLANK  0
#define BAD_CRC   1
#define GOOD_CRC  2

#define MAX_READ_TIME     32000
#define MAX_PROGRAM_TIME  32000
#define MAX_ERASE_TIME    32000

#define LOOP_DELAY 0.1

#define  EMIF_NAND_ADDR                 0x60000000

#define EMIF_NAND_SHIFT 0x1 //May need to shift a bit to the left for the address

#define FLEX_NAND_COMMAND_OFFSET        (0x00 << EMIF_NAND_SHIFT)
#define FLEX_NAND_READSIZE_OFFSET       (0x02 << EMIF_NAND_SHIFT)
#define FLEX_NAND_READCOL_OFFSET        (0x04 << EMIF_NAND_SHIFT)
#define FLEX_NAND_READPAGE_OFFSET       (0x06 << EMIF_NAND_SHIFT)
#define FLEX_NAND_READBLOCK_OFFSET      (0x08 << EMIF_NAND_SHIFT)
#define FLEX_NAND_WRITESIZE_OFFSET      (0x0A << EMIF_NAND_SHIFT)
#define FLEX_NAND_WRITECOL_OFFSET       (0x0C << EMIF_NAND_SHIFT)
#define FLEX_NAND_WRITEPAGE_OFFSET      (0x0E << EMIF_NAND_SHIFT)
#define FLEX_NAND_WRITEBLOCK_OFFSET     (0x10 << EMIF_NAND_SHIFT)
#define FLEX_NAND_ERASEBLOCK_OFFSET     (0x10 << EMIF_NAND_SHIFT)
#define FLEX_NAND_WRITEFIFO_OFFSET      (0x12 << EMIF_NAND_SHIFT)
#define FLEX_NAND_READFIFO_OFFSET       (0x14 << EMIF_NAND_SHIFT)
#define FLEX_NAND_FLASHSTATUS_OFFSET    (0x16 << EMIF_NAND_SHIFT)
#define FLEX_NAND_FIFOSTATUS_OFFSET     (0x18 << EMIF_NAND_SHIFT)
#define FLEX_NAND_SIGHIGH_OFFSET        (0x1A << EMIF_NAND_SHIFT)
#define FLEX_NAND_SIGLOW_OFFSET         (0x1C << EMIF_NAND_SHIFT)
#define FLEX_NAND_DEBUG_OFFSET          (0x1E << EMIF_NAND_SHIFT)

/*
#define NEGATE_FLASH_CS     0x0020 << EMIF_NAND_SHIFT
#define ASSERT_FLASH_CS     0x0000 << EMIF_NAND_SHIFT
#define NAND_READ           0x0001 << EMIF_NAND_SHIFT
#define NAND_WRITE          0x0002 << EMIF_NAND_SHIFT
#define NAND_ERASE          0x0003 << EMIF_NAND_SHIFT
#define NAND_STATUS         0x0004 << EMIF_NAND_SHIFT
#define NAND_RESET          0x0005 << EMIF_NAND_SHIFT
#define NAND_SIGNATURE      0x0006 << EMIF_NAND_SHIFT
*/

#define NEGATE_FLASH_CS     0x0020
#define ASSERT_FLASH_CS     0x0000
#define NAND_READ           0x0001
#define NAND_WRITE          0x0002
#define NAND_ERASE          0x0003
#define NAND_STATUS         0x0004
#define NAND_RESET          0x0005
#define NAND_SIGNATURE      0x0006

#define POLY 0x8408

bool CheckCrc(unsigned short* page_data);
unsigned short Crc16(char *data_p, unsigned short length);
unsigned short WritePage(unsigned short *data, unsigned short page_address, unsigned short block_address);
unsigned short *NandPageRead(unsigned short block_address, unsigned short page_address, unsigned short* read_array);
//unsigned short* NandPageReadDebug(unsigned short block_address, unsigned short page_address, unsigned int sock, sockaddr_in remote);    //Used to print info to the desktop application

int ValidateLastWrite(unsigned short block_address, unsigned short page_address);
bool CompareToWrittenData(unsigned short block_address, unsigned short page_address, char *write_data);
bool ValidateErase(unsigned short block_address, unsigned short* read_result);
unsigned short FifoStatus(void);
unsigned short FlashStatus(void);
void FlashReset(void);
void EraseBlock(unsigned short block_address);
void SendSignatureCommand(void);
void SendReadStatusCommand(void);
void FindInitialBadBlocks(unsigned short block_no_bak);
void LoadBadBlockTable(void);
void UpdateBadBlockTable();
void UpdateBlockNoBackup(unsigned short block_no);
void ClearBadBlockTable(void);

void RestartNandStorage(void);

bool IsBlockBad(int block_index);
void AddBadBlockToTable(int block_index, unsigned short block_no_bak);
bool IsPageBlank(unsigned short *page_data);
int IncrementBlockPointer(bool);
void IncrementPagePointer(bool);
void LocateInitialPointers(void);
void FlashReset(void);
unsigned short Crc16(char *data_p, unsigned short length);
unsigned short ReadNandHigh(void);
unsigned short ReadNandLow(void);
unsigned short ReadNandDebug(void);

#endif /* INCLUDE_NANDFLASH_H_ */
