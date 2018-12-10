#include "BitArray.h"
#include <stdlib.h>

/**
 * @brief Init bitarray
 *
 * @param bitArr bitarray container to init
 * @param arr    array to use
 * @param len      size of array
 */
void BITARRAY_Init(BitArray_t* bitArr, uint8_t* arr, uint16_t len){
    if (bitArr != NULL && arr != NULL) {
        bitArr->data = arr;
        bitArr->len = len;
        bitArr->idx = 0;
    }
}

/**
 * @brief Add bit to bitarray
 *
 * @param bitArr bitarray container
 * @param bit    bit to add
 */
void BITARRAY_AddBit(BitArray_t* bitArr, uint8_t bit) {
    if (bitArr != NULL && bitArr->idx < bitArr->len) {
        bitArr->data[bitArr->idx++] = bit & 1;
    }
}

/**
 * @brief Add bits to bitarray
 *
 * @param bitArr bitarray container
 * @param bits   variable containing bits
 * @param cnt    count of bits to add
 */
void BITARRAY_AddBits(BitArray_t* bitArr, uint16_t bits, uint8_t cnt){
    if (bitArr != NULL && bitArr->idx < bitArr->len) {
        for (uint8_t i = 1; i <= cnt; i++) {
        	BITARRAY_AddBit(bitArr, bits >> (cnt - i));
        }
    }
}
