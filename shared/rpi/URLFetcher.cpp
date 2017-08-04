#include "URLFetcher.h"
#include <curl/curl.h>

URLFetcher::URLFetcher()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

URLFetcher::~URLFetcher()
{
    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();
}

bool URLFetcher::fetchURL(const char* url, MemoryStruct& mem)
{
    CURL *curl_handle;
    CURLcode res;

    //mem.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    //mem.size = 0;    /* no data at this point */

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, URLFetcher::WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&mem);

    /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */
    res = curl_easy_perform(curl_handle);

    bool success = true;
    /* check for errors */
    if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
            success = false;
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);
    return success;
}

size_t URLFetcher::WriteMemoryCallback(void *contents,
                                       size_t size,
                                       size_t nmemb,
                                       void *userp)
{
  size_t realsize = size * nmemb;
  MemoryStruct *mem = (MemoryStruct*)userp;

  /*
  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  */

  mem->append(contents, realsize);

  return realsize;
}
