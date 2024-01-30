#define CURL_STATICLIB

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "web.h"
#include <curl/curl.h>

/* Memory for json response. */
struct MemoryStruct {
	char* memory;
	size_t size;
};

/* Callback to write json response in MemoryStruct, resizes dynamically. */
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);

/* Converts json string to struct Price array. */
float parseJsonResponse(const char* res, struct Price* priceArr);


void fetchData(struct Price* priceArr) {
	curl_global_init(CURL_GLOBAL_ALL);
	CURL* curl = curl_easy_init();
	CURLcode res;
	if (curl) {
		struct MemoryStruct chunk;
		chunk.memory = malloc(1);
		chunk.size = 0;
		if (chunk.memory == NULL) {
			printf("Cannot allocate memory for chunk.memory.\n");
			curl_global_cleanup();
		}

		/* Prepare and perform request */
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.porssisahko.net/v1/latest-prices.json");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else {
			parseJsonResponse(chunk.memory, priceArr);
		}

		free(chunk.memory);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}
}


float parseJsonResponse(const char* res, struct Price* priceArr) {
	// Skip the opening { and jump right in [ character
	int iterator = 10;
	int priceObjIterator = 0;
	bool parsingObject = false;
	char searchPropertyPrice[] = "price";

	while (res[iterator] != 0) {
		// start parsing object
		if (res[iterator] == '{' || parsingObject) {
			parsingObject = true;
			char priceStr[10] = { 0 };
			char utcYYYYStr[5] = { 0 };
			char utcDDStr[3] = { 0 };
			char utcMMStr[3] = { 0 };
			char utcHH[3] = { 0 };

			int priceStrIterator = 0;

			// Fast forward to 'p' character and parse price
			while (res[iterator] != 'p' && res[iterator] != 0){ iterator++; }
			iterator++;
			// Fast forward to first number digit or -
			while (res[iterator] != ':' && res[iterator] != 0) { iterator++; }
			iterator++;
			// Start extracting price
			while (res[iterator] != ',' && res[iterator] != 0)
			{
				priceStr[priceStrIterator] = res[iterator];
				iterator++;
				priceStrIterator++;
			}
			// Set price to struct convert cents to eur
			priceArr[priceObjIterator].price = atof(priceStr);

			// Fast forward to 's' character and parse date
			while (res[iterator] != 's' && res[iterator] != 0) { iterator++; }
			iterator++;

			// Fast forward to first year digit
			while (res[iterator] != ':' && res[iterator] != 0) { iterator++; }
			iterator+=2;
			utcYYYYStr[0] = res[iterator];
			utcYYYYStr[1] = res[iterator + 1];
			utcYYYYStr[2] = res[iterator + 2];
			utcYYYYStr[3] = res[iterator + 3];
			// set year
			//priceArr[priceObjIterator].YYYY = atoi(utcYYYYStr);
			iterator += 3;

			// Skip - and jump to first month digit
			iterator += 2;
			utcMMStr[0] = res[iterator];
			utcMMStr[1] = res[iterator + 1];
			// set month
			//priceArr[priceObjIterator].MM = atoi(utcMMStr);

			// Skip - and jump to first day digit
			iterator += 3;
			utcDDStr[0] = res[iterator];
			utcDDStr[1] = res[iterator + 1];
			// set year
			//priceArr[priceObjIterator].DD = atoi(utcDDStr);
			
			// Skip t and jump to first hour digit
			iterator += 3;
			utcHH[0] = res[iterator];
			utcHH[1] = res[iterator + 1];
			// set hour
			//priceArr[priceObjIterator].utcHour = atoi(utcHH);

			// Build up unix time stamp from parsed strings.
			struct tm m_time = {
				.tm_year = atoi(utcYYYYStr) - 1900,
				.tm_mon = atoi(utcMMStr) - 1 < 0 ? 0 : atoi(utcMMStr) - 1,
				.tm_mday = atoi(utcDDStr),
				.tm_hour = atoi(utcHH) + 2,// Push forward two hours so gmtime function returns correct utc time
				.tm_min = 0,
				.tm_sec = 0,
				.tm_isdst = -1,
			};


			time_t utc = mktime(&m_time);
			priceArr[priceObjIterator].utcTime = utc;

			// Object parsing is finished
			parsingObject = false;
			priceObjIterator++;
		}
		iterator++;
	}
}


static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)userp;

	char* ptr = realloc(mem->memory, mem->size + realsize + 1);
	if (!ptr) {
		/* out of memory! */
		printf("Not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}
