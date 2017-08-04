#include "URLUploader.h"
#include <curl/curl.h>

URLUploader::URLUploader(const string& base)
: _base(base)
{
    curl_global_init(CURL_GLOBAL_ALL);
}

URLUploader::~URLUploader()
{
    curl_global_cleanup();
}

bool URLUploader::upload(const char* name, MemoryStruct& mem)
{
    string fullUrl = _base + string(name);
    printf("name %s fullurl: %s\n", name, fullUrl.c_str());
    mem.pos = 0;

    CURL *curl;
    CURLcode res;

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
      /* we want to use our own read function */
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, URLUploader::ReadCallback);

      /* enable uploading */
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

      /* HTTP PUT please */
      curl_easy_setopt(curl, CURLOPT_PUT, 1L);

      /* specify target URL, and note that this URL should include a file
         name, not only a directory */
      curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

      /* now specify which file to upload */
      curl_easy_setopt(curl, CURLOPT_READDATA, &mem);

      /* provide the size of the upload, we specicially typecast the value
         to curl_off_t since we must be sure to use the correct data size */
      curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                       (curl_off_t)mem.size);

      /* Now run off and do what you've been told! */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

      /* always cleanup */
      curl_easy_cleanup(curl);
    }
    //fclose(hd_src); /* close the local file */

    //curl_global_cleanup();
    return true;
}

size_t URLUploader::ReadCallback(void* ptr,
                                 size_t size,
                                 size_t nmemb,
                                 void* stream)
{
    size_t retcode;
    curl_off_t nread;

    MemoryStruct* mem = (MemoryStruct*)stream;
    /* in real-world cases, this would probably get this data differently
       as this fread() stuff is exactly what the library already would do
       by default internally */
    //retcode = fread(ptr, size, nmemb, (FILE*)stream);

    int bytesToRead = size*nmemb;
    int bytesRemaining = mem->size - mem->pos + 1;
    if (bytesToRead > bytesRemaining)
    {
        bytesToRead = bytesRemaining;
    }

    memcpy(ptr, &mem->memory[mem->pos], bytesToRead);
    mem->pos += bytesToRead;

    retcode = bytesToRead;
    nread = (curl_off_t)retcode;

    fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
            " bytes from file\n", nread);

    return retcode;
}
