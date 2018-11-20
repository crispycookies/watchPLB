//
//  plb.h
//  
//
//  Created by Olia Sviridova on 30.09.18.
//

#ifndef PLB_H
#define PLB_H

#include "position.h"

#define LEN_PROTOCOL      144

typedef void (*PLB_Transmit)(uint16_t data);

typedef struct {
    PLB_Transmit transmit;
    uint8_t data[LEN_PROTOCOL];
    POS_Position pos;
} PLB_Instance;

void PLB_Init(PLB_Instance* plb, PLB_Transmit transmit);
void PLB_Process(PLB_Instance* plb);

POS_Position* PLB_GetPosition(PLB_Instance* plb);
void PLB_SetPosition(PLB_Instance* plb, POS_Position* pos);

#endif /* PLB_H */
