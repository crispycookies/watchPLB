#ifndef NMEA_H
#define NMEA_H

#include "position.h"

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

typedef void (*NMEA_Callback_Position)(POS_Position *pos);

typedef struct {
    NMEA_State state;
    NMEA_Type type;
    NMEA_Callback_Position cb_pos;
    uint8_t cs;
    uint8_t data[NMEA_DATA_LENGTH];
    uint8_t idx;
} NMEA_Instance;

void NMEA_Init(NMEA_Instance* nmea);
void NMEA_SetPositionCallback(NMEA_Instance* nmea, NMEA_Callback_Position cb);
void NMEA_Process(NMEA_Instance* nmea, uint8_t byte);

#endif //NMEA_H