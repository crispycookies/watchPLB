#ifndef RADIO_H
#define RADIO_H

#include "spi_driver.h"

#define RADIO_FRAME_LENGTH 256

typedef enum {
    RADIO_STATE_IDLE = 0,
    RADIO_STATE_CONFIGURE,
    RADIO_STATE_WAIT_CONF,
    RADIO_STATE_START_TX,
    RADIO_STATE_WAIT_TX,
    RADIO_STATE_WAIT_AR,
    RADIO_STATE_PREAMBLE,
    RADIO_STATE_BITSYNC,
    RADIO_STATE_FRAMESYNC,
    RADIO_STATE_FRAME,
    RADIO_STATE_POSTAMBLE
} RADIO_State;

typedef struct {
    SPI_Init_Struct* spi;
    uint8_t frame[RADIO_FRAME_LENGTH];
    RADIO_State state;
    uint16_t len;
    uint32_t idx;
    uint32_t nextAR;
    
} RADIO_Instance;

void        RADIO_Init(RADIO_Instance *inst, SPI_Init_Struct *spi);
void        RADIO_Process(RADIO_Instance *inst);
void        RADIO_SetFrame(RADIO_Instance *inst, uint8_t *data, uint16_t len);
RADIO_State RADIO_GetState(RADIO_Instance *inst);


#endif //RADIO_H