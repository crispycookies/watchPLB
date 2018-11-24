//
//  plb.h
//  
//
//  Created by Olia Sviridova on 30.09.18.
//

#ifndef PLB_H
#define PLB_H

#include "position.h"

uint16_t PLB_CreateFrame(uint8_t *frame, uint8_t len, POS_Position* pos);

#endif /* PLB_H */
