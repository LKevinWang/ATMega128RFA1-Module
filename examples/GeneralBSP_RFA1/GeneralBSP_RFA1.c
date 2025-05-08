#include "GeneralBSP_RFA1.h"

#define SPI_RF_IDLE 0
#define SPI_RF_BLOCKING 1

uint8_t SPI_RF_Handler_state = 0;

uint8_t globalPANID[2] = {0x01, 0x00};
const uint8_t selfShortAddress[2] = {0x0D, 0xB5};
const uint8_t dstShortAddress[2] = {0x0C, 0xB5};

//------Frame Control Field defined in IEEE 802.15.4 Frame Format - MAC-Layer Frame Structure (MPDU) 
uint8_t frameType[2] = {0x21, 0x18};

uint8_t TX_SerialNum = 0;

uint8_t channelSet = 0;
uint8_t rx_interruptPended;


uint8_t RFA1_SetRadioRegister(uint8_t addr, uint8_t data){
    uint8_t spiTxBuf[2] = {addr+0xC0, data};
    if (addr> 0x40){
        return 0;
    }else if(addr == 0x02){
        SPI_RF_TXBytes(spiTXBuf, 2);
    }
}

uint8_t RFA1_GetRadioRegister(uint8_t addr, uint8_t* data){
    if (addr> 0x40){
        return 0;
    }
    if(addr == 0x0F){
        //Tag to ReadRadioInterruptStatus;
        return 2;
    }
    uint8_t spiTxBuf[2] = {addr+0x80, 0};
    uint8_t spiRXBuf[2];
    SPI_RF_TRXBytes(spiTXBuf, spiRXBuf, 2);
    *data = spiRXBuf[1];
    return 1;
}

uint8_t RFA1_ReadRadioInterruptStatus(void){
    uint8_t spiTxBuf[2] = {0x0F+0x80, 0};
    uint8_t spiRXBuf[2];
    SPI_RF_TRXBytes(spiTXBuf, spiRXBuf, 2);
    return spiRXBuf[1];
}

uint8_t RFA1_InterruptOnINTIN(void){
    while(SPI_RF_Handler_state != SPI_RF_IDLE){
        ;
    }
    SPI_RF_Handler_state = SPI_RF_BLOCKING;
    uint8_t intp_flags = RFA1_ReadRadioInterruptStatus();
    SPI_RF_Handler_state = SPI_RF_IDLE;
    return intp_flags;
}

uint8_t RFA1_Init_Radio(void){
    
    uint8_t trx_status = 0;
    uint8_t rcvd_reg_val;
    while( (trx_status&0x1F)!= 0x08){
        RFA1_GetRadioRegister(0x01,&trx_status);
    }

    RFA1_SetRadioRegister(0x05, 0xC0);
    RFA1_GetRadioRegister(0x05,&rcvd_reg_val);
    RFA1_SetRadioRegister(0x15, 0x01);
    RFA1_GetRadioRegister(0x15,&rcvd_reg_val);
    RFA1_SetRadioRegister(0X0C, 0x80);
    RFA1_GetRadioRegister(0x0C,&rcvd_reg_val);
    RFA1_SetRadioRegister(0X17, 0x30);
    RFA1_GetRadioRegister(0x17,&rcvd_reg_val);
    RFA1_SetRadioRegister(0X04, 0x20);
    RFA1_SetRadioRegister(0X0E, 0x00);
    RFA1_SetRadioRegister(0X0C, 0x83);
    RFA1_SetRadioRegister(0X2F, 0x41);
    RFA1_SetRadioRegister(0X2E, 0x82);
    RFA1_SetRadioRegister(0X2C, 0x0E);
    RFA1_SetRadioRegister(0X17, 0x34);
    RFA1_SetRadioRegister(0X04, 0x20);
    RFA1_SetRadioRegister(0X08, 0x6B);
    RFA1_SetRadioRegister(0X09, 0xFF);
    RFA1_SetRadioRegister(0X17, 0x04);
    RFA1_SetRadioRegister(0X17, 0x04);
    RFA1_SetRadioRegister(0X17, 0x04);
    RFA1_SetRadioRegister(0X02, 0x09);

    while( (trx_status&0x1F)!= 0x09){
        RFA1_GetRadioRegister(0x01,&trx_status);
    }
    RFA1_SetRadioRegister(0X0E, 0x00);
    RFA1_SetRadioRegister(0X0E, 0x10);

    //----------------------PAN ID, Initialized as AAAA-------------------
    RFA1_SetRadioRegister(0X22, 0xAA);
    RFA1_SetRadioRegister(0X23, 0xAA);
    //----------------------Self short address----------------------------
    RFA1_SetRadioRegister(0X21, selfShortAddress[1]);
    RFA1_SetRadioRegister(0X20, selfShortAddress[0]);

    RFA1_GetRadioRegister(0x21,&trx_status);
    RFA1_GetRadioRegister(0x20,&trx_status);

    while( (trx_status&0x1F)!= 0x09){
        RFA1_GetRadioRegister(0x01,&trx_status);
    }
    RFA1_SetRadioRegister(0X0E, 0x00);
    RFA1_SetRadioRegister(0X02, 0x06);

    //----------------------------Find ED Level lowest channel--------------------
    float channelEDLevel[16] = {0x00};
    uint8_t currentChnEDLevelMax = 0;
    RFA1_SetRadioRegister(0X07, 0x00);
    delay_ms(1);
    RFA1_GetRadioRegister(0X07,&trx_status);
    delay_ms(1);
    RFA1_SetRadioRegister(0X07, 0x00);
    delay_ms(1);
    channelSet = 0;
    for(uint8_t i=0;i<16;i++){
        float sum_edlev = 0; 
        for(uint8_t j=0;j<255;j++){
            uint8_t temp;
            
            RFA1_GetRadioRegister(0X07,&temp);
            delay_ms(1);
            RFA1_SetRadioRegister(0X07,0X00);
//            if(temp >= currentChnEDLevelMax){
//                currentChnEDLevelMax = temp;
//            }
            sum_edlev+=temp;
            temp = 0;
            delay_ms(5);
        }
        channelEDLevel[i] = sum_edlev/255.0;//currentChnEDLevelMax;
        currentChnEDLevelMax = 0;
        sum_edlev = 0;
        
        RFA1_SetRadioRegister(0X07, 0x00);
        RFA1_SetRadioRegister(0X08, 0x6B+i);
        trx_status = 0;
        while( (trx_status&0x1F)!= 11+i){
            RFA1_GetRadioRegister(0x08,&trx_status);
        }
        trx_status = 0;
        RFA1_SetRadioRegister(0X02, 0x06);
        while( (trx_status&0x1F)!= 0x06){
            RFA1_GetRadioRegister(0x01,&trx_status);
        }

    }
    float temp = 255.0*255.0;
    uint8_t minChannelI = 0;
    for(uint8_t i=0;i<16;i++){
        if(channelEDLevel[i] < temp){
            minChannelI = i;
            temp = channelEDLevel[i];
        }
    }
    channelSet = minChannelI;
    RFA1_SetRadioRegister(0X07, 0x00);
    //RFA1_SetRadioRegister(0X08, 0x60+channelSet+11);
    RFA1_SetRadioRegister(0X08, 0x60+channelSet+11);
    RFA1_SetRadioRegister(0X0E, 0x48);

    return 1;
}

uint8_t RFA1_ClearTXEndInterrupt(void){
    while(SPI_RF_Handler_state != SPI_RF_IDLE){
        ;
    }
    SPI_RF_Handler_state = SPI_RF_BLOCKING;
    uint8_t spiTxBuf[2] = {0x02+0xC0, 0x19};

    SPI_RF_TXBytes(spiTXBuf, 2);

}
uint8_t RFA1_MaskTXEndInterrupt(void){
    while(SPI_RF_Handler_state != SPI_RF_IDLE){
        ;
    }
    SPI_RF_Handler_state = SPI_RF_BLOCKING;
    uint8_t spiTxBuf[2] = {0x02+0xC0, 0x16};

    SPI_RF_TXBytes(spiTXBuf, 2);
}

uint8_t RFA1_TXFrame_Blocking(uint8_t* data, uint8_t len, uint8_t askForACK){
    //This configuration can only transmit frames with dst short address and PANID. 
    //But you can edit this by changing MAC Control field. Attention to cofigure frameType corresponded.

    while(SPI_RF_Handler_state != SPI_RF_IDLE){
        ;
    }
    SPI_RF_Handler_state = SPI_RF_BLOCKING;
    //------------------TX Command-----------------------
    SPI_RF_TX_Byte_CSHold(0x40);

    //------------------TX Length------------------------
    SPI_RF_TX_Byte_CSHold(len+9);//9 should be attributed when frame control field(frameType) is changed
    delay_us(20);

    //------------------TX MAC Control field-------------------------------
    SPI_RF_TX_Byte_CSHold(frameType[0]);

    SPI_RF_TX_Byte_CSHold(frameType[1]);

    SPI_RF_TX_Byte_CSHold(TX_SerialNum);

    SPI_RF_TX_Byte_CSHold(globalPANID[0]);

    SPI_RF_TX_Byte_CSHold(globalPANID[1]);

    SPI_RF_TX_Byte_CSHold(dstShortAddress[0]);

    SPI_RF_TX_Byte_CSHold(dstShortAddress[1]);

    //------------------TX Frame Stuff---------------------
    SPI_RF_TXBytes(data, len);

    SPI_RF_Handler_state = SPI_RF_IDLE;
    RFA1_GenerateSLPTR();

    return 1;
}

uint8_t RFA1_RXFrame_Blocking(uint8_t* data, uint8_t* len){
    while(SPI_RF_Handler_state != SPI_RF_IDLE){
        ;
    }
    SPI_RF_Handler_state = SPI_RF_BLOCKING;
    SPI_RF_Handler_state = SPI_RF_BLOCKING;
    SPI_RF_TX_Byte_CSHold(0x20);//8'b 01000000
    // while(!SPI_RF_TXBuf.drdy){
    //     ;
    // }
    delay_us(20);
    uint8_t lenRcved = SPI_RF_RX_Byte_CSHold(0x00);
    delay_us(20);
    uint8_t txbuf[128] = {0};
    
    SPI_RF_TRXBytes(txbuf, data, lenRcved+1);

    data[0] = data[1];
    SPI_RF_Handler_state = SPI_RF_IDLE;
    *len = lenRcved;
    return 1;
}

uint8_t RFA1_GenerateSLPTR(void){
    SLPTR_HIGH;
    delay_us(50);
    SLPTR_LOW;
    return 1;
}

uint8_t RFA1_TagPANID(uint8_t ID_High, uint8_t ID_Low){
    globalPANID[0] = ID_High;
    globalPANID[1] = ID_Low;

    RFA1_SetRadioRegister(0X22, globalPANID[0]);
    RFA1_SetRadioRegister(0X23, globalPANID[1]);
    return 1;
}

uint8_t RFA1_TagChannel(uint8_t channel){
    if(channel < 11 || channel > 26){
        return 0;
    }
    channelSet = channel-11;
    uint8_t trx_status = 0;
    
    RFA1_SetRadioRegister(0x02,0x08);
    while( (trx_status&0x1F)!= 0x08){
        RFA1_GetRadioRegister(0x01,&trx_status);
    }
    RFA1_SetRadioRegister(0X0E, 0x00);
    while( (trx_status&0x1F)!= 0x00){
        RFA1_GetRadioRegister(0x0E,&trx_status);
    }
    RFA1_SetRadioRegister(0X07, 0x00);
    RFA1_SetRadioRegister(0X08, 0x60+channel);
    RFA1_SetRadioRegister(0X0E, 0x48);
    return 1;    
}

uint8_t RFA1_TRXCMD(uint8_t cmd){
    uint8_t trx_status = 0;
    RFA1_SetRadioRegister(0x02, cmd);
    uint8_t set_cnt = 0;
    while((trx_status&0x1F)!=cmd){
        RFA1_GetRadioRegister(0x01,&trx_status);
        set_cnt++;
        if(set_cnt>100){
            return 0;
        }
    }
    return 1;
}


//---------------User Editable----------------------
void template_RFInterruptISR(){
    rx_interruptPended = 1;
}

void* PulseInterruptISR() = &template_RFInterruptISR;
//---------------User Editable END------------------


//Followed 2 funcs Could be directly executed after RFA1_Radio_Init and TRXCMD(0x08)
void example_TX(){
    uint8_t trx_status = 0;
    uint8_t interruptName = 0x00;
    uint8_t RFtxBuf[130];
    RFtxBuf[0] = 1;
    RFtxBuf[1] = 1;
    RFtxBuf[2] = 4;
    RFtxBuf[3] = 5;
    RFtxBuf[4] = 1;
    RFtxBuf[4] = 4;
    RFA1_TRXCMD(0x09);
    //RFA1_TRXCMD(0x19);//Decomment this if you want to use TX ARET mode to accept ACK
    trx_status = 0;
    RFA1_TXFrame_Blocking(RFtxBuf, 6);
    RFA1_GenerateSLPTR();

    interruptName = 0;
    while(!rx_interruptPended);
    interruptName = RFA1_ReadRadioInterruptStatus();
    rx_interruptPended = 0;
    RFA1_TRXCMD(0x09);
    /*
    //Decomment this if you want to use TX ARET mode to accept ACK
    RFA1_GetRadioRegister(0x02,&trx_status);
    if((trx_status&0xE0) == 0xA0){
        //ACK timeout
        ;
    }
    */


}

void example_RX_Block(){
    while(!rx_interruptPended);
    interruptName = RFA1_ReadRadioInterruptStatus();
    rx_interruptPended = 0;
    uint8_t RFrxBuf[130];//[0] and [1] is length
    if(interruptName&0x08){
        trx_status = 0x56;            
        interruptName = 0;
//                trx_status  = 0;
//                for(uint8_t i = 0;i<16;i++)
//                    RFA1_GetRadioRegister(0x02,&trx_status);
        trx_status = 0x00;
        RFA1_RXFrame_Blocking(RFrxBuf+1, RFrxBuf);

        //This Section is Necessary------------------------
        RFA1_SetRadioRegister(0x0C, 0x03);
        while(trx_status!=0x03)
             RFA1_GetRadioRegister(0x0C,&trx_status);
        trx_status = 0x00;
        RFA1_SetRadioRegister(0x0C, 0x83);
        //This Section is Necessary------------------------
    }else{
        return;
    }
}

