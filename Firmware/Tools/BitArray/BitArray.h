#ifndef BITARRY_H
#define BITARRY_H

typedef struct {
    uint8_t* data;
    uint16_t len;
    uint16_t idx;
} BitArray_t;


void BITARRAY_Init(BitArray_t* bitArr, uint8_t* arr, uint16_t len);
void BITARRAY_AddBit(BitArray_t* bitArr, uint8_t bit);
void BITARRAY_AddBits(BitArray_t* bitArr, uint16_t bits, uint8_t cnt);

#endif //!BITARRY_H