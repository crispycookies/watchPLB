//
//  plb.c
//  
//
//  Created by Olia Sviridova on 30.09.18.
//

#include <string.h>
#include <math.h>
#include "plb.h"
#include "BitArray.h"

#define LENSYNC           24
#define LENPDF1           61
#define LENPDF1_WITH_BCH1 82
#define LENPDF2           26
#define LENPDF2_WITH_BCH2 38
#define LENALL            (LENSYNC + LENPDF1_WITH_BCH1 + LENPDF2_WITH_BCH2)

#define MIN_DIV           4

static uint16_t const sync_bit = 0b111111111111111; //from 1-15
static uint16_t const frame_sync = 0b000101111; //from 16-24
//PDF1
static uint8_t  const format_flag = 0b1; //25 -> begin pdf1
static uint8_t  const protocol_flag = 0b1; //26
static uint16_t const country_code = 0b0011001011; //from 27-36; for Austria 203 in dec
static uint8_t  const test_protocol = 0b111; //from 37-39; type of protocol
static uint8_t  const typeplb = 0b110; //from 40-42; type->plb
static uint8_t  const certif = 0b1; //43; Cospas-Sarsat Certif
static uint32_t const serial_number = 0b11010000111011100101; //from 44-63; random
static uint16_t const national_use = 0b0000000000; //from 64-73, if national not used all 10 bits should be '0'
static uint16_t const certif_number = 0b1111111111; //from 74-83, random
static uint8_t  const radiolocating = 0b01; //from 84-85; 121,5MHz
//BCH polynoms were generated with matlab function bchgenpoly
//static int const bch1_poly = 0b1001101101100111100011; //22 with matlab function bchgenpoly(127,106)
static uint8_t  bch1_poly[22] = {1,0,0,1,1,0,1,1,0,1,1,0,0,1,1,1,1,0,0,0,1,1};
static uint8_t bch2_poly[13] = {1,0,1,0,1,0,0,1,1,1,0,0,1};
//static uint16_t const bch2_poly = 0b1010100111001; //13 with matlab function bchgenpoly(63,51)
//PDF2
static uint8_t const position_data_source = 0b1; //107; GPS internal

/**
 * @brief Encode data using bch algorithm
 *
 * @param data   data array (size: n)
 * @param g_poly generator polynom array (size: n - k + 1)
 * @param n      data array length (length of output)
 * @param k      payload length (length of input)
 */
static void bch_encode(uint8_t* data, uint8_t* g_poly, uint16_t n, uint16_t k);

uint16_t PLB_CreateFrame(uint8_t *frame, uint8_t len, POS_Position* pos) {
    if (frame == 0 || len == 0 || len < LENALL 
            || pos == 0 || pos->valid == POS_Valid_Flag_Invalid) {
        return 0;
    }
    
    //protocol
    BitArray_t startBits;
    BITARRAY_Init(&startBits, frame, LENSYNC);
    BITARRAY_AddBits(&startBits, sync_bit, 15);
    BITARRAY_AddBits(&startBits, frame_sync, 9);

    LOG("[PLB] Protocol sync bits: ");
    LOG_BITARRAY(frame, LENSYNC);
    
    //add bits into array for pdf1 and calculate the bch_code for pdf1
    uint8_t *pdf1 = frame+LENSYNC;
    BitArray_t data1;
    BITARRAY_Init(&data1, pdf1, LENPDF1);
    BITARRAY_AddBits(&data1, format_flag, 1);
    BITARRAY_AddBits(&data1, protocol_flag, 1);
    BITARRAY_AddBits(&data1, country_code, 10);
    BITARRAY_AddBits(&data1, test_protocol, 3);
    BITARRAY_AddBits(&data1, typeplb, 3);
    BITARRAY_AddBits(&data1, certif, 1);
    BITARRAY_AddBits(&data1, serial_number, 20);
    BITARRAY_AddBits(&data1, national_use, 10);
    BITARRAY_AddBits(&data1, certif_number, 10);
    BITARRAY_AddBits(&data1, radiolocating, 2);

    LOG("[PLB] PDF1: ");
    LOG_BITARRAY(pdf1, LENPDF1);
    
    bch_encode(pdf1, bch1_poly, LENPDF1_WITH_BCH1, LENPDF1);

    LOG("[PLB] PDF1 + BCH: ");
    LOG_BITARRAY(pdf1, LENPDF1_WITH_BCH1);
    
    //add bits into array for pdf2 and calculate the bch_code for pdf2
    uint8_t *pdf2 = frame+LENSYNC+LENPDF1_WITH_BCH1;
    BitArray_t data2;
    BITARRAY_Init(&data2, pdf2, LENPDF2);
    BITARRAY_AddBits(&data2, position_data_source, 1);
    BITARRAY_AddBits(&data2, pos->latitude.direction, 1);
    BITARRAY_AddBits(&data2, pos->latitude.degree, 7);
    BITARRAY_AddBits(&data2, pos->latitude.minute/MIN_DIV, 4);
    BITARRAY_AddBits(&data2, pos->longitude.direction, 1);
    BITARRAY_AddBits(&data2, pos->longitude.degree, 8);
    BITARRAY_AddBits(&data2, pos->longitude.minute/MIN_DIV, 4);

    LOG("[PLB] PDF2: ");
    LOG_BITARRAY(pdf2, LENPDF2);
    
    bch_encode(pdf2, bch2_poly, LENPDF2_WITH_BCH2, LENPDF2);

    LOG("[PLB] PDF2 + BCH: ");
    LOG_BITARRAY(pdf2, LENPDF2_WITH_BCH2);

    return LENALL;
}

static void bch_encode(uint8_t* data, uint8_t* g_poly, uint16_t n, uint16_t k){
    uint8_t feedback;
    
    for (uint16_t i = 0; i < n - k; i++)
        data[i + k] = 0;
    
    for (int16_t i = k - 1; i >= 0; i--) {
        feedback = data[i] ^ data[n - 1];
        if (feedback != 0) {
            for (uint16_t j = n - k - 1; j > 0; j--)
                if (g_poly[j] != 0)
                    data[j + k] = data[j + k - 1] ^ feedback;
                else
                    data[j + k] = data[j + k - 1];
            data[k] = g_poly[0] && feedback;
        } else {
            for (uint16_t j = n - k - 1; j > 0; j--)
                data[j + k] = data[j + k - 1];
            data[k] = 0;
        }
    }
}
