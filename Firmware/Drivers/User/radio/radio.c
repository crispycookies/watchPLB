#include "radio.h"
#include "string.h"

#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))

#define SPI_TIMEOUT   100

#define TX_BUF_SIZE   2
#define TX_BUF_ADDR   0
#define TX_BUF_DATA   1

#define MAX_ADDR      0x50

//RW Flag
#define SPI_READ      0
#define SPI_WRITE     (1 << 7)

//Adresses
#define ADDR_REVISION     0x00
#define ADDR_SCRATCH      0x01
#define ADDR_PWRMODE      0x02
#define ADDR_XTALOSC      0x03
#define ADDR_FIFOCTRL     0x04
#define ADDR_FIFODATA     0x05
#define ADDR_IRQMASK      0x06

#define ADDR_PINCFG1      0x0C
#define ADDR_PINCFG2      0x0D
#define ADDR_PINCFG3      0x0E
  
#define ADDR_MODULATION   0x10
#define ADDR_ENCODING     0x11
#define ADDR_FRAMING      0x12
  
#define ADDR_FREQB3       0x1C
#define ADDR_FREQB2       0x1D
#define ADDR_FREQB1       0x1E
#define ADDR_FREQB0       0x1F
#define ADDR_FREQ3        0x20
#define ADDR_FREQ2        0x21
#define ADDR_FREQ1        0x22
#define ADDR_FREQ0        0x23
#define ADDR_FSKDEV2      0x25
#define ADDR_FSKDEV1      0x26
#define ADDR_FSKDEV0      0x27
#define ADDR_PLLLOOP      0x2C
#define ADDR_PLLRANGING   0x2D
  
#define ADDR_TXPWR        0x30
#define ADDR_TXRATEHI     0x31
#define ADDR_TXRATEMID    0x32
#define ADDR_TXRATELO     0x33

//Configuration Values
#define CONF_XTALOSC      0x18
#define CONF_MODULATION   0x06
#define CONF_ENCODING     0x00
#define CONF_FRAMING      0x00
#define CONF_FREQ3        0x19
#define CONF_FREQ2        0x60
#define CONF_FREQ1        0xC8
#define CONF_FREQ0        0xB5
#define CONF_TXPWR        0x0f
#define CONF_TXRATEHI     0x01
#define CONF_TXRATEMID    0x99
#define CONF_TXRATELO     0x9a
#define CONF_PLLRANGING   0x18
#define CONF_PLLLOOP      0x29
#define CONF_FSKDEV2      0x00  //should be 0?!
#define CONF_FSKDEV1      0x00
#define CONF_FSKDEV0      0x00

#define MASK_PLLRANGING_START 0x10
#define MASK_PLLRANGING_ERROR 0x20

//States
#define STATE_S0_FIFOSTAT0  (1 << 0)
#define STATE_S1_FIFOSTAT1  (1 << 1)
#define STATE_S2_FIFO_EMPTY (1 << 2)
#define STATE_S3_FIFO_FULL  (1 << 3)
#define STATE_S4_FIFO_UNDER (1 << 4)
#define STATE_S5_FIFO_OVER  (1 << 5)
#define STATE_S6_PLL_LOCK   (1 << 6)

//Power modes
#define PWRMODE_STANDBY 0x05
#define PWRMODE_SYNTHTX 0x0C
#define PWRMODE_FULLTX  0x0D

//Modulation values
#define IQ_1                (0x369U)
#define IQ_0                (0x097U)

//Return values
#define TX_OK  0x1
#define TX_NOK 0x0

#define PREAMBLE_MSG 0x55

#define CONFIGURATION_DELAY 5
#define STARTUP_DELAY       1
#define PREAMBLE_DURATION 160
#define AR_INTERVAL       (5*60*1000)

#define BITSYNCPATTERN_LEN 16
#define FRAMESYNCPATTERN_LEN 9

static const uint8_t bitsyncPattern[BITSYNCPATTERN_LEN]     = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static const uint8_t framesyncPattern[FRAMESYNCPATTERN_LEN] = {0,0,0,1,0,1,1,1,1};

static uint8_t Transmit10(RADIO_Instance *inst, uint8_t data);
static uint8_t SetReg(RADIO_Instance *inst, uint8_t addr, uint8_t data);
static uint8_t GetReg(RADIO_Instance *inst, uint8_t addr, uint8_t *data);
static void DumpRegister(RADIO_Instance *inst);

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
                SetReg(inst, ADDR_PWRMODE, PWRMODE_STANDBY);
                inst->idx = HAL_GetTick() + CONFIGURATION_DELAY;
                SetReg(inst, ADDR_XTALOSC, CONF_XTALOSC);
                SetReg(inst, ADDR_PLLLOOP, CONF_PLLLOOP);
                SetReg(inst, ADDR_FREQ3, CONF_FREQ3);
                SetReg(inst, ADDR_FREQ2, CONF_FREQ2);
                SetReg(inst, ADDR_FREQ1, CONF_FREQ1);
                SetReg(inst, ADDR_FREQ0, CONF_FREQ0);
                SetReg(inst, ADDR_TXPWR, CONF_TXPWR);
                SetReg(inst, ADDR_FSKDEV2, CONF_FSKDEV2);
                SetReg(inst, ADDR_FSKDEV1, CONF_FSKDEV1);
                SetReg(inst, ADDR_FSKDEV0, CONF_FSKDEV0);        
                SetReg(inst, ADDR_TXRATEHI, CONF_TXRATEHI);
                SetReg(inst, ADDR_TXRATEMID, CONF_TXRATEMID);
                SetReg(inst, ADDR_TXRATELO, CONF_TXRATELO);
                SetReg(inst, ADDR_MODULATION, CONF_MODULATION);
                SetReg(inst, ADDR_ENCODING, CONF_ENCODING);
                SetReg(inst, ADDR_FRAMING, CONF_FRAMING);
                inst->nextAR = 0;

                DumpRegister(inst);

                LOG("[RADIO] Configuration complete\n");
                //LOG("[RADIO] Change State: RADIO_STATE_CONFIGURE -> RADIO_STATE_WAIT_CONF\n");
                inst->state = RADIO_STATE_WAIT_CONF;
                break;
            case RADIO_STATE_WAIT_CONF:
                if (HAL_GetTick() > inst->idx) {
                    //LOG("[RADIO] Change State: RADIO_STATE_WAIT_CONF -> RADIO_STATE_IDLE\n");
                    inst->state = RADIO_STATE_IDLE;
                }
                break;
            case RADIO_STATE_START_TX:
                //LOG("[RADIO] Change State: RADIO_STATE_START_TX -> RADIO_STATE_WAIT_TX\n");
                SetReg(inst, ADDR_PWRMODE, PWRMODE_SYNTHTX);
                inst->idx = HAL_GetTick() + STARTUP_DELAY;
                inst->state = RADIO_STATE_WAIT_TX;
                break;
            case RADIO_STATE_WAIT_TX:
                if (HAL_GetTick() > inst->idx) {
                    if (HAL_GetTick() > inst->nextAR) {
                        //LOG("[RADIO] Change State: RADIO_STATE_WAIT_TX -> RADIO_STATE_PREAMBLE\n");
                        SetReg(inst, ADDR_PWRMODE, PWRMODE_FULLTX);
                        inst->idx   = HAL_GetTick() + PREAMBLE_DURATION;
                        inst->state = RADIO_STATE_PREAMBLE;
                    } else {
                        //LOG("[RADIO] Change State: RADIO_STATE_WAIT_TX -> RADIO_STATE_WAIT_AR\n");
                        SetReg(inst, ADDR_PLLRANGING, CONF_PLLRANGING);
                        inst->state = RADIO_STATE_WAIT_AR;
                    }
                }
                break;
            case RADIO_STATE_WAIT_AR:
                {
                    uint8_t reg;
                    GetReg(inst, ADDR_PLLRANGING, &reg);
                    if (reg & MASK_PLLRANGING_ERROR) {
                        LOG("[RADIO] PLL Ranging failed! Restart Configuration\n");
                        //LOG("[RADIO] Change State: RADIO_STATE_WAIT_AR -> RADIO_STATE_CONFIGURE\n");
                        inst->state = RADIO_STATE_CONFIGURE;
                    } else if ((reg & MASK_PLLRANGING_START) == 0) {
                        //LOG("[RADIO] Change State: RADIO_STATE_WAIT_AR -> RADIO_STATE_PREAMBLE\n");
                        SetReg(inst, ADDR_PWRMODE, PWRMODE_FULLTX);
                        inst->state  = RADIO_STATE_PREAMBLE;
                        inst->nextAR = HAL_GetTick() + AR_INTERVAL;
                        inst->idx    = HAL_GetTick() + PREAMBLE_DURATION;
                    }
                    break;
                }
            case RADIO_STATE_PREAMBLE:
                //set preamble sync message
                if (HAL_GetTick() < inst->idx) {
                    //SetReg(inst, ADDR_FIFODATA, PREAMBLE_MSG); 
                } else {
                    inst->idx = 0;
                    inst->state = RADIO_STATE_BITSYNC; 
                    //LOG("[RADIO] Change State: RADIO_STATE_PREAMBLE -> RADIO_STATE_BITSYNC\n");
                }
                break;
            case RADIO_STATE_BITSYNC:
                if (inst->idx < BITSYNCPATTERN_LEN) {
                    while (inst->idx < BITSYNCPATTERN_LEN && Transmit10(inst, bitsyncPattern[inst->idx])) {
                        inst->idx++;
                    }
                } else {
                    //LOG("[RADIO] Change State: RADIO_STATE_BITSYNC -> RADIO_STATE_FRAMESYNC\n");
                    inst->idx = 0;
                    inst->state = RADIO_STATE_FRAMESYNC; 
                }
                break;
            case RADIO_STATE_FRAMESYNC:
                if (inst->idx < FRAMESYNCPATTERN_LEN) {
                    while (inst->idx < FRAMESYNCPATTERN_LEN && Transmit10(inst, framesyncPattern[inst->idx])) {
                        inst->idx++;
                    }
                } else {
                    //LOG("[RADIO] Change State: RADIO_STATE_FRAMESYNC -> RADIO_STATE_FRAME\n");
                    inst->idx = 0;
                    inst->state = RADIO_STATE_FRAME; 
                }
                break;
            case RADIO_STATE_FRAME:
                if (inst->idx >= inst->len) {
                    inst->state = RADIO_STATE_POSTAMBLE;
                    inst->idx = 0;
                    //LOG("[RADIO] Change State: RADIO_STATE_FRAME -> RADIO_STATE_POSTAMBLE\n");
                } else {
                    //send modulated frame
                    while (inst->idx < inst->len && Transmit10(inst, inst->frame[inst->idx])) {
                        inst->idx++;
                    }
                }                
                break;
            case RADIO_STATE_POSTAMBLE:
                Transmit10(inst, 0);
                if (inst->idx == 0) {
                    inst->idx++;
                } else {
                    SetReg(inst, ADDR_PWRMODE, PWRMODE_STANDBY);
                    inst->state = RADIO_STATE_IDLE;
                    inst->idx = 0;
                    //LOG("[RADIO] Change State: RADIO_STATE_POSTAMBLE -> RADIO_STATE_IDLE\n");
                }
                break;
            default:
                break;
        }
    }
}

void RADIO_SetFrame(RADIO_Instance *inst, uint8_t *data, uint16_t len) {
    if (inst != 0 && inst->state == RADIO_STATE_IDLE 
            && data != 0 && len != 0 && len < RADIO_FRAME_LENGTH) {
        LOG("[RADIO] New Frame\n");
        memcpy(inst->frame, data, len);
        inst->len = len;
        inst->idx = 0;
        inst->state = RADIO_STATE_START_TX;
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

    //LOG("[RADIO] TX: %3x\n", tx);

    uint8_t ret = SetReg(inst, ADDR_FIFOCTRL, tx >> 8);

    if ((ret  & STATE_S3_FIFO_FULL) == 0) {
        SetReg(inst, ADDR_FIFODATA, tx & 0xFF);
    }
    
    return (ret & STATE_S3_FIFO_FULL) == 0;
}

static uint8_t SetReg(RADIO_Instance *inst, uint8_t addr, uint8_t data) {
    uint8_t status;

    //chip select -> 0
    SPI_CS_Enable(inst->spi);
    addr = SPI_WRITE | (addr & 0x7F);
    SPI_WriteRead(inst->spi, addr, &status, SPI_TIMEOUT);

    //LOG("[RADIO] SetReg 0x%x = 0x%x => 0x%x\n", addr, data, status);

    uint8_t tmp;
    SPI_WriteRead(inst->spi, data, &tmp, SPI_TIMEOUT);
    
    //chip select -> 1
    SPI_CS_Disable(inst->spi);

    return status;
}

static uint8_t GetReg(RADIO_Instance *inst, uint8_t addr, uint8_t *data) {
    uint8_t status;

    //chip select -> 0
    SPI_CS_Enable(inst->spi);
    addr = SPI_READ | (addr & 0x7F);
    SPI_WriteRead(inst->spi, addr, &status, SPI_TIMEOUT);

    //LOG("[RADIO] SetReg 0x%x = 0x%x => 0x%x\n", addr, data, status);

    SPI_WriteRead(inst->spi, 0xff, data, SPI_TIMEOUT);
    
    //chip select -> 1
    SPI_CS_Disable(inst->spi);

    return status;
}

static void DumpRegister(RADIO_Instance *inst) {
    LOG("\n[RADIO] --- Radio memory dump ---\n");
    for (uint8_t i = 0; i <= MAX_ADDR; i++) {
        uint8_t reg = 0xff;
        GetReg(inst, i, &reg);
        LOG("[RADIO] 0x%2.2x: 0x%2.2x\n", i, reg);
    }
    LOG(  "[RADIO] -------------------------\n\n");
}
