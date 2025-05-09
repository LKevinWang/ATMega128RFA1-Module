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
    
    brbc 1, NotWriteStatus
    ;if writing 142
    lds r24, 0x0203
    andi r24, 0x1f
    std Y+1, r24
    ldd r24, Y+1
    cpi r24, 0x16
    brbc 1, NotWrite0x16
    ;else if writing 0x16
    ldi r24, 0x4e
    ldi r25, 0x01
    ldi r18, 0x4e
    ldi r19, 0x01
    movw r30, r18
    ld r18, Z
    andi r18, 0xbf
    movw r30, r24
    st Z, r18
    rjmp NotWriteStatus
    
    NotWrite0x16:
        ldd r24, Y+1
        cpi r24, 0x19
        brbs 1, NotWrite0x19
        ldd r24, Y+1
        cpi r24, 0x09
        brbc 1, NotWriteStatus
        NotWrite0x19:
        ldi r24, 0x4f
        ldi r25, 0x01
        ldi r18, 0x4f
        ldi r19, 0x01
        movw r30, r18
        ld r18, Z
        ori r18, 0x40
        movw r30, r24
        st Z, r18
        ldi r24, 0x4e
        ldi r25, 0x01
        ldi r18, 0x4e
        ldi r19, 0x01
        movw r30, r18
        ld r18, Z
        ori r18, 0x40
        movw r30, r24
        st Z, r18
    NotWriteStatus:
    lds r24, 0x0200
    lds r25, 0x0201
    lds r18, 0x0203
    movw r30, r24
    st Z, r18
    pop r0
    pop r29
    pop r28
    ret

SPI_RX_Frame:
    push r28
    push r29
    in r28, SPL
    in r29, SPH
    std Y+1, r1
    Wait_SPICplt:
    in r0, 0x2d
    sbrs r0, 7
    rjmp Wait_SPICplt
    ldi r24, 0x4e
    ldi r25, 0x00
    movw r30, r24
    ld r24, Z
    std Y+2, r24
    ldi r24, 0x80
    ldi r25, 0x01
    ldd r18, Y+2
    movw r30, r24
    st Z, r18
    ldd r24, Y+2
    subi r24, 0x01
    std Y+2, r24
    ldi r24, 0x01
    std Y+1, r24
    rjmp l04
    Wait_SPICplt_2:
    in r0, 0x2d
    sbrs r0, 7
    rjmp Wait_SPICplt_2
    ldd r24, Y+1
    mov r24, r24
    ldi r25, 0x00
    subi r24, 0x80
    sbci r25, 0xfe
    ldi r18, 0x4e
    ldi r19, 0x00
    movw r30, r18
    ld r18, Z
    movw r30, r24
    st Z, r18
    ldd r24, Y+1
    subi r24, 0xff
    std Y+1, r24
    ldd r25, Y+1
    ldd r24, Y+2
    cp r25, r24;CNT > 1
    brbs 0, Wait_SPICplt_2
    pop r29
    pop r28
    ret

SPI_TX_Frame:
    push r16
    push r28
    push r29
    push r1
    in r28, SPL
    in r29, SPH
    ldi r24, 0x7b
    ldi r25, 0x01
    movw r30, r24
    ld r24, Z ;r24 = rlength
    sts 0x0204, r24
    ldi r24, 0x4e
    ldi r25, 0x00
    lds r18, 0x0204
    movw r30, r24
    st Z, r18
    lds r24, 0x0204
    mov r24, r24
    ldi r25, 0x00
    subi r24, 0x80
    sbci r25, 0xfe
    ldi r18, 0x47
    ldi r19, 0x01
    movw r30, r18
    ld r18, Z
    movw r30, r24
    st Z, r18
    lds r24, 0x0204
    subi r24, 0xff
    sts 0x0204, r24
    ldi r31, 0x01
    ldi r30, 0x80;Z = BST
    lds r17, 0x0204
    Wait_SPICplt_3:
    in r0, SPSR
    sbrs r0, 7
    rjmp Wait_SPICplt_3
    ld r16, Z+
    out SPDR, r16
    dec r17
    cp r17, r1
    brbc 1, Wait_SPICplt_3
    Wait_SPICplt_4:
    in r0, SPSR
    sbrs r0, 7
    rjmp Wait_SPICplt_4
    ldi r24, 0x4e
    ldi r25, 0x00
    movw r30, r24
    ld r24, Z
    std Y+1, r24
    pop r0
    pop r29
    pop r28
    pop r16
    ret

