#ifndef IQM_H
#define IQM_H

//#include "libmf.h"
//
// Frequency, Frame and Packet structure
//
//

#define AX5031_PWRMODE_POWERDOWN   0x0
#define AX5031_PWRMODE_STANDBY     0x5
#define AX5031_PWRMODE_SYNTHTX     0xc
#define AX5031_PWRMODE_FULLTX      0xd

#define AX5031_MODULATION          0x06
#define AX5031_XTALOSC             0x18
#define AX5031_ENCODING            0x00
#define AX5031_FRAMING             0x00

#define AX5031_FREQ1               0xcb
#define AX5031_FREQ0               0xcb
#define AX5031_FREQ                AX5031_FREQ0+AX5031_FREQ1

//TX
#define AX5031_TXPWR               0x0f
#define AX5031_TXRATEHI            0x01
#define AX5031_TXBITRATEMID        0x99
#define AX5031_TXRATELO            0x9a

//PLL
#define AX5031_PLLRANGING
#define AX5031_PLLLOOP
#define AX5031_BANDSEL             0x01
#define AX5031_PLLCPI              0x02
#define AX5031_FLT                 0x01
#define AX5031_FREQSEL             0x00

#define AX5031_FIFOCTRL            0x02

#define AX5031_FIFODATA [7:0] //data information


// Packet constants

#define TX_PREAMBLE      32  //0010000
#define ADDRESS_LENGTH   6
#define PAYLOAD_LENGTH   24
#define CRC_LENGTH       2
#define TX_POSTAMBLE     6


// TX parameter
#define PREAMBLE_BYTE      0x55 //010101




#endif   /* IQM_H */

