/**
 * @file plb.h
 * @author Paul Götzinger, Olia Sviridova
 * @brief PLB frame creation
 * @version 1.0
 * @date 2019-02-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef PLB_H
#define PLB_H

#include "position.h"

/**
 * @brief Create PLB frame
 * 
 * @param frame pointer to memory
 * @param len length of available memory
 * @param pos position
 * @return uint16_t length of frame
 */
uint16_t PLB_CreateFrame(uint8_t *frame, uint8_t len, POS_Position* pos);

#endif /* PLB_H */
