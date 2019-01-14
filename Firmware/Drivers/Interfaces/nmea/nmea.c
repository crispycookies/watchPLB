/*
 * nmea.h
 *
 *  Created on: 9.11.2018
 *      Author: Paul
 */

#include "nmea.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define NMEA_TYPE_STR_LENGTH 5  //length of nmea type string

#define NMEA_TYPE_STR_GPGLL "GPGLL"
#define NMEA_TYPE_STR_GNGLL "GNGLL"
#define NMEA_TYPE_STR_GLGSB "GLGSB"
#define NMEA_TYPE_STR_GPGSV "GPGSV"
#define NMEA_TYPE_STR_GNGSA "GNGSA"
#define NMEA_TYPE_STR_GNGGA "GNGGA"
#define NMEA_TYPE_STR_GNVTG "GNVTG"
#define NMEA_TYPE_STR_GNRMC "GNRMC"

#define NMEA_GPGLL_LENGTH 60    // TODO: check real length

/**
 * @brief Parses character as hex digit
 * 
 * @param ch 
 * @return uint8_t 
 */
static uint8_t charToHex(uint8_t ch);

/**
 * @brief parse nmea message
 * 
 * @param nmea nmea instance structure
 */
static void parse(NMEA_Instance* nmea);

/**
 * @brief parse GPGLL message
 * 
 * @param nmea nmea instance structure
 */
static void parseGPGLL(NMEA_Instance* nmea);

void NMEA_Init(NMEA_Instance* nmea) {
    if (nmea != 0) {
        //init state and callback
        nmea->state = NMEA_State_IDLE;
        nmea->cb_pos = 0;
        nmea->cb_unk = 0;
    }
}

void NMEA_Process(NMEA_Instance* nmea, uint8_t byte) {
    if (nmea != 0) {
        //check for start byte
        if (byte == '$') {
            //init message and set next state
            nmea->state = NMEA_State_TYPE;
            nmea->idx = 0;
            nmea->cs = 0;
        } else {
            switch(nmea->state) {
                case NMEA_State_IDLE:
                    break;
                case NMEA_State_TYPE:
                    //check for delimiter character
                    if (byte == ',') {
                        //check if read length matches
                        if (nmea->idx == NMEA_TYPE_STR_LENGTH) {
                            //detect message format
                            if (strncmp((char*)nmea->data, NMEA_TYPE_STR_GPGLL, NMEA_TYPE_STR_LENGTH) == 0) {
                                nmea->type = NMEA_Type_GPGLL;
                            } else if (strncmp((char*)nmea->data, NMEA_TYPE_STR_GNGLL, NMEA_TYPE_STR_LENGTH) == 0) {
                                nmea->type = NMEA_Type_GNGLL;
                            } else if (strncmp((char*)nmea->data, NMEA_TYPE_STR_GLGSB, NMEA_TYPE_STR_LENGTH) == 0) {
                                nmea->type = NMEA_Type_GLGSB;
                            } else if (strncmp((char*)nmea->data, NMEA_TYPE_STR_GPGSV, NMEA_TYPE_STR_LENGTH) == 0) {
                                nmea->type = NMEA_Type_GPGSV;
                            } else if (strncmp((char*)nmea->data, NMEA_TYPE_STR_GNGSA, NMEA_TYPE_STR_LENGTH) == 0) {
                                nmea->type = NMEA_Type_GNGSA;
                            } else if (strncmp((char*)nmea->data, NMEA_TYPE_STR_GNGGA, NMEA_TYPE_STR_LENGTH) == 0) {
                                nmea->type = NMEA_Type_GNGGA;
                            } else if (strncmp((char*)nmea->data, NMEA_TYPE_STR_GNVTG, NMEA_TYPE_STR_LENGTH) == 0) {
                                nmea->type = NMEA_Type_GNVTG;
                            } else if (strncmp((char*)nmea->data, NMEA_TYPE_STR_GNRMC, NMEA_TYPE_STR_LENGTH) == 0) {
                                nmea->type = NMEA_Type_GNRMC;
                            } else {
                                nmea->type = NMEA_Type_NONE;
                            }

                            //set next state and reset index
                            nmea->state = NMEA_State_DATA;
                            nmea->idx = 0;
                        } else {
                            //cancel message
                            nmea->state = NMEA_State_IDLE;
                        }
                    //check if index is out of range
                    } else if (nmea->idx >= NMEA_TYPE_STR_LENGTH) {
                        //cancel message
                        nmea->state = NMEA_State_IDLE;
                    } else {
                        //save character
                        nmea->data[nmea->idx++] = byte;
                    }
                    //calc checksum
                    nmea->cs = nmea->cs ^ byte;
                    break;
                case NMEA_State_DATA:
                    //check fro delimiter character
                    if (byte == '*') {
                        //set next state
                        nmea->state = NMEA_State_CS0;
                        nmea->data[nmea->idx++] = 0;
                    }
                    //check if index is out of range
                    if (byte >= NMEA_DATA_LENGTH) {
                        //cancel message
                        nmea->state = NMEA_State_IDLE;
                    } else {
                        //store byte
                        nmea->data[nmea->idx++] = byte;
                        nmea->cs = nmea->cs ^ byte;
                    }
                    break;
                case NMEA_State_CS0:
                    nmea->state = charToHex(byte) & nmea->cs >> 4 ? NMEA_State_CS1 : NMEA_State_IDLE;
                    break;
                case NMEA_State_CS1:
                    nmea->state = charToHex(byte) & nmea->cs ? NMEA_State_CR : NMEA_State_IDLE;
                    break;
                case NMEA_State_CR:
                    //check for carriage return
                    // true:  next state
                    // false: cancel message
                    nmea->state = byte == '\r' ? NMEA_State_LF : NMEA_State_IDLE;
                    break;
                case NMEA_State_LF:
                    //check for linefeed
                    if (byte == '\n') {
                        //parse message
                        parse(nmea);
                    }
                    //set idle state
                    nmea->state = NMEA_State_IDLE;
                    break;
                default:
                    break;
            }
        }
    }
}

void NMEA_SetPositionCallback(NMEA_Instance* nmea, NMEA_Callback_Position cb) {
    if (nmea != 0) {
        //set callback
        nmea->cb_pos = cb;
    }
}

void NMEA_SetUnknownCallback(NMEA_Instance* nmea, NMEA_Callback_Unknown cb) {
    if (nmea != 0) {
        //set callback
        nmea->cb_unk = cb;
    }
}

static uint8_t charToHex(uint8_t ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else if (tolower(ch) >= 'a' && tolower(ch) <= 'f') {
        return 10 + tolower(ch) - 'a';
    }
    return 0;
}

static void parse(NMEA_Instance* nmea) {
    if (nmea != 0) {
        switch (nmea->type)
        {
            case NMEA_Type_GPGLL:
                parseGPGLL(nmea);
                break;
            default:
                if (nmea->cb_unk != 0) {
                    nmea->cb_unk(nmea->type, nmea->data, nmea->idx);
                }
                break;
        }
    }
}

static void parseGPGLL(NMEA_Instance* nmea) {
    if (nmea != 0 && nmea->type == NMEA_Type_GPGLL && nmea->idx >= NMEA_GPGLL_LENGTH) {
        POS_Position pos;
        uint8_t *buf = nmea->data;

        //read latitude

        //read degree
        pos.latitude.degree = strtol((char*)buf, 0, 10);
        if (pos.latitude.degree > 90) {
            return;
        }
        buf += 2;

        //read minute
        pos.latitude.minute = strtod((char*)buf, 0);
        buf += 6; //skip delimiter ','
        
        //read flag
        if (*buf == 'N') {
            pos.latitude.direction = POS_Latitude_Flag_N;
        } else if (*buf == 'S') {
            pos.latitude.direction = POS_Latitude_Flag_S;
        } else {
            return;
        }
        buf += 2;

        //read longitude

        //read degree
        pos.longitude.degree = strtol((char*)buf, 0, 10);
        if (pos.longitude.degree > 180) {
            return;
        }
        buf += 2;

        //read minute
        pos.longitude.minute = strtod((char*)buf, 0);
        buf += 6; //skip delimiter ','
        
        //read flag
        if (*buf == 'E') {
            pos.longitude.direction = POS_Longitude_Flag_E;
        } else if (*buf == 'W') {
            pos.longitude.direction = POS_Longitude_Flag_E;
        } else {
            return;
        }
        buf += 2;
        
        //read time

        //read hour
        pos.time.hour = strtol((char*)buf, 0, 10);
        if (pos.time.hour > 23) {
            return;
        }
        buf += 2;

        //read minute
        pos.time.minute = strtol((char*)buf, 0, 10);
        if (pos.time.minute > 59) {
            return;
        }
        buf += 2;
        
        //read second
        pos.time.second = strtol((char*)buf, 0, 10);
        if (pos.time.second > 59) {
            return;
        }
        buf += 3;   //also skip decimal point

        //read split-second
        pos.time.split = strtol((char*)buf, 0, 10);
        if (pos.time.split > 99) {
            return;
        }
        buf += 2;

        //read valid flag
        if (*buf == ',' && *(buf+1) == 'A') {
            pos.valid = 1;
        } else {
            pos.valid = 0;
        }
        
        //execute callback
        nmea->cb_pos(&pos);
    }
}
