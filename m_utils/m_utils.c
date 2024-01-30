#include <stdlib.h>
#include <time.h>
#include "m_utils.h"


float clamp(float num, float min, float max) {
    const float t = num < min ? min : num;
    return t > max ? max : t;
}


time_t calculateCurrentHour(void) {
    time_t t;
    time(&t);
    return t - (t % 3600);
}

