/*
 * ubx.h
 *
 *  Created on: 9.11.2018
 *      Author: Paul
 */

#ifndef UBX_H
#define UBX_H

#define UBX_ACK_CB_Count 3
#define UBX_CB_Count 5
#define UBX_MSG_MAX_LENGTH 128

/**
 * @brief UBX message state
 * 
 */
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

/**
 * @brief UBX Message classes
 * 
 */
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

/**
 * @brief Message IDs for ACK message class
 * 
 */
typedef enum {
    UBX_Id_Ack_Ack = 0x01,
    UBX_Id_Ack_Nak = 0x00,
} UBX_Id_Ack;

/**
 * @brief Pointer to data structure 
 * to avoid void* and casting
 * 
 */
typedef union {
    
} UBX_DataPtr;

/**
 * @brief Acknowledge callback function pointer
 * 
 * @param msgClass Message class of message to acknowledge
 * @param id Id of message to acknowledge
 * @param ack ACK/NAK flag
 * 
 */
typedef void (*UBX_AckCallback)(UBX_Class msgClass, uint8_t id, UBX_Id_Ack ack);

/**
 * @brief General message callback function pointer
 * 
 * @param msgClass message class of message
 * @param id message Id of message
 * @param data pointer to parsed data
 * 
 */
typedef void (*UBX_Callback)(UBX_Class msgClass, uint8_t id, UBX_DataPtr data);

/**
 * @brief Acknowledge callback configuration structure
 * 
 */
typedef struct {
    uint8_t msgClass;   //class of message to be acknowledged
    uint8_t id;         //id of message to be acknowledged
    UBX_AckCallback cb; //callback function
} UBX_AckCallbackStruct;

/**
 * @brief General message callback configuration structure
 * 
 */
typedef struct {
    uint8_t msgClass;   //class of message to be received
    uint8_t id;         //id of message to be received
    UBX_Callback cb;    //callback function
} UBX_CallbackStruct;

typedef struct {
    UBX_State state;    //state of parsed message
    UBX_Class msgClass; //class of message
    uint8_t id;         //id of message
    uint16_t msgLength; //message length
    uint8_t ck_a;       //checksum a
    uint8_t ck_b;       //checksum b
    uint16_t idx;       //current index in payload
    uint8_t msg[UBX_MSG_MAX_LENGTH];                //payload
    UBX_AckCallbackStruct cb_ack[UBX_ACK_CB_Count]; //acknowledge callback configurations
    UBX_CallbackStruct cb[UBX_CB_Count];            //general message callback configurations
} UBX_Instance;

/**
 * @brief UBX initialization routine
 * 
 * @param ubx ubx instance structure
 */
void UBX_Init(UBX_Instance* ubx);

/**
 * @brief Configure acknowledge callback
 * 
 * @param ubx ubx instance structure
 * @param cb callback function
 * @param msgClass message class to be acknowledged
 * @param id message id to be acknowledged
 */
void UBX_SetAckCallback(UBX_Instance* ubx, UBX_AckCallback cb, uint8_t msgClass, uint8_t id);

/**
 * @brief Configure general message callback 
 * 
 * @param ubx ubx instance structure
 * @param cb callback function
 * @param msgClass message class of message to be received
 * @param id message Id of message to be received
 */
void UBX_SetCallback(UBX_Instance* ubx, UBX_Callback cb, uint8_t msgClass, uint8_t id);

/**
 * @brief Process received byte
 * 
 * @param ubx ubx instance structure
 * @param byte received byte
 */
void UBX_Process(UBX_Instance* ubx, uint8_t byte);

uint16_t UBX_CreateNMEAConfigFrame(UBX_Instance* ubx, uint8_t *frame, uint16_t len);

#endif //!UBX_H
