/**
 * @file location.h
 * @author Paul Götzinger
 * @brief Location module
 * @version 1.0
 * @date 2019-02-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef LOCATION_H
#define LOCATION_H

#include "position.h"

/**
 * @brief Location initialization
 * 
 */
void LOC_Init();

/**
 * @brief location processing
 * 
 */
void LOC_Process();

/**
 * @brief Returns if a position is available
 * 
 * @return uint8_t '1' when available, '0' when unavailable
 */
uint8_t LOC_PositionAvailable();

/**
 * @brief Retrieve last position
 * 
 * @return POS_Position* Pointer to last position
 */
POS_Position* LOC_GetLastPosition();

/**
 * @brief Allows injecting a position (mainly for test purposes)
 * 
 * @param pos position to inject
 */
void LOC_InjectPosition(POS_Position* pos);

#endif //!LOCATION_H