#ifndef UI_HEADER
#define UI_HEADER
#include <time.h>
#include "../web.h"
#include "raylib.h"


/* View model used in app */
typedef struct {
    struct Price* priceArr;
    int currHourIndex;
    int NextHourIndex;
    unsigned long nextDataUpdateStamp;


    //float currHour;
    //float nextHour;
    //float history[24];
    //int hourList[24];
} ViewModel;

/* Initialize gui library */
void initGui(const char* applicationName);

/* Clean Gui library */
void stopGui(void);

/* Draw full gui. */
void drawGui(ViewModel* viewModel, const char* version, bool isLoading);

/* Draws time related gui box */
void drawGuiTime();

/* Draws prices related gui box. */
void drawGuiPrices(ViewModel* viewModel);

/* Draws gui history graph box. */
void drawGuiDatePrices(ViewModel* viewModel);

/* Draws gui version stamp box. */
void drawGuiVersion(const char* version);

/* Draws gui background. */
void drawGuiBackground(void);

/* Draws loading screen background. */
void drawLoadingGui(void);

#endif // !UI_HEADER
