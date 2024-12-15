#ifndef _CDC120_H   
  #define _CDC120_H

#define BCD_CDC                      0x0120
#define USB_CLASS_CDC                0x02
#define CDC_CLASS_DATA               0x0A
// cdc subclass codes table 4
#define CDC_SUBCLASS_RESERVED        0x00
#define CDC_SUBCLASS_DIRECT_LINE     0x01
#define CDC_SUBCLASS_ABSTRACT        0x02
#define CDC_SUBCLASS_TELEPHONE       0x03
#define CDC_SUBCLASS_MULTI_CHANNEL   0x04
#define CDC_SUBCLASS_CAPI            0x05 
#define CDC_SUBCLASS_ETHERNET        0x06
#define CDC_SUBCLASS_ATM             0x07  
#define CDC_SUBCLASS_WIRELESS        0x08 
#define CDC_SUBCLASS_DEVICE          0x09
#define CDC_SUBCLASS_MOBILE          0x0A 
#define CDC_SUBCLASS_OBEX            0x0B
// ... more

#define CDC_CS_INTERFACE             0x24  
#define CDC_CS_ENDPOINT              0x25

// sub descriptor types table 13 
#define CDC_SUB_HEADER               0x00
#define CDC_SUB_CALL_FUNCTION        0x01
#define CDC_SUB_ABSTRACT_CTRL        0x02
#define CDC_SUB_DIRECT_LINE          0x03
#define CDC_SUB_PHONE_RING           0x04
#define CDC_SUB_PHONE_CALL           0x05 
#define CDC_SUB_UNION_FUNCTIONAL     0x06
#define CDC_SUB_COUNTRY              0x07 
#define CDC_SUB_PHONE_OPERATION      0x08 
// ... incomplete

#define CDC_PROTOCOL_UNDEFINED       0x00
#define CDC_PROTOCOL_V250            0x01
//class requests
#define  SET_LINE_CODING             0x20  
#define  GET_LINE_CODING             0x21  
#define  SET_CONTROL_LINE_STATE      0x22  
// many more

#endif  