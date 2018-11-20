#include "emergencyCall.h"
#include "spi_driver.h"
#include "location.h"
#include "plb.h"

#define SPI_TIMEOUT   100

#define TX_BUF_SIZE   2
#define TX_BUF_ADDR   0
#define TX_BUF_DATA   1

#define SPI_READ      0
#define SPI_WRITE     (1 << 7)

#define ADDR_FIFOCTRL 4
#define ADDR_FIFODATA 5

//plb module

static uint8_t emergencyState;
static SPI_Init_Struct spi;
static PLB_Instance plb; 

static void plbTransmit(uint16_t data);

static void SetReg(uint8_t addr, uint8_t data);

void EMC_Init(void) {
    
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
    cs.bank = GPIOC;
    cs.pin = GPIO_PIN_1;
    
    spi.SPI.Instance = SPI1;
    spi.SPI.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    spi.SPI.Init.Mode = SPI_MODE_MASTER;
    spi.SPI.Init.NSS = SPI_NSS_SOFT;
    spi.SPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi.SPI.Init.CRCPolynomial = 7;
    spi.SPI.Init.TIMode = SPI_TIMODE_DISABLE;
    spi.SPI.Init.Direction = SPI_DIRECTION_2LINES;
    spi.SPI.Init.DataSize = SPI_DATASIZE_8BIT;
    spi.SPI.Init.FirstBit = SPI_FIRSTBIT_LSB;
    
    spi.SPI.Init.CLKPolarity = SPI_POLARITY_LOW;
    spi.SPI.Init.CLKPhase = SPI_PHASE_1EDGE;
    
    //not defined
    //spi.SPI.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    //spi.SPI.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    
    spi.CS = cs;
    spi.MISO = miso;
    spi.MOSI = mosi;
    spi.SCLK = slclk;
    
    SPI_Init(&spi);

    //init plb with spi
    PLB_Init(&plb, plbTransmit);

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

static void plbTransmit(uint16_t data) {
    SetReg(ADDR_FIFOCTRL, (data >> 8) & 0x03);
    
    //maybe wait 1ms??
    
    SetReg(ADDR_FIFODATA, data & 0xFF);
}

static void SetReg(uint8_t addr, uint8_t data) {
    //chip select -> 0
    SPI_CS_Enable(&spi);
    uint8_t txBuf[TX_BUF_SIZE];
    txBuf[TX_BUF_ADDR] = SPI_WRITE | (addr & 0x7F);
    txBuf[TX_BUF_DATA] = data;
    SPI_SendData(&spi, txBuf, TX_BUF_SIZE, SPI_TIMEOUT);
    //chip select -> 1
}
