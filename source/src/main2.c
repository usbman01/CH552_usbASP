#include <stdint.h>

__code uint8_t DevDesc[18] = { 0x12,0x01,0x10,0x01,0xFF,0x00,0x00,0x08,
                               0xC0,0x16,0xDC,0x05,0x00,0x10,0x01,0x02,
                               0x03,0x10};

__xdata __at (0x0000) uint8_t  EP0_Buffer[0x08 + 2];

uint8_t *ptr;

void main (void)
{
	 uint8_t i;

   ptr = &DevDesc[0];
	 for (i= 0;i<8;i++) 
      EP0_Buffer[i] = *ptr++;

	 while(1) {}	 
}