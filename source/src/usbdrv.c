#include "..\inc\global.h"
#include "..\inc\usb\usb11.h"
#include "..\inc\usb\cdc12.h"
#include "..\inc\isp.h"
#include "..\inc\clock.h"
#include "..\inc\usbasp.h"
#if (USE_TPI)
  #include "..\inc\tpi.h"
  #include "..\inc\tpi_defs.h"
#endif

//*************************************************************
#ifdef __RC51__            // rc51
//*************************************************************
  xdata at 0x0000 uint8_t EP0_Buffer[EP0_SIZE + 2];
  xdata at 0x0030 uint8_t serno[30];
  xdata at 0x0080 uint8_t EP2_OutBuffer[BULK_SIZE];
  xdata at 0x00C0 uint8_t EP2_InBuffer [BULK_SIZE];
  xdata at 0x0100 uint8_t tx_buffer[CDC_BUFSIZE];
  xdata at 0x0200 uint8_t rx_buffer[CDC_BUFSIZE];
  
  extern uint32_t htonl(uint32_t val);
  extern void fastcpy8 (const uint8_t *src, uint8_t xdata *dest, uint8_t size);
//*************************************************************
#elif __C51__            // keil c51
//*************************************************************
  uint8_t  xdata EP0_Buffer[EP0_SIZE + 2] _at_ 0x0000;
  uint8_t  xdata serno[30]                _at_ 0x0042;
  uint8_t  xdata EP2_OutBuffer[BULK_SIZE] _at_ 0x0080;
  uint8_t  xdata EP2_InBuffer [BULK_SIZE] _at_ 0x00C0;
  uint8_t  xdata tx_buffer[CDC_BUFSIZE]   _at_ 0x0100;
  uint8_t  xdata rx_buffer[CDC_BUFSIZE]   _at_ 0x0200;
  
  extern uint32_t htonl(uint32_t val);
  extern void fastcpy8 (const uint8_t *src, uint8_t xdata *dest, uint8_t size);
//*************************************************************
#elif __SDCC_mcs51         // SDCC
//*************************************************************
  __xdata __at (0x0000) uint8_t  EP0_Buffer[EP0_SIZE + 2];
  __xdata __at (0x0042) uint8_t  serno[30];
  __xdata __at (0x0080) uint8_t  EP2_OutBuffer[BULK_SIZE];
  __xdata __at (0x00C0) uint8_t  EP2_InBuffer [BULK_SIZE];
  __xdata __at (0x0100) uint8_t  tx_buffer[CDC_BUFSIZE];
  __xdata __at (0x0200) uint8_t  rx_buffer[CDC_BUFSIZE];
  extern void fastcpy8 (const uint8_t *src, uint8_t XDATA *dest, uint8_t size);
//*************************************************************
#elif __IAR_SYSTEMS_ICC__ // IAR EW8051
//*************************************************************
  __xdata __no_init uint8_t  EP0_Buffer[EP0_SIZE + 2] @0x0000; 
  __xdata __no_init uint8_t  serno[30]                @0x0042;
  __xdata __no_init uint8_t  EP2_OutBuffer[BULK_SIZE] @0x0080;
  __xdata __no_init uint8_t  EP2_InBuffer [BULK_SIZE] @0x00C0;
  __xdata __no_init uint8_t  tx_buffer[CDC_BUFSIZE]   @0x0100;
  __xdata __no_init uint8_t  rx_buffer[CDC_BUFSIZE]   @0x0200;
  extern void fastcpy8 (const uint8_t __generic *src, uint8_t XDATA *dest, uint8_t size);
//*************************************************************
#else
  #error unknown compiler
#endif

// usb vars
tSETUP    Setup;          /**< holding the setup packet */
uint8_t   UsbConfig;      /**< config value here 1 after the device is configured */
#ifdef __IAR_SYSTEMS_ICC__
uint8_t   bRequestError;  /**< indicates any unknown or unsupported request */
uint8_t   bFischl;        /**< can be used to switch vendor string */
#else
BIT       bRequestError;  /**< indicates any unknown or unsupported request */
BIT       bFischl;        /**< can be used to switch vendor string */
#endif

#define CBYTE ((uint8_t volatile CODE  *) 0)

// usb interfaces
#define USBASP_INTERFACE        0
#if(USE_CDC)
  #define CDC_CTL_INTERFACE0    1 // and USB_SET_INTERFACE, USB_GET_INTERFACE
  #define CDC_STREAM_INTERFACE0 2 // and _CLASS requests
  line_t LineCoding;
#endif

uint8_t prog_sck;
static uint8_t  prog_address_newmode;
static uint32_t prog_address;
static uint16_t prog_nbytes;
static uint16_t prog_pagesize;
static uint8_t  prog_blockflags;
static uint8_t  prog_pagecounter;

extern uint16_t tpi_dly_cnt; //todo move it

// const is just for satisfy IAR
#if (USE_CDC)
   const uint8_t CODE alpha[16] = {'0','1','2','3','4','5','6','7',
                                   '8','9','A','B','C','D','E','F'};
#endif
#if (USE_WCID)
   const uint8_t CODE VendorDesc[170] =
   {
       10,0,             // wLength
       0,0,              // header
       0,0,3,6,          // OS
#ifdef __RC51__
       170,0,
#else      
       sizeof (VendorDesc) & 0xFF,
       sizeof (VendorDesc) >> 8,
#endif
       8,0,
       2,0,              // MS_OS_20_SUBSET_HEADER_FUNCTION
       USBASP_INTERFACE, // bFirstInterface == interface 0
       0,                // unused
       160,0,            // wLength

       20,0,
       3,0,              // MS_OS_20_FEATURE_COMPATBLE_ID
       'W','I','N','U','S','B',0,0, //Compatible ID String
       0,0,0,0,0,0,0,0,             //Sub-compatible ID String

       132,0,
       4,0,              // MS_OS_20_FEATURE_REG_PROPERTY
       7,0,              // Multiple NULL-terminated Unicode strings (REG_MULTI_SZ)

   // unicode "DeviceInterfaceGUIDs"
       0x2A,0,
       'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,
       'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,
       'G',0,'U',0,'I',0,'D',0,'s',0,
       0,0,
   // unicode "{CDB3B5AD-293B-4663-AA36-1AAE46463776}"
       0x50,0,
       '{',0,'C',0,'D',0,'B',0,'3',0,'B',0,'5',0,'A',0,'D',0,'-',0,
       '2',0,'9',0,'3',0,'B',0,'-',0,
       '4',0,'6',0,'6',0,'3',0,'-',0,
       'A',0,'A',0,'3',0,'6',0,'-',0,
       '1',0,'A',0,'A',0,'E',0,'4',0,'6',0,'4',0,'6',0,'3',0,'7',0,'7',0,'6',0,'}',0,
       0,0,

       0,0
   };

   const uint8_t CODE BosDesc[40] =
   {
       5,0x0F,                 // bLength + BOS see 3.0 Spec page 357
#ifdef __RC51__
       40,0,
#else      
       sizeof(BosDesc) & 0xFF, // wTotalLengtn
       sizeof(BosDesc) >> 8,
#endif
       2,                      // 2 sets (Extension Desc + MS OS Desc)
   //USB2 Extension Descriptor (set no 1)
       7,0x10,                 // blength + DEVICE_CAPABILITY see 3.0 Spec page 357
       2,                      // USB 2.0 Extension Descriptor
       0,0,0,0,                // cap bitmap nothing used
   //Microsoft OS 2.0 platform capability descriptor header (set no 2)
       28,0x10,               // bLength + DEVICE CAPABILITY Descriptor
       5,                     // MS_PLATFORM
       0,                     // reserved
   //MS OS 2.0 descriptor platform capability ID
   //D8DD60DF-4589-4CC7-9CD2-659D9E648A9F
       0xDF,0x60,0xDD,0xD8,
       0x89,0x45,
       0xC7,0x4C,
       0x9C,0xD2,
       0x65,0x9D,0x9E,0x64,0x8A,0x9F,
       0,0,3,6,                 // 0x06030000 NTDDI_WINBLUE (W8.1)
       sizeof (VendorDesc) & 0xFF,
       sizeof (VendorDesc) >> 8,// vendor request  wTotalength
       VENDOR_CODE,            // vendor bRequest code (0x40)
       0                       // reserved
   };

   const uint8_t CODE DevDesc[18] =
   {
      sizeof(DeviceDescriptor), USB_DEVICE_DESCRIPTOR,
      0x210 & 0xFF, 0x210 >> 8,   // USB 2.1 (2.0 with extensions)
#if (USE_CDC)
      0xEF,                       // Misc device class
      0x02,                       // Subclass common
      0x01,                       // IAD protocol
#else
      0xFF,                       // vendor  class
      USB_SUBCLASS_UNDEFINED,     //
      USB_PROTOCOL_UNDEFINED,     //
#endif
      EP0_SIZE,
      0x16c0 & 0xFF,0x16c0 >> 8,  //
      //0xDEAD & 0xFF,0xDEAD >> 8,  //
      1500   & 0xFF,  1500 >> 8,  //
      0x0108 & 0xFF,0x0108 >> 8,  // Version 1.8
      0x01,                       // vendorstr
      0x02,                       // devicestr
#if (USE_CDC)
      0x03,                       // cdc should use a serial string
#else
      USB_STRING_UNDEFINED,       // no serial no at this time
#endif
      1                           // 1 configuration
   };
#else
   const uint8_t CODE DevDesc[18] =
   {
      sizeof(DeviceDescriptor), USB_DEVICE_DESCRIPTOR,
      //BCD_USB & 0xFF, BCD_USB >> 8,
      0,2,
#if (USE_CDC)
      0xEF,                       // Misc device class
      0x02,                       // Subclass common
      0x01,                       // IAD protocol
#else
      0xFF,                       // vendor  class
      USB_SUBCLASS_UNDEFINED,     //
      USB_PROTOCOL_UNDEFINED,     //
#endif
      EP0_SIZE,
      0x16c0 & 0xFF,0x16c0 >> 8,  //
      1500   & 0xFF,  1500 >> 8,  //
      0x0108 & 0xFF,0x0108 >> 8,  // Version 1.8
      0x01,                       // vendorstr
      0x02,                       // devicestr
#if (USE_CDC)
      0x03,                       // cdc should use a serial string
#else
      USB_STRING_UNDEFINED,       // no serial no at this time
#endif
      1                           // 1 configuration
   };
#endif

#if (USE_CDC)
   const uint8_t CODE CfgDesc[   sizeof (ConfigurationDescriptor)+
                               3*sizeof (InterfaceDescriptor)    +
                               3*sizeof (EndpointDescriptor)     +
                               1* 8 +19 //1 IAD + 3*5 +4
                       ] =
   {
      sizeof (ConfigurationDescriptor), USB_CONFIGURATION_DESCRIPTOR,
#ifdef __RC51__
      84,0,
#else
      sizeof (CfgDesc) & 0xFF, sizeof (CfgDesc) >> 8,
#endif
      3,                              // 3 Interfaces
      1,                              // Configuration Value
      USB_STRING_UNDEFINED,           // Iconfig
      0xA0,                           // buspowered | remotewakeup
      100/2,                          // 100 mA

      // Interface 0 usbASP
      sizeof(InterfaceDescriptor),USB_INTERFACE_DESCRIPTOR,
      USBASP_INTERFACE,               // InterfaceNr 0
      0,                              // no alt.
      0,                              // no EP
      0xFF,                           // VENDOR CLASS
      USB_SUBCLASS_UNDEFINED,         // SubClass Control
      USB_PROTOCOL_UNDEFINED,         // no protokoll
      2,                              // usbASP

      // IAD1
      0x08,0x0B,
      1,                              // control interface 1
      2,                              // 2 interfaces
      USB_CLASS_CDC,
      CDC_SUBCLASS_ABSTRACT,
      CDC_PROTOCOL_V250,
      USB_STRING_UNDEFINED,

      //interface 1 CDC Control
      sizeof(InterfaceDescriptor), USB_INTERFACE_DESCRIPTOR,
      CDC_CTL_INTERFACE0,         // interface No 1
      0,                          // alternate setting
      1,                          // 1 extra endpoint
      USB_CLASS_CDC,              // class
      CDC_SUBCLASS_ABSTRACT,      // subclass
      CDC_PROTOCOL_V250,          // protokol
      USB_STRING_UNDEFINED,

      5,CDC_CS_INTERFACE,
      CDC_SUB_HEADER,             // identifier header
      BCD_CDC & 0xFF,BCD_CDC >> 8,//

      5,CDC_CS_INTERFACE,
      CDC_SUB_CALL_FUNCTION,      // identifier Call Management
      0x00,                       // no bmCapablities
      0,                          // no interface for call Management

      4,CDC_CS_INTERFACE,
      CDC_SUB_ABSTRACT_CTRL,      // identifier Abstract
      0x02,                       // cababilities

      5,CDC_CS_INTERFACE,
      CDC_SUB_UNION_FUNCTIONAL,   // identifier Functional
      CDC_CTL_INTERFACE0,         // interface 1 Ctrl
      CDC_STREAM_INTERFACE0,      // interface 2 Data

      sizeof(EndpointDescriptor),USB_ENDPOINT_DESCRIPTOR,
      1 | _HOST,                  // ep1 in
      EP_INTERRUPT,               // interrupt
      0x08,0x00,
      0xFF,                       // interval

      //interface2 CDC data interface
      sizeof(InterfaceDescriptor),USB_INTERFACE_DESCRIPTOR,
      CDC_STREAM_INTERFACE0,      // interface No 2
      0,                          // alternate setting
      2,                          // 2 extra endpoint
      CDC_CLASS_DATA,
      USB_SUBCLASS_UNDEFINED,
      USB_PROTOCOL_UNDEFINED,
      USB_STRING_UNDEFINED,

      sizeof(EndpointDescriptor),USB_ENDPOINT_DESCRIPTOR,
      2 | _DEVICE,                // ep2 out
      EP_BULK,                    // bulk
      BULK_SIZE & 0xFF,BULK_SIZE >> 8,
      0x00,                       // interval

      sizeof(EndpointDescriptor),USB_ENDPOINT_DESCRIPTOR,
      2 | _HOST,                  // ep2 In
      EP_BULK,                    // bulk
      BULK_SIZE & 0xFF,BULK_SIZE >> 8,
      0x00                        // interval
   };

#else
   const uint8_t CODE CfgDesc[18] =
   {
      sizeof (ConfigurationDescriptor), USB_CONFIGURATION_DESCRIPTOR,
#ifdef __RC51__
      18,0,
#else      
      sizeof (CfgDesc) & 0xFF, sizeof (CfgDesc) >> 8,
#endif
      1,                              // 1 Interfaces
      1,                              // Configuration Value
      USB_STRING_UNDEFINED,           // Iconfig
      0xA0,                           // buspowered | remotewakeup
      100/2,                          // 100 mA

      // Interface 0 control
      sizeof(InterfaceDescriptor),USB_INTERFACE_DESCRIPTOR,
      USBASP_INTERFACE,               // InterfaceNr 0
      0,                              // no alt.
      0,                              // no EP
      0,                              //
      USB_SUBCLASS_UNDEFINED,         // SubClass Control
      USB_PROTOCOL_UNDEFINED,         // no protokoll
      2                               // usbASP
   };
#endif

const uint8_t CODE Lang_Desc[4] =
{
#ifdef __RC51__
   4,USB_STRING_DESCRIPTOR,
#else   
   sizeof(Lang_Desc),USB_STRING_DESCRIPTOR,
#endif   
   9,4
};

const uint8_t CODE fischl_Desc[28] =
{
#ifdef __RC51__
   28,USB_STRING_DESCRIPTOR,
#else   
   sizeof(fischl_Desc),USB_STRING_DESCRIPTOR,
#endif   
   'w',0,'w',0,'w',0,'.',0,'f',0,'i',0,'s',0,'c',0,'h',0,'l',0,'.',0,'d',0,'e',0
};

const uint8_t CODE usbman_Desc[72] =
{
#ifdef __RC51__
   72,USB_STRING_DESCRIPTOR,
#else
   sizeof(usbman_Desc),USB_STRING_DESCRIPTOR,
#endif
   'w',0,'w',0,'w',0,'.',0,'g',0,'i',0,'t',0,'h',0,'u',0,'b',0,'.',0,
   'c',0,'o',0,'m',0,'/',0,'u',0,'s',0,'b',0,'m',0,'a',0,'n',0,'1',0,
   '/',0,'C',0,'H',0,'5',0,'5',0,'2',0,'_',0,'u',0,'s',0,'b',0,'A',0,'S',0,'P',0
};

const uint8_t CODE Prod_Desc[14] =
{
#ifdef __RC51__
   14,USB_STRING_DESCRIPTOR, 
#else   
   sizeof(Prod_Desc),USB_STRING_DESCRIPTOR,
#endif
   'U',0,'S',0,'B',0,'a',0,'s',0,'p',0
};

#if (USE_CDC)
  void Init_UsbSerialNo(void)
  {
    uint8_t by;

    serno[0] = 22;
    serno[1] = 3;
    by = CBYTE[ROM_CHIP_ID_HX];
    serno[2] = alpha [by >> 4];   serno[3]=0;
    serno[4] = alpha [by & 0x0F]; serno[5]=0;

    by = CBYTE[ROM_CHIP_ID_LO];
    serno[6] = alpha [by >> 4];   serno[7]=0;
    serno[8] = alpha [by & 0x0F]; serno[9]=0;

    by = CBYTE[ROM_CHIP_ID_LO+1];
    serno[10] = alpha [by >> 4];   serno[11]=0;
    serno[12] = alpha [by & 0x0F]; serno[13]=0;

    by = CBYTE[ROM_CHIP_ID_LO+2];
    serno[14] = alpha [by >> 4];   serno[15]=0;
    serno[16] = alpha [by & 0x0F]; serno[17]=0;

    by = CBYTE[ROM_CHIP_ID_LO+3];
    serno[18] = alpha [by >> 4];   serno[19]=0;
    serno[20] = alpha [by & 0x0F]; serno[21]=0;
  }

  void Init_LineSettings(void) //todo move it
  {
     LineCoding.s.dwDTERate   = 9600L;
     LineCoding.s.bCharFormat = 0;
     LineCoding.s.bParityType = 0;
     LineCoding.s.bDataBits   = 8;
  }
#endif

void usbInit(void)
{
    USB_CTRL   = 0;
#if(USE_CDC)
    Init_UsbSerialNo();
    Init_LineSettings();
    UEP2_3_MOD = bUEP2_RX_EN | bUEP2_TX_EN;
#else
    UEP2_3_MOD = 0;
#endif
    UEP4_1_MOD = 0;
    UEP0_DMA_H = ((uint16_t) &EP0_Buffer[0]) >> 8;
    UEP0_DMA_L = ((uint16_t) &EP0_Buffer[0]) & 0xFF;
#if(USE_CDC)
    UEP2_DMA_H = ((uint16_t) &EP2_OutBuffer[0]) >> 8;
    UEP2_DMA_L = ((uint16_t) &EP2_OutBuffer[0]) & 0xFF;
#endif
    UEP2_CTRL  = UEP_T_RES_NAK;
    USB_CTRL   = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;
    UDEV_CTRL  = bUD_PD_DIS | bUD_PORT_EN;
    USB_INT_FG = 0xFF;
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
}

#ifdef __C51__
  void Usb_IRQ (void) interrupt INT_NO_USB
#elif __SDCC_mcs51
  void Usb_IRQ (void) __interrupt (INT_NO_USB)
#elif __IAR_SYSTEMS_ICC__
  #pragma vector = INT_ADDR_USB
  __interrupt void Usb_IRQ (void) 
#endif
{
#ifdef __IAR_SYSTEMS_ICC__   // hack for stupid IAR
   static const  __generic uint8_t  *ptr;
#else   
   static const uint8_t  *ptr;
#endif   
   //uint8_t rxlen;
   uint8_t i;
   //uint8_t status=0;  // init to zero because of SDCC warning
   uint8_t len = 0;
   if(UIF_TRANSFER)
   {  // transfer interrupt
      switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
      {
         //
         // todo place here bulkrequests for CDC
         //
         case UIS_TOKEN_SETUP | 0:
              {  // handle setup stage
                 UEP0_CTRL &= 0xF2; // remove any possible STALL contition from previous request
                 if(USB_RX_LEN == (sizeof(tSETUP)))
                 {  // copy the setup packet
                    // USB is lsb first, keil is msb first
                    Setup.bRequestType = EP0_Buffer[0];
                    Setup.bRequest     = EP0_Buffer[1];
                    Setup.wValueLo     = EP0_Buffer[2];
                    Setup.wValueHi     = EP0_Buffer[3];
                    Setup.wIndexLo     = EP0_Buffer[4];
                    Setup.wIndexHi     = EP0_Buffer[5];
                    Setup.wLength      =(EP0_Buffer[7] << 8) | EP0_Buffer[6];

                    bRequestError=0;
                    len = 0;
                    //
                    // chk the std recipient
                    //
                    if ((Setup.bRequestType & _REQUESTMASK) == _STANDART)
                    {
                       switch(Setup.bRequest)
                       {
                          case USB_GET_DESCRIPTOR:
                               switch(Setup.wValueHi)
                               {
                                  case USB_DEVICE_DESCRIPTOR:
                                       {
                                          ptr  = (const uint8_t *)&DevDesc[0];
                                          //len  = DevDesc[0];
                                          len = sizeof(DevDesc);
                                       }
                                       break;
                                  case USB_CONFIGURATION_DESCRIPTOR:
                                       {
                                           ptr = (const uint8_t *)&CfgDesc[0];
                                           //len = CfgDesc[2]; //dodo just ok if size <= 255 bytes
                                           len = sizeof(CfgDesc);
                                       }
                                       break;
                                  case USB_STRING_DESCRIPTOR:
                                       {
                                          switch (Setup.wValueLo)
                                          {
                                             case 0:
                                                  ptr = (const uint8_t *)&Lang_Desc[0];
                                                  //len = Lang_Desc[0];
                                                  len = sizeof(Lang_Desc);
                                                  break;
                                             case 1:
                                                  if (bFischl)
                                                  {
                                                     ptr = (const uint8_t *)&fischl_Desc[0];
                                                     len = sizeof(fischl_Desc);
                                                  }
                                                  else
                                                  {
                                                     ptr = (const uint8_t *)&usbman_Desc[0];
                                                     len = sizeof(usbman_Desc);
                                                  }
                                                  break;
                                             case 2:
                                                  ptr = (const uint8_t *)&Prod_Desc[0];
                                                  //len = Prod_Desc[0];
                                                  len = sizeof(Prod_Desc);
                                                  break;
#if(USE_CDC)
                                             case 3:
                                                  ptr = (const uint8_t *) &serno[0];
                                                  len = serno[0];
                                                  break;
#endif
                                             default:
                                                  bRequestError=1;
                                                  //break;
                                          }
                                       }
                                       break;
#if(USE_WCID)
                                  case 0x0F:
                                       {
                                          ptr = (const uint8_t*)&BosDesc[0];
                                          len = sizeof(BosDesc);
                                       }
                                       break;
#endif
                                  default:
                                       bRequestError=1;
                                       //break;
                               }
                               if (Setup.wLength < len) len = Setup.wLength;
                               else Setup.wLength = len;
                               if (len > EP0_SIZE) len = EP0_SIZE;
#if(USE_FCPY)
                               fastcpy8(ptr,&EP0_Buffer[0],len); 
                               ptr+=len;
#else
                               for (i=0; i<len; i++) {EP0_Buffer[i] = *ptr++;}
#endif
                               Setup.wLength -= len;
                               break;
                          case USB_SET_ADDRESS:
                               break;
                          case USB_GET_CONFIGURATION:
                               EP0_Buffer[0] = UsbConfig;
                               len = 1;
                               break;
                          case USB_SET_CONFIGURATION:
                               {
                                  UsbConfig = Setup.wValueLo;
                                  if (UsbConfig)
                                  {
                                     UEP2_3_MOD = bUEP2_RX_EN | bUEP2_TX_EN;
                                     UEP2_CTRL  = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;
                                     //bTx0Empty  = 1;
                                     //bUsb0InReady = 1;
                                     //UsbState   = USB_STATE_CONFIG;
                                     //MidiRxCtl.rd = MidiRxCtl.wr = MidiRxCtl.level=0;
                                     break;
                                  }
                                  //bTx0Empty  = 0;
                                  UEP2_3_MOD = 0;
                                  //UsbState   = USB_STATE_ADDRESSED;
                               }
                               break;
                          case USB_CLEAR_FEATURE:
                               if(( Setup.bRequestType & _RECIPIENTMASK) == _DEVICE )
                               {
                                   if(Setup.wValueLo == FEATURE_REMOTE_WAKEUP )
                                   {
                                       if( CfgDesc[7] & 0x20 )
                                       {

                                       }
                                       else bRequestError=1;
                                   }
                                   else bRequestError=1;
                               }
                               else bRequestError=1;
                               break;
                          case USB_SET_FEATURE:
                               if((Setup.bRequestType & 0x1F) == _DEVICE)
                               {
                                   if(Setup.wValueLo == FEATURE_REMOTE_WAKEUP )
                                   {
                                       if( CfgDesc[7] & 0x20 )
                                       {
                                           while ( XBUS_AUX & bUART0_TX )
                                           {
                                              ;
                                           }
                                           SAFE_MOD = 0x55;
                                           SAFE_MOD = 0xAA;
                                           WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO | bWAK_RXD1_LO;
                                           PCON |= PD;
                                           SAFE_MOD = 0x55;
                                           SAFE_MOD = 0xAA;
                                           WAKE_CTRL = 0x00;
                                       }
                                       else bRequestError=1;
                                   }
                                   else bRequestError=1;
                               }
                               else bRequestError=1;
                               break;
                          case USB_GET_STATUS: //todo code here still missing
                               EP0_Buffer[0] = 0x00;
                               EP0_Buffer[1] = 0x00;
                               len = 2;
                               break;
                          case USB_GET_INTERFACE:
                               {
                                  if(Setup.wIndexHi==0)
                                  {
                                     switch(Setup.wIndexLo)
                                     {
                                        case USBASP_INTERFACE:
#if (USE_CDC)
                                        case CDC_CTL_INTERFACE0:
                                        case CDC_STREAM_INTERFACE0:
#endif
                                             EP0_Buffer[0] = 0; //only alt. 0 available
                                             len = 1;
                                             break;
                                        default: bRequestError = 1;
                                     }
                                  } else bRequestError = 1;

                               }
                               break;
                          case USB_SET_INTERFACE:
                               {
                                  if(Setup.wIndexHi==0)
                                  {
                                     switch(Setup.wIndexLo)
                                     {
                                        case USBASP_INTERFACE:
#if (USE_CDC)
                                        case CDC_CTL_INTERFACE0:
                                        case CDC_STREAM_INTERFACE0:
#endif
                                             if (Setup.wValueLo !=0) bRequestError = 1;
                                             break;
                                        default: bRequestError = 1;
                                     }
                                  } else bRequestError = 1;
                               }
                               break;
                          default:
                               bRequestError=1;
                               //break;
                       }
                    }
#if (USE_CDC)
                    //
                    // chk the CDC recipient
                    //
                    else if ((Setup.bRequestType & _REQUESTMASK) == _CLASS)
                    {
                       switch(Setup.bRequest)
                       {
                          case GET_LINE_CODING:
                               ptr = (uint8_t*) &LineCoding;
                               len  = sizeof(LineCoding);
                               if (Setup.wLength < sizeof(LineCoding)) len = Setup.wLength;
#ifdef BIG_ENDIAN_
                               LineCoding.s.dwDTERate = htonl(LineCoding.s.dwDTERate);  //lsb first
                               for (i=0; i<len; i++) EP0_Buffer[i] = *ptr++;
                               LineCoding.s.dwDTERate = htonl(LineCoding.s.dwDTERate);  //msb first
#else
                               for (i=0; i<len; i++) EP0_Buffer[i] = *ptr++;
#endif
                               Setup.wLength -= len;
                               break;
                          case SET_LINE_CODING:
//                             break;
                          case SET_CONTROL_LINE_STATE:
                               break;
                          default:
                               bRequestError=1;
//                             break;
                       }
                    }
#endif
                    //
                    // chk the USBasp recipient
                    //
                    else if ((Setup.bRequestType & _REQUESTMASK) == _VENDOR)
                    {
                       //len = 0;
                       switch(Setup.bRequest)
                       {
                          case USBASP_FUNC_CONNECT:
                               {
                                  ispSetSCKOption(prog_sck);  // set SCK speed
                                  prog_address_newmode = 0;   // set compatibility mode of address delivering
                                  LED_RED = ON;
                                  ispConnect();
                               }
                               break;
                          case USBASP_FUNC_DISCONNECT:
                               {
                                  ispDisconnect();
                                  LED_RED = OFF;
                               }
                               break;
                          case USBASP_FUNC_TRANSMIT:
                               {
                                  EP0_Buffer[0] = ispTransmit(Setup.wValueLo);
                                  EP0_Buffer[1] = ispTransmit(Setup.wValueHi);
                                  EP0_Buffer[2] = ispTransmit(Setup.wIndexLo);
                                  EP0_Buffer[3] = ispTransmit(Setup.wIndexHi);
                                  len = 4;
                               }
                               break;
                          case USBASP_FUNC_READFLASH:
                          case USBASP_FUNC_READEEPROM:
                               {
                                  if (!prog_address_newmode)
                                  {
                                     prog_address = (Setup.wValueHi << 8) | Setup.wValueLo;
                                  }
                                  prog_nbytes = Setup.wLength;
                                  if (Setup.wLength >= EP0_SIZE) len = EP0_SIZE;
                                  else len =  Setup.wLength;
                                  if (Setup.bRequest == USBASP_FUNC_READFLASH)
                                  {
                                     //prog_state = PROG_STATE_READFLASH;
                                     for (i=0;i<len;i++) EP0_Buffer[i] = ispReadFlash(prog_address++);
                                  }
                                  else
                                  {
                                     //prog_state = USBASP_FUNC_READEEPROM;
                                     for (i=0;i<len;i++) EP0_Buffer[i] = ispReadEEPROM(prog_address++);
                                  }
                                  Setup.wLength -= len;
                               }
                               break;
                          case USBASP_FUNC_ENABLEPROG:
                               {
                                  EP0_Buffer[0] = ispEnterProgrammingMode();
                                  len = 1;
                               }
                               break;
                          case USBASP_FUNC_WRITEFLASH:
                               {
                                  if (!prog_address_newmode)
                                  {
                                     prog_address = (Setup.wValueHi << 8) | Setup.wValueLo;
                                  }
                                  prog_pagesize   = Setup.wIndexLo;
                                  prog_blockflags = Setup.wIndexHi & 0x0F;
                                  prog_pagesize  += (((uint16_t) Setup.wIndexHi & 0xF0) << 4);
                                  if (prog_blockflags & PROG_BLOCKFLAG_FIRST)
                                  {
                                     prog_pagecounter = prog_pagesize;
                                  }
                                  prog_nbytes = Setup.wLength;
                                  //prog_state  = PROG_STATE_WRITEFLASH;
                               }
                               break;
                          case USBASP_FUNC_WRITEEEPROM:
                               {
                                  if (!prog_address_newmode)
                                  {
                                     prog_address = (Setup.wValueHi << 8) | Setup.wValueLo;
                                  }
                                  prog_pagesize   = 0;
                                  prog_blockflags = 0;
                                  prog_nbytes     = Setup.wLength;
                                  //prog_state      = PROG_STATE_WRITEEEPROM;
                               }
                               break;
                          case USBASP_FUNC_SETLONGADDRESS:
                               {
                                  /* set new mode of address delivering (ignore address delivered in commands) */
                                  prog_address_newmode = 1;
                                  /* set new address */
                                  prog_address = *((uint32_t*) &Setup.wValueLo); //todo change it
#ifdef BIG_ENDIAN_
                                  prog_address = htonl(prog_address);
#endif
                               }
                               break;
                          case USBASP_FUNC_SETISPSCK:
                               {
                                  /* set sck option */
                                  prog_sck = Setup.wValueLo;
                                  EP0_Buffer[0] = 0;
                                  len = 1;
                               }
                               break;
#if (USE_TPI)
                          case USBASP_FUNC_TPI_CONNECT:
                               {
                                  tpi_dly_cnt =  Setup.wValueLo | (Setup.wValueHi << 8);

                                  RST_TO_OUTPUT(); // RST high
                                  PIN_RST =1;
                                  clockWait(3);

                                  PIN_RST = 1;     // RST low
                                  LED_RED = ON;
                                  clockWait(16);

                                  tpi_init();
                               }
                               break;
                          case USBASP_FUNC_TPI_DISCONNECT:
                               {

                                  tpi_send_byte(TPI_OP_SSTCS(TPISR));
                                  tpi_send_byte(0);

                                  clockWait(10);

                                  /* pulse RST */
                                  ISP_RST = 1; clockWait(5);
                                  ISP_RST = 0; clockWait(5);

                                  /* set all ISP pins inputs */
                                  SPI_TO_INPUT();
                                  RST_TO_INPUT();

                                  LED_RED = OFF;
                               }
                               break;
                          case USBASP_FUNC_TPI_RAWREAD:
                               {
                                  EP0_Buffer[0] = tpi_recv_byte();
                                  len = 1;
                               }
                               break;
                          case USBASP_FUNC_TPI_RAWWRITE:
                               {
                                  tpi_send_byte(Setup.wValueLo);
                               }
                               break;
                          case USBASP_FUNC_TPI_READBLOCK:
                               {
                                  prog_address = (Setup.wValueHi << 8) | Setup.wValueLo;
                                  prog_nbytes  = Setup.wLength;
                                  if (Setup.wLength >= EP0_SIZE) len = EP0_SIZE;
                                  else len =  Setup.wLength;
                                  tpi_read_block(prog_address, (uint8_t *) &EP0_Buffer[0], len);
                                  prog_address  += len;
                                  Setup.wLength -= len;
                               }
                               break;
                          case USBASP_FUNC_TPI_WRITEBLOCK:
                               {
                                  prog_address = (Setup.wValueHi << 8) | Setup.wValueLo;
                                  prog_nbytes  = Setup.wLength;
                               }
                               break;
#endif //USE_TPI

#if(USE_WCID)
                          case VENDOR_CODE:
                               { // process autoload vendor requests
                                  if (Setup.wIndexLo==0x07)
                                  {
                                     ptr = (uint8_t *) &VendorDesc[0];
                                     len = sizeof(VendorDesc);
                                     if (Setup.wLength < len) len = Setup.wLength;
                                     else Setup.wLength = len;
                                     if (len >= EP0_SIZE) len = EP0_SIZE;
#if(USE_FCPY)
                                     fastcpy8(ptr,&EP0_Buffer[0],len); 
                                     ptr+=len;
#else
                                     for (i=0; i<len; i++) {EP0_Buffer[i] = *ptr++;}
#endif
                                     Setup.wLength -= len;
                                  }
                               }
                               break;
#endif
                          case USBASP_FUNC_GETCAPABILITIES:
                               {
#if(USE_TPI)
                                  EP0_Buffer[0] = 1;
#else
                                  EP0_Buffer[0] = 0;
#endif
                                  EP0_Buffer[1] = 0;
                                  EP0_Buffer[2] = 0;
                                  EP0_Buffer[3] = 0;
                                  len = 4;
                               }
                               break;
                          default:
                               bRequestError=1;
                               //break;
                       }
                    }
                    else  bRequestError=1;
                 }
                 else bRequestError=1;
                 //
                 // buffer is filled (on IN transfer)
                 // if the request is valid arm the EP
                 //
                 if(bRequestError) //STALL on any error
                 {
                    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;
                 }
                 else
                 {
                    // arm both in and out start transfer with toggle = 1
                    // do it because the host can cancel any transfer
                    UEP0_T_LEN = len;
                    UEP0_CTRL  = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
                 }
              }
              break;
         case UIS_TOKEN_IN | 0:
              {  //handle in stages
                 if ((Setup.bRequestType & _REQUESTMASK) == _STANDART)
                 {  // datastage [in]
                    switch(Setup.bRequest)
                    {
                       case USB_GET_DESCRIPTOR:
                            if (Setup.wLength >= EP0_SIZE) len = EP0_SIZE;
                            else len = Setup.wLength;
#if(USE_FCPY)
                            fastcpy8(ptr,&EP0_Buffer[0],len); 
                            ptr+=len;
#else
                            for (i=0; i<len; i++) {EP0_Buffer[i] = *ptr++;}
#endif
                            Setup.wLength -= len;
                            UEP0_T_LEN     = len;
                            UEP0_CTRL     ^= bUEP_T_TOG;
                            break;
                       case USB_SET_ADDRESS:
                            USB_DEV_AD = (USB_DEV_AD & bUDA_GP_BIT) | (Setup.wValueLo & 0x7F);
                            UEP0_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
                            //UsbState   = USB_STATE_ADDRESSED;
                            break;
                       default: //ZLP for out
                            UEP0_T_LEN = 0;
                            UEP0_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
                            //break;
                    }
                 }
#if USE_CDC
                 else if ((Setup.bRequestType & _REQUESTMASK) == _CLASS)
                 {  // cdc class requests
                    UEP0_T_LEN = 0;
                    UEP0_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
                 }
#endif
                 else if ((Setup.bRequestType & _REQUESTMASK) == _VENDOR)
                 {
                    switch(Setup.bRequest)
                    {
                       case USBASP_FUNC_READFLASH:
                       case USBASP_FUNC_READEEPROM:
                            {
                               if (Setup.wLength >= EP0_SIZE) len = EP0_SIZE;
                               else len = Setup.wLength;
                               if (Setup.bRequest == USBASP_FUNC_READFLASH)
                               {
                                  for (i=0;i<len;i++) EP0_Buffer[i] = ispReadFlash(prog_address++);
                               }
                               else // if (Setup.bRequest == USBASP_FUNC_READEEPROM)
                               {
                                  for (i=0;i<len;i++) EP0_Buffer[i] = ispReadEEPROM(prog_address++);
                               }
                               Setup.wLength -= len;
                               UEP0_T_LEN     = len;
                               UEP0_CTRL     ^= bUEP_T_TOG;
                            }
                            break;
#if (USE_TPI)
                       case PROG_STATE_TPI_READ:
                            {
                               if (Setup.wLength >= EP0_SIZE) len = EP0_SIZE;
                               else len = Setup.wLength;
                               tpi_read_block(prog_address, (uint8_t *)&EP0_Buffer[0], len);
                               prog_address  += len;
                               Setup.wLength -= len;
                               UEP0_T_LEN     = len;
                               UEP0_CTRL     ^= bUEP_T_TOG;

                            }
                            break;
#endif
                       case VENDOR_CODE:
                            {
                               len = Setup.wLength;
                               if (len >= EP0_SIZE) len = EP0_SIZE;
                               if (len)
                               {
                                  for (i=0; i<len; i++) {EP0_Buffer[i] = *ptr++;}
                               }
                               Setup.wLength -= len;
                               UEP0_T_LEN     = len;
                               UEP0_CTRL     ^= bUEP_T_TOG;
                            }
                            break;
                       default:
                            UEP0_T_LEN = 0;
                            UEP0_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
                            //break;
                    }
                 }
              }
              break;
         case UIS_TOKEN_OUT | 0:
              {  //handle out stages
                 if((Setup.bRequestType & _REQUESTMASK) == _STANDART)
                 {
                    UEP0_T_LEN = 0;
                    UEP0_CTRL |= (UEP_R_RES_ACK | UEP_T_RES_NAK);
                 }
#if (USE_CDC)
                 else if ((Setup.bRequestType & _REQUESTMASK) == _CLASS)
                 {
                    // the only cdc cLass out request with datastage
                    if(Setup.bRequest == SET_LINE_CODING)
                    {
                       if(U_TOG_OK)
                       {
                          //ptr = (uint8_t*) &LineCoding;
                          for (i=0; i<sizeof (LineCoding); i++) LineCoding.b[i] = EP0_Buffer[i];
#ifdef BIG_ENDIAN_
                          LineCoding.s.dwDTERate = htonl(LineCoding.s.dwDTERate);
#endif
                          UEP0_T_LEN = 0;
                          UEP0_CTRL ^= bUEP_R_TOG;
                          UEP0_CTRL |= UEP_R_RES_NAK | UEP_T_RES_ACK;
                          //break;
                       }
                    }
                    else
                    {
                       UEP0_T_LEN = 0;
                       UEP0_CTRL |= (UEP_R_RES_ACK | UEP_T_RES_NAK);
                    }
                 }
#endif
                 else if ((Setup.bRequestType & _REQUESTMASK) == _VENDOR)
                 {
                    switch(Setup.bRequest)
                    {
                       case USBASP_FUNC_WRITEFLASH:
                            {
                               if(U_TOG_OK)
                               {
                                  if ( Setup.wLength > EP0_SIZE) len = EP0_SIZE;
                                  else len = Setup.wLength;
                                  for (i=0;i<len;i++)
                                  {
                                     if (prog_pagesize)
                                     {  /* paged */
                                        ispWriteFlash(prog_address, EP0_Buffer[i], 0);
                                        prog_pagecounter--;
                                        if (prog_pagecounter == 0)
                                        {
                                           ispFlushPage(prog_address, EP0_Buffer[i]);
                                           prog_pagecounter = prog_pagesize;
                                        }
                                     }
                                     else
                                     {  /* not paged */
                                        ispWriteFlash(prog_address, EP0_Buffer[i], 1);
                                     }
                                     prog_nbytes--;
                                     if (prog_nbytes == 0)
                                     {
                                        //prog_state = PROG_STATE_IDLE;
                                        if ((prog_blockflags & PROG_BLOCKFLAG_LAST) &&
                                            (prog_pagecounter != prog_pagesize))
                                        {  /* last block and page flush pending, so flush it now */
                                           ispFlushPage(prog_address, EP0_Buffer[i]);
                                        }
                                        //retVal = 1; // Need to return 1 when no more data is to be received
                                     }
                                     prog_address++;
                                  }
                                  Setup.wLength -= len;
                                  UEP0_T_LEN     = 0;
                                  UEP0_CTRL     ^= bUEP_R_TOG;
                               }
                            }
                            break;
                       case USBASP_FUNC_WRITEEEPROM:
                            {
                               if(U_TOG_OK)
                               {
                                  if ( Setup.wLength > EP0_SIZE) len = EP0_SIZE;
                                  else len = Setup.wLength;
                                  for (i=0;i<len;i++)
                                  {
                                     ispWriteEEPROM(prog_address, EP0_Buffer[i]);
                                     prog_nbytes--;
                                     prog_address++;
                                  }
                                  Setup.wLength -= len;
                               }
                            }
                            break;
#if(USE_TPI)
                       case USBASP_FUNC_TPI_WRITEBLOCK:
                            {
                               if(U_TOG_OK)
                               {
                                  if ( Setup.wLength > EP0_SIZE) len = EP0_SIZE;
                                  else len = Setup.wLength;
                                  tpi_write_block(prog_address, (uint8_t *) &EP0_Buffer[0], len);
                                  prog_address  += len;
                                  Setup.wLength -= len;
                                  UEP0_T_LEN     = 0;
                                  UEP0_CTRL     ^= bUEP_R_TOG;
                               }
                            }
                            break;
#endif
                    }
                 }
              }
              break;
//       default:
//            break;
      }
      UIF_TRANSFER = 0;
   }

   if(UIF_BUS_RST)
   {  // usbreset
      //UsbState     = USB_STATE_RESET;
      UEP0_CTRL    = UEP_R_RES_ACK | UEP_T_RES_NAK;
      //prog_state = PROG_STATE_IDLE;
      prog_sck     = USBASP_ISP_SCK_AUTO;
      //todo disable ep1..ep4
      USB_DEV_AD   = 0x00;
      UIF_SUSPEND  = 0;
      UIF_TRANSFER = 0;
      UIF_BUS_RST  = 0;
      //todo reset the bufctls
      UsbConfig     = 0;
      bFischl       = PIN_SLOW;
      //bUsb0InReady  = 1;

   }

   if (UIF_SUSPEND)
   {  // suspend
       UIF_SUSPEND = 0;
/*
       if ( USB_MIS_ST & bUMS_SUSPEND )
       {
           while ( XBUS_AUX & bUART0_TX )
           {
              ;
           }
           SAFE_MOD  = 0x55;
           SAFE_MOD  = 0xAA;
           WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO | bWAK_RXD1_LO;
           UsbState |= USB_STATE_SUSPEND; //a bit stupid anyway
           PCON     |= PD;
           UsbState &=  ~USB_STATE_SUSPEND;
           SAFE_MOD  = 0x55;
           SAFE_MOD  = 0xAA;
           WAKE_CTRL = 0x00;
       }
*/
   }
   // ignore all others
   USB_INT_FG = 0xFF;
}

#ifdef __IAR_SYSTEMS_ICC__
void fastcpy8 ( const uint8_t __generic *src, uint8_t XDATA *dest, uint8_t size)
{
   while(size--) *dest++ = *src++;
}
#endif
