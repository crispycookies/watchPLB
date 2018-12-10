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
					return left->split - right->split;
				}
			} else {
				return left->second - right->second;
			}
		} else {
			return left->minute - right->minute;
		}
	} else {
		return left->hour - right->hour;
	}
}