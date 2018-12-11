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