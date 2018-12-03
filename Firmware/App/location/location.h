#ifndef LOCATION_H
#define LOCATION_H

#include "position.h"

void LOC_Init();

void LOC_Process();

uint8_t LOC_PositionAvailable();

POS_Position* LOC_GetLastPosition();

void LOC_InjectPosition(POS_Position* pos);

#endif //!LOCATION_H