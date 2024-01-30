#ifndef UI_HEADER
#define UI_HEADER
#include "viewModel.h"

/* Forward dec */

struct Price;

/* View model contains all the relevant data that ui needs to function properly. */
//typedef struct {
//    struct Price* priceArr;
//    int currHourIndex;
//    int NextHourIndex;
//    unsigned long nextDataUpdateStamp;
//} ViewModel;


/* Initialize gui library */
void initGui(const char* applicationName);

/* Clean Gui library */
void stopGui(void);

/* Draw full gui. */
void drawGui(ViewModel* viewModel, const char* version, bool isLoading);

/* Draws time related gui box */
void drawGuiTime();

/* Draws current and next hour price related gui box. */
void drawGuiPrices(ViewModel* viewModel);

/* Draws gui todays price graph box. */
void drawGuiDatePrices(ViewModel* viewModel);

/* Draws gui version stamp box. */
void drawGuiVersion(const char* version);

/* Draws gui background. */
void drawGuiBackground(void);

/* Draws loading screen background. */
void drawLoadingGui(void);

#endif // !UI_HEADER
