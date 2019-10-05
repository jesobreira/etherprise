#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "jsmn.h"

CURL *curl;
CURLcode res;

jsmn_parser p;
jsmntok_t t[128];

struct MemoryStruct {
	char *memory;
	size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */ 
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

int etherprise_daemon_bootstrap(char *rpchost, char *hotwallet, char *auth) {
	struct MemoryStruct chunk;
	chunk.memory = malloc(1);
	chunk.size = 0;

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();
	if (!curl) {
		puts("Unable to load curl library");
		return -1;
	}

	// test RPC connectivity
	curl_easy_setopt(curl, CURLOPT_URL, rpchost);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"jsonrpc\":\"2.0\",\"method\":\"web3_clientVersion\",\"params\":[],\"id\":67}");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

	struct curl_slist *hs=NULL;
	hs = curl_slist_append(hs, "Content-Type: application/json");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

	if (auth != NULL) {
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		curl_easy_setopt(curl, CURLOPT_USERPWD, auth);
	}

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		fprintf("Cannot connect to the JSON-RPC: %s\n", curl_easy_strerror(res));
	}

	char *body;

	curl_easy_cleanup(curl);

	jsmn_init(&p);
	int r = jsmn_parse(&p, chunk.memory, strlen(chunk.memory), t, sizeof(t));

	if (r < 0) {
	    printf("Failed to parse JSON: %d\n", r);
	    return 1;
	}

	for (int i = 1; i < r; i++) {
		if (jsoneq(chunk.memory, &t[i], "result") == 0) {
			printf("Connected to server: %.*s\n", t[i + 1].end - t[i + 1].start, chunk.memory + t[i + 1].start);
		}
	}

	// check if some wallet has balance (sent while the daemon was off)
	

	return 0;
}

int etherprise_daemon(char *rpchost, char *hotwallet, int delay, char *auth) {
	int last_block = 0;

	while (1) {

	}
}