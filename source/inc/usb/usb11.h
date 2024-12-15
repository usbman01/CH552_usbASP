// usb1.1 definitions
// bitmasks start with an underscore
// origianlly used with an audio device 
// so there is still some mixup with UAC

#ifndef _USB11_H
  #define _USB11_H
  
#define BCD_USB                   0x0110

#define USB_GET_STATUS            0x00
#define USB_CLEAR_FEATURE         0x01
#define USB_SET_FEATURE           0x03
#define USB_SET_ADDRESS           0x05
#define USB_GET_DESCRIPTOR        0x06
#define USB_SET_DESCRIPTOR        0x07
#define USB_GET_CONFIGURATION     0x08
#define USB_SET_CONFIGURATION     0x09
#define USB_GET_INTERFACE         0x0A
#define USB_SET_INTERFACE         0x0B
#define USB_SYNC_FRAME            0x0C

// feature selectors
#define FEATURE_ENDPOINT_HALT     0x00
#define FEATURE_REMOTE_WAKEUP     0x01

#define _ENDPOINT_HALT            0x01
#define _DEVICE_SELFPOWERED       0x01
#define _DEVICE_REMOTE_WAKEUP     0x02

//bmRequestBits
#define _REQUESTMASK              0x60     // Bit 5 ..6 Reqquesttype
#define _RECIPIENTMASK            0x1F     // Bit 0..4  Reciepient
// bmRequestFlags
#define _DEVICE                   0x00     // to Device
#define _HOST                     0x80     // to Host            
#define _STANDART                 0x00     // USB Std Request    
#define _CLASS                    0x20     // USB Class Request  
#define _VENDOR                   0x40     // USB Vendor Request 
#define _RESERVED                 0x60     // reserved Request   
// Recipient
//#define _DEVICE                 0x00     // already defined    
#define _INTERFACE                0x01
#define _ENDPOINT                 0x02
#define _OTHER                    0x03

// std descriptors
#define USB_DEVICE_DESCRIPTOR         0x01
#define USB_CONFIGURATION_DESCRIPTOR  0x02
#define USB_STRING_DESCRIPTOR         0x03
#define USB_INTERFACE_DESCRIPTOR      0x04
#define USB_ENDPOINT_DESCRIPTOR       0x05
// ep types
#define EP_ISOCRON                 0x01
#define EP_BULK                    0x02 
#define EP_INTERRUPT               0x03

//#define BCD_DEVICE               0x0100  
//#define BCD_ADC                  0x0100 // should not be here
   
#define USB_CLASS_UNDEFINED        0x00
#define USB_SUBCLASS_UNDEFINED     0x00
#define USB_PROTOCOL_UNDEFINED     0x00
#define USB_STRING_UNDEFINED       0x00 

typedef struct _DEVICE_DESCRIPTOR
{
    uint8_t blength;
    uint8_t bDescriptorType;
    uint8_t bcdUSBLo;
    uint8_t bcdUSBHi;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint8_t idVendorLo;
    uint8_t idVendorHi;
    uint8_t idProductLo;
    uint8_t idProductHi;
    uint8_t bcdDeviceLo;
    uint8_t bcdDeviceHi;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} DeviceDescriptor,*pDeviceDescriptor;

typedef struct _CONFIGURATION_DESCRIPTOR
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t wTotalLengthLo;
    uint8_t wTotalLengthHi;
    uint8_t bNumInteraces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} ConfigurationDescriptor,*pConfigurationDescriptor;

typedef struct _STRING_DESCRIPTOR
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bstring[2];    // var Size
}StringDescriptor,*pStringDescriptor;

typedef struct _INTERFACE_DESCRIPTOR
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
}InterfaceDescriptor,*pInterfaceDescriptor;

typedef struct _ENDPOINT_DESCRIPTOR
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndPoindAddress;
    uint8_t bmAttributes;
    uint8_t wMaxPacketSizeLo;
    uint8_t wMaxPacketSizeHi;
    uint8_t bIntervall;
}EndpointDescriptor,*pEndpointDescriptor;

// AC_CLASS should not be here
/*
typedef struct _ENDPOINT_DESCRIPTOR1
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndPoindAddress;
    uint8_t bmAttributes;
    uint8_t wMaxPacketSizeLo;
    uint8_t wMaxPacketSizeHi;
    uint8_t bIntervall;
    uint8_t bRefresh;
    uint8_t bSynchAddress;
}EndPointDescriptor1,*pEndPointDescriptor1;
*/
#endif


