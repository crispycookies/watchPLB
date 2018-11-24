#ifndef EMERGENCYCALL_H
#define EMERGENCYCALL_H

typedef enum {
    EMC_State_Idle = 0,
    EMC_State_Emergency
} EMC_State;

void EMC_Init(void);

void EMC_Process(void);

void EMC_SetEmergency(EMC_State emc);

#endif //!EMERGENCYCALL_H