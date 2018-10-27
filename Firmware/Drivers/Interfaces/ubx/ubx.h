#ifndef UBX_H
#define UBX_H

#include <stdint.h>

#define UBX_ACK_CB_Count 3
#define UBX_CB_Count 5
#define UBX_MSG_MAX_LENGTH 128

typedef enum {
    UBX_State_IDLE = 0,
    UBX_State_Sync2,
    UBX_State_Class,
    UBX_State_Id,
    UBX_State_Length0,
    UBX_State_Length1,
    UBX_State_Payload,
    UBX_State_CK_A,
    UBX_State_CK_B
} UBX_State;

typedef enum {
  UBX_Class_None = 0x0,
  UBX_Class_NAV = 0x01,
  UBX_Class_RXM = 0x02,
  UBX_Class_INF = 0x04,
  UBX_Class_ACK = 0x05,
  UBX_Class_CFG = 0x06,
  UBX_Class_UPD = 0x09,
  UBX_Class_MON = 0x0A,
  UBX_Class_AID = 0x0B,
  UBX_Class_TIM = 0x0D,
  UBX_Class_ESF = 0x10,
  UBX_Class_MGA = 0x13,
  UBX_Class_LOG = 0x21,
  UBX_Class_SEC = 0x27,
  UBX_Class_HNR = 0x28  
} UBX_Class;

typedef enum {
    UBX_Id_Ack_Ack = 0x01,
    UBX_Id_Ack_Nak = 0x00,
} UBX_Id_Ack;

typedef union {
    
} UBX_DataPtr;

typedef void (*UBX_AckCallback)(UBX_Class msgClass, uint8_t id, UBX_Id_Ack ack);
typedef void (*UBX_Callback)(UBX_Class msgClass, uint8_t id, UBX_DataPtr data);

typedef struct {
    uint8_t msgClass;
    uint8_t id;
    UBX_AckCallback cb;
} UBX_AckCallbackStruct;

typedef struct {
    uint8_t msgClass;
    uint8_t id;
    UBX_Callback cb;
} UBX_CallbackStruct;

typedef struct {
    UBX_State state;
    UBX_Class msgClass;
    uint8_t id;
    uint16_t msgLength;
    uint8_t ck_a;
    uint8_t ck_b;
    uint16_t idx;
    uint8_t msg[UBX_MSG_MAX_LENGTH];
    UBX_AckCallbackStruct cb_ack[UBX_ACK_CB_Count];
    UBX_CallbackStruct cb[UBX_CB_Count];
} UBX_Instance;

void UBX_Init(UBX_Instance* ubx);
void UBX_SetAckCallback(UBX_Instance* ubx, UBX_AckCallback cb, uint8_t msgClass, uint8_t id);
void UBX_SetCallback(UBX_Instance* ubx, UBX_Callback cb, uint8_t msgClass, uint8_t id);
void UBX_Process(UBX_Instance* ubx, uint8_t byte);

#endif //!UBX_H
