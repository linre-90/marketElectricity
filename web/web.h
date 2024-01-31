#ifndef WEB_H
#define WEB_H

#include <time.h>

/* How many results api returs on fetchData() */
static const int NUM_OF_API_RESULTS = 48;

/* Presents data parsed by web module json converter. Consists of [price] in cents and [utcTime] in unix time stamp. */
struct Price {
	float price;
	time_t utcTime;
};

/* Fetch latest data blop that contains 48 hours of data.*/
void fetchData(struct Price* const out_PriceArr);

#endif // !WEB_H
