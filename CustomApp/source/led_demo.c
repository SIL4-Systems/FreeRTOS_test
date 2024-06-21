/** @file led_demo.c
*   @brief led demo implementation File
*   @date 11.August.2009
*   @version 1.00.000
*
*   (c) Texas Instruments 2009, All rights reserved.
*
*   RPL - I hacked this file up to suit my "demo" needs.
*/

#include "het.h"
#include "gio.h"
#include "sci.h"
#include "adc.h"
#include "led_demo.h"
#include "FreeRTOS.h"
#include "os_task.h"

uint32_t map_led_pins[12] = {0x02000000,0x00040000,0x20000000,0x00000001,0x80000000,
                             0x00020000,0x00000010,0x08000000,0x00010000,0x00000004,
                             0x00000020,0x00100000};
uint32_t map_led_pins_reve[12] = {0x02000000,0x00040000,0x20000000,0x00000001,0x80000000,
                             0x00020000,0x08000000,0x00000020,0x00010000,0x00000004,
                             0x00000020,0x00100000};
uint32_t map_leds_pwms[6] = {3,4,5,0,1,2};
uint32_t map_leds_pwms_reve[8] = {3,4,5,0,1,2,6,7};


/** @fn void Running_LED_demo(void)
*
*   This function is used for LED demo based on the SCI
*   command different LED's are turned on and a running
*   LED demo is included
*/
void Running_LED_demo(void)
{
    unsigned int pwm;           /** - Variable to count the width */
    unsigned int count,i;       /** - Vaiable to toggle the pulse width*/
    unsigned int insane = 0;
    unsigned int repeat;

    /** - NHET is configured as Master
     *  - NHET is turned On */
    hetREG1->GCR = 0x01000001;

    /** - PULL functinality is enabled */
    hetREG1->PULDIS = 0x00000000;

    /** - Configure NHET[0,2,4,5,25,16,17,18,20,27,29,31] pins as output
     *  - These pins are connected to the LED's */
    hetREG1->DIR = 0xAA178035;

    /* Task delay ticks - assumes 1 millisecond per tick */
    TickType_t ticks = 50;

    pwm = 0;
    count = 0;
    if(0 == SubTask_Number)
    {
        repeat = 5;
    }
    else
    {
        repeat = 50;
    }
    while(repeat > insane++)
    {
        switch(SubTask_Number)
        {
            case 0:
                /** - Do the LED Demo Toggle, until another task selected
                 *  - The below loop generates a varying pulse width at
                 *    HET pins[0,2,4,5,15,16,17,18,20,27,29,31]
                 *    which helps in blinking LED demo */

                /** - Set NHET Pins 29,31,0,17,18,25
                 *  - Set NHET Pins  20,5,2,16,27,4
                 *  - This turns the white LEDs on and the RGB LEDs off*/
                    hetREG1->DOUT = 0xAA170035;

                    vTaskDelay(ticks);

                    if((pwm == 0x800))
                    {
                        vTaskDelay(ticks);
                    }

                    /** - Clr all NHETs (turns on RGB LEDs) */
                    hetREG1->DOUT = 0x00000000;

                    vTaskDelay(ticks);

                    if((pwm == 0x00))
                    {
                        vTaskDelay(ticks);
                    }

                    if(pwm == 0x800)
                    {
                        count = 1;

                    }
                    if(pwm == 0)
                    {
                        count = 0;
                    }

                    if(count != 0)
                    {
                        pwm--;
                    }
                    else
                    {
                        pwm++;
                    }

                    /** - Clear HET pins end of the demo */
                    hetREG1->DOUT = 0; //0x08110034;
                    break;
            case 1:
                    /** - Take all LEDs low */
                    hetREG1->DOUT = 0x08110034; //0x0
                    break;
            case 2:
                    /** - Set NHET Pin 25 */
                    hetREG1->DSET       = 0x02000000;
                    break;
            case 3:
                    /** - Clear NHET Pin 25 */
                    hetREG1->DCLR       = 0x02000000;
                    break;
            case 4:
                    /** - Set NHET Pin 18 */
                    hetREG1->DSET       = 0x00040000;
                    break;
            case 5:
                    /** - Clear NHET Pin 18 */
                    hetREG1->DCLR       = 0x00040000;
                    break;
            case 6:
                    /** - Set NHET Pin 29 */
                    hetREG1->DSET       = 0x20000000;
                    break;
            case 7:
                    /** - Clear NHET Pin 29 */
                    hetREG1->DCLR       = 0x20000000;
                    break;
            case 8:
                    /** - Set NHET Pin 0 */
                    hetREG1->DSET       = 0x00000001;
                    break;
            case 9:
                    /** - Clear NHET Pin 0 */
                    hetREG1->DCLR       = 0x00000001;
                    break;
            case 10:
                    /** - Set NHET Pin 31 */
                    hetREG1->DSET       = 0x80000000;
                    break;
            case 11:
                    /** - Clear NHET Pin 31 */
                    hetREG1->DCLR       = 0x80000000;
                    break;
            case 12:
                    /** - Set NHET Pin 17 */
                    hetREG1->DSET       = 0x00020000;
                    break;
            case 13:
                    /** - Clear NHET Pin 17 */
                    hetREG1->DCLR       = 0x00020000;
                    break;
            case 14:
                    /** - Set NHET Pin 2 */
                    hetREG1->DCLR       = 0x00000004;
                    break;
            case 15:
                    /** - Clear NHET Pin 2 */
                    hetREG1->DSET       = 0x00000004;
                    break;
            case 16:
                    /** - Set NHET Pin 20 */
                    hetREG1->DCLR       = 0x00100000;
                    break;
            case 17:
                    /** - Clear NHET Pin 20 */
                    hetREG1->DSET       = 0x00100000;
                    break;
            case 18:
                    /** - Set NHET Pin 5 */
                    hetREG1->DCLR       = 0x00000020;
                    break;
            case 19:
                    /** - Clear NHET Pin 5 */
                    hetREG1->DSET       = 0x00000020;
                    break;
            case 20:
                    /** - Set NHET Pin 4 */
                    hetREG1->DCLR       = 0x00000010;
                    break;
            case 21:
                    /** - Clear NHET Pin 4 */
                    hetREG1->DSET       = 0x00000010;
                    break;
            case 22:
                    /** - Set NHET Pin 16 */
                    hetREG1->DCLR       = 0x00010000;
                    break;
            case 23:
                    /** - Clear NHET Pin 16 */
                    hetREG1->DSET       = 0x00010000;
                    break;
            case 24:
                    /** - Set NHET Pin 27 */
                    hetREG1->DCLR       = 0x08000000;
                    break;
            case 25:
                    /** - Clear NHET Pin 27 */
                    hetREG1->DSET       = 0x08000000;
                    break;
            case 26:
                    /** - Set NHET Pin 27 */
                    hetREG1->DSET       = 0x08000000;
                    break;
            case 27:
                    /** - Clear NHET Pin 27 */
                    hetREG1->DCLR       = 0x08000000;
                    break;
            case 28:
                    /** - Set NHET Pin 5 */
                    hetREG1->DSET       = 0x00000020;
                    break;
            case 29:
                    /** - Clear NHET Pin 5 */
                    hetREG1->DCLR   = 0x00000020;
                    break;

            case 30:
                    hetREG1->DIR    = 0xAA178035;
                    hetREG1->DOUT       = 0;
                    /* turn off all PWMs */
                    for(i=0; i<8; i++){pwmStop(hetRAM1,i);}
                    /* Reset the task number to reenter main loop */
                    break;
        }
        if(0 != SubTask_Number)
        {
            SubTask_Number += 2;
            if(31 == SubTask_Number)
            {
                SubTask_Number = 2;
            }
            if(32 == SubTask_Number)
            {
                SubTask_Number = 1;
            }

            vTaskDelay(ticks);
        }
    }
    if(0 != SubTask_Number)
    {
        SubTask_Number = 1;
    }
}

/** @fn run_LED_StartUp(void)
*
*   This function is called during the start up, LED's are turned on
*   in a sequence to indicate the board is powerd up and ready to run
*   the demo software.
*
*/
void run_LED_StartUp(void)
{
    int count = 0;

    /* Task delay ticks - assumes 1 millisecond per tick */
    TickType_t ticks = 100;

    /** - NHET is configured as Master
     *  - NHET is turned On */
    hetREG1->GCR        = 0x01000001;

    /** - PULL functinality is enabled */
    hetREG1->PULDIS = 0x00000000;

    while(3 > count++)
    {
        /** - Configure NHET pins as output */
        hetREG1->DIR    = 0xAA178035;
        /** - Turn all LEDs off */
        hetREG1->DOUT       = 0x0;   //0x08110034;

        /** - Set only NHET[25,0] */
        hetREG1->DOUT       = 0 | (1 << 25) | (1 << 0); //0x0A110035;
        /** - Simple Delay */
        vTaskDelay(ticks);

        /** - Set only NHET[18,31] */
        hetREG1->DOUT       = 0 | (1 << 31) | (1 << 18) ; //0x88150034;
        /** - Simple Delay */
        vTaskDelay(ticks);

        /** - Set only NHET[29,17] */
        hetREG1->DOUT       = 0 | (1 << 29) | (1 << 17);   //0x28130034;
        /** - Simple Delay */
        vTaskDelay(ticks);

        /** - Set only NHET[31,18] */
        hetREG1->DOUT       = 0 | (1 << 31) | ( 1 << 18);   //0x88150034;
        /** - Simple Delay */
        vTaskDelay(ticks);

        /** - Set only NHET[0,25] */
        hetREG1->DOUT       = 0 | (1 << 25) | (1 << 0); //0x0A118035;
        /** - Simple Delay */
        vTaskDelay(ticks);

        /** - Set only NHET[0,25,17,18,29,31] */
        hetREG1->DOUT       = 0 | (1<<25) | (1<<17) | (1<<18) | (1<<29) | (1 <<31) | (1 <<0); //0xAA178035;
        /** - Simple Delay */
        vTaskDelay(ticks);

        /** - Clr all NHET[0,25,17,18,29,31] pins */
        hetREG1->DOUT       = 0; //0x08110034;
   }
}
