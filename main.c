#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

//#include "cee_event.h"
#include "ui.h"
#include "raylib.h"
#include "m_utils.h"
#include "styles.h"
#include "web.h"

/************** Defines **********************/

// #### Application settings
#define APP_DEV true
#define GENERATE_RANDOM_DATA true
#define GENERATE_SAMPLE_DATA false
#define ALLOW_API_REQUESTS true

// #### program constants
#define VERSION "V: 0.1.0"
#define APPLICATION_NAME  "Market electricity watcher"
#define HOUR 3600       // one hour in unix seconds
#define MINUTE 60       // one minute in unix seconds
#define DEV_HOUR 10   // accelarated dev hour to speed up development unix time

/************** Forwrd dec **********************/

/* Update view model */
void updateViewModel(ViewModel* viewModel, time_t* t, unsigned int nextUpdateStamp);

/* Initialize view model */
void initViewModel(ViewModel* viewModel, time_t* t, unsigned int nextUpdateStamp);

/* Get current hour in unix time */
time_t calculateCurrentHour(void);

/* Get next hour */
time_t calculateNextHour(void);

/* Populate Viewmodel hour list with hours*/
void generateHours(ViewModel* viewModel, time_t* t);


/**************** Magic *************************/
int main(void) {
    ViewModel viewModel = { 0 };
    time_t dateTime = time(NULL);

    unsigned long interval = HOUR + MINUTE;
    unsigned long nextViewModelUpdate = calculateCurrentHour() + interval;
   
#if APP_DEV == true
    srand(time(NULL));
    // Speed up development 
    interval = DEV_HOUR;
    nextViewModelUpdate = time(NULL) + interval;
#endif

    // Init thingies
    initViewModel(&viewModel, &dateTime, nextViewModelUpdate);
    initGui(APPLICATION_NAME);
    initWeb();


    while (!WindowShouldClose())
    {
        if (nextViewModelUpdate - time(NULL) <= 0) {
            drawGui(&viewModel, &dateTime, VERSION, true);
            nextViewModelUpdate = calculateCurrentHour() + interval;
#if APP_DEV == true
            // Speed time up
            nextViewModelUpdate = time(NULL) + interval;
#endif // APP_DEV == true
            updateViewModel(&viewModel, &dateTime, nextViewModelUpdate);
        }
        drawGui(&viewModel, &dateTime, VERSION, false);
    }

    // Clean up
    cleanWeb();
    stopGui();

	return 0;
}


void initViewModel(ViewModel* viewModel, time_t* t, unsigned int nextUpdateStamp) {
#if GENERATE_RANDOM_DATA == true && ALLOW_API_REQUESTS == false
    generateRandomValues(viewModel);
#endif // GENERATE_RANDOM_DATA == true
#if GENERATE_SAMPLE_DATA == true && ALLOW_API_REQUESTS == false
    generateValues(viewModel);
#endif // GENERATE_SAMPLE_DATA == true
    viewModel->nextDataUpdateStamp = nextUpdateStamp;

#if ALLOW_API_REQUESTS == false
    viewModel->currHour = (float)rand() / (float)(RAND_MAX / 3.5f);
    viewModel->nextHour = (float)rand() / (float)(RAND_MAX / 3.5f);
#else
    viewModel->currHour = fetchSingleHourPrice(calculateCurrentHour());
    viewModel->currHour = fetchSingleHourPrice(calculateNextHour());
#endif
    generateHours(viewModel, t);
}


void updateViewModel(ViewModel* viewModel, time_t* t, unsigned int nextUpdateStamp) {
    insertLast_F(viewModel->history, 24, viewModel->currHour);
    viewModel->nextDataUpdateStamp = nextUpdateStamp;
    viewModel->currHour = viewModel->nextHour;
#if ALLOW_API_REQUESTS == false
        viewModel->nextHour = (float)rand() / (float)(RAND_MAX / 3.5f);
#else
    viewModel->nextHour = fetchSingleHourPrice(calculateNextHour());
#endif
    generateHours(viewModel, t);
}

time_t calculateCurrentHour(void) {
    time_t t;
    time(&t);
    return t - (t % 3600);
}

time_t calculateNextHour(void) {
    time_t t;
    time(&t);
    t += 3600;
    return t - (t % 3600);
}

void generateHours(ViewModel* viewModel, time_t* time) {
    int t = localtime(time)->tm_hour;

    for (int j = 23; j > -1; j--)
    {
        if (t == 0) {
            t = 23;
        }
        else {
            t--;
        }
        viewModel->hourList[j] = t;
    }
}
