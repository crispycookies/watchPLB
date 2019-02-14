
/**
 * @file emergencyCall.h
 * @author Paul Götzinger
 * @brief Emergency call module
 * @version 1.0
 * @date 2019-02-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef EMERGENCYCALL_H
#define EMERGENCYCALL_H

/**
 * @brief Emergency states
 * EMC_State_Idle      - No current emergency
 * EMC_State_Emergency - Current emergency
 */
typedef enum {
    EMC_State_Idle = 0,
    EMC_State_Emergency
} EMC_State;

/**
 * @brief Init emergency module
 * 
 */
void EMC_Init(void);

/**
 * @brief Process emergency module
 * 
 */
void EMC_Process(void);

/**
 * @brief (Un-)Set emergency
 * 
 * @param emc New emergency state
 */
void EMC_SetEmergency(EMC_State emc);

#endif //!EMERGENCYCALL_H