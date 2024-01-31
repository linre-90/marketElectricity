#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>
#include "web.h"

/* Memory for holding curl response. */
struct MemoryStruct {
	char* memory;
	size_t size;
};

/* Callback to write json response in MemoryStruct, resizes dynamically. */
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);

/* Converts json string to struct Price array. */
void parseJsonResponse(const char* res, struct Price* const out_PriceArr);


void fetchData(struct Price* const out_PriceArr) {
	// Init curl
	curl_global_init(CURL_GLOBAL_ALL);
	CURL* curl = curl_easy_init();
	CURLcode res;
	// If curl pointer is not null
	if (curl) {
		// reserve memory chunk
		struct MemoryStruct chunk;
		chunk.memory = malloc(1); // Resized in WriteMemoryCallback
		chunk.size = 0; // nothing is stored

		if (chunk.memory == NULL) {
			printf("Cannot allocate memory for chunk.memory.\n");
			curl_global_cleanup();
			return;
		}

		// Prepare request url
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.porssisahko.net/v1/latest-prices.json");
		// response data is sent to WriteMemoryCallback function
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		// chunk struct is passed to WriteMemoryCallback
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
		// Perform request
		res = curl_easy_perform(curl);
		// Result code is not 200
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else {
			// Succesfully retrieved data
			parseJsonResponse(chunk.memory, out_PriceArr);
		}
		// free mem and clean up curl.
		free(chunk.memory);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}
}


void parseJsonResponse(const char* res, struct Price* const out_priceArr) {
	// Skip the opening { and jump right in [ character
	int iterator = 10;
	int priceObjIterator = 0;
	// Loop over the whole res after skipping
	while (res[iterator] != 0) {
		// start parsing object when ever '{' character is found.
		if (res[iterator] == '{') {
			char priceStr[10] = { 0 };	// string buff to store price
			char utcYYYYStr[5] = { 0 }; // string buff to store year
			char utcDDStr[3] = { 0 };	// string buff to store day
			char utcMMStr[3] = { 0 };	// string buff to store month
			char utcHH[3] = { 0 };		// string buff to store hours
			int priceStrIterator = 0;	// Prices change in length that is why this is needed

			// Move forward to 'p' ([p]rice) character
			while (res[iterator] != 'p' && res[iterator] != 0){ iterator++; }
			iterator++;

			// Move forward to first number digit or '-' (prices can be negative)
			while (res[iterator] != ':' && res[iterator] != 0) { iterator++; }
			iterator++;

			// Start extracting price until ',' is found, prices do not contain quotes.
			while (res[iterator] != ',' && res[iterator] != 0)
			{
				priceStr[priceStrIterator] = res[iterator];
				iterator++;
				priceStrIterator++;
			}

			// Set price to struct convert cents to eur
			out_priceArr[priceObjIterator].price = (float)atof(priceStr);

			// Move forward to 's' ([s]tartDate) character and parse date
			while (res[iterator] != 's' && res[iterator] != 0) { iterator++; }
			iterator++;

			// Fast forward to first year digit
			while (res[iterator] != ':' && res[iterator] != 0) { iterator++; }
			iterator += 2;
			utcYYYYStr[0] = res[iterator];
			utcYYYYStr[1] = res[iterator + 1];
			utcYYYYStr[2] = res[iterator + 2];
			utcYYYYStr[3] = res[iterator + 3];
			iterator += 3;

			// Skip - and jump to first month digit
			iterator += 2;
			utcMMStr[0] = res[iterator];
			utcMMStr[1] = res[iterator + 1];

			// Skip - and jump to first day digit
			iterator += 3;
			utcDDStr[0] = res[iterator];
			utcDDStr[1] = res[iterator + 1];
			
			// Skip t and jump to first hour digit
			iterator += 3;
			utcHH[0] = res[iterator];
			utcHH[1] = res[iterator + 1];

			// Build up unix time stamp from parsed strings.
			struct tm m_time = {
				.tm_year = atoi(utcYYYYStr) - 1900,
				.tm_mon = atoi(utcMMStr) - 1 < 0 ? 0 : atoi(utcMMStr) - 1,
				.tm_mday = atoi(utcDDStr),
				.tm_hour = atoi(utcHH) + 2,// Push forward two hours so gmtime function returns correct utc time.
				.tm_min = 0,
				.tm_sec = 0,
				.tm_isdst = -1,
			};

			// Make time and add it to price
			time_t utc = mktime(&m_time);
			out_priceArr[priceObjIterator].utcTime = utc;
			

			// Object parsing is finished not interested in the rest of the object properties
			priceObjIterator++;
		}

		// Just move along
		iterator++;
	}
}


static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	// Cast userp to correct type
	struct MemoryStruct* mem = (struct MemoryStruct*)userp;

	// Realloc data + realsize + 1
	char* ptr = realloc(mem->memory, mem->size + realsize + 1);
	if (!ptr) {
		/* out of memory! */
		printf("Not enough memory (realloc returned NULL)\n");
		return 0;
	}
	// Update ptr to memory to reallocated address
	mem->memory = ptr;
	// Copy contents after existing data up to realsize
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	// Update MemoryStruct size property
	mem->size += realsize;
	// Set last allocated mem location to 0
	mem->memory[mem->size] = 0;

	return realsize;
}
