/**
 * @file emergencyCall.c
 * @author Paul Götzinger
 * @brief Emergency call module
 * @version 1.0
 * @date 2019-02-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "emergencyCall.h"
#include "spi_driver.h"
#include "location.h"
#include "plb.h"
#include "radio.h"
#include <string.h>

#define FRAME_SIZE 144

#define MSG_INTERVAL 0

static EMC_State emergencyState;
static uint8_t dataFrame[FRAME_SIZE];
static uint16_t frameLength;

static SPI_Init_Struct spi;
static POS_Time lastPosUpdate;
static RADIO_Instance radio;
static uint32_t lastMsgSent;

void EMC_Init(void) {
    //init spi for radio module
    SPI_GPIO_Pair miso;
    miso.bank = GPIOC;
    miso.pin = GPIO_PIN_2;

    SPI_GPIO_Pair mosi;
    mosi.bank = GPIOC;
    mosi.pin = GPIO_PIN_3;
    
    SPI_GPIO_Pair slclk;
    slclk.bank = GPIOB;
    slclk.pin = GPIO_PIN_10;
    
    SPI_GPIO_Pair cs;
    cs.bank = GPIOC;
    cs.pin = GPIO_PIN_1;
    
    spi.SPI.Instance = SPI2;
    spi.SPI.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    spi.SPI.Init.Mode = SPI_MODE_MASTER;
    spi.SPI.Init.NSS = SPI_NSS_SOFT;
    spi.SPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi.SPI.Init.CRCPolynomial = 7;
    spi.SPI.Init.TIMode = SPI_TIMODE_DISABLE;
    spi.SPI.Init.Direction = SPI_DIRECTION_2LINES;
    spi.SPI.Init.DataSize = SPI_DATASIZE_8BIT;
    spi.SPI.Init.FirstBit = SPI_FIRSTBIT_MSB;
    
    spi.SPI.Init.CLKPolarity = SPI_POLARITY_LOW;
    spi.SPI.Init.CLKPhase = SPI_PHASE_1EDGE;
    
    spi.CS = cs;
    spi.MISO = miso;
    spi.MOSI = mosi;
    spi.SCLK = slclk;
    
    SPI_Init(&spi);

    //init radio with spi
    RADIO_Init(&radio, &spi);

    memset(&lastPosUpdate, 0, sizeof(POS_Time));
    emergencyState = EMC_State_Idle;
    frameLength = 0;
    lastMsgSent = 0;
}

void EMC_Process(void) {

    if (emergencyState == EMC_State_Emergency) {
        //if next message should be sent        
        if (RADIO_GetState(&radio) == RADIO_STATE_IDLE && HAL_GetTick() > lastMsgSent) {
            POS_Position* locPos = LOC_GetLastPosition();

            if (locPos != 0 && locPos->valid == POS_Valid_Flag_Valid
                    && POS_CmpTime(&locPos->time, &lastPosUpdate) > 0) {
                memcpy(&lastPosUpdate, &locPos->time, sizeof(POS_Time));

                LOG("[EMC] Found new Position:\n");
                LOG_POS(locPos);

                frameLength = PLB_CreateFrame(dataFrame, FRAME_SIZE, locPos);

                LOG("[EMC] Frame: \n");
                LOG_BITARRAY(dataFrame, frameLength);
            }
            
            if (frameLength != 0) {
                LOG("[EMC] Start Frame\n");

                RADIO_SetFrame(&radio, dataFrame, frameLength);
                lastMsgSent = HAL_GetTick() + MSG_INTERVAL;
            }
        }

        //Process radio
        RADIO_Process(&radio);
    }
}

void EMC_SetEmergency(EMC_State emc) {
    emergencyState = emc;
}
