#ifndef RADIO_H
#define RADIO_H

#include "spi_driver.h"

#define RADIO_FRAME_LENGTH 256

typedef enum {
    RADIO_STATE_IDLE = 0,
    RADIO_STATE_CONFIGURE,
    RADIO_STATE_SYNC,
    RADIO_STATE_FRAME
} RADIO_State;

typedef struct {
    SPI_Init_Struct* spi;
    uint8_t frame[RADIO_FRAME_LENGTH];
    uint16_t len;
    uint16_t idx;
    RADIO_State state;
    
} RADIO_Instance;

void        RADIO_Init(RADIO_Instance *inst, SPI_Init_Struct *spi);
void        RADIO_Process(RADIO_Instance *inst);
void        RADIO_SetFrame(RADIO_Instance *inst, uint8_t *data, uint16_t len);
RADIO_State RADIO_GetState(RADIO_Instance *inst);


#endif //RADIO_H