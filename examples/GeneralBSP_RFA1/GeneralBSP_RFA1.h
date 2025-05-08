#ifndef _RFA1_GENE

#include "platform.h"
#include <stdlib.h>
#include <string.h>
#include "platform_SPI.h"
//#include <stdint.h>

#define SLPTR_HIGH //GPIOB.ODR|=0X20
#define SLPTR_LOW //GPIOB.ODR&=0xDF

uint8_t RFA1_SetRadioRegister(uint8_t addr, uint8_t data);
uint8_t RFA1_GetRadioRegister(uint8_t addr, uint8_t* data);
uint8_t RFA1_ReadRadioInterruptStatus(void);

uint8_t RFA1_InterruptOnINTIN(void);

uint8_t RFA1_Init_Radio(void);
uint8_t RFA1_ClearTXEndInterrupt(void);
uint8_t RFA1_MaskTXEndInterrupt(void);

uint8_t RFA1_TXFrame_Blocking(uint8_t* data, uint8_t len);
uint8_t RFA1_RXFrame_Blocking(uint8_t* data, uint8_t* len);
uint8_t RFA1_GenerateSLPTR(void);

uint8_t RFA1_TagPANID(uint8_t ID_High, uint8_t ID_Low);
uint8_t RFA1_TagChannel(uint8_t channel);
uint8_t RFA1_TRXCMD(uint8_t cmd);

#endif