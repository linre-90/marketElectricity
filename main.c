#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "ui.h"
#include "m_utils.h";
#include "web.h"
#include "viewModel.h";

/************** Defines **********************/

// #### Application settings
#define APP_DEV false                                   /* Speeds up time */

// #### program constants
#define VERSION "V: 0.1.0"                              /* Program version */
#define APPLICATION_NAME  "Market electricity watcher"  /* Program title */
#define HOUR 3600                                       /* one hour in unix seconds */
#define DEV_HOUR 10                                     /* accelarated dev hour to speed up development unix time */

/************** Forwrd dec **********************/

/* Perform view model update. Updates data, curr hour index and next update time. */
void updateViewModel(ViewModel* viewModel);

/* Set or update viewmodel current hour index. Hour indicates which data is relevant for current time. */
void setCurrentHourIndex(ViewModel* viewModel);

/* Set or update viewmodel data fetch update. New value will be: now + 12hours.*/
void setViewModelUpdate(ViewModel* viewModel);


/**************** The program *************************/
int main(void) {
    // Init thingies
    ViewModel viewModel = { 0 };
    viewModel.priceArr = (struct Price*)malloc(sizeof(struct Price) * NUM_OF_API_RESULTS);
    updateViewModel(&viewModel);
    initGui(APPLICATION_NAME);

    // Program loop
    while (!WindowShouldClose())
    {
        // Data fetch update check
        if (viewModel.nextDataUpdateStamp - time(NULL) <= 0) {
            drawGui(&viewModel, VERSION, true);
            updateViewModel(&viewModel);
        }
        // Update active hour and ui
        setCurrentHourIndex(&viewModel);
        drawGui(&viewModel, VERSION, false);
    }

    // Clean up
    free(viewModel.priceArr);
    stopGui();

	return 0;
}

void updateViewModel(ViewModel* viewModel) {
    fetchData(viewModel->priceArr);
    setCurrentHourIndex(viewModel);
    setViewModelUpdate(viewModel);
}

void setCurrentHourIndex(ViewModel* viewModel) {
    // Loop over data until current unix time collapsed to hour is found.
    for (int i = 0; i < NUM_OF_API_RESULTS; i++)
    {
        // viewmodel price seconds are always 0 so this works.
        if (viewModel->priceArr[i].utcTime - calculateCurrentHour() == 0)
        {
            viewModel->currHourIndex = i;
            // Next hour data is n - 1
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


