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

#include <iostream>
#include <fstream>
#include <json-c/json.h>
#include <curl/curl.h>

#include "helper_file.h"
#include "config_reader.h"
#include "helper_string.h"
#include "curl_wrapper.h"

const char *ReadPostFieldsConfig(const json_object *config_obj);
/**
 * @param argc Amount of arguments received
 * @param argv Array of arguments received, argv[0] is name and path of executable
 */
int main(int argc, char **argv) {
  std::string path_binary = GetBinaryPath(argv, 2);
  std::string path_config = path_binary;
  path_config = path_config.append("config.json");

  if (!FileExists(path_config)) {
    fprintf(stderr, "File not found: config.json\n");
    return 1;
  }

  const char *url;
  const char *user_agent;
  bool use_ajax;
  bool write_response_body_to_file;
  const char *post_fields;

  json_object *config_obj;

  ReadConfig(path_config, url, user_agent, use_ajax, write_response_body_to_file, config_obj);

  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (!curl) {
    fprintf(stderr, "Curl init failed\n");
    return 1;
  }

  curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

  // set cookies
  curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

  const char *cookie_domain;
  struct json_object *cookie_items_obj;

  ReadCookiesConfig(config_obj, cookie_domain, cookie_items_obj);

  for (int i=0; i < json_object_array_length(cookie_items_obj); i++) {
    struct json_object *cookie_value_obj = json_object_array_get_idx(cookie_items_obj, i);

    const char *cookie_key = json_object_get_string(json_object_array_get_idx(cookie_value_obj, 0));
    const char *cookie_value = json_object_get_string(json_object_array_get_idx(cookie_value_obj, 1));
    bool cookie_host_only = json_object_get_int(json_object_array_get_idx(cookie_value_obj, 2)) == 1;
    bool cookie_http_only = json_object_get_int(json_object_array_get_idx(cookie_value_obj, 3)) == 1;
    const char *cookie_path = json_object_get_string(json_object_array_get_idx(cookie_value_obj, 4));

    SetCookie(curl, cookie_domain, cookie_path, cookie_host_only, cookie_http_only, cookie_key, cookie_value);
  }

  //PrintCookies(curl);

  if (use_ajax) {
    struct curl_slist *http_headers = nullptr;
    http_headers = curl_slist_append(http_headers, "Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);
    http_headers = curl_slist_append(http_headers, "X-Requested-With: XMLHttpRequest");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);
  }

  post_fields = ReadPostFieldsConfig(config_obj);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);

  std::string filename_response_body;
  if (write_response_body_to_file) {
    // write response to file instead stdout
    filename_response_body = UrlToFilename(url);
    FILE *f = fopen(filename_response_body.c_str(), "wb");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
  }
  // perform request, write response
  res = curl_easy_perform(curl);
  if (res!=CURLE_OK) {
    fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
    return 1;
  }

  char *content_type = nullptr;
  curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
  if (write_response_body_to_file) {
    AddFileExtensionByContentType(path_binary, filename_response_body, content_type);
  }

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  return 0;
}