#ifndef NMEA_H
#define NMEA_H

#include <stdint.h>

#define NMEA_DATA_LENGTH 72

typedef enum {
    NMEA_State_IDLE = 0,
    NMEA_State_TYPE,
    NMEA_State_DATA,
    NMEA_State_CS0,
    NMEA_State_CS1,
    NMEA_State_CR,
    NMEA_State_LF
} NMEA_State;

typedef enum {
    NMEA_Type_NONE = 0,
    NMEA_Type_GPGLL,
} NMEA_Type;

typedef enum {
	NMEA_Latitude_Flag_S = 0,
	NMEA_Latitude_Flag_N = 1
} NMEA_Latitude_Flag;

typedef enum {
	NMEA_Longitude_Flag_W = 0,
	NMEA_Longitude_Flag_E = 1
} NMEA_Longitude_Flag;

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t split;
} NMEA_Time;

typedef struct{
	NMEA_Latitude_Flag direction;
	uint16_t degree;
	double minute;
} NMEA_Latitude;

typedef struct{
	NMEA_Longitude_Flag direction;
	uint16_t degree;
	double minute;
} NMEA_Longitude;

typedef struct {
    NMEA_Time time;
	NMEA_Latitude latitude;
	NMEA_Longitude longitude;
	uint8_t valid;
} NMEA_Position;

typedef void (*NMEA_Callback_Position)(NMEA_Position *pos);

typedef struct {
    NMEA_State state;
    NMEA_Type type;
    NMEA_Callback_Position cb_pos;
    uint8_t cs;
    uint8_t data[NMEA_DATA_LENGTH];
    uint8_t idx;
} NMEA_Instance;

void NMEA_Init(NMEA_Instance* nmea);
void NMEA_SetPositionCallback(NMEA_Callback_Position cb);
void NMEA_Process(NMEA_Instance* nmea, uint8_t byte);

#endif //NMEA_H