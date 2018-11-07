#include <string.h>

#include "ubx.h"


#define SYNC_CHAR_1 0xB5
#define SYNC_CHAR_2 0x62

#define MSG_ACK_LEN     0x2
#define MSG_ACK_POS_CLS 0x0
#define MSG_ACK_POS_ID  0x1

static void processMsg(UBX_Instance* ubx);
static void processAck(UBX_Instance* ubx);

void UBX_Init(UBX_Instance* ubx) {
    if (ubx != 0) {
        ubx->state = UBX_State_IDLE;
        memset(&ubx->cb, 0, sizeof(UBX_CallbackStruct) * UBX_CB_Count);
        memset(&ubx->cb_ack, 0, sizeof(UBX_AckCallbackStruct) * UBX_ACK_CB_Count);
    }
}

void UBX_SetAckCallback(UBX_Instance* ubx, UBX_AckCallback cb, uint8_t msgClass, uint8_t id) {
    if (ubx != 0) {
        uint8_t found = 0;
        for (uint8_t i = 0; i < UBX_ACK_CB_Count; i++) {
            if (ubx->cb_ack[i].cb == 0 || (
                    ubx->cb_ack[i].msgClass == msgClass && ubx->cb_ack[i].id == id)) {
                if (found == 0) {
                    ubx->cb_ack[i].cb = cb;
                    ubx->cb_ack[i].msgClass = msgClass;
                    ubx->cb_ack[i].id = id;
                    found = 1;
                } else {
                    ubx->cb_ack[i].cb = 0;
                }
            }
        }
    }
}

void UBX_SetCallback(UBX_Instance* ubx, UBX_Callback cb, uint8_t msgClass, uint8_t id) {
    if (ubx != 0) {
        uint8_t found = 0;
        for (uint8_t i = 0; i < UBX_CB_Count; i++) {
            if (ubx->cb[i].cb == 0 || (ubx->cb[i].msgClass == msgClass && ubx->cb[i].id == id)) {
                if (found == 0) {
                    ubx->cb[i].cb = cb;
                    ubx->cb[i].msgClass = msgClass;
                    ubx->cb[i].id = id;
                    found = 1;
                } else {
                    ubx->cb[i].cb = 0;
                }
            }
        }
    }
}

void UBX_Process(UBX_Instance* ubx, uint8_t byte) {
    if (ubx != 0) {
        if (byte == SYNC_CHAR_1) {
            ubx->state = UBX_State_Sync2;
            ubx->idx = 0;
            ubx->ck_a = 0;
            ubx->ck_b = 0;
        } else {
            
            if (ubx->state != UBX_State_CK_A || ubx->state != UBX_State_CK_B) {
                ubx->ck_a += byte;
                ubx->ck_b += ubx->ck_a;
            }

            switch (ubx->state)
            {
                case UBX_State_Sync2:
                    if (byte == SYNC_CHAR_2) {
                        ubx->idx = 0;
                        ubx->ck_a = 0;
                        ubx->ck_b = 0;
                        ubx->state = UBX_State_Class;
                    } else {
                        ubx->state = UBX_State_IDLE;
                    }
                    break;
                case UBX_State_Class:
                    ubx->msgClass = (UBX_Class)byte;
                    ubx->state = UBX_State_Id;
                    break;
                case UBX_State_Id:
                    ubx->id = byte;
                    ubx->state = UBX_State_Length0;
                    break;
                case UBX_State_Length0:
                    ubx->msgLength = byte;
                    ubx->state = UBX_State_Length1;
                    break;
                case UBX_State_Length1:
                    ubx->msgLength |= byte << 8;
                    ubx->state = UBX_State_Payload;
                    break;
                case UBX_State_Payload:
                    if (ubx->idx >= UBX_MSG_MAX_LENGTH) {
                        ubx->state = UBX_State_IDLE;
                    } else {
                        ubx->msg[ubx->idx++] = byte;

                        if (ubx->idx >= ubx->msgLength) {
                            ubx->state = UBX_State_CK_A;
                        }
                    }
                    break;
                case UBX_State_CK_A:
                    if (ubx->ck_a == byte) {
                        ubx->state = UBX_State_CK_B;
                    } else {
                        ubx->state = UBX_State_IDLE;
                    }
                    break;
                case UBX_State_CK_B:
                    if (ubx->ck_b == byte) {
                        processMsg(ubx);
                    }
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
        switch (ubx->msgClass)
        {
            case UBX_Class_ACK:
                processAck(ubx);
                break;
            default:
                break;
        }
    }
}

static void processAck(UBX_Instance* ubx) {
    if (ubx != 0 && ubx->msgClass == UBX_Class_ACK) {
        for(uint8_t i = 0; i < UBX_ACK_CB_Count; i++) {
            if (ubx->cb_ack[i].cb != 0 && 
                    ubx->cb_ack[i].msgClass == ubx->msgClass && 
                    ubx->cb_ack[i].id == ubx->id) {
                ubx->cb_ack[i].cb((UBX_Class)ubx->msg[MSG_ACK_POS_CLS], (UBX_Id_Ack)ubx->msg[MSG_ACK_POS_ID], ubx->id);
                ubx->cb_ack[i].cb = 0;
                return;
            }
        }
    }
}

