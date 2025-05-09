.org 0x0000
STARTPOINT:
rjmp Initial

.org 0x0120;Skip Interrupt


Initial:
clr r1
out sreg, r1 ;Clear Sreg

ldi r28, 0xff
ldi r29, 0x41
out SPH, r29
out SPL, r28

ldi r17, 0x02 
ldi r26, 0x00 
ldi r27, 0x02 ;X = 512
ldi r30, 0x78 
ldi r31, 0x07 ;Z = 1912 ????
ldi r16, 0x00
out RAMPZ, r16; Clear Z high2bits

;Block as memcpy(512++, 1912++, 0)
rjmp if_X_EQ_512
LOOP_X_EQ_512:
elpm r0, Z+
st X+, r0
if_X_EQ_512:
cpi r26, 0x00
cpc r27, r17
brbc 1, LOOP_X_EQ_512;while x != 512

;[512:518] == 0x00
ldi r18, 0x02
ldi r26, 0x00
ldi r27, 0x02;X = 512
rjmp if_X_EQ_518
LOOP_X_EQ_518:
st X+, r1
if_X_EQ_518:
cpi r26, 0x06
cpc r27, r18
brbc 1, LOOP_X_EQ_518

rcall main

rjmp ENDPOINT
rjmp STARTPOINT

main:
push r28
push r29
in r28, SPL
in r29, SPH
rcall GPIO_Init
rcall CLK_init
;rcall SPI_IO_Init
rcall RF_Init

SPI_Enable:
    ldi r18, 0x4C
    ldi r19, 0x00;
    movw r30, r18
    ld r18, Z; r18 = SPCR
    ori r18, 0x40;
    st Z, r18;SPCR.SPE = 1

Intps_Init:

    ldi r24, 0x3c
    ldi r25, 0x00
    ldi r18, 0xff
    movw r30, r24
    st Z, r18   ;EIFR = 0xFF

    ldi r18, 0x3d
    ldi r19, 0x00
    movw r30, r18
    ld r18, Z
    ori r18, 0x01
    st Z, r18   ;EIMSK.INT0 = 1

    ldi r18, 0x69
    ldi r19, 0x00
    movw r30, r18
    ld r18, Z
    ori r18, 0x03
    st Z, r18   ;EICRA.[ICS00:ICS01] = 1
    bset 7;Enable Global Interrupt

Wait_SPIF:
    in r18, SPSR
    sbrs r18, 7
    rjmp Wait_SPIF

    in r24, SPDR
    sts 0x0203, r24
    lds r24, 0x0203
    andi r24, 0xc0
    sts 0x0202, r24

    ldi r25, 0x00
    cpi r24, 0x40
    cpc r25, r1;
    ;spi data == 0x40
    brbs 1, Subroutine_TX_Frame

    ;else
    out sreg, r1
    sbiw r24, 0x00
    ;spi data == 0x20
    brbs 1, Subroutine_RX_Frame

    out sreg,r1
    cpi r24, 0x80
    cpc r25, r1;
    brbs 1, Subroutine_Rd_Reg

    out sreg,r1
    cpi r24, 0xC0
    cpc r25, r1;
    brbs 1, Subroutine_Wr_Reg

    rjmp Wait_SPIF

Subroutine_Rd_Reg:
    lds r24, 0x0203
    andi r24, 0x3f
    ldi r25, 0x00
    sts 0x0201, r25
    sts 0x0200, r24
    subi r24, 0xc0
    sbci r25, 0xfe
    sts 0x0201, r25
    sts 0x0200, r24
    rcall SPI_TX_Reg
    rjmp Wait_SPIF

Subroutine_Wr_Reg:
    lds r24, 0x0203
    andi r24, 0x3f
    ldi r25, 0x00
    sts 0x0201, r25
    sts 0x0200, r24
    subi r24, 0xc0
    sbci r25, 0xfe
    sts 0x0201, r25
    sts 0x0200, r24
    rcall SPI_RX_Reg
    rjmp Wait_SPIF

Subroutine_TX_Frame:
    rcall SPI_RX_Frame
    rjmp Wait_SPIF

Subroutine_RX_Frame:
    rcall SPI_TX_Frame
    rjmp Wait_SPIF
    


