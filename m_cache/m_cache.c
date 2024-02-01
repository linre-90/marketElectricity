#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "m_cache.h"
#include "viewModel.h"
#include "web.h"
#include "logger.h"

/* ############# Typedef & constants ############ */
#define DEBUG_PRINTS 0						// Print debug messages
const char* CACHE_FILE = "cache.mecache";	// File name where cache is stored
const uint32_t MAGIC_NUMBER = 69420;		// Magic number to identify file


/* ############# Implementation ############ */

int writeCache(const ViewModel* const viewModel) {
	FILE* file = fopen(CACHE_FILE, "wb");

	if (file == NULL) {
		Llog(L_ERR_CF, L_FILE_ERR, "File null pointer in: writeCache()");
		return -1;
	}
	// Write magic number to file
	size_t magicNumSize = fwrite(&MAGIC_NUMBER, sizeof(uint32_t), 1, file);
	// Next update to file
	size_t nextUpdateSize = fwrite(&viewModel->nextDataUpdateStamp, sizeof(time_t), 1, file);
	// Write price arr to file
	size_t priceArrSize = fwrite(&viewModel->priceArr[0], sizeof(viewModel->priceArr[0]) * NUM_OF_API_RESULTS, 1, file);

	if (magicNumSize != 1 || nextUpdateSize != 1 || priceArrSize != 1) {
		fclose(file);
		Llog(L_ERR_CF, L_FILE_ERR, "Error writing data: writeCache()");
		return -1;
	}

#if DEBUG_PRINTS == 1
	printf("Write done\n");
#endif // DEBUG_PRINTS == 1

	fclose(file);
	return 0;
}

int readCache(ViewModel* const out_viewModel) {
	FILE* file = fopen(CACHE_FILE, "rb");
	if (file == NULL) {
		Llog(L_ERR_CF, L_FILE_ERR, "File null pointer in: readCache()");
		return -1;
	}

	// read magic number to identify that file is created by this program
	uint32_t magicNum = 0;
	fread(&magicNum, sizeof(uint32_t), 1, file);
	if (magicNum != MAGIC_NUMBER) {
		Llog(L_ERR_CF, L_FILE_ERR, "Invalid magic number in: readCache().");
		fclose(file);
		return -1;
	}

	// read update time stamp
	time_t nextUpdateStamp = 0;
	fread(&nextUpdateStamp, sizeof(time_t), 1, file);
	if (nextUpdateStamp != 0 && nextUpdateStamp - time(NULL) <= 0) {
		Llog(L_ERR_CF, L_FILE_ERR, "Cache data is too old in: readCache().");
		fclose(file);
		return -1;
	}
	out_viewModel->nextDataUpdateStamp = nextUpdateStamp;

	// Read price arr to view model
	size_t readPricesBytes = fread(&out_viewModel->priceArr[0], sizeof(out_viewModel->priceArr[0]) * NUM_OF_API_RESULTS, 1, file);
	if (readPricesBytes != 1) {
		Llog(L_ERR_CF, L_FILE_ERR, "Cannot read all price data: readCache().");
		fclose(file);
		return -1;
	}

#if DEBUG_PRINTS == 1
	printf("Magic num: %d\n", magicNum);
	printf("Update   : %lu\n", nextUpdateStamp);
	for (int i = 0; i < NUM_OF_API_RESULTS; i++)
	{
		printf("Price:%f , Time:%s\n", out_viewModel->priceArr[i].price, asctime(gmtime(&out_viewModel->priceArr[i].utcTime)));
	}
#endif

	fclose(file);
	return 0;
}


void cleanCache(void) {
	// Check if the cache file exists
	FILE* filePtr = fopen(CACHE_FILE, "r");
	if (filePtr == NULL) {
		Llog(L_MSG_CF, L_NONE_ERR, "No cache to clean: cleanCache()");
		return;
	}

	// File exists delete it.
	fclose(filePtr);
	int success = remove(CACHE_FILE);
	if (success != 0) {
		Llog(L_ERR_CF, L_FILE_ERR, "Cannot remove cache file: cleanCache().");
	}
}
