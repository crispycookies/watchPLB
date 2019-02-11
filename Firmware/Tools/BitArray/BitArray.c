/**
 * @file BitArray.c
 * @author Paul Götzinger
 * @brief Creates an array of bits
 * @version 1.0
 * @date 2019-02-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "BitArray.h"
#include <stdlib.h>

void BITARRAY_Init(BitArray_t* bitArr, uint8_t* arr, uint16_t len){
    if (bitArr != NULL && arr != NULL) {
        bitArr->data = arr;
        bitArr->len = len;
        bitArr->idx = 0;
    }
}

void BITARRAY_AddBit(BitArray_t* bitArr, uint8_t bit) {
    if (bitArr != NULL && bitArr->idx < bitArr->len) {
        bitArr->data[bitArr->idx++] = bit & 1;
    }
}

void BITARRAY_AddBits(BitArray_t* bitArr, uint16_t bits, uint8_t cnt){
    if (bitArr != NULL && bitArr->idx < bitArr->len) {
        for (uint8_t i = 1; i <= cnt; i++) {
        	BITARRAY_AddBit(bitArr, bits >> (cnt - i));
        }
    }
}
