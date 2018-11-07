//
//  plb.c
//  
//
//  Created by Olia Sviridova on 30.09.18.
//

#include <stdint.h>
#include <string.h>
#include <math.h>
#include "plb.h"
#include "spi_driver.h"
#include "gps.h"

/*
#include <stddef.h>
#include "stm32l0xx.h"
#include "stm32l0xx_hal_gpio.h"

#include "key.h"
*/

#define IQ_1             0xDA
#define IQ_0             0x26
#define LENPDF1           61
#define LENPDF1_WITH_BCH1 82
#define LENPDF2           26
#define LENPDF2_WITH_BCH2 38
#define LEN_PROTOCOL      144

typedef struct {
    uint8_t* data;
    uint16_t len;
    uint16_t idx;
} bitarray_t;

static void bitarray_init(bitarray_t* bitArr, uint8_t* arr, uint16_t len);
static void bitarray_addBit(bitarray_t* bitArr, uint8_t bit);
static void bitarray_addBits(bitarray_t* bitArr, uint16_t bits, uint8_t cnt);
static void bch_encode(uint8_t* data, uint8_t* g_poly, uint16_t n, uint16_t k);

static uint16_t const sync_bit = 0b111111111111111; //from 1-15
static uint16_t const frame_sync = 0b000101111; //from 16-24
//PDF1
static uint8_t const format_flag = 0b1; //25 -> begin pdf1
static uint8_t const protocol_flag = 0b1; //26
static uint16_t const country_code = 0b0011001011; //from 27-36; for Austria 203 in dec
static uint8_t const test_protocol = 0b111; //from 37-39; type of protocol
static uint8_t const typeplb = 0b110; //from 40-42; type->plb
static uint8_t const certif = 0b1; //43; Cospas-Sarsat Certif
static int const serial_number = 0b11010000111011100101; //from 44-63; random
static uint16_t const national_use = 0b0000000000; //from 64-73, if national not used all 10 bits should be '0'
static uint16_t const certif_number = 0b1111111111; //from 74-83, random
static uint8_t const radiolocating = 0b01; //from 84-85; 121,5MHz
//BCH polynoms were generated with matlab function bchgenpoly
//static int const bch1_poly = 0b1001101101100111100011; //22 with matlab function bchgenpoly(127,106)
static uint8_t  bch1_poly[22] = {1,0,0,1,1,0,1,1,0,1,1,0,0,1,1,1,1,0,0,0,1,1};
static uint8_t bch2_poly[13] = {1,0,1,0,1,0,0,1,1,1,0,0,1};
//static uint16_t const bch2_poly = 0b1010100111001; //13 with matlab function bchgenpoly(63,51)
//PDF2
static uint8_t const position_data_source = 0b1; //107; GPS internal
uint8_t latitude_flag = 0b0; // N = '1'; S = '0'; pos.108
uint8_t latitude_degrees = 0b0; //from 109-115
uint8_t latitude_minutes = 0b0; //from 116-119
uint8_t longtitude_flag = 0b0; // E = '1'; W = '0'; pos.120
uint8_t longtitude_degrees = 0b0; //from 121-128
uint8_t longtitude_minutes = 0b0; //from 129-132;


void plb_init(){
    SPI_Init_Struct spi_for_send;
    
    SPI_GPIO_Pair miso;
    miso.bank = GPIOA;
    miso.pin = GPIO_PIN_6;
    SPI_GPIO_Pair mosi;
    mosi.bank = GPIOA;
    mosi.pin = GPIO_PIN_7;
    SPI_GPIO_Pair slclk;
    slclk.bank = GPIOA;
    slclk.pin = GPIO_PIN_5;
    SPI_GPIO_Pair cs;
    cs.bank = GPIOA;
    cs.pin = GPIO_PIN_4;
    
    SPI_HandleTypeDef spidef;
    spidef.Instance = SPI1;
    spidef.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    spidef.Init.Mode = SPI_MODE_MASTER;
    spidef.Init.NSS = SPI_NSS_SOFT;
    spidef.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spidef.Init.CRCPolynomial = 7;
    spidef.Init.TIMode = SPI_TIMODE_DISABLE;
    spidef.Init.Direction = SPI_DIRECTION_2LINES;
    spidef.Init.DataSize = SPI_DATASIZE_8BIT;
    spidef.Init.FirstBit = SPI_FIRSTBIT_LSB;
    
    spidef.Init.CLKPolarity = SPI_POLARITY_LOW;
    spidef.Init.CLKPhase = SPI_PHASE_1EDGE;
    
    spidef.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    
    spidef.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    
    spi_for_send.CS = cs;
    spi_for_send.MISO = miso;
    spi_for_send.MOSI = mosi;
    spi_for_send.SCLK = slclk;
    spi_for_send.SPI = &spidef;
    
    SPI_Init(&spi_for_send);

}

void plb_protocol(){
    
    plb_init();
    
    //protocol
    uint8_t protocol_to_modulate[LEN_PROTOCOL];
    bitarray_t data_all;
    bitarray_init(&data_all, protocol_to_modulate, 24);
    bitarray_addBits(&data_all, sync_bit, 15);
    bitarray_addBits(&data_all, frame_sync, 9);
    
    
    //add bits into array for pdf1 and calculate the bch_code for pdf1
    uint8_t *pdf1 = protocol_to_modulate+24;
    bitarray_t data1;
    bitarray_init(&data1, pdf1, LENPDF1);
    bitarray_addBits(&data1, format_flag, 1);
    bitarray_addBits(&data1, protocol_flag, 1);
    bitarray_addBits(&data1, country_code, 10);
    bitarray_addBits(&data1, test_protocol, 3);
    bitarray_addBits(&data1, typeplb, 3);
    bitarray_addBits(&data1, certif, 1);
    bitarray_addBits(&data1, serial_number, 20);
    bitarray_addBits(&data1, national_use, 10);
    bitarray_addBits(&data1, certif_number, 10);
    bitarray_addBits(&data1, radiolocating, 2);
    
    bch_encode(pdf1, bch1_poly, LENPDF1_WITH_BCH1, LENPDF1);
    
    //add bits into array for pdf2 and calculate the bch_code for pdf2
    uint8_t *pdf2 = protocol_to_modulate+24+LENPDF1_WITH_BCH1;
    bitarray_t data2;
    bitarray_init(&data2, pdf2, LENPDF2);
    bitarray_addBits(&data2, position_data_source, 1);
    bitarray_addBits(&data2, latitude_flag, 1);
    bitarray_addBits(&data2, latitude_degrees, 7);
    bitarray_addBits(&data2, latitude_minutes, 4);
    bitarray_addBits(&data2, longtitude_flag, 1);
    bitarray_addBits(&data2, longtitude_degrees, 8);
    bitarray_addBits(&data2, longtitude_minutes, 4);
    
    bch_encode(pdf2, bch2_poly, LENPDF2_WITH_BCH2, LENPDF2);
    
    for (int i = 0; i < LEN_PROTOCOL; i++) {
        protocol_to_modulate[i] = protocol_to_modulate[i] == 0 ? IQ_0 : IQ_1;
    }
    
    SPI_Send(protocol_to_modulate, LEN_PROTOCOL);
    
}

/**
 * @brief Init bitarray
 *
 * @param bitArr bitarray container to init
 * @param arr    array to use
 * @param len      size of array
 */
static void bitarray_init(bitarray_t* bitArr, uint8_t* arr, uint16_t len){
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
static void bitarray_addBit(bitarray_t* bitArr, uint8_t bit) {
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
static void bitarray_addBits(bitarray_t* bitArr, uint16_t bits, uint8_t cnt){
    if (bitArr != NULL && bitArr->idx < bitArr->len) {
        for (uint8_t i = 1; i <= cnt; i++) {
            bitarray_addBit(bitArr, bits >> cnt - i);
        }
    }
}

/**
 * @brief Encode data using bch algorithm
 *
 * @param data   data array (size: n)
 * @param g_poly generator polynom array (size: n - k + 1)
 * @param n      data array length (length of output)
 * @param k      payload length (length of input)
 */
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
