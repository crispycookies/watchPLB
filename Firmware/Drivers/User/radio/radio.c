#include "radio.h"
#include "string.h"

#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))

#define SPI_TIMEOUT   100

#define TX_BUF_SIZE   2
#define TX_BUF_ADDR   0
#define TX_BUF_DATA   1

#define SPI_READ      0
#define SPI_WRITE     (1 << 7)

#define ADDR_FIFOCTRL 4
#define ADDR_FIFODATA 5

#define STATE_S0_FIFOSTAT0  (1 << 7)
#define STATE_S1_FIFOSTAT1  (1 << 6)
#define STATE_S2_FIFO_EMPTY (1 << 5)
#define STATE_S3_FIFO_FULL  (1 << 4)
#define STATE_S4_FIFO_UNDER (1 << 3)
#define STATE_S5_FIFO_OVER  (1 << 2)
#define STATE_S6_PLL_LOCK   (1 << 1)

#define IQ_1                (0x369U)
#define IQ_0                (0x259U)

static void SetReg(RADIO_Instance *inst, uint8_t addr, uint8_t data);

void RADIO_Init(RADIO_Instance *inst, SPI_Init_Struct *spi) {
    if (inst != 0 && spi != 0) {
        inst->spi = spi;
        inst->idx = 0;
        inst->len = 0;
        inst->state = RADIO_STATE_CONFIGURE;
    }
}

void RADIO_Process(RADIO_Instance *inst) {
    if (inst != 0) {
        switch (inst->state)
        {
            case RADIO_STATE_CONFIGURE:
                //configure radio module
                break;

            case RADIO_STATE_SYNC:
                //set preamble sync message
                break;

            case RADIO_STATE_FRAME:
                //send modulated frame
                break;
        
            default:
                break;
        }
    }
}

void RADIO_SetFrame(RADIO_Instance *inst, uint8_t *data, uint16_t len) {
    if (inst != 0 && inst->state == RADIO_STATE_IDLE) {
        if (data == 0 || len == 0 || len > RADIO_FRAME_LENGTH) {
            inst->state = RADIO_STATE_IDLE;
        } else {
            memcpy(inst->frame, data, len);
            inst->state = RADIO_STATE_SYNC;
        }
    }
}

RADIO_State RADIO_GetState(RADIO_Instance *inst) {
    if (inst != 0) {
        return inst->state;
    }

    return 0;
}

//TODO: Integrate in process
static void plbTransmit(RADIO_Instance *inst, uint16_t data) {
    SetReg(inst, ADDR_FIFOCTRL, (data >> 8) & 0x03);
    
    //maybe wait 1ms??
    
    SetReg(inst, ADDR_FIFODATA, data & 0xFF);
}

static void SetReg(RADIO_Instance *inst, uint8_t addr, uint8_t data) {
    //chip select -> 0
    SPI_CS_Enable(inst->spi);
    uint8_t txBuf[TX_BUF_SIZE];
    txBuf[TX_BUF_ADDR] = SPI_WRITE | (addr & 0x7F);
    txBuf[TX_BUF_DATA] = data;
    SPI_SendData(inst->spi, txBuf, TX_BUF_SIZE, SPI_TIMEOUT);
    //chip select -> 1
}
