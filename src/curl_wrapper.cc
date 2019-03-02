/*
  Copyright (c) 2019, Kay Stenschke
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of  nor the names of its contributors may be used to
     endorse or promote products derived from this software without specific
     prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include <curl/curl.h>
#include <cstdlib>

#include "curl_wrapper.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

void PrintCookies(CURL *curl)
{
  CURLcode res;
  struct curl_slist *cookies;
  struct curl_slist *nc;
  int i;

  printf("Cookies, curl knows:\n");
  res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
  if(res != CURLE_OK) {
    fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n",
            curl_easy_strerror(res));
    exit(1);
  }
  nc = cookies;
  i = 1;
  while(nc) {
    printf("[%d]: %s\n", i, nc->data);
    nc = nc->next;
    i++;
  }
  if(i == 1) {
    printf("(none)\n");
  }
  curl_slist_free_all(cookies);
}

void SetCookie(CURL *curl_handle,
               const char *cookie_url,
               const char *cookie_path,
               bool host_only,
               bool http_only,
               const char *cookie_key,
               const char *cookie_value) {
  // Netscape format cookie
  char nline[256];
  snprintf(
      nline,
      sizeof(nline),
      "%s\t%s\t%s\t%s\t%lu\t%s\t%s",
      cookie_url, host_only ? "TRUE" : "FALSE", cookie_path, http_only ? "TRUE" : "FALSE",
      (unsigned long) time(nullptr) + 991337UL,
      cookie_key, cookie_value
  );

  CURLcode res = curl_easy_setopt(curl_handle, CURLOPT_COOKIELIST, nline);

  if (res!=CURLE_OK) {
    fprintf(stderr, "Curl curl_easy_setopt failed: %s\n",
            curl_easy_strerror(res));
//return 1;
  }

  //printf("Erasing curl's knowledge of cookies!\n");
  //curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
}