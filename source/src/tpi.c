/*
 * tpi.c - part of USBasp
 *
 * Autor..........: usbman (for ch552)
 * Description....: Provides functions for timing/waiting
 * Licence........: GNU GPL v2
 * Creation Date..: 2005-02-23
 * Last change....: 2024-12-13
 */
 
#include "..\inc\global.h"

#if (USE_TPI)
#include "..\inc\tpi_defs.h"
#define TPIDATA   PIN_MOSI
#define TPICLK    PIN_SCK


uint16_t tpi_dly_cnt;

/**
  * bit time delay
 */
static void delay(uint16_t wait)
{
	 while (wait--) {;}
}

/**
 * exchange one bit
 */
static uint8_t tpi_bit (uint8_t t)
{
	 TPIDATA = t; 
	 delay(tpi_dly_cnt);
	 TPICLK = 1; 
	 t = TPIDATA; 
	 delay(tpi_dly_cnt); 
	 TPICLK = 0; 
	 return t;
}

/**
 * TPI init
 */
void tpi_init(void)
{
   uint8_t i =32;

   P1_MOD_OC &= ~bSCK;         // clk to push/pull
 //P1_DIR_PU |=  bSCK;
   P1_DIR_PU |=  bMOSI | bSCK; // data to BiDir mode
   P1_MOD_OC |=  bMOSI;  
 //P1_DIR_PU |=  bMOSI;
   while (i--) tpi_bit(1);
}

/**
 * Send one byte
 */
void tpi_send_byte(uint8_t send)
{
	 uint8_t i;
	 uint8_t parity = 0;
	 
   tpi_bit(0);
   for (i=0;i<8;i++)
   {
   	  parity ^= send;
   	  tpi_bit (send & 0x01);
   	  send  >>= 1;   	  
   }
   tpi_bit (parity & 0x01);
   tpi_bit(1);
   tpi_bit(1);
}

/**
 * Receive one byte
 */
uint8_t tpi_recv_byte(void)
{
	 uint8_t dIn;
	 uint8_t parity = 0;
	 uint8_t i      = 192;
	 
	 while (i--) 
	 {
	 	 if(tpi_bit(1)==0) break;
	 }
	 
	 if(i == 0) //no startbit detected
	 {  //send 2 breaks;
	 	  do { tpi_bit(0); i ++; } while(i < 26);
      tpi_bit(1); 
	 	  return 0;
	 }
	 dIn = 0;
	 for (i=0;i<8;i++)
	 {
	 	  dIn   >>= 1;	
	 	  if (tpi_bit(1)) dIn |= 0x80;
	 	  parity ^= dIn & 0x80;
	 }
	 //check parity;
	 if (tpi_bit(1)) parity ^= 0x80;
	 if(parity)
	 {	//send 2 breaks;
	 	  i = 0;
	 	  do { tpi_bit(0); i ++; } while(i < 26);
      tpi_bit(1); 
	 	  return 0;
	 }
	 tpi_bit(1);
	 tpi_bit(1);
	 return dIn;
}

/**
 * Update PR
 */
static void tpi_pr_update(uint16_t pr)
{
	 tpi_send_byte(TPI_OP_SSTPR(0));
	 tpi_send_byte(pr & 0xFF);
	 tpi_send_byte(TPI_OP_SSTPR(1));
	 tpi_send_byte(pr >> 8);
}


/**
 * Read Block
 */
void tpi_read_block(uint16_t addr, uint8_t *rptr, uint8_t len)
{
	 uint8_t i;
   tpi_pr_update (addr);
   
	 for(i=0;i<len;i++)
	 {
	 	  tpi_send_byte(TPI_OP_SLD_INC);
	 	  *rptr = tpi_recv_byte(); 
	 	  rptr++;
	 }
}

/**
 * Write block
 */
void tpi_write_block(uint16_t addr, const uint8_t *wptr, uint8_t len)
{
	 uint8_t i;
   tpi_pr_update (addr);
   
	 for(i=0;i<len;i++)
   {
   	  tpi_send_byte(TPI_OP_SOUT(NVMCMD));
   	  tpi_send_byte(NVMCMD_WORD_WRITE);
   	  tpi_send_byte(TPI_OP_SST_INC);
   	  tpi_send_byte(*wptr++);
   	  do
   	  {
   	  	 tpi_send_byte (TPI_OP_SIN(NVMCSR)); 
   	  } while(tpi_recv_byte() &NVMCSR_BSY);
   }
}

#endif  //(USE_TPI)