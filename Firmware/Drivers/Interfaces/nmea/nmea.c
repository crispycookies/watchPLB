#include "nmea.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NMEA_TYPE_STR_LENGTH 5

#define NMEA_TYPE_STR_GPGLL "GPGLL"

#define NMEA_GPGLL_LENGTH 60    // TODO: check real length

static uint8_t charToHex(uint8_t ch);
static uint8_t toLower(uint8_t ch);
static void parse(NMEA_Instance* nmea);
static void parseGPGGL(NMEA_Instance* nmea);

void NMEA_Init(NMEA_Instance* nmea) {
    if (nmea != 0) {
        nmea->state = NMEA_State_IDLE;
        nmea->cb_pos = 0;
    }
}

void NMEA_Process(NMEA_Instance* nmea, uint8_t byte) {
    if (nmea != 0) {
        if (byte == '$') {
            nmea->state = NMEA_State_TYPE;
            nmea->idx = 0;
            nmea->cs = 0;
        } else {
            switch(nmea->state) {
                case NMEA_State_IDLE:
                    break;
                case NMEA_State_TYPE:
                    if (byte == ',') {
                        if (nmea->idx == NMEA_TYPE_STR_LENGTH) {
                            if (strncmp((char*)nmea->data, NMEA_TYPE_STR_GPGLL, NMEA_TYPE_STR_LENGTH) == 0) {
                                nmea->type = NMEA_Type_GPGLL;
                            } else {
                                nmea->type = NMEA_Type_NONE;
                            }
                            nmea->state = NMEA_State_DATA;
                            nmea->idx = 0;
                        } else {
                            nmea->state = NMEA_State_IDLE;
                        }
                    } else if (nmea->idx >= NMEA_TYPE_STR_LENGTH) {
                        nmea->state = NMEA_State_IDLE;
                    } else {
                        nmea->data[nmea->idx++] = byte;
                    }
                    nmea->cs = nmea->cs ^ byte;
                    break;
                case NMEA_State_DATA:
                    if (byte == '*') {
                        nmea->state = NMEA_State_CS0;
                    }
                    if (byte >= NMEA_DATA_LENGTH) {
                        nmea->state = NMEA_State_IDLE;
                    } else {
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
                    nmea->state = byte == '\r' ? NMEA_State_LF : NMEA_State_IDLE;
                    break;
                case NMEA_State_LF:
                    if (byte == '\n') {
                        parse(nmea);
                    }
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
        nmea->cb_pos = cb;
    }
}

static uint8_t charToHex(uint8_t ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else if (toLower(ch) >= 'a' && toLower(ch) <= 'f') {
        return 10 + toLower(ch) - 'a';
    }
    return 0;
}

static uint8_t toLower(uint8_t ch) {
    return ch >= 'A' && ch <= 'Z' ? ch - 'A' + 'a' : ch;
}

static void parse(NMEA_Instance* nmea) {
    if (nmea != 0) {
        switch (nmea->type)
        {
            case NMEA_Type_GPGLL:
                parseGPGGL(nmea);
                break;
            default:
                break;
        }
    }
}

static void parseGPGGL(NMEA_Instance* nmea) {
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

        if (*buf == ',' && *(buf+1) == 'A') {
            pos.valid = 1;
        } else {
            pos.valid = 0;
        }
        
        nmea->cb_pos(&pos);
    }
}
