;*** SDCC asm version 
;    translate with:
;    sdas8051 -losg lowlevel.as    
;***
.module fastcopy

.area RSEG    (ABS,DATA)
XBUS_AUX = 0xa2

;*******************************************************
;void fastcpy8 ( const uint8_t *src,   // DPL DPH B 
;                      uint8_t *dest   // param 2
;                      uint8_t size)   // param 3
;*******************************************************
.globl _fastcpy8_PARM_2
.globl _fastcpy8_PARM_3
.globl _fastcpy8

.area OSEG
_fastcpy8_PARM_2:
       .ds 3           ;storage dest
_fastcpy8_PARM_3:
       .ds 1           ;storage len

.area CSEG    (CODE)

_fastcpy8: 
       mov    A,_fastcpy8_PARM_3
       jnz    00000$                  ; nothing todo -> exit
       ret
00000$:
       mov    R7,A                    ; Param 2 is loop counter
       push   XBUS_AUX                ; save aux reg
       mov    XBUS_AUX,#0x01          ; select DPTR1 no autoinc
       mov    DPL,_fastcpy8_PARM_2+0  ; and load &dest to DPTR1
       mov    DPH,_fastcpy8_PARM_2+1  ;
       dec    XBUS_AUX                ; DPTR0 = *src
       mov    A,B                     ; 0x00 -> XDATA 
       JNZ    00002$
       mov    XBUS_AUX,#0x04          ; select DPTR0 + autoinc
0001$:                                ; xmem loop 
       movx   A,@DPTR                 ; read source (xmem)
       .db    0xA5                    ; MOVX @DPTR1,A & INC DPTR1
       djnz   R7,0001$
       pop     XBUS_AUX
       ret

0002$:                                ; cmem loop 
       clr    A
       movc   A,@A+DPTR               ; read source (codemem)
       inc    DPTR                    ; src ++ no autoinc 
       .db    0xA5                    ; MOVX @DPTR1,A & INC DPTR1
       djnz   R7,0002$
       pop     XBUS_AUX
       ret

