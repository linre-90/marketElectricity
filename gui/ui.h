#ifndef UI_HEADER
#define UI_HEADER

#include "viewModel.h"
#include <stdbool.h>

/* Forward dec price */

struct Price;

/* Initialize gui library */
void initGui(const char* const applicationName);

/* Clean Gui library */
void stopGui(void);

/* Detect if user wants to close window. */
bool detectWindowShouldClose();

/* Draw full gui. */
void drawGui(const ViewModel* const viewModel, const char* const version, bool isLoading, bool isError);


#endif // !UI_HEADER
