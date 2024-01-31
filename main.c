#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "ui.h"
#include "m_utils.h"
#include "web.h"
#include "viewModel.h"
#include "m_cache.h"


/************** Defines **********************/

// #### Application run/compile settings
#define APP_DEV false                                   /* Speeds up time */
#define APP_ENABLE_CACHE true                           /* Enable or disable cache */
#define DISABLE_WIN_CONSOLE true                        /* Disable console on windows */

// #### program constants
#define APPLICATION_NAME  "Market electricity watcher"  /* Program title */
#define VERSION "V: 0.5.0"                              /* Program version */
#define HOUR 3600                                       /* one hour in unix seconds */
#define DEV_HOUR 10                                     /* accelarated dev hour to speed up development unix time */

// Disable console window underneath gui
#if DISABLE_WIN_CONSOLE == true
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif


/************** Forwrd dec **********************/

/* Perform view model update. Updates data, curr hour index and next update time. */
void updateViewModel(ViewModel* viewModel);

/* Set or update viewmodel current hour index. Hour indicates which data is relevant for current time. */
void setViewModelHourIndexes(ViewModel* viewModel);

/* Set or update viewmodel data fetch update. New value will be: now + 12hours.*/
void setViewModelUpdate(ViewModel* viewModel);


/**************** The program *************************/

int main(void) {
    // Init view model
    ViewModel viewModel = { 0 };
    viewModel.priceArr = (struct Price*)malloc(sizeof(struct Price) * NUM_OF_API_RESULTS);
    if (viewModel.priceArr == NULL) {
        printf("\nError reserving memory in start.!\n");
        exit(1);
    }

#if APP_ENABLE_CACHE == true
    int validCache = readCache(&viewModel);
    if (validCache == 0) {
        // Use cached data
        printf("\nStart with cached values!\n\n");
        setViewModelHourIndexes(&viewModel);
    }
    else {
        // Start program with full viewmodel and data reset.
        printf("\nStart with full reset values!\n\n");
        updateViewModel(&viewModel);
    }
#endif
#if APP_ENABLE_CACHE == false
    printf("\nStart with full reset values! Cache is disabled.\n\n");
    updateViewModel(&viewModel);
#endif

    // init gui
    initGui(APPLICATION_NAME);

    // Program loop
    while (!detectWindowShouldClose())
    {
        // Data fetch update check
        if (viewModel.nextDataUpdateStamp - time(NULL) <= 0) {
            drawGui(&viewModel, VERSION, true);
            updateViewModel(&viewModel);
        }
        // Update active hour and ui
        setViewModelHourIndexes(&viewModel);
        drawGui(&viewModel, VERSION, false);
    }

    // Clean up
    free(viewModel.priceArr);
    stopGui();

	return 0;
}

void updateViewModel(ViewModel* viewModel) {
    fetchData(viewModel->priceArr);
    setViewModelHourIndexes(viewModel);
    setViewModelUpdate(viewModel);
#if APP_ENABLE_CACHE == true
    writeCache(viewModel);
#endif
}

void setViewModelHourIndexes(ViewModel* viewModel) {
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
