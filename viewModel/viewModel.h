#ifndef APP_VIEW_MODEL
#define APP_VIEW_MODEL
#include <time.h>

/* View model contains all the relevant data that app libraries need to talk each other. */
typedef struct {
    struct Price* priceArr;
    int currHourIndex;
    int NextHourIndex;
    time_t nextDataUpdateStamp;
} ViewModel;


#endif // !APP_VIEW_MODEL


