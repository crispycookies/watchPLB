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

#define MSG_ACK_LEN     0x2
#define MSG_ACK_POS_CLS 0x0
#define MSG_ACK_POS_ID  0x1

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

