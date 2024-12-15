/*
 * USBasp - USB in-circuit programmer for Atmel AVR controllers
 *
 * Thomas Fischl <tfischl@gmx.de>
 * 2020 fixes and tweaks by Ralph Doncaster (Nerd Ralph)
 * 2024 usbman (for ch552)
 * License........: GNU GPL v2 (see Readme.txt)
 * Target.........: CH552 @12Mhz
 * Creation Date..: 2005-02-20
 * Last change....: 2020-12-13
 *
 */

#include "..\inc\global.h"
#include "..\inc\usbasp.h"
#include "..\inc\clock.h"

extern uint8_t  UsbConfig;
extern void usbInit(void); 

void main(void) 
{
    clockInit();
    clockWait(10 / 0.320);              // 10ms

 	  SPI_TO_INPUT();
    RST_TO_INPUT();
	
	  usbInit();
	  IE_USB = 1; 
	  EA     = 1;
	  while(UsbConfig ==0) //wait til configured
		{	
			;
		}	

	  LED_GREEN = ON;
    for (;;) 
	  {
       //usbPoll();
    }
}

