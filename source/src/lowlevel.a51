;*******************************************
    NAME    lowlevel
;*******************************************
; uint32_t htonl(uint32_t val)
?PR?_htonl?lowlevel SEGMENT CODE 
    PUBLIC  _htonl
if(__KEIL__)
    $REGUSE _htonl(R4-R7)
endif        
    RSEG    ?PR?_htonl?lowlevel
_htonl:        
    xch     a,R7   ;R7<>R4
    xch     a,R4
    xch     a,R7

    xch     a,R6   ;R6<>R5
    xch     a,R5
    xch     a,R6    
    ret

;********************************************
;void fastcpy (const uint8_t *src,        //R1R2R3
;                    uint8_t xdata *desst,//r4r5 
;                    uint8_t size)        //?_fastccpy8?BYTE+05H  
;src and dest swapped
?PR?_fastcpy8?lowlevel  SEGMENT CODE
?DT?_fastcpy8?lowlevel  SEGMENT DATA
    PUBLIC  _fastcpy8
    PUBLIC  ?_fastcpy8?BYTE

if(__KEIL__)
   $REGUSE _fastcpy8(R7,A,DPTR)
   sfr XBUS_AUX = 0xA2; 
   _XDAT_   EQU 0x01
   _offs_   EQU 0x05
else
   XBUS_AUX EQU 0xA2
   DPL      EQU 0x82
   DPH      EQU 0x83
   _XDAT_   EQU 0x02
   _offs_   EQU 0x00   
endif

;param seg
    RSEG  ?DT?_fastcpy8?lowlevel
?_fastcpy8?BYTE:
    DS  6               ; 3B *src, 2B *dest, 1B size

; code seg
    RSEG   ?PR?_fastcpy8?lowlevel

cloop:
    CLR  A
    MOVC A,@A+DPTR      ; read source
    INC  DPTR           ; autoinc is not supported for MOVC
    DB   0A5H           ; MOVX @DPTR1,A & INC DPTR1
    DJNZ R7,cloop
    POP  XBUS_AUX       ; rewstore DPS settings 
    RET

_fastcpy8:
    PUSH XBUS_AUX       ; save XBUS_AUX
    MOV  XBUS_AUX,#0x01 ; select DPTR1, no auto inc
    MOV  DPH,R4         ; DPTR1 is  
    MOV  DPL,R5         ;   destination
          
    DEC  XBUS_AUX       ; select DPTR0
    MOV  DPH, R2
    MOV  DPL, R1        ; is source

    mov  A,?_fastcpy8?BYTE+_offs_; check len
    JZ   ?C0002         ; nothing todo 
    MOV  R7,A           ; loopcounter    

    CJNE R3, #_XDAT_, cloop;  
    
    MOV  XBUS_AUX,#0x04 ; switch on autoinc
xloop:
    MOVX A,@DPTR        ; read source
    DB   0A5H           ; MOVX @DPTR1,A & INC DPTR1
    DJNZ R7,xloop

?C0002:
    POP  XBUS_AUX       ; restore DPS settings 
    RET
END
