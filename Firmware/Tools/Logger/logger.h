/**
 * @file logger.h
 * @author Paul Götzinger
 * @brief Global logging definitions
 * @version 1.0
 * @date 2019-02-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "log.h"
#include "position.h"

/**
 * @brief printf formatted logging
 * 
 */
#define LOG(...) LOG_Log(__VA_ARGS__)

/**
 * @brief Position logging
 * 
 */
#define LOG_POS(POS) LOG("[%02u:%02u:%02u:%02u] %c %2u° %2.6f' %c %3u° %2.6f' %c\n", \
			POS->time.hour, POS->time.minute, POS->time.second, POS->time.split, \
			POS->latitude.direction == POS_Latitude_Flag_N ? 'N' : 'S', \
				POS->latitude.degree, POS->latitude.minute, \
			POS->longitude.direction == POS_Longitude_Flag_E ? 'E' : 'W', \
				POS->longitude.degree, POS->longitude.minute, \
			POS->valid == POS_Valid_Flag_Valid ? 'V' : 'I')

/**
 * @brief Bitarray logging
 * 
 */
#define LOG_BITARRAY(ARR, LEN) LOG_BitArray(ARR, LEN)

#endif //!LOGGER_H