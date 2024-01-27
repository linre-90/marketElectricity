#ifndef WEB_H
#define WEB_H

/* Initialize web request*/
void initWeb(void);

void cleanWeb(void);

/* Get price for single hour. Returns price in euros. */
float fetchSingleHourPrice(unsigned long hourUnixTime);

#endif // !WEB_H
