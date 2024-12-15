# CH552_usbASP

A CH552 version of the popular usbasp from fischl. The code is mostly based on version 1.8 of nerdralph https://github.com/nerdralph/usbasp with some minor changes, exept for tpi.c and usbdrv.c which are completely new. It will work without any change on the following WCH microcontroller:

  - CH551
  - CH552
  - CH554

Usbasp is on new USB Ports (3.1) much more stable because it no longer depends on VUSB. On windows machines it can automatically load winusb as driver so no more installation issues and hassles with zadig. If there is already a usbasp installation onthe PC with libusb, libusb will used. On Linux always libusb wil be used because WCID is windows only.

## Features:

  - fullspeed (12MHz) design
  - firmware update via USB using WCHs 
  - automatic driver installation on Win8.1 and above
  - winusb driver (tested with avrDude >= 7.1)
  - compound device with USB CDC-ACM serial port 
  - 5V and 3.3V operation

WCID, TPI support and the extra CDC can be switched off. For details check global.h The code can be compiled using Keil c51 or SDCC. Since i dont have AVRs using TPI at hand TPI is just simulated and may or may not work. 

## Limits:

When using the compound version including CDC its neccessary to use avrdude-v8.0-windows-mingw-x64.zip because the normal version can not handle compount devices.

## todo:

 - make usbdrv usb2cv compilant
 - finish CDC support
 - switch to 24MHz fsys so CDC can work up to 115k2 baud
 - complete SDCC support (build) 

## References:

 - WCH:     https://www.wch-ic.com/downloads/WCHISPTool_Setup_exe.html
 - usbasp:  https://www.fischl.de/usbasp/
 - avrdude: https://github.com/avrdudes/avrdude/releases