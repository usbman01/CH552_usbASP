/*
 * isp.h - part of USBasp
 *
 * Autor..........: Thomas Fischl <tfischl@gmx.de>
 *                  usbman (for ch552)
 * Description....: Provides functions for communication/programming
 *                  over ISP interface
 * Licence........: GNU GPL v2 (see Readme.txt)
 * Creation Date..: 2005-02-23
 * Last change....: 2024-11-01
 */

#ifndef __isp_h_included__
#define	__isp_h_included__

#define ISP_RST   P3_2
#define ISP_MOSI  P1_5
#define ISP_MISO  P1_6
#define ISP_SCK   P1_7

#define TURBO_MODE 0
/* defined in main.c */
extern uint8_t prog_sck;

/* Prepare connection to target device */
void ispConnect(void);

/* Close connection to target device */
void ispDisconnect(void);

/* read an write a byte from isp using software (slow) */
uint8_t ispTransmit_sw(uint8_t send_byte);

/* read an write a byte from isp using hardware (fast) */
uint8_t ispTransmit_hw(uint8_t send_byte);

/* enter programming mode */
uint8_t ispEnterProgrammingMode(void);

/* read byte from eeprom at given address */
uint8_t ispReadEEPROM(uint16_t address);

/* write byte to flash at given address */
uint8_t ispWriteFlash(uint32_t address, uint8_t value, uint8_t pollmode);

uint8_t ispFlushPage(uint32_t address, uint8_t pollvalue);

/* read byte from flash at given address */
uint8_t ispReadFlash(uint32_t address);

/* write byte to eeprom at given address */
uint8_t ispWriteEEPROM(uint16_t address, uint8_t value);

/* pointer to sw or hw transmit function */
// uint8_t (*ispTransmit)(uint8_t); remove function ptr
uint8_t ispTransmit (uint8_t send_byte);
/* set SCK speed. call before ispConnect! */
void ispSetSCKOption(uint8_t sckoption);

/* load extended address byte */
void ispLoadExtendedAddressByte(uint32_t address);

#endif /* __isp_h_included__ */
