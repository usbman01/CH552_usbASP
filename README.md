# CH552_usbASP

A CH552 version of the popular usbasp from fischl. The code is mostly based on version 1.8 of nerdralph https://github.com/nerdralph/usbasp with some minor changes, exept for tpi.c and usbdrv.c which are completely new. It will work on the following WCH microcontroller:

  - CH551 (needs some changes because of XDATA limits)
  - CH552
  - CH554

CH552_usbasp is much more stable because it no longer depends on VUSB. On windows machines it can automatically load winusb as driver so no more installation issues and hassles with zadig. If there is already a usbasp installation on the PC with libusb, libusb will be used. On Linux always libusb will be used because WCID is windows only.

## Features:

  - Fullspeed (12MHz) design
  - Firmware update via USB using WCHs WCHISPTool 
  - Automatic driver installation on Win8.1 and above
  - Winusb driver (tested with avrDude >= 7.1)
  - Compound device with USB CDC-ACM serial port 
  - 5V and 3.3V operation

## Config Options:

WCID, TPI, CDC and the fast copy function can be switched off. For details check global.h The code can be compiled using Keil c51, SDCC or Raisonance rc51. A port to IAR EW8051 is still under develpment and currently not working.   
Since i dont have AVRs using TPI at hand TPI is just simulated and may or may not work. 

## Benchmarks:

Since the code is prepared to work with various C compiler it also can be used for benchmarking popular MCS51 C compiler. 

## Limits:

When using the compound version including CDC its neccessary to use avrdude-v8.0-windows-mingw-x64.zip because the normal version can not handle compount devices.

## todo:

 - make usbdrv usb2cv compilant
 - complete CDC support
 - switch to 24MHz fsys so CDC can work up to 115k2 baud
 - complete SDCC support (build) ✔
 - use that extra opcode for memcpy functions ✔
 - add Raisonance RC51 support ✔
 - complete IAR EW8051 support

## References:

 - Datasheet:  https://www.wch-ic.com/downloads/CH552DS1_PDF.html
 - Isptool:    https://www.wchic.com/downloads/WCHISPTool_Setup_exe.html
 - usbasp 1.4: https://www.fischl.de/usbasp/
 - usbasp 1.8: https://github.com/nerdralph/usbasp
 - avrdude:    https://github.com/avrdudes/avrdude/releases
