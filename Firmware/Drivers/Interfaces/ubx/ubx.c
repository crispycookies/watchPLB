/*
 * ubx.c
 *
 *  Created on: 9.11.2018
 *      Author: Paul
 */

#include <string.h>

#include "ubx.h"


#define SYNC_CHAR_1 0xB5
#define SYNC_CHAR_2 0x62

#define HEADER_LEN      0x06
#define CK_LEN          0x02
#define MSG_ACK_LEN     0x02
#define MSG_ACK_POS_CLS 0x00
#define MSG_ACK_POS_ID  0x01

#define MSG_CFG_NMEA_LEN 0x14
#define MSG_CFG_NMEA_ID  0x17

/**
 * @brief Process/parse message
 * 
 * @param ubx ubx instance structure
 */
static void processMsg(UBX_Instance* ubx);

/**
 * @brief Process/parse acknowledge message
 * 
 * @param ubx ubx instance structure
 */
static void processAck(UBX_Instance* ubx);

static void createChecksum(uint8_t *data, uint16_t len);

void UBX_Init(UBX_Instance* ubx) {
    if (ubx != 0) {

        //set idle state
        ubx->state = UBX_State_IDLE;

        //clear callback config arrays
        memset(&ubx->cb, 0, sizeof(UBX_CallbackStruct) * UBX_CB_Count);
        memset(&ubx->cb_ack, 0, sizeof(UBX_AckCallbackStruct) * UBX_ACK_CB_Count);
    }
}

void UBX_SetAckCallback(UBX_Instance* ubx, UBX_AckCallback cb, uint8_t msgClass, uint8_t id) {
    if (ubx != 0) {
        uint8_t found = 0;

        //search first matching or empty callback configuration
        for (uint8_t i = 0; i < UBX_ACK_CB_Count; i++) {
            if (ubx->cb_ack[i].cb == 0 || (
                    ubx->cb_ack[i].msgClass == msgClass && ubx->cb_ack[i].id == id)) {
                if (found == 0) {
                    //set configuration
                    ubx->cb_ack[i].cb = cb;
                    ubx->cb_ack[i].msgClass = msgClass;
                    ubx->cb_ack[i].id = id;
                    found = 1;
                } else {
                    //disable all other callback configurations witch same class and id
                    ubx->cb_ack[i].cb = 0;
                }
            }
        }
    }
}

void UBX_SetCallback(UBX_Instance* ubx, UBX_Callback cb, uint8_t msgClass, uint8_t id) {
    if (ubx != 0) {
        uint8_t found = 0;

        //search first matching or empty callback configuration
        for (uint8_t i = 0; i < UBX_CB_Count; i++) {
            if (ubx->cb[i].cb == 0 || (ubx->cb[i].msgClass == msgClass && ubx->cb[i].id == id)) {
                if (found == 0) {
                    //set configuration
                    ubx->cb[i].cb = cb;
                    ubx->cb[i].msgClass = msgClass;
                    ubx->cb[i].id = id;
                    found = 1;
                } else {
                    //disable all other callback configurations witch same class and id
                    ubx->cb[i].cb = 0;
                }
            }
        }
    }
}

void UBX_Process(UBX_Instance* ubx, uint8_t byte) {
    if (ubx != 0) {
        //check for message start
        if (byte == SYNC_CHAR_1) {
            //set next state
            ubx->state = UBX_State_Sync2;
        } else {
            //check if new byte needs to be added to checksum 
            if (ubx->state != UBX_State_CK_A || ubx->state != UBX_State_CK_B) {
                //add byte to checksum
                ubx->ck_a += byte;
                ubx->ck_b += ubx->ck_a;
            }

            switch (ubx->state)
            {
                case UBX_State_Sync2:
                    //check for correct byte
                    if (byte == SYNC_CHAR_2) {
                        //init message
                        ubx->idx = 0;
                        ubx->ck_a = 0;
                        ubx->ck_b = 0;
                        
                        //set next state
                        ubx->state = UBX_State_Class;
                    } else {
                        //cancel message
                        ubx->state = UBX_State_IDLE;
                    }
                    break;
                case UBX_State_Class:
                    //read message class
                    ubx->msgClass = (UBX_Class)byte;

                    //set next state
                    ubx->state = UBX_State_Id;
                    break;
                case UBX_State_Id:
                    //read message id
                    ubx->id = byte;
                    
                    //set next state
                    ubx->state = UBX_State_Length0;
                    break;
                case UBX_State_Length0:
                    //read message length low byte
                    ubx->msgLength = byte;
                    
                    //set next state
                    ubx->state = UBX_State_Length1;
                    break;
                case UBX_State_Length1:
                    //read message lenth high byte
                    ubx->msgLength |= byte << 8;
                    
                    //set next state
                    ubx->state = UBX_State_Payload;
                    break;
                case UBX_State_Payload:
                    //check if index is outside the buffer
                    if (ubx->idx >= UBX_MSG_MAX_LENGTH) {
                        //cancel message
                        ubx->state = UBX_State_IDLE;
                    } else {
                        //save byte
                        ubx->msg[ubx->idx++] = byte;

                        //check if end of payload reached
                        if (ubx->idx >= ubx->msgLength) {
                            //set next state
                            ubx->state = UBX_State_CK_A;
                        }
                    }
                    break;
                case UBX_State_CK_A:
                    //compare checksum A
                    if (ubx->ck_a == byte) {
                        //set next state
                        ubx->state = UBX_State_CK_B;
                    } else {
                        //cancel message
                        ubx->state = UBX_State_IDLE;
                    }
                    break;
                case UBX_State_CK_B:
                    //compare chesum B
                    if (ubx->ck_b == byte) {
                        //process message
                        processMsg(ubx);
                    }
                    //set IDLE state -> message finished
                    ubx->state = UBX_State_IDLE;
                    break;
                default:
                    break;
            }
        }
    }
}

uint16_t UBX_CreateNMEAConfigFrame(UBX_Instance* ubx, uint8_t *frame, uint16_t len) {
    if (frame == 0 || len < (MSG_CFG_NMEA_LEN + HEADER_LEN + CK_LEN)) {
        return 0;
    }

    uint16_t idx = 0;

    //sync
    frame[idx++] = SYNC_CHAR_1;
    frame[idx++] = SYNC_CHAR_2;

    //class
    frame[idx++] = UBX_Class_CFG;

    //id
    frame[idx++] = MSG_CFG_NMEA_ID;

    //length
    frame[idx++] = MSG_CFG_NMEA_LEN;
    frame[idx++] = 0x00;

    //payload
    frame[idx++] = 0x1f; // filter       - 0b00011111
    frame[idx++] = 0x41; // nmeaVersion  - NMEA version 4.1
    frame[idx++] = 0x00; // numSV        - unlimited number of SVs reported
    frame[idx++] = 0x0A; // flags        - 0b00001010
    frame[idx++] = 0x72; // gnssToFilter - 0b01110010
    frame[idx++] = 0x00; // gnssToFilter
    frame[idx++] = 0x00; // gnssToFilter
    frame[idx++] = 0x00; // gnssToFilter
    frame[idx++] = 0x00; // svNumbering  - Strict - Satellites are not output
    frame[idx++] = 0x01; // mainTalkerId - GPS
    frame[idx++] = 0x00; // gsvTalkerId  - default
    frame[idx++] = 0x01; // version      - 1
    frame[idx++] = 0x00; // bdsTalkerId  - 0
    frame[idx++] = 0x00; // bdsTalkerId  - 0
    frame[idx++] = 0x00; // reserved1
    frame[idx++] = 0x00; // reserved1
    frame[idx++] = 0x00; // reserved1
    frame[idx++] = 0x00; // reserved1
    frame[idx++] = 0x00; // reserved1
    frame[idx++] = 0x00; // reserved1

    idx += CK_LEN;
    createChecksum(frame, idx);

    return idx;
}

static void processMsg(UBX_Instance* ubx) {
    if (ubx != 0) {
        //process message by class
        switch (ubx->msgClass)
        {
            case UBX_Class_ACK:
                //process acknowledge message
                processAck(ubx);
                break;
            default:
                break;
        }
    }
}

static void processAck(UBX_Instance* ubx) {
    //check if class matches
    if (ubx != 0 && ubx->msgClass == UBX_Class_ACK) {
        //search callback function
        for(uint8_t i = 0; i < UBX_ACK_CB_Count; i++) {
            if (ubx->cb_ack[i].cb != 0 && 
                    ubx->cb_ack[i].msgClass == ubx->msg[MSG_ACK_POS_CLS] && 
                    ubx->cb_ack[i].id == ubx->msg[MSG_ACK_POS_ID]) {
                //execute callback
                ubx->cb_ack[i].cb((UBX_Class)ubx->msg[MSG_ACK_POS_CLS], (UBX_Id_Ack)ubx->msg[MSG_ACK_POS_ID], ubx->id);
                
                //clear callback configuration
                ubx->cb_ack[i].cb = 0;
                return;
            }
        }
    }
}

static void createChecksum(uint8_t *data, uint16_t len) {
    uint8_t ck_a = 0;
    uint8_t ck_b = 0;

    for (uint16_t i = 2; i < len-2; i++) {
        ck_a = ck_a + data[i];
        ck_b = ck_b + ck_a;
    }

    data[len-2] = ck_a;
    data[len-1] = ck_b;
}