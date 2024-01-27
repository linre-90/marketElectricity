#define CURL_STATICLIB

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "web.h"
#include <curl/curl.h>


struct MemoryStruct {
	char* memory;
	size_t size;
};

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);


void initWeb(void) {
	curl_global_init(CURL_GLOBAL_ALL);
}

void cleanWeb(void) {
	curl_global_cleanup();
}


float fetchSingleHourPrice(unsigned long hourUnixTime) {
	time_t t = hourUnixTime;
	int hour = localtime(&t)->tm_hour;
	CURL* curl = curl_easy_init();
	CURLcode res;

	if (curl) {
		struct MemoryStruct chunk;
		chunk.memory = malloc(1);
		chunk.size = 0;
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.porssisahko.net/v1/price.json?date=2024-01-27&hour=16");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else {
			printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
			printf("%s\n", chunk.memory);
		}
		free(chunk.memory);
		curl_easy_cleanup(curl);
	}


	return 1;
}

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)userp;

	char* ptr = realloc(mem->memory, mem->size + realsize + 1);
	if (!ptr) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}
