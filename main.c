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
#include "logger.h"


//################## Defines ##################

// #### Application run/compile settings
#define APP_DEV false                                   /* Speeds up time */
#define APP_ENABLE_CACHE true                           /* Enable or disable cache */
#define DISABLE_WIN_CONSOLE true                        /* Disable console on windows */

// #### program constants
#define APPLICATION_NAME  "Market electricity watcher"  /* Program title */
#define VERSION "V: 1.0.0"                              /* Program version */
#define HOUR 3600                                       /* one hour in unix seconds */
#define DEV_HOUR 10                                     /* accelarated dev hour to speed up development unix time */

// Disable console window underneath gui
#if DISABLE_WIN_CONSOLE == true
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif


// ################## Forwrd dec ##################

/* Perform view model update. Updates data, curr hour index and next update time. 
Returns true if operations succeed and false otherwise.
*/
bool updateViewModel(ViewModel* const viewModel);

/* Set or update viewmodel current hour index. Hour indicates which data is relevant for current time.
Returns true if operations succeed and false otherwise.
*/
bool setViewModelHourIndexes(ViewModel* const viewModel);

/* Set or update viewmodel data fetch update. New value will be: now + 12hours.*/
void setViewModelUpdate(ViewModel* const viewModel);

/* Log program start up time and variable values. */
void logProgramStartUp();


// ################## The program ##################
int main(void) {
    logProgramStartUp();

    // Init view model
    ViewModel viewModel = { 0 };
    viewModel.priceArr = (struct Price*)malloc(sizeof(struct Price) * NUM_OF_API_RESULTS);
    if (viewModel.priceArr == NULL) {
        Llog(L_ERR_CF, L_MEMORY_ERR, NULL);
        exit(1);
    }
    bool dataUpdateSucceeded = true;
    bool dataUpdateErrorHandled = false;

#if APP_ENABLE_CACHE == true
    int validCache = readCache(&viewModel);
    if (validCache == 0) {
        // Use cached data
        Llog(L_MSG_CF, L_NONE_ERR, "Start with cached values");
        dataUpdateSucceeded = setViewModelHourIndexes(&viewModel);
    }
    else {
        // Start program with full viewmodel and data reset.
        Llog(L_MSG_CF, L_NONE_ERR, "Start with full reset values");
        dataUpdateSucceeded = updateViewModel(&viewModel);
    }
#endif
#if APP_ENABLE_CACHE == false
    Llog(L_MSG_CF, L_NONE_ERR, "Start with full reset values! Cache is disabled.");
    dataUpdateSucceeded = updateViewModel(&viewModel);
#endif

    // init gui
    initGui(APPLICATION_NAME);

    // Program loop
    while (!detectWindowShouldClose())
    {
        // Check errors, error message should be printed by function.
        if (!dataUpdateSucceeded) {
            if (!dataUpdateErrorHandled) {
                dataUpdateErrorHandled = true;
                cleanCache();
            }
            drawGui(&viewModel, VERSION, false, true);
            continue; 
        }

        // Data fetch and update check
        if (viewModel.nextDataUpdateStamp - time(NULL) <= 0) {
            // draw gui with loading flag set to true
            drawGui(&viewModel, VERSION, true, false);
            dataUpdateSucceeded = updateViewModel(&viewModel);
        }
        // Update active hour and ui
        dataUpdateSucceeded = setViewModelHourIndexes(&viewModel);
        drawGui(&viewModel, VERSION, false, false);
    }

    // Clean up
    free(viewModel.priceArr);
    stopGui();
	return 0;
}

bool updateViewModel(ViewModel* const viewModel) {
    bool callSuccess = false;
    callSuccess = fetchData(viewModel->priceArr);
    if (!callSuccess) { 
        Llog(L_ERR_CF, L_FETCH_ERR, "Fetch data error on: updateViewModel()");
        return false; 
    }

    callSuccess = setViewModelHourIndexes(viewModel);
    if (!callSuccess) { 
        Llog(L_ERR_CF, L_UPDATE_ERR, "Hour index set error on: updateViewModel()");
        return false; 
    }

    setViewModelUpdate(viewModel);

#if APP_ENABLE_CACHE == false
    return true;
#endif
#if APP_ENABLE_CACHE == true
    int cacheWriteOK = writeCache(viewModel);
    if (cacheWriteOK != 0) {
        Llog(L_ERR_CF, L_CACHE_ERR, "Cache write error on: updateViewModel()");
        return false;
    }
    else {
        return true;
    }
#endif
}

bool setViewModelHourIndexes(ViewModel* const viewModel) {
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
            return true;
        }
    }
    Llog(L_ERR_CF, L_UPDATE_ERR, "Error finding current index from ViewModel data. Current hour does not exists. On setViewModelHourIndexes().");
    return false;
}

void setViewModelUpdate(ViewModel* const viewModel) {
    time_t t = time(NULL);
#if APP_DEV
    viewModel->nextDataUpdateStamp = t + DEV_HOUR;
    return;
#endif
    viewModel->nextDataUpdateStamp = t + HOUR * 12;
}

void logProgramStartUp() {
    // Log seperator to file to keep formatting readable.
    Llog(L_MSG_CF, L_NONE_ERR, "--------------------------");

    // init char buffer and time
    char startUpStrsBuffer[100] = { 0 };
    time_t startup_t = time(NULL);

    // Log launch time
    char* timeStampStr = asctime(gmtime(&startup_t));
    if (timeStampStr != NULL && timeStampStr[strlen(timeStampStr - 1)] == '\n') {
        // Remove newline from asctime string
        timeStampStr[strlen(timeStampStr - 1)] = '\0';
    }
    sprintf(startUpStrsBuffer, "Program startup (UTC)=%s", timeStampStr);
    Llog(L_MSG_CF, L_NONE_ERR, startUpStrsBuffer);

    // Log version
    memset(startUpStrsBuffer, 0, 100);
    sprintf(startUpStrsBuffer, "Version=%s", VERSION);
    Llog(L_MSG_CF, L_NONE_ERR, startUpStrsBuffer);

    // Log APP_DEV value
    memset(startUpStrsBuffer, 0, 100);
    sprintf(startUpStrsBuffer, "APP_DEV=%s", APP_DEV == true ? "true" : "false");
    Llog(L_MSG_CF, L_NONE_ERR, startUpStrsBuffer);

    // Log APP_ENABLE_CACHE value
    memset(startUpStrsBuffer, 0, 100);
    sprintf(startUpStrsBuffer, "APP_ENABLE_CACHE=%s", APP_ENABLE_CACHE == true ? "true" : "false");
    Llog(L_MSG_CF, L_NONE_ERR, startUpStrsBuffer);

    // Log DISABLE_WIN_CONSOLE value
    memset(startUpStrsBuffer, 0, 100);
    sprintf(startUpStrsBuffer, "DISABLE_WIN_CONSOLE=%s", DISABLE_WIN_CONSOLE == true ? "true" : "false");
    Llog(L_MSG_CF, L_NONE_ERR, startUpStrsBuffer);
}
