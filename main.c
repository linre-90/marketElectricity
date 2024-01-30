#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "ui.h"
#include "m_utils.h";
#include "web.h"

/************** Defines **********************/

// #### Application settings
#define APP_DEV false
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

/* Initialize view model */
void updateViewModel(ViewModel* viewModel);

/* Set viewmodel current hour index. */
void setCurrentHourIndex(ViewModel* viewModel);

/* Sets viewmodel data fetch update to now + 12 hours */
void setViewModelUpdate(ViewModel* viewModel);

/* Get current hour in unix time */
time_t calculateCurrentHour(void);

/* Get next hour */
time_t calculateNextHour(void);


/**************** Magic *************************/
int main(void) {
    ViewModel viewModel = { 0 };
    time_t dateTime = time(NULL);
   
    // Init thingies
    viewModel.priceArr = (struct Price*)malloc(sizeof(struct Price) * NUM_OF_API_RESULTS);
    updateViewModel(&viewModel);
    initGui(APPLICATION_NAME);

    // Program loop
    while (!WindowShouldClose())
    {
        if (viewModel.nextDataUpdateStamp - time(NULL) <= 0) {
            drawGui(&viewModel, VERSION, true);
            updateViewModel(&viewModel);
        }

        setCurrentHourIndex(&viewModel);
        drawGui(&viewModel, VERSION, false);
    }

    free(viewModel.priceArr);
    // Clean up
    stopGui();

	return 0;
}

void updateViewModel(ViewModel* viewModel) {
    fetchData(viewModel->priceArr);
    setCurrentHourIndex(viewModel);
    setViewModelUpdate(viewModel);
}

void setCurrentHourIndex(ViewModel* viewModel) {
    time_t t = calculateCurrentHour();
    int currentUtcHour = gmtime(&t)->tm_hour;

    for (int i = 0; i < NUM_OF_API_RESULTS; i++)
    {
        if (viewModel->priceArr[i].utcTime - calculateCurrentHour(time(NULL)) == 0)
        {
            viewModel->currHourIndex = i;
            if (i == 0) {
                viewModel->NextHourIndex = -1;
            }
            else {
                viewModel->NextHourIndex = i - 1;
            }
            return;
        }
    }
    printf("\nError finding curent index from ViewModel data. Current hour does not exists.\n");
}

void setViewModelUpdate(ViewModel* viewModel) {
    time_t t = time(NULL);
#if APP_DEV
    viewModel->nextDataUpdateStamp = t + DEV_HOUR;
    return;
#endif
    viewModel->nextDataUpdateStamp = t + HOUR * 12;
}

time_t calculateNextHour(void) {
    time_t t;
    time(&t);
    t += 3600;
    return t - (t % 3600);
}

