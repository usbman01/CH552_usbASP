/*
 * globsl.h - part of USBasp
 *
 * Autor..........: usbman (for ch552)
 * Description....: some wrapper macros 
 * Licence........: GNU GPL v2 (see Readme.txt)
 * Creation Date..: 2005-02-23
 * Last change....: 2024-12-13
 */
#ifndef __global_h__
#include "ch554.h"

#ifdef __C51__       // keil has no stdint so we emulate it
  #define uint8_t  unsigned char
  #define uint16_t unsigned int
  #define uint32_t unsigned long
#else
  #include <stdint.h>
#endif

//  config options 

#define  USE_WCID 1
#define  USE_CDC  0
#define  USE_TPI  1

#define  USB_CFG_VENDOR_ID  0x16c0  /* = 5824 = voti.nl */
#define  USB_CFG_DEVICE_ID   1500   
#define  VENDOR_CODE         0x40 
#define  CDC_BUFSIZE        0x100

#define  CDC_BUFMAX         CDC_BUFSIZE-1
#define  EP0_SIZE           8
#define  BULK_SIZE          0x40          
	
// two leds low active
#define LED_RED   P3_3
#define LED_GREEN P3_4
#define OFF   1
#define ON    0

#define PIN_SLOW  P1_1
#define PIN_RST   P3_2

// spi pins
#define PIN_MOSI  P1_5
#define PIN_MISO  P1_6  
#define PIN_SCK   P1_7


#define SPI_TO_INPUT()  P1_MOD_OC &= ~(bSCK | bMOSI); \
                        P1_DIR_PU &= ~(bSCK | bMOSI)

#define SPI_TO_OUTPUT() P1_MOD_OC &= ~(bSCK | bMOSI); \
	                      P1_DIR_PU |=  (bSCK | bMOSI) 

#define RST_TO_INPUT()  P3_MOD_OC &= ~(bTXD1_); \
	                      P3_DIR_PU &= ~(bTXD1_)
	
#define RST_TO_OUTPUT() P3_MOD_OC &= ~(bTXD1_); \
	                      P3_DIR_PU |=  (bTXD1_)

#define FREQ_SYS   12000000L
#define NO_TIMEOUT 0xFF

#define USB_STATE_POWERED   0                  // maintaining the enum state
#define USB_STATE_RESET     1
#define USB_STATE_ADDRESSED 2
#define USB_STATE_CONFIG    3
#define USB_STATE_SUSPEND   4
/*
typedef struct _LINESETTINGS
{
  uint32_t dwDTERate;   // bautrate in lsbfirst
  uint8_t  bCharFormat; // 0=1 Stopbit; 1= 1.5 Stopbits; 2=2 Stopbits
  uint8_t  bParityType; // 0=none; 1=even; 2=odd; 2=mark; 4=space
  uint8_t  bDataBits;   // [5..8];16
}line_t;
*/
typedef union
{
	struct
	{
    uint32_t dwDTERate;   // bautrate in lsbfirst
    uint8_t  bCharFormat; // 0=1 Stopbit; 1= 1.5 Stopbits; 2=2 Stopbits
    uint8_t  bParityType; // 0=none; 1=even; 2=odd; 2=mark; 4=space
    uint8_t  bDataBits;   // [5..8];16
	}s;	
  uint8_t b[7]; 
}line_t;

typedef struct _FIFOCTL
{
	 uint8_t wr;     // wr index
	 uint8_t rd;     // rd index
   uint8_t level;  // fillgrade
}tFIFOCTL;

#ifdef BIG_ENDIAN_
  typedef struct _LongVar
  {
     uint8_t b3;
     uint8_t b2;
     uint8_t b1;
     uint8_t b0; 
  }tLong;
#else
  typedef struct _LongVar
  {
     uint8_t b0;
     uint8_t b1;
     uint8_t b2;
     uint8_t b3; 
  }tLong;
#endif


typedef union 
{
   tLong    b;
   uint32_t l;
}tlunion;


typedef struct _SETUP
{
    uint8_t  bRequestType;
    uint8_t  bRequest;
    uint8_t  wValueLo;
    uint8_t  wValueHi;
    uint8_t  wIndexLo;
    uint8_t  wIndexHi;
    uint16_t wLength;
}tSETUP;


#define __global_h__
#endif 