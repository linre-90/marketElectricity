#ifndef M_CACHE
#define M_CACHE

/* Forward dec */

struct ViewModel;

/* Write viewmodel items to cache file on disk. */
void writeCache(ViewModel* viewModel);

/* Read view model items from cache. */
void readCache(ViewModel* out_viewModel);

#endif