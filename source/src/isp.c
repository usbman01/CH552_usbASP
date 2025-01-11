/*
 * isp.c - part of USBasp
 *
 * Autor..........: Thomas Fischl <tfischl@gmx.de>
 *                  Ralph Doncaster at gmail dot com
 *                  usbman (for ch552)
 * Description....: Provides functions for communication/programming
 *                  over ISP interface
 * Licence........: GNU GPL v2 (see Readme.txt)
 * Creation Date..: 2005-02-23
 * Last change....: 2024-12-13
 */

#include "..\inc\global.h"
#include "..\inc\isp.h"
#include "..\inc\clock.h"
#include "..\inc\usbasp.h"

uint8_t sck_sw_delay;
uint8_t isp_hiaddr;
uint8_t bHwSpi;

static void spiHWenable(void) 
{
    /* enable SPI, master */
   SPI0_CTRL = bS0_MOSI_OE | bS0_SCK_OE;
}

static void spiHWdisable(void) 
{
   SPI0_CTRL = 0;
}

void ispSetSCKOption(uint8_t option) 
{

    if (option == USBASP_ISP_SCK_AUTO)
        option = USBASP_ISP_SCK_1500;

    if (option >= USBASP_ISP_SCK_93_75) 
    {
        bHwSpi = 1;
        sck_sw_delay = 1;    /* force RST#/SCK pulse for 320us */

        switch (option) 
        {

           case USBASP_ISP_SCK_3000:
                /* 3MHz, XTAL/4 */
                SPI0_CK_SE = 4; //SPCR = 0;
                break;
           case USBASP_ISP_SCK_1500:
           default:
                /* 1.5MHz, XTAL/8 */
                //SPSR = (1 << SPI2X);
                // fall through
                SPI0_CK_SE= 8;
                break;   
           case USBASP_ISP_SCK_750:
                /* 750kHz, XTAL/16 */
                //SPCR = (1 << SPR0);
                SPI0_CK_SE = 16; 
                break;
           case USBASP_ISP_SCK_375:
                /* 375kHz, XTAL/32 (default) */
                //SPSR = (1 << SPI2X);
                // fall through
                SPI0_CK_SE = 32;
                break;
           case USBASP_ISP_SCK_187_5:
                /* 187.5kHz XTAL/64 */
                //SPCR = (1 << SPR1);
                SPI0_CK_SE = 64;
                break;
           case USBASP_ISP_SCK_93_75:
                /* 93.75kHz XTAL/128 */
                //SPCR = (1 << SPR1) | (1 << SPR0);
                SPI0_CK_SE = 128;
                break;
        }
    } 
    else 
    {
        //ispTransmit = ispTransmit_sw;
        bHwSpi = 0;
#if 0
        switch (option) {

        case USBASP_ISP_SCK_32:
            sck_sw_delay = 3; break;
        case USBASP_ISP_SCK_16:
            sck_sw_delay = 6; break;
        case USBASP_ISP_SCK_8:
            sck_sw_delay = 12; break;
        case USBASP_ISP_SCK_4:
            sck_sw_delay = 24; break;
        case USBASP_ISP_SCK_2:
            sck_sw_delay = 48; break;
        case USBASP_ISP_SCK_1:
            sck_sw_delay = 96; break;
        case USBASP_ISP_SCK_0_5:
            sck_sw_delay = 192; break;
        }
#endif
        /* more efficient than switch */
        sck_sw_delay = 3 << (USBASP_ISP_SCK_32 - option);
    }
}

void ispDelay(void) 
{
    delay_us(sck_sw_delay);
}

void ispConnect(void) 
{

    /* all ISP pins were inputs before, now set output pins*/
    SPI_TO_OUTPUT(); // push/pull for SPI
    RST_TO_OUTPUT(); // andRST 

    /* positive pulse on RST for at least 2 target clock cycles */
    ISP_RST = 1;   //ISP_OUT |= (1 << ISP_RST);
    clockWait(1);  /* 320us */
    ISP_RST = 0;   //ISP_OUT &= ~(1 << ISP_RST);

    /* Initial extended address value */
    isp_hiaddr = 0xff;  /* ensure that even 0x00000 causes a write of the extended address byte */
}

void ispDisconnect(void) 
{
    /* set all ISP pins inputs */
    SPI_TO_INPUT();
    RST_TO_INPUT(); 

    /* disable hardware SPI */
    spiHWdisable();
    prog_sck = USBASP_ISP_SCK_AUTO;
}

// todo: make ispTransmit function that checks mode and branches
uint8_t ispTransmit_sw(uint8_t send_byte) 
{
    uint8_t rec_byte = 0;
    uint8_t i;

   for (i = 0; i < 8; i++) 
   {
        /* set MSB to MOSI-pin */
        if ((send_byte & 0x80) != 0) ISP_MOSI = 1; //ISP_OUT |= (1 << ISP_MOSI); /* MOSI high */
        else                         ISP_MOSI = 0; //ISP_OUT &= ~(1 << ISP_MOSI); /* MOSI low */
        
        /* shift to next bit */
        send_byte = send_byte << 1;

        /* receive data */
        rec_byte = rec_byte << 1;

        if (ISP_MISO) rec_byte++;
        
        /* pulse SCK */
        ISP_SCK = 1;
        ispDelay();
        ISP_SCK = 0;
        ispDelay();
    }

    return rec_byte;
}

uint8_t ispTransmit_hw(uint8_t send_byte) 
{
    SPI0_DATA = send_byte;
    while (S0_FREE == 0);
    return SPI0_DATA;
}

uint8_t ispEnterProgrammingMode(void) 
{
    uint8_t check;
    uint8_t tries;

    if (prog_sck == USBASP_ISP_SCK_AUTO)
        prog_sck = USBASP_ISP_SCK_1500;

    while (prog_sck >= USBASP_ISP_SCK_0_5) 
    {
        //uint8_t (*spiTx)(uint8_t) = ispTransmit;

        if (bHwSpi) spiHWenable();

        tries = 3;
        do 
        {
            /* pulse RST */
            ISP_RST = 1;   /* RST high */
            clockWait(1);  /* 320us */
            ISP_RST = 0;   /* RST low */

            /* datasheet says wait 20ms, even though less seems fine */
            clockWait(2000 / 32);          /* wait before PE */
            ispTransmit (0xAC);
            ispTransmit (0x53);
            check = ispTransmit(0);
            ispTransmit (0);

               if (check == 0x53) 
            {
#               if TURBO_MODE
                /* bump up speed now that programming mode is enabled */
                /* http://nerdralph.blogspot.com/2020/09/recording-reset-pin.html */
                spiHWdisable();
                ispSetSCKOption(prog_sck + 1);
                if (bHwSpi) spiHWenable();
#               endif
                return 0;
            }
        } while (--tries);

        spiHWdisable();

        ispSetSCKOption(--prog_sck);    /* try lower speed */
    }

    return 1; /* error: device dosn't answer */
}

static void ispUpdateExtended(uint32_t address)
{
    uint8_t curr_hiaddr;

    curr_hiaddr = (address >> 17);

    /* check if extended address byte is changed */
    if(isp_hiaddr != curr_hiaddr)
    {
        isp_hiaddr = curr_hiaddr;
        /* Load Extended Address byte */
        ispTransmit(0x4D);
        ispTransmit(0x00);
        ispTransmit(isp_hiaddr);
        ispTransmit(0x00);
    }
}

uint8_t ispReadFlash(uint32_t address) 
{
    ispUpdateExtended(address);

    ispTransmit(0x20 | ((address & 1) << 3));
    ispTransmit(address >> 9);
    ispTransmit(address >> 1);
    return ispTransmit(0);
}

uint8_t ispWriteFlash(uint32_t address, uint8_t value, uint8_t pollmode) 
{
    uint8_t retries;
    
    ispUpdateExtended(address);

    ispTransmit(0x40 | ((address & 1) << 3));
    ispTransmit(address >> 9);
    ispTransmit(address >> 1);
    ispTransmit(value);

    if (pollmode == 0)
        return 0;

    if (value == 0x7F) 
    {
        clockWait(15); /* wait 4,8 ms */
        return 0;
    } 
    else 
    {
        /* polling flash */
        retries = 30;
        while (retries != 0) 
        {
            if (ispReadFlash(address) != 0x7F) 
            {
                return 0;
            }
            clockWait(1);
            retries--;
        }
        return 1; /* error */
    }
}

uint8_t ispFlushPage(uint32_t address, uint8_t pollvalue) 
{
    uint8_t retries;

    ispUpdateExtended(address);
    
    ispTransmit(0x4C);                  // write page
    ispTransmit(address >> 9);
    ispTransmit(address >> 1);
    ispTransmit(0);

    if (pollvalue == 0xFF) 
    {
        clockWait(15);
        return 0;
    } 
    else 
    {   /* polling flash */
        retries = 30;
        while (retries != 0) 
        {
            if (ispReadFlash(address) != 0xFF) 
            {
                return 0;
            }
            clockWait(1);
            retries--;
        }
        return 1; /* error */
    }
}

uint8_t ispReadEEPROM(uint16_t address) 
{
    ispTransmit(0xA0);
    ispTransmit(address >> 8);
    ispTransmit(address);
    return ispTransmit(0);
}

uint8_t ispWriteEEPROM(uint16_t address, uint8_t value) 
{
    ispTransmit(0xC0);
    ispTransmit(address >> 8);
    ispTransmit(address);
    ispTransmit(value);

    clockWait(30); // wait 9,6 ms
    return 0;
}

uint8_t ispTransmit(uint8_t send_byte) 
{
   if(bHwSpi) return ispTransmit_hw(send_byte);
   else       return ispTransmit_sw(send_byte); 
}