#ifndef M_CACHE
#define M_CACHE

#include "viewModel.h"

/* Write viewmodel next update and price array to cache file on disk. 
Returns 0 if operation is succesfull, other values indicate error.
*/
int writeCache(const ViewModel* const viewModel);


/* Read view model items from cache file. Validates next update stamp and returns either -1 if old.
Returns 0 if operations is successfull, other values indicate error. 
*/
int readCache(ViewModel* const out_viewModel);

/* Removes cached data. */
void cleanCache(void);


#endif