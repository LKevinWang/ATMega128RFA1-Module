SPI_TX_Reg:
    push r28
    push r29
    push r1
    in r28, SPL
    in r29, SPH
    lds r24, 0x0200
    lds r25, 0x0201
    cpi r24, 0x4f
    ldi r18, 0x01
    cpc r25, r18
    brbc 1, ReadInterrupt
    ;else
    ldi r24, 0x4e
    ldi r25, 0x00
    lds r18, 0x0200
    lds r19, 0x0201
    movw r30, r18
    ld r18, Z
    movw r30, r24
    st Z, r18
    rjmp Wait_RdEnd
    

    ReadInterrupt:
    ldi r24, 0x4e
    ldi r25, 0x00
    lds r18, 0x0205
    movw r30, r24
    st Z, r18
    in r24, SREG
    std Y+1, r24;PUSH SREG
    bclr 7
    sts 0x0205, r1
    ldi r24, 0x5F
    ldi r25, 0x00
    movw Z,r24
    ldd r18, Y+1
    out SREG, r18
    
    Wait_RdEnd:
    in r24 SPSR
    and r24, r24
    and r24, Wait_RdEnd
    ld r24, SPDR
    pop r0
    pop r29
    pop r28
    ret


SPI_RX_Reg:
    push r28
    push r29
    push r1
    Wait_WrEnd:
    in r28, SPL
    in r29, SPH
    in r24, SPSR
    and r24, r24
    brbc 4, Wait_WrEnd
    in r24, SPDR
    sts 0x0203, r24

    lds r24, 0x0200
    lds r25, 0x0201
    cpi r24, 0x42
    ldi r31, 0x01
    cpc r25, r31
    ;if writing 142
    brbc 1, WriteStatus

    lds r24, 0x0203
    andi r24, 0x1f
    std Y+1, r24
    ldd r24, Y+1
    cpi r24, 0x16
  

    WriteStatus: