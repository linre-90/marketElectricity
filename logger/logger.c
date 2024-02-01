#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "logger.h"

// ########## const ############

/* Array containing default error messsage. 
Must be kept in sync with "LOGGER_ERR_CODE_*" variables in top of header file. Error code equals index.
*/
const char* defaultErrMesg[LOGGER_ERR_COUNT] = {
	"Fetching data failed. Check internet connection or service is unavailable.",
	"Cached data cannot be used.",
	"Updating data failed. Check internet connection or service is unavailable.",
	"Memory error. Memory cannot be reserved.",
	"File error. File cannot be operated on."
};

/* Log file file name */
const char* LOG_FILE = "me_logs.txt";


// ########## Forweard dec ############

/* Write Log message to log file. */
void writeLogFile(int errCode, const char* const msg);

/* Print formatted errors.
[msg] is optional and if NULL default error message for code is used.
Writes to log file.
*/
void log_err(int errCode, const char* const msg);

/* Print formatted message on console.*/
void log_msg(const char* const msg);

/* Print formatted message to console and to log file.*/
void log_f_msg(const char* const msg);


// ########## Implementations ############

void Llog(L_LOG_LEVEL logLevel, L_ERROR_CODE errCode, const char* const msg){
	switch (logLevel)
	{
	case L_ERR_CF:
		log_err(errCode, msg);
		break;
	case L_MSG_C:
		log_msg(msg);
		break;
	case L_MSG_CF:
		log_f_msg(msg);
		break;
	default:
		break;
	}
}

void log_err(int errCode, const char* const msg) {
	if (msg == NULL) {
		if (errCode < 0 || errCode >= LOGGER_ERR_COUNT) {
			printf("ERROR: code: %d, message: %s\n", errCode, "Unknown error code.");
			writeLogFile(errCode, "Unknown error code.");
		}
		else {
			printf("ERROR: code: %d, message: %s\n", errCode, defaultErrMesg[errCode]);
			writeLogFile(errCode, defaultErrMesg[errCode]);
		}
	}
	else {
		printf("ERROR: code: %d, message: %s\n", errCode, msg);
		writeLogFile(errCode, msg);
	}
}

void log_msg(const char* const msg) {
	if (msg != NULL) {
		printf("MESSAGE: %s\n", msg);
	}
	else {
		printf("MESSAGE: Unknown message.\n");
	}
}

void log_f_msg(const char* const msg) {
	if (msg != NULL) {
		printf("MESSAGE_F: %s\n", msg);
		writeLogFile(-1, msg);
	}
	else {
		printf("MESSAGE_F: Unknown message.\n");
		writeLogFile(-1, "Unknown message.");
	}
}

void writeLogFile(int errCode, const char* const msg) {
	// Open file handle exit on error
	FILE* logFilePtr = fopen(LOG_FILE, "a");
	if (logFilePtr == NULL) { return; }


	// Convert errCode and msg to single string
	size_t logStringLength = strlen(msg) + sizeof(errCode) + sizeof(int) * 4 + sizeof(char) * 2;
	char* logString = (char*)calloc(1 , logStringLength);
	if (logString == NULL) {
		fclose(logFilePtr);
		return;
	}

	// Format, write and clean up, ignore write errors it happens if it happens
	sprintf(logString, "LOG: code: %d, msg: %s\n", errCode, msg);
	fputs(logString, logFilePtr);
	fclose(logFilePtr);
	free(logString);
}
