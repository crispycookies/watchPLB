/**
 * @file radio.h
 * @author Paul Götzinger
 * @brief Radio driver using spi
 * @version 1.0
 * @date 2019-02-11
 */

#ifndef RADIO_H
#define RADIO_H

#include "spi_driver.h"

#define RADIO_FRAME_LENGTH 256

/**
 * @brief Radio state enum
 * 
 */
typedef enum {
    RADIO_STATE_IDLE = 0,
    RADIO_STATE_CONFIGURE,
    RADIO_STATE_WAIT_CONF,
    RADIO_STATE_START_TX,
    RADIO_STATE_WAIT_TX,
    RADIO_STATE_WAIT_AR,
    RADIO_STATE_PREAMBLE,
    RADIO_STATE_FRAME,
    RADIO_STATE_POSTAMBLE
} RADIO_State;

/**
 * @brief Radio instance structure
 * 
 */
typedef struct {
    SPI_Init_Struct* spi;
    uint8_t frame[RADIO_FRAME_LENGTH];
    RADIO_State state;
    uint16_t len;
    uint32_t idx;
    uint32_t nextAR;
    
} RADIO_Instance;

/**
 * @brief Initializes \ref RADIO_Instance with a \ref SPI_Init_Struct
 * 
 * @param inst Radio instance to initialize
 * @param spi SPI instance to use
 */
void        RADIO_Init(RADIO_Instance *inst, SPI_Init_Struct *spi);

/**
 * @brief Process radio
 * 
 * @param inst radio instance
 */
void        RADIO_Process(RADIO_Instance *inst);

/**
 * @brief Sets a data frame to send
 * 
 * @param inst radio instance
 * @param data pointer to data
 * @param len length of data
 */
void        RADIO_SetFrame(RADIO_Instance *inst, uint8_t *data, uint16_t len);

/**
 * @brief Retrieve current state
 * 
 * @param inst radio instance
 * @return RADIO_State current state
 */
RADIO_State RADIO_GetState(RADIO_Instance *inst);


#endif //RADIO_H