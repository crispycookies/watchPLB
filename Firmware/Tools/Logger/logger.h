#ifndef LOGGER_H
#define LOGGER_H

#include "log.h"
#include "position.h"

#define LOG(...) LOG_Log(__VA_ARGS__)

#define LOG_POS(POS) LOG("[%02u:%02u:%02u:%02u] %c %2u° %2.6f' %c %3u° %2.6f' %c\n", \
			POS->time.hour, POS->time.minute, POS->time.second, POS->time.split, \
			POS->latitude.direction == POS_Latitude_Flag_N ? 'N' : 'S', \
				POS->latitude.degree, POS->latitude.minute, \
			POS->longitude.direction == POS_Longitude_Flag_E ? 'E' : 'W', \
				POS->longitude.degree, POS->longitude.minute, \
			POS->valid == POS_Valid_Flag_Valid ? 'V' : 'I')


#endif //!LOGGER_H