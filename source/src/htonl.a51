; uint32_t htonl(uint32_t val)
?PR?_htonl?htonl SEGMENT CODE 
	PUBLIC	_htonl
    $REGUSE _htonl(R4-R7)
    RSEG    ?PR?_htonl?htonl
_htonl:        
    xch     a,R7   ;R7<>R4
    xch     a,R4
    xch     a,R7

    xch     a,R6   ;R6<>R5
    xch     a,R5
    xch     a,R6    
    ret
end