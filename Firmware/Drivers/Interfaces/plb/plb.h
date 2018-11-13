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

typedef void (*PLB_Send)(uint8_t *data, uint16_t len);

typedef struct {
    PLB_Send send;
    uint8_t data[LEN_PROTOCOL];
    POS_Position pos;
} PLB_Instance;

void PLB_Init(PLB_Instance* plb, PLB_Send send);
void PLB_Process(PLB_Instance* plb);

POS_Position* PLB_GetPosition(PLB_Instance* plb);
void PLB_SetPosition(PLB_Instance* plb, POS_Position* pos);

#endif /* PLB_H */
