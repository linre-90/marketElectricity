#ifndef M_UTILS_H
#define M_UTILS_H

#include <time.h>

/* Clamp float value [num] between [min] and [max] */
float clamp(float num, float min, float max);

/* Calculates current unix hour with 0 zero seconds. */
time_t calculateCurrentHour(void);

#endif