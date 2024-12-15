/*
 * clock.h - part of USBasp
 *
 * Autor..........: Thomas Fischl <tfischl@gmx.de>
 *                  usbman (for ch552)
 * Description....: Provides functions for timing/waiting
 * Licence........: GNU GPL v2 (see Readme.txt)
 * Creation Date..: 2005-02-23
 * Last change....: 2024-11-01
 */

#ifndef __clock_h_included__
#define	__clock_h_included__

#define F_CPU           FREQ_SYS   /* 12MHz */
//#define TIMERVALUE      TCNT0
#define CLOCK_T_320us	  316        /* 320us */

/* setup clock to 12MHz */
void clockInit(void);

/* wait time * 320 us */
void clockWait (uint8_t time);
void delay_us (uint16_t delay); 
#endif /* __clock_h_included__ */
