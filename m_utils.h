#ifndef M_UTILS_H
#define M_UTILS_H
#include "ui.h"

/* Clamp float value between min and max */
float clamp(float num, float min, float max);

/* Calculate current unix hour. */
time_t calculateCurrentHour(void);

#endif