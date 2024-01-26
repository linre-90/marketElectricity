#ifndef UI_HEADER
#define UI_HEADER
#include <time.h>
#include "raylib.h"

/* View model used in app */
typedef struct {
    float currHour;
    float nextHour;
    float history[24];
    int hourList[24];
    unsigned long nextDataUpdateStamp;
} ViewModel;

/* Initialize gui library */
void initGui(const char* applicationName);

/* Clean Gui library */
void stopGui(void);

/* Draw full gui. */
void drawGui(ViewModel* viewModel, time_t* dateTime, const char* version, bool isLoading);

/* Draws time related gui box */
void drawGuiTime(time_t* t);

/* Draws prices related gui box. */
void drawGuiPrices(ViewModel* viewModel);

/* Draws gui history graph box. */
void drawGuiHistory(ViewModel* viewModel, time_t* t);

/* Draws gui version stamp box. */
void drawGuiVersion(const char* version);

/* Draws gui background. */
void drawGuiBackground(void);

/* Draws loading screen background. */
void drawLoadingGui(void);

#endif // !UI_HEADER
