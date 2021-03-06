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

#ifndef RQ_CONFIG_READER_H
#define RQ_CONFIG_READER_H

#include <json-c/json_object.h>
#include <string>

namespace rq {
class ConfigJsonParser {
 public:
  const char *user_agent;
  bool use_ajax;
  bool write_response_body_to_file;
  std::string post_fields;

  const char *cookie_domain;
  json_object *cookie_items_obj;

  int amount_urls;
  struct json_object *urls_obj;

  // Constructor
  ConfigJsonParser(std::string &path_config);

 private:
  json_object *config_obj;

  // resolve basic config from JSON: url, user_agent, us_ajax, write_response_body_to_file
  void ResolveSettings();
  void ResolveUrls();
  void ResolveCookies();
  void ResolvePostFields();

};

} // namespace rq

#endif //RQ_CONFIG_READER_H
