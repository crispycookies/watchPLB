/**
 * @file BitArray.h
 * @author Paul Götzinger
 * @brief Creates an array of bits
 * @version 1.0
 * @date 2019-02-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BITARRY_H
#define BITARRY_H

typedef struct {
    uint8_t* data;
    uint16_t len;
    uint16_t idx;
} BitArray_t;

/**
 * @brief Init bitarray
 *
 * @param bitArr bitarray container to init
 * @param arr    array to use
 * @param len      size of array
 */
void BITARRAY_Init(BitArray_t* bitArr, uint8_t* arr, uint16_t len);

/**
 * @brief Add bit to bitarray
 *
 * @param bitArr bitarray container
 * @param bit    bit to add
 */
void BITARRAY_AddBit(BitArray_t* bitArr, uint8_t bit);

/**
 * @brief Add bits to bitarray
 *
 * @param bitArr bitarray container
 * @param bits   variable containing bits
 * @param cnt    count of bits to add
 */
void BITARRAY_AddBits(BitArray_t* bitArr, uint16_t bits, uint8_t cnt);

#endif //!BITARRY_H