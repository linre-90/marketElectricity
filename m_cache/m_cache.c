#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "m_cache.h"
#include "viewModel.h"
#include "web.h"


/* ############# Typedef & constants ############ */
#define DEBUG_PRINTS 0						// Print debug messages
const char* CACHE_FILE = "cache.mecache";	// File name where cache is stored
const uint32_t MAGIC_NUMBER = 69420;		// Magic number to identify file


/* ############# Implementation ############ */

int writeCache(ViewModel* viewModel) {
	FILE* file = fopen(CACHE_FILE, "wb");
	if (file == NULL) {
		printf("\nFile null pointer in: writeCache()\n");
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
		printf("\nError writing data: writeCache()\n");
		return 0;
	}

#if DEBUG_PRINTS == 1
	printf("Write done\n");
#endif // DEBUG_PRINTS == 1

	fclose(file);
	return 0;
}

int readCache(ViewModel* out_viewModel) {
	FILE* file = fopen(CACHE_FILE, "rb");
	if (file == NULL) {
		printf("\nFile null pointer in: readCache()\n");
		return -1;
	}

	// read magic number to identify that file is created by this program
	uint32_t magicNum = 0;
	fread(&magicNum, sizeof(uint32_t), 1, file);
	if (magicNum != MAGIC_NUMBER) {
		printf("\nInvalid magic number in: readCache(). Received: %d instead of %d.\n", magicNum, MAGIC_NUMBER);
		fclose(file);
		return -1;
	}

	// read update time stamp
	time_t nextUpdateStamp = 0;
	fread(&nextUpdateStamp, sizeof(time_t), 1, file);
	if (nextUpdateStamp != 0 && nextUpdateStamp - time(NULL) <= 0) {
		printf("\nCache data is too old in: readCache().\n");
		fclose(file);
		return -1;
	}
	out_viewModel->nextDataUpdateStamp = nextUpdateStamp;

	// Read price arr to view model
	size_t readPricesBytes = fread(&out_viewModel->priceArr[0], sizeof(out_viewModel->priceArr[0]) * NUM_OF_API_RESULTS, 1, file);
	if (readPricesBytes != 1) {
		printf("\nCannot read all price data: readCache().\n");
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