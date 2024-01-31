#ifndef UI_HEADER
#define UI_HEADER
#include "viewModel.h"
#include <stdbool.h>

/* Forward dec */

struct Price;


/* Initialize gui library */
void initGui(const char* const applicationName);

/* Clean Gui library */
void stopGui(void);

bool detectWindowShouldClose();

/* Draw full gui. */
void drawGui(const ViewModel* const viewModel, const char* const version, bool isLoading);

/* Draws time related gui box */
void drawGuiTime();

/* Draws current and next hour price related gui box. */
void drawGuiPrices(const ViewModel* const viewModel);

/* Draws gui todays price graph box. */
void drawGuiDatePrices(const ViewModel* const viewModel);

/* Draws gui version stamp box. */
void drawGuiVersion(const char* const version);

/* Draws gui background. */
void drawGuiBackground(void);

/* Draws loading screen background. */
void drawLoadingGui(void);

#endif // !UI_HEADER
