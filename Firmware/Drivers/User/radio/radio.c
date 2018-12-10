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
#define IQ_0                (0x97U)

#define TX_OK  0x1
#define TX_NOK 0x0

static uint8_t Transmit10(RADIO_Instance *inst, uint8_t data);
static uint8_t SetReg(RADIO_Instance *inst, uint8_t addr, uint8_t data);

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
                LOG("[RADIO] Change State: RADIO_STATE_CONFIGURE -> RADIO_STATE_IDLE\n");
                inst->state = RADIO_STATE_IDLE; //temp state skip
                break;

            case RADIO_STATE_SYNC:
                //set preamble sync message

                //temp state skip
                inst->idx = 0;
                inst->state = RADIO_STATE_FRAME; 
                LOG("[RADIO] Change State: RADIO_STATE_SYNC -> RADIO_STATE_FRAME\n");
                break;

            case RADIO_STATE_FRAME:
                if (inst->idx >= inst->len) {
                    inst->state = RADIO_STATE_IDLE;
                    LOG("[RADIO] Change State: RADIO_STATE_FRAME -> RADIO_STATE_IDLE\n");
                } else {
                    //send modulated frame
                    while (inst->idx < inst->len && Transmit10(inst, inst->frame[inst->idx])) {
                        inst->idx++;
                    }
                }                
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

static uint8_t Transmit10(RADIO_Instance *inst, uint8_t data) {
    uint16_t tx = (data == 0) ? IQ_0 : IQ_1;

    LOG("[RADIO] TX: %3x\n", tx);

    uint8_t ret = SetReg(inst, ADDR_FIFOCTRL, tx >> 8);

    if (ret == TX_OK) {
        SetReg(inst, ADDR_FIFODATA, tx & 0xFF);
    }
    
    return ret;
}

static uint8_t SetReg(RADIO_Instance *inst, uint8_t addr, uint8_t data) {
    uint8_t rec;
    uint8_t ret = TX_NOK;

    //chip select -> 0
    SPI_CS_Enable(inst->spi);
    addr = SPI_WRITE | (addr & 0x7F);
    SPI_SendData(inst->spi, &addr, 1, SPI_TIMEOUT);

    SPI_ReadData(inst->spi, &rec, 1, SPI_TIMEOUT);

    if ((rec & STATE_S3_FIFO_FULL) == 0) {
        SPI_SendData(inst->spi, &data, 1, SPI_TIMEOUT);
        ret = TX_OK;
    }
    
    //chip select -> 1
    SPI_CS_Disable(inst->spi);

    return ret;
}
