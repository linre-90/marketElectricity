#ifndef WEB_H
#define WEB_H

#include <time.h>

/* How many results api returs on fetchData() */
static const int NUM_OF_API_RESULTS = 48;

/* Presents data parsed by web module json converter */
struct Price {
	float price;
	time_t utcTime;

	int utcHour;
	int MM;
	int YYYY;
	int DD;
};

/* Fetch 48 hour price listing from API.*/
void fetchData(struct Price* priceArr);

#endif // !WEB_H
