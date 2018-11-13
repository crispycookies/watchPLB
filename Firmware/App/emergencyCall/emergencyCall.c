#include "emergencyCall.h"
#include "spi_driver.h"
#include "location.h"
#include "plb.h"

#define SPI_TIMEOUT 100

//plb module

static uint8_t emergencyState;
static SPI_Init_Struct spi;
static PLB_Instance plb; 

static void send(uint8_t *data, uint16_t length);

void EMC_Init(void) {
    SPI_Init_Struct spi_for_send;
    
    SPI_GPIO_Pair miso;
    miso.bank = GPIOA;
    miso.pin = GPIO_PIN_6;
    SPI_GPIO_Pair mosi;
    mosi.bank = GPIOA;
    mosi.pin = GPIO_PIN_7;
    SPI_GPIO_Pair slclk;
    slclk.bank = GPIOA;
    slclk.pin = GPIO_PIN_5;
    SPI_GPIO_Pair cs;
    cs.bank = GPIOA;
    cs.pin = GPIO_PIN_4;
    
    SPI_HandleTypeDef spidef;
    spidef.Instance = SPI1;
    spidef.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    spidef.Init.Mode = SPI_MODE_MASTER;
    spidef.Init.NSS = SPI_NSS_SOFT;
    spidef.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spidef.Init.CRCPolynomial = 7;
    spidef.Init.TIMode = SPI_TIMODE_DISABLE;
    spidef.Init.Direction = SPI_DIRECTION_2LINES;
    spidef.Init.DataSize = SPI_DATASIZE_8BIT;
    spidef.Init.FirstBit = SPI_FIRSTBIT_LSB;
    
    spidef.Init.CLKPolarity = SPI_POLARITY_LOW;
    spidef.Init.CLKPhase = SPI_PHASE_1EDGE;
    
    //not defined
    //spidef.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    //spidef.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    
    spi_for_send.CS = cs;
    spi_for_send.MISO = miso;
    spi_for_send.MOSI = mosi;
    spi_for_send.SCLK = slclk;
    spi_for_send.SPI = &spidef;
    
    SPI_Init(&spi_for_send);

    //init plb with spi
    PLB_Init(&plb, send);

    emergencyState = 0;
}

void EMC_Process(void) {

    if (emergencyState != 0) {
        POS_Position* locPos = LOC_GetLastPosition();
        POS_Position* plbPos = PLB_GetPosition(&plb);
        
        if (locPos != 0 && plbPos != 0 && locPos->valid != 0 && 
                (plbPos->valid == 0 || POS_CmpTime(&locPos->time, &plbPos->time) > 0)) {
            PLB_SetPosition(&plb, locPos);
        }

        PLB_Process(&plb);
    } 
}

void EMC_SetEmergency(uint8_t emc) {
    emergencyState = emc;
}

static void send(uint8_t *data, uint16_t length) {
    SPI_SendData(&spi, data, length, SPI_TIMEOUT);
}
