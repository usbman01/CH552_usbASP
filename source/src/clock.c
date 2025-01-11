/*
 * clock.c - part of USBasp
 *
 * Autor..........: Thomas Fischl <tfischl@gmx.de>
 *                  usbman (for ch552)
 * Description....: Provides functions for timing/waiting
 * Licence........: GNU GPL v2 (see Readme.txt)
 * Creation Date..: 2005-02-23
 * Last change....: 2024-12-13
 */

#include "..\inc\global.h"
#include "..\inc\clock.h"

//12MHz
void delay_us(uint16_t us)
{
   while (us) 
   {
      NOP();
      NOP();
      -- us;
   }
}
/* setup clock to 12MHz */
void clockInit(void)
{
   SAFE_MOD  = 0x55;
   SAFE_MOD  = 0xAA;	 
   CLOCK_CFG = CLOCK_CFG & ~MASK_SYS_CK_SEL | 0x04; //12 MHz;
   delay_us (100);
}

/* wait time * 320 us */
void clockWait(uint8_t time) 
{
  do 
  {
     delay_us (CLOCK_T_320us);
  } while (--time);
}
