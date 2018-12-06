#include "position.h"
#include "stdio.h"

#define POS_STRLEN 61

int16_t POS_CmpTime(POS_Time *left, POS_Time *right) {
	if (left == 0 || right == 0) {
		return 0;
	}
	if (left->hour == right->hour) {
		if (left->minute == right->minute) {
			if (left->second == right->second) {
				if (left->split == right->split) {
					return 0;
				} else {
					return ((int16_t)left->split) - ((int16_t)right->split);
				}
			} else {
				return ((int16_t)left->second) - ((int16_t)right->second);
			}
		} else {
			return ((int16_t)left->minute) - ((int16_t)right->minute);
		}
	} else {
		return ((int16_t)left->hour) - ((int16_t)right->hour);
	}
}

uint16_t POS_ToString(POS_Position *pos, uint8_t* str, uint16_t len) {
	if (len < POS_STRLEN) {
		return 0;
	}

	return sprintf(str, "[%02u:%02u:%02u:%02u] %c %2u° %2.6f' %c %3u° %2.6f' %c\n", 
			pos->time.hour, pos->time.minute, pos->time.second, pos->time.split,
			pos->latitude.direction == POS_Latitude_Flag_N ? 'N' : 'S', 
				pos->latitude.degree, pos->latitude.minute, 
			pos->longitude.direction == POS_Longitude_Flag_E ? 'E' : 'W', 
				pos->longitude.degree, pos->longitude.minute, 
			pos->valid == POS_Valid_Flag_Valid ? 'V' : 'I');
}