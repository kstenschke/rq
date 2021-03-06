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
#include <sys/stat.h>

#include "config.h"
#include "helper/helper_file.h"
#include "config_json_parser.h"
#include "helper/helper_string.h"
#include "helper/helper_curl.h"

/**
 * @param argc Amount of arguments received
 * @param argv Array of arguments received, argv[0] is name and path of executable
 */
int main(int argc, char **argv) {
  if (argc == 2 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)) {
    std::cout << "rq version " << Rq_VERSION_MAJOR << "." << Rq_VERSION_MINOR << "\n"
                 "Copyright (c) 2019 Kay Stenschke\n\n";
    return 0;
  }

  std::string path_binary = helper::File::GetBinaryPath(argv, 2);
  std::string path_config = path_binary;

  // get parameters from config.json
  path_config = path_config.append("config.json");
  if (!helper::File::FileExists(path_config)) {
    fprintf(stderr, "File not found: config.json\n");
    return 1;
  }
  auto *Config = new rq::ConfigJsonParser(path_config);

  // init cURL
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (!curl) {
    fprintf(stderr, "Curl init failed\n");
    return 1;
  }

  curl_easy_setopt(curl, CURLOPT_USERAGENT, Config->user_agent);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

  // unset cookies
  curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
  // set cookies as in config.json
  for (int i = 0; i < json_object_array_length(Config->cookie_items_obj); i++) {
    struct json_object *cookie_value_obj = json_object_array_get_idx(Config->cookie_items_obj, i);

    const char *cookie_key = json_object_get_string(json_object_array_get_idx(cookie_value_obj, 0));
    const char *cookie_value = json_object_get_string(json_object_array_get_idx(cookie_value_obj, 1));
    bool cookie_host_only = json_object_get_int(json_object_array_get_idx(cookie_value_obj, 2))==1;
    bool cookie_http_only = json_object_get_int(json_object_array_get_idx(cookie_value_obj, 3))==1;
    const char *cookie_path = json_object_get_string(json_object_array_get_idx(cookie_value_obj, 4));

    helper::Curl::SetCookie(curl,
                            Config->cookie_domain,
                            cookie_path,
                            cookie_host_only,
                            cookie_http_only,
                            cookie_key,
                            cookie_value);
  }

  // helper::Curl::PrintCookies(curl);

  if (Config->use_ajax) {
    struct curl_slist *http_headers = nullptr;
    http_headers = curl_slist_append(http_headers, "Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);
    http_headers = curl_slist_append(http_headers, "X-Requested-With: XMLHttpRequest");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);
  }

  int amount_prefix_digits = helper::String::GetAmountDigits(Config->amount_urls);
  for (int i=0; i<Config->amount_urls; i++) {
    std::string url = json_object_get_string(json_object_array_get_idx(Config->urls_obj, i));
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // set post fields
    // TODO implement post field types with generic data values
    std::string post_fields = Config->post_fields;
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());

    std::string filename_response_body;
    std::string path_file_response_body;
    if (Config->write_response_body_to_file) {
      // write response to file instead stdout
      const char *pathOutDirectory = "results";
      if (!helper::File::DirectoryExists(pathOutDirectory) && -1==mkdir(pathOutDirectory, 0777)) {
        fprintf(stderr, "Failed to create results directory\n");
        return 1;
      }

      filename_response_body = helper::String::UrlToFilename(url.c_str(), i+1, amount_prefix_digits);
      path_file_response_body = pathOutDirectory;
      path_file_response_body = path_file_response_body.append("/").append(filename_response_body);

      FILE *file_handle = fopen(path_file_response_body.c_str(), "wb");
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, file_handle);
    }

    // perform request, write response body (to stdout or file)
    res = curl_easy_perform(curl);
    if (res!=CURLE_OK) {
      fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
      return 1;
    }

    char *content_type = nullptr;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
    if (Config->write_response_body_to_file) {
      // change ending of result file, to type detected from response body
      helper::File::AddFileExtensionByContentType(path_file_response_body, content_type);
    }
  }

  // clean up
  delete Config;
  curl_easy_cleanup(curl);
  curl_global_cleanup();

  return 0;
}