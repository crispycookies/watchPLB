#ifndef POSITION_H
#define POSITION_H

#include <stdint.h>

typedef enum {
	POS_Latitude_Flag_S = 0,
	POS_Latitude_Flag_N = 1
} POS_Latitude_Flag;

typedef enum {
	POS_Longitude_Flag_W = 0,
	POS_Longitude_Flag_E = 1
} POS_Longitude_Flag;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t split;
} POS_Time;

typedef struct{
	POS_Latitude_Flag direction;
	uint16_t degree;
	double minute;
} POS_Latitude;

typedef struct{
	POS_Longitude_Flag direction;
	uint16_t degree;
	double minute;
} POS_Longitude;

typedef struct {
    POS_Time time;
	POS_Latitude latitude;
	POS_Longitude longitude;
	uint8_t valid;
} POS_Position;

int16_t POS_CmpTime(POS_Time *left, POS_Time *right);

#endif //!POSITION_H