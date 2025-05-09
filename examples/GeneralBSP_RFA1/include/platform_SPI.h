#ifndef platform_SPI

#include "platform.h"

//------------------USER Editable----------------------------

#define CS_LOW //
#define CS_HIGH //

void SPI_RF_Init();//You must redefine this, as we need a SPI Master port to control RFA1.
//On hardware we have a match between RFA1 pins and SPI pin:
/*
    RFA1 PIN    User MCU Pin
    PB1            MISO
    PB2            MOSI
    PB0            CS
    PB1            SCK
    PD0            OUTPUT_SLPTR
    PD1            INPUT_INTERRUPT
*/

//------------------USER Editable END------------------------

void SPI_RF_TXBytes(uint8_t *buf, uint8_t len);
void SPI_RF_TRXBytes(uint8_t *tbuf, uint8_t *rbuf,uint8_t len);
void SPI_RF_TX_Byte_CSHold(uint8_t byte);
uint8_t SPI_RF_RX_Byte_CSHold(uint8_t txbyte);
void SPI_RF_TX_Byte(uint8_t byte);
uint8_t SPI_RF_RX_Byte(uint8_t txbyte);

//--------------------USER Should code a corresponded C file to define these Functions
//SPI Frequency lower than 4MHz
//Byte interpolation must longer than 20us
//CPOL = 0 (IDLE Low)
//CPHA = 0 (Rising edge data sample)
//CS should be soft controlled

#endif