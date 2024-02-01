#ifndef M_LOGGER
#define M_LOGGER

#include <stdio.h>

/* Number of available error codes L_ERROR_CODE enum, that are linked to default message arr defined in logger.c. */
#define LOGGER_ERR_COUNT 5

/* Available error codes */
typedef enum {
	// No error
	L_NONE_ERR = -1,
	// Network fetch error
	L_FETCH_ERR,
	// Cache read or write error
	L_CACHE_ERR,
	// Update error
	L_UPDATE_ERR,
	// Memory allocation error
	L_MEMORY_ERR,
	// File error
	L_FILE_ERR,
} L_ERROR_CODE;

/* Log level */
typedef enum {
	/* Error logging to console and file */
	L_ERR_CF,
	/* Message logging to console */
	L_MSG_C,
	/* Message logging to console and file */
	L_MSG_CF
} L_LOG_LEVEL;


/* Log messages to file or console. */
void Llog(L_LOG_LEVEL logLevel, L_ERROR_CODE errCode, const char* const msg);

#endif